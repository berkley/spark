var http = require('http');
var request = require('request');

var wuQuery = "http://api.wunderground.com/api/3f11e33db017b48f/conditions/q/OR/97215.json"
var heatOn = "http://home.blinkyhouse.com:3000/chickens/run/action?action=on&coreName=Chickens&valve=2"
var heatOff = "http://home.blinkyhouse.com:3000/chickens/run/action?action=off&coreName=Chickens&valve=2"
var heatThreshold = 40; //temp in F when the light comes on

request(wuQuery, function (error, response, body) {
  if (!error && response.statusCode == 200) {
    // console.log(body) // Show the HTML for the Google homepage.
    var wx = JSON.parse(body);
    var temp = wx.current_observation.temp_f;
    console.log("temp: ", temp);
    if(temp > heatThreshold) {
    	console.log("Turning heat lamp OFF");
    	//turn the heat lamp off
    	request.get(heatOff).on('response', function(response) {
    		if(response.statusCode == 200) {
    			console.log("Heat lamp turned off.");
    		} else {
    			console.log("Heat lamp FAILED to turn off.");
    		}
		});
    } else {
    	console.log("Turning heat lamp ON");
    	//turn the heat lamp on
    	request.get(heatOn).on('response', function(response) {
    		if(response.statusCode == 200) {
    			console.log("Heat lamp turned on.");
    		} else {
    			console.log("Heat lamp FAILED to turn on.");
    		}
		});

    }
  } else {
  	console.log("Error getting weather: ", error);
  }
});
