var nconf = require('nconf');
var config;

exports.setConfig = function(conf) {
	config = conf;
};

exports.setVPixel = function(sockets, vPixel, red, green, blue) {	
	console.log("setting vPixel " + vPixel + " to rgb: " + red + "," + green + "," + blue);
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

var getSocketData = function(coreId, vPixel, red, green, blue) {
	var screen = config.get("screen");
	var name = getNameForCoreId(coreId);
	for(var i=0; i<screen.length; i++)
	{
		if(screen[i].name == name)
		{
			console.log("creating socket data for core: " + screen[i].name);
			var vPixelStart = screen[i].vPixelStart;
			var data = (vPixel - vPixelStart) + "," + red + "," + green + "," + blue;
			console.log("vPixelStart: " + vPixelStart);
			console.log("vPixel: " + vPixel);
			console.log("scren data " + data);
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
