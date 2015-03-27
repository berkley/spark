var async = require('async');
var util = require('./util.js');
var config;

var sockets = {};
var listeners = []; //array of function(message)

exports.setConfig = function(conf) {
  console.log("sockets setting config");
  config = conf;
  util.setConfig(conf);
};

exports.registerListener = function(listener) {
  listeners.push(listener);
};

var notifyListeners = function(message) {
  for(var i=0; i<listeners.length; i++)
  {
    var listener = listeners[i];
    listener(message); 
  }
}

function appendTransactionId(data, transactionId) {
  //add the transactionId as the 2nd value in the CSV
  var firstCommaIndex = data.indexOf(",");
  var begin = data.substring(0, firstCommaIndex);
  var end = data.substring(firstCommaIndex, data.length);
  var backTogether = begin + "," + transactionId + end;
  return backTogether;
};

exports.send = function(cores, data, callback, sendDelay) {
  sendDelay = sendDelay || config.get("sendDelay");
  console.log("sending with delay ", sendDelay);
  async.each(cores, function(coreName, cb){
    var coreId = util.getCoreIdForName(coreName);
    var socket = sockets[coreId];
    console.log("sending data to core ", coreName);
    if(socket)
    {
      socket.send(data, function(err){
        cb(null);
      });
    }
    else
    {
      cb("socket for core " + coreName + " is null");
    }
  }, 
  function(err)  {
    console.log("sendDelay: ", sendDelay);
    setTimeout(function() {
      callback(null);
    }, sendDelay); 
  });  
  
};

var WebSocketServer = require('ws').Server
  , wss = new WebSocketServer({ port: 3001 });
console.log("WebSocketServer created on port 3001");

wss.on('connection', function connection(ws) {
  ws.on('message', function(message) {
    console.log("message from WS: ", message);

    var coreid = JSON.parse(message).coreid;
    var messageBody = JSON.parse(message).message;
    var transactionId = JSON.parse(message).transactionId;
    if(messageBody == "ident")
    { 
      console.log("Connection from core ", coreid);
      sockets[coreid] = ws;  
      console.log("connections: ", Object.keys(sockets));
      console.log("notifyListeners with message: ", message);
      notifyListeners(message);

    }
  });

  console.log("sending 99 ping");
  ws.send("99,0,0,0"); //initiate comms with the core, ask for an ident
});

