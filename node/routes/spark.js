var request = require('request');
var nconf = require('nconf');
var config;

exports.setConfig = function(conf) {
	config = conf;
};

exports.index = function(req, res) {
	var cores = {"cores":config.get("cores")};
	console.log(cores);
	res.render('index', cores);
};

exports.action = function(req, res) {
	var action = req.query.action;
	var coreId = req.query.coreId;

	console.log("action: " + action);
	console.log("coreId: " + coreId);

	var url = null;
	var data = null;

	if(action == "allOff")
	{
		url = buildUrl("run", coreId);
		data = "setAll,0,0,0";
	}
	else if(action == "stop")
	{
		url = buildUrl("run", coreId);
		data = "shutdown,";
	}
	else if(action == "setAll")
	{
		url = buildUrl("run", coreId);
		data = "setAll," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1;
	}
	else if(action == "alternate")
	{
		url = buildUrl("run", coreId);	
	    data = "alternate," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2;
	}
	else if(action == "animateAlternate")
	{
		url = buildUrl("run", coreId);	
	    data = "loopAlternate," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.delay;
	}
	else if(action == "rainbow")
	{
		url = buildUrl("run", coreId);
		data = "rainbow,";
	}
	else if(action == "loopBlocks")
	{
		url = buildUrl("run", coreId);
		data = "loopBlocks," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.delay + "," + req.query.blockSize;
	}
	else if(action == "fadeColor")
	{
		url = buildUrl("run", coreId);
		data = "fadeColor," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.delay + "," + req.query.duration;
	}
	else if(action == "blocks")
	{
		url = buildUrl("run", coreId);
		data = "blocks," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.blockSize;
	}
	else if(action == "particles")
	{
		url = buildUrl("run", coreId);
		data = "particles,";
	}

	request.post(url, function(err, response, body) {
		console.log("\n\npost callback:", err, body);
		res.send(body);
	}).form(buildFormData(data));

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
