var request = require('request');

var sparkUrl = "https://api.spark.io/v1/devices";

exports.index = function(req, res) {
	res.redirect("control.html");
};

var access_token = "";

exports.action = function(req, res) {
	var action = req.query.action;
	var coreId = req.query.coreId;
	console.log("action: " + action);
	console.log("coreId: " + coreId);
	if(action == "allOff")
	{
		var url = buildUrl("run", coreId);
		var data = "setAll,0,0,0";
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));
	}
	else if(action == "stop")
	{
		var url = buildUrl("run", coreId);
		var data = "shutdown,";
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));
	}
	else if(action == "setAll")
	{
		var url = buildUrl("run", coreId);
		var data = "setAll," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1;
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));
	}
	else if(action == "alternate")
	{
		var url = buildUrl("run", coreId);	
	    var data = "alternate," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2;
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));
	}
	else if(action == "animateAlternate")
	{
		var url = buildUrl("run", coreId);	
	    var data = "loopAlternate," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.delay;
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));
	}
	else if(action == "rainbow")
	{
		var url = buildUrl("run", coreId);
		var data = "rainbow,";
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));
	}
	else if(action == "loopBlocks")
	{
		var url = buildUrl("run", coreId);
		var data = "loopBlocks," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.delay + "," + req.query.blockSize;
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));
	}
	else if(action == "fadeColor")
	{
		var url = buildUrl("run", coreId);
		var data = "fadeColor," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.delay + "," + req.query.duration;
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));
	}
	else if(action == "blocks")
	{
		var url = buildUrl("run", coreId);
		var data = "blocks," + req.query.r1 + "," + req.query.g1 + "," + req.query.b1 + "," + req.query.r2 + "," + req.query.g2 + "," + req.query.b2 + "," + req.query.blockSize;
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));
	}
	else if(action == "particles")
	{
		var url = buildUrl("run", coreId);
		var data = "particles,";
		request.post(url, function(err, response, body) {
			res.send(body);
		}).form(buildFormData(data));
	}
};

var buildFormData = function(params) {
	var formData = {"access_token":access_token ,"params":params};
	console.log("formData: ", formData);
	return formData;
};

var buildUrl = function(action, coreId) {
	var url = sparkUrl + "/" + coreId + "/" + action;
	console.log("url: ", url);
	return url;
};
