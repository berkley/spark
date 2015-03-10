var nconf = require('nconf');
var config;
var async = require('async');

exports.setConfig = function(conf) {
	config = conf;
};

exports.getCoreForPixel = function(vPixel) {
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

exports.getCoreIdForName = function(name) {
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

exports.getNameForCoreId = function(coreId) {
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