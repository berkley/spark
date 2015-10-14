var request = require('request');
var nconf = require('nconf');
var util = require('./util.js');

var config;

exports.setConfig = function(conf) {
	config = conf;
	util.setConfig(config);
};

exports.index = function(req, res) {
	var cores = {"cores":config.get("cores")};
	console.log(cores);
	res.render('lights', cores);
};

exports.params = function(req, res) {
	var coreId = req.params.coreId;
	console.log("getting params for coreId: " + coreId);
	var url = util.buildParamUrl(coreId);
	request.get(url, function(err, response, body) {
		if(err)
		{
			res.send(500, err);
		}
		else
		{
			res.send(200, body);
		}
	});
};

exports.action = function(req, res) {
	var action = req.query.action;
	var coreId = req.query.coreId;
	var data = "";
	if(action == "allOff")
	{
		data = "setAll,0,0,0";
	}
	else if(action == "stop")
	{
		data = "shutdown,";
	}
	else if(action == "setAll")
	{
		data = "setAll," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1;
	}
	else if(action == "alternate")
	{
	    data = "alternate," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2;
	}
	else if(action == "loopAlternate")
	{
	    data = "loopAlternate," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.delay;
	}
	else if(action == "rainbow")
	{
		data = "rainbow,";
	}
	else if(action == "loopBlocks")
	{
		data = "loopBlocks," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.delay + "," + req.query.blockSize;
	}
	else if(action == "fadeColor")
	{
		data = "fadeColor," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.delay + "," + req.query.duration;
	}
	else if(action == "blocks")
	{
		data = "blocks," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.blockSize;
	}
	else if(action == "particles")
	{
		data = "particles," + req.query.numParticles + "," + req.query.maxVelocity + "," + req.query.respawn + "," + req.query.threed + "," + req.query.flicker;
	}
	else if(action == "endrun")
	{
		data = "endrun," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.delay;
	}
	else if(action == "snow")
	{
		data = "snow,";
	}
	else if(action == "usa")
	{
		data = "usa,";
	}
	else if(action == "lightning")
	{
		data = "lightning,";
	}
	else if(action == "skull")
	{
		data = "skull,";
	}
	else if(action == "skullnolightning")
	{
		data = "skullnolightning,";
	}
	else if(action == "setBrightness")
	{ //brightness: uint8_t 0-255
		data = "setBrightness," + req.query.brightness;
	}

	util.runPost(data, action, coreId, res);
};


