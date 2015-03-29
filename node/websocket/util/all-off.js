var sockets = require('../sockets.js');
var screen = require('../screen.js');
var nconf = require('nconf');
var util = require('../util.js');

nconf.argv()
     .env()
     .file({file: '../config.json'});

sockets.setConfig(nconf);
screen.setConfig(nconf);

var screens = ["Freddy", "Robot"];

function setup(message) {
  console.log("message in setupInvader: ", message);
  var coreid = JSON.parse(message).coreid
    screen.setup(coreid, function(){
      draw();
    });
};

function draw() {
  screen.allOff(screens, function(){
    console.log("All screens off.");
    process.exit(0);
  });
};

function main() {
  sockets.registerListener(function(message){
    console.log("setting up with message ", message);
    setup(message);
  });
};

main();