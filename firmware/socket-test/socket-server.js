var net = require('net');

var HOST = '10.0.1.8';
var PORT = 30001;

var server = net.createServer(function(c) { //'connection' listener
  console.log('client connected');
  c.on('end', function() {
    console.log('client disconnected');
  });
  c.write('hello\r\n');
  c.pipe(c); //echo data back
});

server.listen(PORT, function() { //'listening' listener
  console.log('server bound');
});


// var server = net.createServer();
// server.listen(PORT, HOST);

// console.log('Server listening on ', server);

// server.on('connection', function(sock) {
//     console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);
// });

// server.on('data', function(data) { 
//     console.log('DATA ' + server.remoteAddress + ': ' + data);
//     sock.write('You said "' + data + '"');
// });
    
// server.on('close', function(data) {
//     console.log('CLOSED: ' + server.remoteAddress +' '+ server.remotePort);
// });

// net.createServer(function(sock) {
    
//     // We have a connection - a socket object is assigned to the connection automatically
//     console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);
    
//     // Add a 'data' event handler to this instance of socket
//     sock.on('data', function(data) {
//         console.log('DATA ' + sock.remoteAddress + ': ' + data);
//         // Write the data back to the socket, the client will receive it as data from the server
//         // sock.write('You said "' + data + '"');
//     });
    
//     // Add a 'close' event handler to this instance of socket
//     sock.on('close', function(data) {
//         console.log('CLOSED: ' + sock.remoteAddress +' '+ sock.remotePort);
//     });
    
// }).listen(PORT, HOST);

// console.log('Server listening on ' + HOST +':'+ PORT);
