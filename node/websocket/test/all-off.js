var WebSocket = require('ws')
  , ws = new WebSocket('ws://10.0.1.:3001');
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

function go() {
	console.log("go...");

	ws.send("-98,0,0,0" function(err){
		
	});
};
