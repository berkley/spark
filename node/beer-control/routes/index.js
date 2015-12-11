var express = require('express');
var router = express.Router();
var request = require('request');

var particleUrl = 'https://api.particle.io/v1/devices';
var deviceId = '3b0021000447343337373739';
var particlePost = 'post';
var particlePulseCnt = 'pulseCntT';
var maxPCnt = 'maxPCntT';
var access_token = '70922c71ad426b276056078ade75cc156fab9c81';

/* GET home page. */
router.get('/', function(req, res) {
  res.render('index', { title: 'FSBeer' });
});

router.get('/reset/:tap', function(req, res) {
  	var tap = req.params.tap;
	request.post(
	    particleUrl + "/" + deviceId + "/" + particlePost,
	    { form: { access_token: access_token, args:'reset,' + tap} },
	    function (error, response, body) {
	        if (!error && response.statusCode == 200) {
	        	console.log("Reset on tap " + tap + " success.");
	            console.log(body)
	            res.redirect('/');
	        } else {
	        	console.log("Reset on tap " + tap + " failed.");
	        	res.status(500).end()
	        }
	    }
	);
});

router.get('/set/:tap/:value', function(req, res) {
  	var tap = req.params.tap;
  	var val = req.params.value;
	request.post(
	    particleUrl + "/" + deviceId + "/" + particlePost,
	    { form: { access_token: access_token, args:'setPulseCount,' + tap + ',' + val} },
	    function (error, response, body) {
	        if (!error && response.statusCode == 200) {
	        	console.log("SetPulseCount on tap " + tap + " success.");
	            console.log(body)
	            res.redirect('/');
	        } else {
	        	console.log("SetPulseCount on tap " + tap + " failed.");
	        	res.status(500).end()
	        }
	    }
	);
});

router.get('/status/:tap', function(req, res) {
  	var tap = req.params.tap;
  	var url = particleUrl + "/" + deviceId + "/" + particlePulseCnt + tap + "?access_token=" + access_token;
  	console.log("url: ", url);
	request
	  .get(url)
	  .on('response', function(resp) {
	    // console.log(response.statusCode) 
	    
	  }).on('data', function(data) {
	    // decompressed data as it is received 
	    var jsonD = JSON.parse(data);
	    if(jsonD.result != undefined)
	    {
	    	res.send("" + jsonD.result + "").end();
	    }
	    else
	    	res.sendStatus(500).end();
  	  })
});

router.get('/cal/:tap', function(req, res) {
  	var tap = req.params.tap;
  	var url = particleUrl + "/" + deviceId + "/" + maxPCnt + tap + "?access_token=" + access_token;
  	console.log("url: ", url);
	request
	  .get(url)
	  .on('response', function(resp) {
	    // console.log(response.statusCode) 
	    
	  }).on('data', function(data) {
	    // decompressed data as it is received 
	    var jsonD = JSON.parse(data);
	    if(jsonD.result)
	    {
	    	res.send("" + jsonD.result + "").end();
	    }
	    else
	    	res.sendStatus(500).end();
  	  })
});


module.exports = router;
