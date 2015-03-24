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
	var data = "95," + brightness;
	console.log("Setting brightness to ", brightness);
	sockets.send(coreNames, data, function(err){
		callback(err);
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
exports.setPixel(coreNames, x, y, red, green, blue, callback) {
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
	callback("Not yet implemented");	
};

// exports.setVPixel = function(sockets, vPixel, red, green, blue) {	
// 	// console.log("setting vPixel " + vPixel + " to rgb: " + red + "," + green + "," + blue);
// 	if(sockets.length < 1)
// 		return;
// 	var coreId = getCoreForPixel(vPixel);
// 	if(coreId)
// 	{
// 		var socket = sockets[coreId];
// 		if(socket)
// 		{
// 			var data = getSocketData(coreId, vPixel, red, green, blue);
// 			console.log("data: ", data);
// 			socket.send(data, function(err){
// 				if(err)
// 				{
// 					console.log("Error sending WS data ", data);
// 				}
// 			});
// 		}
// 	}
// };

// exports.setVBMP = function(sockets, upperLeftVPixel, bmp, callback) {
// 	if(sockets.length < 1)
// 		return;
// 	var coreId = getCoreForPixel(upperLeftVPixel);
// 	if(coreId)
// 	{
// 		var socket = sockets[coreId];
// 		if(socket)
// 		{
// 			//key: vals[0]: -97
// 	        //     vals[1]: reset [1|0] //set the screen to all off before writing the bmp
// 	        //     vals[2]: upperLeft
// 	        //     vals[3]: bmp width
// 	        //     vals[4]: bmp height
// 	        //     vals[5]: r1
// 	        //     vals[6]: g1
// 	        //     vals[7]: b1
// 	        //     etc
// 			var data =  "97,1," + upperLeftVPixel + ",8,8,";
// 			for(var i=0; i<bmp.length; i++)
// 			{
// 				data += bmp[i];
// 				if(i != bmp.length - 1)
// 					data += ",";
// 			}

// 			socket.send(data, function(err){
// 				if(err)
// 				{
// 					console.log("Error sending WS data ", data);
// 				}
// 			}).on('message', function(message){
// 				console.log("message: ", message);
// 				callback(err);
// 			});
// 		}
// 	}
// };



// var actualPixPerChannel = 48;
// 	    var mod = addr % actualPixPerChannel;
// 	    var base = addr - mod;
// 	    var newaddr;
// 	    var upperBounds = actualPixPerChannel - WIDTH - 1;
// 	    var addrMinusBase = addr - base;

// 	    if(addrMinusBase >= 0 && addrMinusBase < WIDTH)
// 	    {
// 	        newaddr = addrMinusBase;
// 	    }
// 	    if(addrMinusBase >= WIDTH && addrMinusBase <= upperBounds)
// 	    { //invert
// 	        var offset = addrMinusBase - WIDTH;
// 	        newaddr = upperBounds - offset;
// 	    }
// 	    if(addrMinusBase > upperBounds)
// 	    {
// 	        newaddr = addrMinusBase;
// 	    }
	    
// 	    return newaddr + base;

// var getSocketData = function(coreId, vPixel, red, green, blue) {
// 	var screen = config.get("screen");
// 	var name = getNameForCoreId(coreId);
// 	for(var i=0; i<screen.length; i++)
// 	{
// 		if(screen[i].name == name)
// 		{
// 			var vPixelStart = screen[i].vPixelStart;
// 			var pixelId = (vPixel - vPixelStart); //simple case where we have one long strip, not an array
// 			if(screen[i].rows && screen[i].rows > 1)
// 			{ //this is an array, so the pixelId may be different depending on the wiring
// 				if(screen[i].wiring == "serial")
// 				{
// 					var rowNum = parseInt(vPixel / screen[i].pixelsPerRow);
// 					if(rowNum % 2 != 0)
// 					{
// 						var rowLength = screen[i].pixelsPerRow
// 						var mod = vPixel % rowLength;
// 						console.log("rowNum: ", rowNum);
// 						console.log("vpix mod rowLength: ", mod)
// 						pixelId = ((rowLength * rowNum) + (rowLength - mod)) - 1;
// 						console.log("pixelId: ", pixelId);
// 					}
// 				}
// 			}
// 			var data = pixelId + "," + red + "," + green + "," + blue;
// 			return data;
// 		}
// 	}
// };

