var WebSocket = require('ws')
  , ws = new WebSocket('ws://localhost:3001');
console.log("WebSocketServer created on port 3001");

var serial = 0;

var listeners = [];
//array of function(message)

function registerListener(listener) {
	listeners.push(listener);
};

function notifyListeners(message) {
	for(var i=0; i<listeners.length; i++)
	{
		var listener = listeners[i];
		listener(message); //send the listener the message
		listeners.splice(i, 1); //remove the listener once called.
	}
}

ws.on('open', function() {
	console.log("ws opened on client");
	go();
});

ws.on('close', function(){
	console.log("connection closed");
});

ws.on('message', function(message) {
    console.log("message received on client ", message);
    notifyListeners(message);
});

function transact(message, callback) {
	var transactionId = serial++;
	console.log("begin transacting with message ", message, " and id: ", transactionId);
	var envelope = {"id":transactionId, "message":message};
	registerListener(function(message){
		var jsonMsg = JSON.parse(message);
		if(jsonMsg.id == transactionId)
		{
			console.log("transaction complete for id ", transactionId);
			callback();
		}
	});
	ws.send(JSON.stringify(envelope), function(err){
		//message sent
	});
};

function go() {
	console.log("go...");
	transact("hi", function(){
		//when the transaction is complete, send another message
		go();
	});
};

process.on('SIGINT', function(code) {
	console.log("app exiting");
	ws.close();
});