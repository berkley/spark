  var count = 0;

  var WebSocketServer = require('ws').Server
  , wss = new WebSocketServer({ port: 3001 });
console.log("WebSocketServer created on port 3001");

wss.on('connection', function connection(ws) {
  console.log("connection made...");

  ws.on('message', function(message) {
    console.log("message received on server ", message);
    // //do something then send an ack
    // var msgJSON = JSON.parse(message);
    // var envelope = {"id":msgJSON.id, "message":"ok"};
    // if(ws.readyState == ws.OPEN) {
    //   ws.send(JSON.stringify(envelope), function(err){
    //     console.log(envelope, " sent");
    //   });
    // }

    
  });

  ws.on('close', function(){
    console.log("close");
    ws.terminate();
  });

  setInterval(function(){
      ws.send("96,10,8,1,0");
  }, 20); 
  

});

wss.on('error', function(err){

});
