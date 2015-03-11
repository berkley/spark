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

// var screens = ["Freddy", "Robot"];
var screens = ["Freddy"];

function setupInvader() {
  	screen.addBitmap(screens, invader2_1, invader2_1_width, invader2_1_height, 0, function(){
  		console.log("done with bmp1");
      screen.addBitmap(screens, invader2_2, invader2_2_width, invader2_2_height, 1, function(){
        console.log("done with bmp2");
        drawInvader();   
      });
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
      setTimeout(drawInvader, 500);
    });
  }
  else
  {
    screen.drawBMP(screens, col, 1, function(){
      setTimeout(drawInvader, 500);
    });
  }
};

// process.on('SIGINT', function(code) {
//   console.log("app exiting");
//   sockets.closeAll();
// });

function main() {
	sockets.registerListener(function(message){
    setupInvader();
  });
};

main();