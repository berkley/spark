var request = require('request');
var nconf = require('nconf');
var util = require('./util.js');

var config;

exports.setConfig = function(conf) {
	config = conf;
	util.setConfig(config);
};

exports.index = function(req, res) {
	var chickens = {"chickens":config.get("chickens")};
	console.log(chickens);
	res.render('chickens', chickens);
};

exports.action = function(req, res) {
	var action = req.query.action;
	var core = util.coreForCoreName(req.query.coreName.trim());

	console.log("core: ", core);

	var data = "";
	if(action == "on")
	{
		data = "on," + req.query.valve;
	}
	else if(action == "off")
	{
		data = "off," + req.query.valve;
	}

	util.runPost(data, action, core.id, res);
};
