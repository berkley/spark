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

var WebSocketServer = require('ws').Server
  , wss = new WebSocketServer({ port: 3001 });
console.log("WebSocketServer created on port 3001");

wss.on('connection', function connection(ws) {
  console.log("connection made");
  ws.on('message', function incoming(message) {
    console.log('received: %s', message);
  });

 //  ws.send('{pixel:0, r:255, g:255, b:255}', function ack(error){
	// if(error) {
	// 	console.log("error sending something " + error);
	// }
 //  });

  setInterval(function() {
  		var data = pixel + ',' + parseInt(Math.random() * 255) + ',' + 
			              parseInt(Math.random() * 255) + ',' + 
			              parseInt(Math.random() * 255);
		console.log("sending ws data ", data);
		pixel++;
		if(pixel > 4)
			pixel = 0;
		ws.send(data, function ack(error){
			if(error) {
				console.log("error sending something else " + error);
			}
		});	
	}, 50);

});
