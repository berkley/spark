var request = require('request');
var nconf = require('nconf');
var util = require('./util.js');

var config;

exports.setConfig = function(conf) {
	config = conf;
	util.setConfig(config);
};

exports.index = function(req, res) {
	var water = config.get("water");
	var valves = {"valves":water.valves};
	console.log(valves);
	res.render('water', valves);
};

exports.action = function(req, res) {
	var action = req.query.action;
	var coreId = req.query.coreId;
	var data = "";
	if(action == "on")
	{
		data = "on,"; //TODO - add pin numbers
	}
	else if(action == "off")
	{
		data = "off,"; //TODO - add pin numbers
	}

	util.runPost(data, action, coreId, res);
};