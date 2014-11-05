firmware
========

This is the control firmware for the spark core device(s).  You'll need to use the spark web IDE (http://spark.io) to flash each of your devices with this firmware before using the node server.

This file provides a simple POST route to control the neopixels attached to the device.  You'll need to set the number of pixels and the neopixel chip type (ex: WS2812) at the top of the firmware file.  

The node app communicates with the spark core via the spark API.  The API calls are a POST of the form 
```	
	https://api.spark.io/v1/devices/<deviceId>/run
	formData:  { access_token: '<accessToken>',
	                   params: '<action>,param1,param2,...,paramN' }
```

This request is handled by the run() function in the firmware.  The Params are parsed and the given action is executed.

The current list of actions is in the #define block near the top of spark-neopixel-control.ino.  Different actions take different parameters (or none at all).  You can find the parameter list in the if/then statement within run() for the action you want to use.  You can add additional actions by adding code to the run() and/or loop() functions.  

Here are some example actions:

```
description: blocks of color alternating on the strip
action: "blocks"
params: 
	r1: the first block red value (0-255)
	g1: the first block green value 
	b1: the first block blue value
	r2: the second block red value (0-255)
	g2: the second block green value 
	b2: the second block blue value
	blockSize: the size of each block (in pixels)
POST: https://api.spark.io/v1/devices/<deviceId>/run
		formData:  { access_token: '<accessToken>',
  					       params: 'blocks,255,0,0,0,0,255,5'}
NOTES: this POST creates from 5 pixel red and blue alternating blocks


description: blocks of color alternating on the strip
action: "fadeColor"
params: 
	r1: the first color red value (0-255)
	g1: the first color green value 
	b1: the first color blue value
	r2: the second color red value (0-255)
	g2: the second color green value 
	b2: the second color blue value
	delay: The length of time (in ms) to update each color step
	duration: The total length of time (in ms) it takes to fade from color1 to color2.
POST: https://api.spark.io/v1/devices/<deviceId>/run
		formData:  { access_token: '<accessToken>',
  					       params: 'fadeColor,255,0,0,0,0,255,50,5000'}
NOTES: this POST fades from red to blue, stepping the color every 50ms with a total duration of 5 seconds
```
