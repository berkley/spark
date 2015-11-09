var Insights = require('node-insights');
var insights = new Insights({
  appId: "999999998",
  insertKey: "XXXX",
  queryKey: '<YOUR_QUERY_KEY>',
  accountId: 'XXXX'
});

var EventSource = require('eventsource');
var esInitDict = {rejectUnauthorized: false};

var url = "https://api.particle.io/v1/devices/3b0021000447343337373739/events/?access_token=70922c71ad426b276056078ade75cc156fab9c81";
var es = new EventSource(url);

es.addEventListener('stop-flow-T0', function(e){
	console.log( 'listener0: ', JSON.parse(e.data) );
	handleData("T0", e.data);
}, false);

es.addEventListener('stop-flow-T1', function(e){
	console.log( 'listener1: ', JSON.parse(e.data) );
	handleData("T1", e.data);
}, false);

es.addEventListener('stop-flow-T2', function(e){
	console.log( 'listener2: ', JSON.parse(e.data) );
	handleData("T2", e.data);
}, false);

es.onerror = function(err){
  console.log('ES Error: ', err);
};

handleData = function(name, data)
{
	data = JSON.parse(data);
	console.log("data", data);
	var str = data.data;
	arr = str.split(',');
	var obj = {"tap":name, "max":parseInt(arr[0]), "count":parseInt(arr[1]), 
	           "percent":parseInt(arr[2]), "consumedCount":parseInt(arr[3])};
	console.log("obj: ", obj);
	insights.add(obj, "FSBeer");
}
