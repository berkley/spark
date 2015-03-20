var sockets = require('./sockets.js');
var screen = require('./screen.js');
var nconf = require('nconf');

nconf.argv()
     .env()
     .file({file: '../config.json'});

sockets.setConfig(nconf);
screen.setConfig(nconf);


var invader2_1 = [0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 255,255,255, 255,255,255, 255,0,0, 255,255,255, 255,255,255, 255,0,0, 255,255,255, 255,255,255,255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 255,255,255, 255,255,255, 0,0,0, 255,255,255, 0,0,0, 255,255,255, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 255,255,255];
var invader2_1_width = 8;
var invader2_1_height = 8;

var invader2_2 = [0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 255,255,255, 254,254,254, 254,254,254, 255,255,255, 255,255,255, 0,0,0, 255,255,255, 255,255,255, 0,255,0, 255,255,255, 255,255,255, 0,255,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 15,15,15, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 0,0,0, 0,0,0, 0,0,0, 255,255,255, 15,15,15, 0,0,0, 255,255,255, 0,0,0, 0,0,0];
var invader2_2_width = 8;
var invader2_2_height = 8;

var count = 0;
var on = true;
var col = 0;

var drawing = false;

var screens = ["Freddy", "Robot"];
// var screens = ["Robot"];

function setupInvader(message) {
  console.log("message in setupInvader: ", message);
  	screen.setup(JSON.parse(message).coreid, function(){
      if(!drawing)
      {
        drawInvader();   
        drawing = true;
      }
    });
};

function drawInvader() {
  on = !on;
  col++;
  if(col > 31)
    col = 0;  

  if(on)
  {
    screen.drawBMP(screens, col, 0, function(){
      drawInvader();
    });
  }
  else
  {
    screen.drawBMP(screens, col, 1, function(){
      drawInvader();
    });
  }
};

function main() {
  screen.addBitmap(invader2_1, invader2_1_width, invader2_1_height, 0);
  screen.addBitmap(invader2_2, invader2_2_width, invader2_2_height, 1);

	sockets.registerListener(function(message){
    console.log("setting up invader with message ", message);
    setupInvader(message);
  });
};

main();