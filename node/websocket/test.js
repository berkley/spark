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

function socketMessageHandler(type, message, callback) {
	console.log("on ", type, " with message ", message);
	if(type == "connection" && JSON.parse(message).message == "ident")
	{
		console.log("test heard 'connection' from ", message);
		// setTimeout(setupInvader, 1000);	

		callback(null);
	}
	else if(type == "message")
	{
		console.log("test heard 'message' ", message);
		callback(null);
	}
};

var count = 0;
var on = true;
var col = 0;

function setupInvader() {
	var screens = ["Freddy", "Robot"];
  	screen.addBitmap(screens, invader2_1, invader2_1_width, invader2_1_height, 0, function(){
      screen.addBitmap(screens, invader2_2, invader2_2_width, invader2_2_height, 1, function(){
        console.log("Done adding bitmaps");
        // drawInvader();   
      });
    });
};

function drawInvader() {
  screen.setConfig(config);
  on = !on;
  col++;
  if(col > 31)
    col = 0;  

  if(on)
  {
    screen.drawBMP(sockets, col, 0, function(){
      setTimeout(drawInvader, 250);
    });
  }
  else
  {
    screen.drawBMP(sockets, col, 1, function(){
      setTimeout(drawInvader, 250);
    });
  }
};

function main() {
	sockets.registerListener(socketMessageHandler);
};

main();