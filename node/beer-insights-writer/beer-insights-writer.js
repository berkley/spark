var spark = require('spark');
var Insights = require('node-insights');
var insights = new Insights({
  appId: "999999998",
  insertKey: "tMcXZLm2hNZ4cCaWpG_DHzxEIadMqQoF",
  queryKey: '<YOUR_QUERY_KEY>',
  accountId: '565031'
});

var EventSource = require('eventsource');
var esInitDict = {rejectUnauthorized: false};

var deviceID = process.env.deviceID;	// string, your device ID
var accessToken = process.env.accessToken; // string, your access token

// Spark URL - must use es.addEventListener and specify the event name
var url = "https://api.particle.io/v1/devices/3b0021000447343337373739/events/?access_token=70922c71ad426b276056078ade75cc156fab9c81";
var es = new EventSource(url);

es.addEventListener('start-flow-T0', function(e){
	console.log( 'listener0: ', JSON.parse(e.data) );
	handleData("T0", e.data);
}, false);

es.addEventListener('start-flow-T1', function(e){
	console.log( 'listener1: ', JSON.parse(e.data) );
	handleData("T1", e.data);
}, false);

es.addEventListener('start-flow-T2', function(e){
	console.log( 'listener2: ', JSON.parse(e.data) );
	handleData("T2", e.data);
}, false);

es.onerror = function(err){
  console.log('ES Error: ', err);
};

handleData = function(name, data)
{
	data = JSON.parse(data);
	console.log("data: ", data);
	var str = data.data;
	console.log("str: ", str);
	arr = str.split(',');
	var obj = {"tap":name, "max":arr[0], "count":arr[1], "percent":arr[2]};
	console.log("obj: ", obj);
	insights.add(obj, "FSBeer");
}