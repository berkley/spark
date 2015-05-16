var request = require('request');
var nconf = require('nconf');
var async = require('async');
var config;

exports.setConfig = function(conf) {
	config = conf;
};

exports.index = function(req, res) {
	var cores = {"cores":config.get("cores")};
	console.log(cores);
	res.render('index', cores);
};

exports.params = function(req, res) {
	var coreId = req.params.coreId;
	console.log("getting params for coreId: " + coreId);
	var url = buildParamUrl(coreId);
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
	else if(action == "setBrightness")
	{ //brightness: uint8_t 0-255
		data = "setBrightness," + req.query.brightness;
	}

	runPost(data, action, coreId, res);
};

var runPost = function(data, action, coreId, res) {
	console.log("action: " + action);
	console.log("coreId: " + coreId);
	console.log("data: " + data);

	if(coreId == "all")
	{
		var cores = config.get("cores");
		var resData = [];
		async.each(cores, function(core, callback){
			var url = buildUrl("run", core.id);
			request.post(url, function(err, response, body) {
				resData.push(body);
				callback();
				// res.send(body);
			}).form(buildFormData(data));
		}, function(err){
			if(err){
				console.log("Error making API call to core ", core);	
			} else {
				console.log("resData: ", resData);
				res.send(resData);
			}
		});
	}
	else
	{
		var url = buildUrl("run", coreId);
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));	
	}
	
};

var buildFormData = function(params) {
	var access_token = config.get("access_token");
	var formData = {"access_token":access_token ,"params":params};
	console.log("formData: ", formData);
	return formData;
};

var buildUrl = function(action, coreId) {
	var sparkUrl = config.get("spark_url");
	var url = sparkUrl + "/" + coreId + "/" + action;
	console.log("url: ", url);
	return url;
};

var buildParamUrl = function(coreId) {
	var sparkUrl = config.get("spark_url");
	var url = sparkUrl + "/" + coreId + "/parameters?access_token=" + config.get("access_token");
	console.log("paramUrl: ", url);
	return url;
};
