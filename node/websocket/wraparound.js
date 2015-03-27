var sockets = require('./sockets.js');
var screen = require('./screen.js');
var nconf = require('nconf');
var util = require('./util.js');

nconf.argv()
     .env()
     .file({file: '../config.json'});

sockets.setConfig(nconf);
screen.setConfig(nconf);

var pac0 = [255,255,255, 255,255,255, 255,255,255, 255,255,255, 77,77,13, 68,68,13, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 75,75,0, 222,222,0, 255,255,0, 255,255,0, 63,63,0, 255,255,255, 255,255,255, 255,255,255, 255,255,0, 255,255,0, 255,255,0, 124,124,0, 255,255,255, 255,255,255, 255,255,255, 127,127,0, 255,255,0, 252,252,0, 102,102,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 146,146,0, 255,255,0, 251,251,0, 62,62,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 42,42,0, 255,255,0, 255,255,0, 254,254,0, 76,76,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 113,113,0, 255,255,0, 255,255,0, 255,255,0, 64,64,0, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 59,59,13, 134,134,13, 130,130,13, 255,255,255, 255,255,255];
var pac0_width = 8;
var pac0_height = 8;

var count = 0;
var on = true;
var seq = 0;
var col = 0;
var forward = true;
var drawcount = 0;

var drawing = false;

var screens = ["Freddy", "Robot"];
var brightness = 32;

function setup(message) {
  console.log("message in setup: ", message);
  var coreid = JSON.parse(message).coreid
  	screen.setup(coreid, function(){
      console.log("done with setup, setting brightness");
      screen.setBrightness([util.getNameForCoreId(coreid)], brightness, function(bright, err){
        console.log("Screen birghtness set to ", bright);
      });
      if(!drawing)
      {
        console.log("BEGIN DRAW");
        draw();   
        drawing = true;
      }
    });
};

function draw() {
  drawPac();
  drawcount++;
}

function drawPac() {
  console.log("drawing pac at ", col);
  
  col++;
  if(col > 31)
    col = 0; 

  screen.drawBMP(screens, col, true, 0, function(){
      draw();
  }, 100);
};

function main() {
  screen.addBitmap(pac0, pac0_width, pac0_height, 0);

	sockets.registerListener(function(message){
    console.log("setting up pacman with message ", message);
    setup(message);
  });
};

main();