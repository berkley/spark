var nconf = require('nconf');
var config;
var async = require('async');
var util = require('./util.js');
var sockets = require('./sockets.js');
var bitmaps = [];

exports.setConfig = function(conf) {
	config = conf;
	util.setConfig(config);
};

//////////////////////////////////////////////////////////////
////////// functions to setup and stage cores/screens ////////
//////////////////////////////////////////////////////////////

//Any tasks that need to be completed when a core initially connects it's
//websocket should be done here
exports.setup = function(coreid, done) {
	console.log("core " + coreid + " setup");

	async.eachSeries(bitmaps, function(bitmap, callback)
	{
		console.log("sending bitmap to core " + coreid);
		sockets.send([util.getNameForCoreId(coreid)], bitmap, function(err){
			console.log("screen.send callback");
			callback(err);
		}, 1000);
	}, function(err){
		done(err);
	});
};

//stage a bitmap to be sent to the core upon connection
exports.addBitmap = function(bmp, width, height, index) {
	var data = "97," + width + "," + height + "," + index + ",";
	for(var i=0; i<bmp.length; i++)
	{
		data += bmp[i];
		if(i != bmp.length - 1)
		{
			data += ",";
		}
	}
	bitmaps.push(data);
	console.log("added bitmaps: ", bitmaps);
};

//////////////////////////////////////////////////////////////
////////// functions to control individual screens ///////////
//////////////////////////////////////////////////////////////

//draw the bitmap stored at index on a core at column on the cores in coreNames
exports.drawBMP = function(coreNames, column, wraparound, index, callback, sendDelay, reset){
	if(reset)
		resetScreen = 1;
	else
		resetScreen = 0;

	var wrap = 1;
	var negative = 0;
	if(!wraparound)
		wrap = 0;
	if(column < 0)
	{
		negative = 1;
		column *= -1;
	}

	var data = "96," + column + "," + resetScreen + "," + wrap + "," + index + "," + negative;	
	console.log("drawing bmp " + index + " at: ", column);
	console.log("cores: ", coreNames);
	sockets.send(coreNames, data, function(err){
		callback(err);
	}, sendDelay);
};

exports.allOff = function(coreNames, callback){
	var data = "95";
	console.log("Turning all pixels off for cores ", coreNames);
	sockets.send(coreNames, data, function(err){
		callback(err);
	});
};

//set the brightness of the pixels on core with id coreid
exports.setBrightness = function(coreNames, brightness, callback) {
	brightness = brightness || config.get("defaultBrightness");
	var data = "94," + brightness;
	console.log("Screen setting brightness to ", brightness);
	sockets.send(coreNames, data, function(err){
		callback(brightness, err);
	});
};

//set a screen to all one color
exports.setScreen = function(coreNames, red, green, blue, callback) {
	var data =  "98," + red + "," + green + "," + blue;
	sockets.send(coreNames, data, function(err){
		callback(err);
	});
};

//set a single pixel (x,y) to (reg, green, blue) on the cores in coreNames
exports.setPixel = function(coreNames, x, y, red, green, blue, reset,  callback, sendDelay) {
	var res = "0";
	if(reset)
		res = "1";

	var data =  "93," + x + "," + y + "," + red + "," + green + "," + blue + "," + res;
	sockets.send(coreNames, data, function(err){
		callback(err);
	}, sendDelay);
};

//set a single column to (reg, green, blue) on the cores in coreNames
exports.setColumn = function(coreNames, column, red, green, blue, reset, callback, sendDelay) {
	var res = "0";
	if(reset)
		res = "1";

	var data =  "92," + column + "," + red + "," + green + "," + blue + "," + res;
	sockets.send(coreNames, data, function(err){
		callback(err);
	}, sendDelay);
};

//set a single row row to (reg, green, blue) on the cores in coreNames
exports.setRow = function(coreNames, row, red, green, blue, reset, callback, sendDelay) {
	var res = "0";
	if(reset)
		res = "1";
	var data =  "91," + row + "," + red + "," + green + "," + blue + "," + res;
	sockets.send(coreNames, data, function(err){
		callback(err);
	}, sendDelay);
};

//////////////////////////////////////////////////////////////
////////// functions to control virtual screens  /////////////
//////////////////////////////////////////////////////////////

//draw a bitmap starting at column on the virtual screen (across cores)
//NOTE: this should really iterate over the BMP, not the screens
exports.drawVBMP = function(column, index, callback, resetScreen) {
	if(resetScreen == undefined) resetScreen = true; //default reset to true
	console.log("Drawing VBMP ", index, " at col ", column);
	var screens = config.get("screen");
	var bmp = bitmaps[index];
	// console.log("screens: ", screens);
	for(var i=0; i<screens.length; i++)
	{
		var screen = screens[i];
		if(column >= screen.vColStart && column <= screen.vColEnd)
		{	//find which screen the bmp col starts on
			var colDiff = column - screen.vColEnd;
			var startCol = column - screen.vColStart;
			if(colDiff < 0) //we are straddling two screens
			{
				column = column - screen.vColStart;
				var nextScreenIndex = i + 1;
				if(nextScreenIndex >= screens.length)
					nextScreenIndex = 0;
				var nextScreen = screens[nextScreenIndex];


				async.parallel([
					function(cb){
						console.log("DRAW1 ", screen.name, " col:", startCol);
						exports.drawBMP([screen.name], startCol, false, index, function(err){
							cb(err);
						}, undefined, resetScreen); //draw the first part
					},
					function(cb){
						console.log("DRAW2 ", screen.name, " col:", colDiff);
						exports.drawBMP([nextScreen.name], colDiff, false, index, function(err){
							cb(err);
						}, undefined, resetScreen); //draw the next part
					}
				], function(err){
					callback(err);
				});
			}
			else
			{
				console.log("DRAW3 ", screen.name, " col:", startCol);
				// console.log("drawing vbmp on screen ", screen.name);
				exports.drawBMP([screen.name], startCol, false, index, function(err){
					callback(err);
				}, undefined, resetScreen);
			}
		}
	}
};
