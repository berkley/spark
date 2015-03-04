var screen = require('./screen.js');
var config;

var invader2_1 = [0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 255,255,255, 255,255,255, 255,0,0, 255,255,255, 255,255,255, 255,0,0, 255,255,255, 255,255,255,255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 255,255,255, 255,255,255, 0,0,0, 255,255,255, 0,0,0, 255,255,255, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 255,255,255];
var invader2_1_width = 8;
var invader2_1_height = 8;

var invader2_2 = [0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 254,254,254, 254,254,254, 255,255,255, 255,255,255, 0,0,0, 255,255,255, 255,255,255, 0,255,0, 255,255,255, 255,255,255, 0,255,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 15,15,15, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 15,15,15, 0,0,0, 255,255,255, 0,0,0, 0,0,0];
var invader2_2_width = 8;
var invader2_2_height = 8;

var blank = [0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 15,15,15, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 15,15,15, 0,0,0, 0,0,0, 0,0,0, 0,0,0];

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
    setTimeout(setupInvader, 1000);
  });

  ws.send("-99,0,0,0");
});

var count = 0;
var on = true;
var col = 0;

function setupInvader() {
  screen.addBitmap(sockets, invader2_1, invader2_1_width, invader2_1_height, 0, function(){
      screen.addBitmap(sockets, invader2_2, invader2_2_width, invader2_2_height, 1, function(){
        console.log("Done adding bitmaps");
        drawInvader();   
      });
    });
};

function drawInvader() {
  screen.setConfig(config);
  if(on)
  {
    screen.drawBMP(sockets, col, 0, function(){
      col++;
      if(col > 31)
        col = 0;  
      on = !on;
      setTimeout(drawInvader, 250);
    });
  }
  else
  {
    screen.drawBMP(sockets, col, 1, function(){
      col++;
      if(col > 31)
        col = 0;  
      on = !on;
      setTimeout(drawInvader, 250);
    });
  }
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