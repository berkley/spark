var request = require('request');
var nconf = require('nconf');
var util = require('./util.js');
var net = require('net');

var config;

exports.setConfig = function(conf) {
	config = conf;
	util.setConfig(config);
};

exports.index = function(req, res) {
	// var water = {"water":config.get("water")};
	console.log("camp.index");
	res.render('camp');
};

exports.action = function(req, res) {
	var action = req.query.action;

	console.log("connecting....");

	// var HOST = '10.0.1.47';
	var HOST = '10.0.1.31';
	var PORT = 7000;

	var client = new net.Socket();
	client.connect(PORT, HOST, function() {

	    console.log('CONNECTED TO: ' + HOST + ':' + PORT);
	    // Write a message to the socket as soon as the client is connected, 
	    // the server will receive it as message from the client 
	    console.log("req.query", req.query);
	    client.write(req.query.action);

	});

	// Add a 'data' event handler for the client socket
	// data is what the server sent to this socket
	client.on('data', function(data) {
	    
	    console.log('DATA: ' + data);
	    // Close the client socket completely
	    client.destroy();
	    
	});

	// Add a 'close' event handler for the client socket
	client.on('close', function() {
	    console.log('Connection closed');
	    res.send(200);
	});
};
