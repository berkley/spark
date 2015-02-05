var nconf = require('nconf');
var config;

exports.setConfig = function(conf) {
	config = conf;
};

exports.setVPixel = function(sockets, vPixel, red, green, blue) {	
	// console.log("setting vPixel " + vPixel + " to rgb: " + red + "," + green + "," + blue);
	if(sockets.length < 1)
		return;
	var coreId = getCoreForPixel(vPixel);
	if(coreId)
	{
		var socket = sockets[coreId];
		if(socket)
		{
			var data = getSocketData(coreId, vPixel, red, green, blue);
			console.log("data: ", data);
			socket.send(data, function(err){
				if(err)
				{
					console.log("Error sending WS data ", data);
				}
			});
		}
	}

};

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

var getSocketData = function(coreId, vPixel, red, green, blue) {
	var screen = config.get("screen");
	var name = getNameForCoreId(coreId);
	for(var i=0; i<screen.length; i++)
	{
		if(screen[i].name == name)
		{
			var vPixelStart = screen[i].vPixelStart;
			var pixelId = (vPixel - vPixelStart); //simple case where we have one long strip, not an array
			if(screen[i].rows && screen[i].rows > 1)
			{ //this is an array, so the pixelId may be different depending on the wiring
				if(screen[i].wiring == "serial")
				{
					var rowNum = parseInt(vPixel / screen[i].pixelsPerRow);
					if(rowNum % 2 != 0)
					{
						var rowLength = screen[i].pixelsPerRow
						var mod = vPixel % rowLength;
						console.log("rowNum: ", rowNum);
						console.log("vpix mod rowLength: ", mod)
						pixelId = ((rowLength * rowNum) + (rowLength - mod)) - 1;
						console.log("pixelId: ", pixelId);
					}
				}
			}
			var data = pixelId + "," + red + "," + green + "," + blue;
			return data;
		}
	}
};

var getCoreForPixel = function(vPixel) {
	var screen = config.get("screen");
	for(var i=0; i<screen.length; i++)
	{
		var s = screen[i];
		var start = s.vPixelStart;
		var end = s.vPixelEnd;
		if(vPixel >= start && vPixel <= end)
		{
			var id = getCoreIdForName(s.name);
			return id;
		}
	}
};

var getCoreIdForName = function(name) {
	var cores = config.get("cores");
	for(var i=0; i<cores.length; i++)
	{
		var core = cores[i];
		if(core.name == name)
		{
			return core.id;
		}
	}
};

var getNameForCoreId = function(coreId) {
	var cores = config.get("cores");
	for(var i=0; i<cores.length; i++)
	{
		var core = cores[i];
		if(core.id == coreId)
		{
			return core.name;
		}
	}
};
