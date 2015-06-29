var express = require('express');
var lights = require('./routes/lights');
var water = require('./routes/water');
var index = require('./routes/index');
var security = require('./routes/security');
var http = require('http');
var path = require('path');
var nconf = require('nconf');
var fs = require('fs');
var sockets = require('./websocket/sockets');
sockets.setConfig(nconf);

var app = express();

nconf.argv()
     .env()
     .file({file: 'config.json'});

lights.setConfig(nconf);
water.setConfig(nconf);
security.setConfig(nconf);

app.set('port', nconf.get("port") || process.env.PORT || 3000);
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
app.use(express.favicon());
app.use(express.logger('dev'));
app.use(express.json());
app.use(express.urlencoded());
app.use(express.methodOverride());
app.use(app.router);
app.use(express.static(path.join(__dirname, 'public')));

app.get('/', index.index);
app.get('/lights', lights.index);
app.get('/water', water.index);
app.get('/security', security.index);
app.get('/house/run/action', lights.action);
app.get('/house/params/:coreId', lights.params); //get the current param state for the given coreId;
app.get('/water/run/action', water.action);

var server = http.createServer(app).listen(app.get('port'), function(){
  console.log('Express server listening on port ' + app.get('port'));
});

