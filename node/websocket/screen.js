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
exports.drawBMP = function(coreNames, column, index, callback){
	var data = "96," + column + ",1," + index;	
	console.log("drawing bmp " + index + " at: ", column);
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
exports.setColor = function(coreNames, red, green, blue, callback) {
	var data =  "98," + red + "," + green + "," + blue;
	sockets.send(coreNames, data, function(err){
		callback(err);
	});
};

//set a single pixel (x,y) to (reg, green, blue) on the cores in coreNames
exports.setPixel = function(coreNames, x, y, red, green, blue, callback) {
	console.log("Not yet implemented");
	callback("Not yet implemented");
};

//set a single column to (reg, green, blue) on the cores in coreNames
exports.setColumn = function(coreNames, column, red, green, blue, callback) {
	console.log("Not yet implemented.");
	callback("Not yet implemented");
};

//set a single row row to (reg, green, blue) on the cores in coreNames
exports.setRow = function(coreNames, row, red, green, blue, callback) {
	console.log("Not yet implemented.");
	callback("Not yet implemented");
};

//////////////////////////////////////////////////////////////
////////// functions to control virtual screens  /////////////
//////////////////////////////////////////////////////////////

//draw a bitmap starting at column on the virtual screen (across cores)
exports.drawVBMP = function(column, index, callback) {
	console.log("Drawing VBMP ", index, " at col ", column);
	var screens = config.get("screen");
	console.log("screens: ", screens);
	for(var i=0; i<screens.length; i++)
	{
		var screen = screens[i];
		console.log("screen: ", screen);
		console.log("screen.name: ", screen.name);
		if(column >= screen.vColStart && column <= screen.vColEnd)
		{
			console.log("drawing vbmp on screen ", screen.name);
			column = column - screen.vColStart;
			exports.drawBMP([screen.name], column, index, function(err){
				callback(err);
			});
		}
	}
};


