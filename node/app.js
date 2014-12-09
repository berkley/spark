var express = require('express');
var spark = require('./routes/spark');
var http = require('http');
var path = require('path');
var nconf = require('nconf');
var fs = require('fs');

var app = express();

nconf.argv()
     .env()
     .file({file: 'config.json'});

spark.setConfig(nconf);

app.set('port', nconf.get("port") || process.env.PORT || 3000);
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
app.use(express.favicon());
app.use(express.logger('dev'));
app.use(express.json());
app.use(express.urlencoded());
app.use(express.methodOverride());
app.use(app.router);
app.use(express.static(path.join(__dirname, 'public')));

app.get('/', spark.index);
app.get('/house/run/action', spark.action);
app.get('/house/params/:coreId', spark.params); //get the current param state for the given coreId;

var server = http.createServer(app).listen(app.get('port'), function(){
  console.log('Express server listening on port ' + app.get('port'));
});

//websockets

var pixel = 0;
var sockets = {};

var WebSocketServer = require('ws').Server
  , wss = new WebSocketServer({ port: 3001 });
console.log("WebSocketServer created on port 3001");

wss.on('connection', function connection(ws) {
  console.log("connection made");

  ws.on('message', function incoming(message) {
    console.log('message received: ', message);
    var coreid = JSON.parse(message).coreid;
    sockets[coreid] = ws;
  });

  ws.send("-99,0,0,0");
});

setInterval(function() {
	console.log("trying ", Object.keys(sockets));
	if(Object.keys(sockets).length <2)
		return;

	for(var i=0; i<10; i++)
	{
		if(i < 5)
		{
			var key = Object.keys(sockets)[0];
			console.log("key: ", key);
			var ws = sockets[key];
			// console.log("ws1: ", ws);
			var data = i + ',' + parseInt(Math.random() * 255) + ',' + 
				              		 parseInt(Math.random() * 0) + ',' + 
				              		 parseInt(Math.random() * 0);
			console.log("1 sending ws data ", data);
			
			ws.send(data, function ack(error){
				if(error) {
					console.log("error sending something else " + error);
				}
			});
		}
		else if(i > 4)
		{
			var key = Object.keys(sockets)[1];
			console.log("key2: ", key);
			var ws = sockets[key];
			// console.log("ws2: ", ws);
			var data = (9 - i) + ',' + parseInt(Math.random() * 0) + ',' + 
				              	   	    parseInt(Math.random() * 0) + ',' + 
				              		    parseInt(Math.random() * 255);
			console.log("2 sending ws data ", data);
			
			ws.send(data, function ack(error){
				if(error) {
					console.log("error sending something else " + error);
				}
			});
		}
	}
	// var ws = sockets["54ff6c066672524820571167"];
	// if(!ws)
	// 	return;

	// var data = pixel + ',' + parseInt(Math.random() * 255) + ',' + 
	// 	              		 parseInt(Math.random() * 255) + ',' + 
	// 	              		 parseInt(Math.random() * 255);
	// console.log("sending ws data ", data);
	// pixel++;
	// if(pixel > 4)
	// 	pixel = 0;
	// ws.send(data, function ack(error){
	// 	if(error) {
	// 		console.log("error sending something else " + error);
	// 	}
	// });	
}, 1000);
