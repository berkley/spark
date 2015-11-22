var express = require('express');
var lights = require('./routes/lights');
var water = require('./routes/water');
var index = require('./routes/index');
var freddy = require('./routes/freddy');
var camp = require('./routes/camp'); //control the camp fire puffers
var security = require('./routes/security');
var http = require('http');
var path = require('path');
var nconf = require('nconf');
var chickens = require('./routes/chickens');
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
chickens.setConfig(nconf);

app.set('port', process.env.PORT || nconf.get("port") || 3000);
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
app.get('/chickens/run/action', chickens.action);
app.get('/chickens', chickens.index);
app.get('/freddy', freddy.index);
app.get('/freddy/run/action', freddy.action);
app.get('/camp', camp.index);

var server = http.createServer(app).listen(app.get('port'), function(){
  console.log('Express server listening on port ' + app.get('port'));
});

