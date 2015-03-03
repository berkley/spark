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
    setTimeout(drawInvader, 1000);
  });

  ws.send("-99,0,0,0");
});

var count = 0;
var on = true;
var invader = [0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 254,254,254, 254,254,254, 255,255,255, 255,255,255, 0,0,0, 255,255,255, 255,255,255, 0,255,0, 255,255,255, 255,255,255, 0,255,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 15,15,15, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 15,15,15, 0,0,0, 255,255,255, 0,0,0, 0,0,0];
var blank = [0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 15,15,15, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 15,15,15, 0,0,0, 0,0,0, 0,0,0, 0,0,0];
var col = 0;

function drawInvader() {
  screen.setConfig(config);
    // screen.setVScreen(sockets, "Freddy", 0, 0, 0, function(){
      console.log("all pix off")
      setTimeout(function(){
        screen.setVBMP(sockets, col, invader, function(){
          console.log("bmp on")
          col++;
          if(col > 31)
            col = 0;  
          console.log("col: ", col);
          setTimeout(drawInvader, 500);
        });
      }, 0);
    // });
};

// setInterval(function() {	
 // console.log("connections: ", Object.keys(sockets));
  // screen.setConfig(config);
  // // screen.setVScreen(sockets, "Freddy", 0, 0, 0);
  // screen.setVBMP(sockets, col, invader, function(){

  // });
  // col++;
  // if(col > 31)
  //   col = 0;
  // if(on)
  // {
  //   screen.setVScreen(sockets, "Robot", 255, 0, 0);
  //   screen.setVScreen(sockets, "Freddy", 0, 0, 0);
  // }
  // else
  // {
  //   screen.setVScreen(sockets, "Freddy", 0, 255, 0);
  //   screen.setVScreen(sockets, "Robot", 0, 0, 0);
  // }
  
  // on = !on;
// }, 500);