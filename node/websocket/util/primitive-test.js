var sockets = require('../sockets.js');
var screen = require('../screen.js');
var nconf = require('nconf');
var util = require('../util.js');

nconf.argv()
     .env()
     .file({file: '../config.json'});

sockets.setConfig(nconf);
screen.setConfig(nconf);

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
  if(drawcount >= 0 && drawcount < 32)
    drawCols();
  else if(drawcount >= 32 && drawcount < 40)
    drawRows();
  //TODO test setPixel here 

  drawcount++;
  if(drawcount > 39)
    drawcount = 0;
}

function drawCols() {
  var red = Math.random() * 255;
  var green = Math.random() * 255;
  var blue = Math.random() * 255;
  console.log("drawing column: ", drawcount);
  screen.setColumn(screens, drawcount, red, green, blue, false, function(){
      draw();
  }, 500);
}

function drawRows() {
  var red = Math.random() * 255;
  var green = Math.random() * 255;
  var blue = Math.random() * 255;
  console.log("drawing row ", drawcount - 31);
  screen.setRow(screens, drawcount - 31, red, green, blue, true, function(){
      draw();
  }, 500);
}

function main() {
	sockets.registerListener(function(message){
    console.log("setting up pacman with message ", message);
    setup(message);
  });
};

main();