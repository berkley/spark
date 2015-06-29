var request = require('request');
var nconf = require('nconf');
var util = require('./util.js');

var config;

exports.setConfig = function(conf) {
	config = conf;
	util.setConfig(config);
};

exports.index = function(req, res) {
	res.render('security', {"config":config.get()});
};

//TODO: monitor the event stream from particle and log the events