var async = require('async');
var util = require('./util.js');
var config;

var sockets = {};
var listeners = []; //array of listener functions
var oneTimeListeners = []; //array of listeners that callback once then die

exports.setConfig = function(conf) {
  console.log("sockets setting config");
  config = conf;
  util.setConfig(conf);
};

exports.registerListener = function(listener) {
  listeners.push(listener);
};

exports.registerOneTimeListener = function(listener) {
  oneTimeListeners.push(listener);
};

exports.dropListener = function(listener) {
  for(var i=0; i<listeners.length; i++) {
    if(listeners[i] === l)
      listeners.splice(i, 1);
  }
};

exports.dropOneTimeListener = function(listener) {
  for(var i=0; i<oneTimeListeners.length; i++) {
    if(listeners[i] === listener)
      oneTimeListeners.splice(i, 1);
  }
};

exports.send = function(cores, data, callback) {
  async.each(cores, function(coreName, cb){
    var coreId = util.getCoreIdForName(coreName);
    var socket = sockets[coreId];
    console.log("sending data to core ", coreName);
    if(socket)
    {
      exports.registerOneTimeListener(function(type, message, done){
        var parsedMessage = JSON.parse(message);
        console.log("message recieved: ", message, " from core ", parsedMessage.coreid);
        if(coreId == parsedMessage.coreid && parsedMessage.message !== "ident"){
          console.log("transaction done for core ", coreId);
          done(true);  
          cb();
        }
        else {
          // ignore this message
          done(false);
        }

      });
      socket.send(data, function(err){
        console.log("transaction start: data sent to core ", coreId);
      });
    }
    else
    {
      cb(null);
    }
  }, 
  function(err) {
    callback(err);  
  });  
  
};

var WebSocketServer = require('ws').Server
  , wss = new WebSocketServer({ port: 3001 });
console.log("WebSocketServer created on port 3001");

wss.on('connection', function connection(ws) {
  ws.on('message', function(message) {
    console.log("message from WS: ", message);
    var coreid = JSON.parse(message).coreid;
    if(!coreid)
    { //normal message
      notifyListeners("message", message, function(err){
        return;  
      });
    }
    //else // connection message

    console.log("Connection from core ", coreid);

    sockets[coreid] = ws;
    console.log("connections: ", Object.keys(sockets));
    
    notifyListeners("connection", message, function(err){
    });
  });

  ws.send("-99,0,0,0");
});

function notifyListeners(type, message, done) {
  async.each(listeners, function(listener, callback){
    if(listener)
      listener(type, message, function(err){
        callback(err);
      });
  }, function(err) {
    console.log("Done notifying listeners.");
    async.each(oneTimeListeners, function(listener, callback){
      listener(type, message, function(xactionComplete) {
        if (xactionComplete) {
          exports.dropOneTimeListener(listener);
        }
        callback(null);
      });
    },function(err){
      done(err);
    });
  });

  
};
