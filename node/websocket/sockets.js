var screen = require('./screen.js');
var config;

exports.setConfig = function(conf) {
  config = conf;
};

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

var count = 0;

setInterval(function() {	
  console.log("connections: ", Object.keys(sockets));
  screen.setConfig(config);
	screen.setVPixel(sockets, count, parseInt(Math.random() * 255), parseInt(Math.random() * 255), parseInt(Math.random() * 255));
	count++;
	if(count == 10)
  {
		count = 0;
  }
}, 100);