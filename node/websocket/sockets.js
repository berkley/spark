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
  console.log("connection made...");

  ws.on('message', function(message) {
    var coreid = JSON.parse(message).coreid;
    console.log("message from core ", coreid);
    sockets[coreid] = ws;
    console.log("connections: ", Object.keys(sockets));
  });

  ws.send("-99,0,0,0");
});

var count = 0;
var on = true;

setInterval(function() {	
 // console.log("connections: ", Object.keys(sockets));
  screen.setConfig(config);
  if(on)
	 screen.setVPixel(sockets, count, 255, 0, 0);
  else
    screen.setVPixel(sockets, count, 0, 255, 0);
	count++;
	if(count == 10)
  {
		count = 0;
    on = !on;
  }
}, 100);