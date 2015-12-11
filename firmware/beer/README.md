![#include beer.h](https://c1.staticflickr.com/1/712/22913838471_53f38de2da.jpg "Beer Nerds")

@alexwitherspoon and I created this system to monitor and publish beer usage stats from our kegs at @FlightStats.  The kegerator has one flow meter installed per tap.  The @spark Particle Photon development board collects the stats and publishes them via the cloud API's listed below.  It also displays the current volume of the keg in LED lights on the tap handle.  Green means the keg is full and the color slowly changes to red as the volume drops over time.  The handles also animate the pixels when the beer is actively flowing.  

![LED Beer Taps](https://c1.staticflickr.com/1/575/22510564369_e563be6be2.jpg "LED tap handles showing current volume by color.")

This firmware runs on a @spark Particle Photon connected to neopixel LED strips (WS2812B) and pulsing flow meters
to generate metrics on beer (or other liquid) usage and display it visually.  This
system is tended for a 3 tap beer system but could be expanded.

Fun volumetric facts about beer kegs:
  * 1/2 barrel = 15.5 gallons = 124 pints = 165 12oz bottles - (Full Size Keg)
  * 50 Liter = 13.2 gallons = 105 pints = 140 12oz bottles
  * 1/4 barrel (30 Ltr) = 7.75-8.1 gallons = 62-66 pints = 83-88 12oz bottles (Pony Keg)
  * 1/6 barrel (20 Ltr) = 5.2 gallons = 41 pints = 55 12oz bottles (Sixtel)

Note: At @FlightStats, we use 6th barrels!  
      Approx 16 pulses per pint = 656 pulses per 6th barrel
      
![Kegerator installation](https://c2.staticflickr.com/6/5650/22876727816_e9e6af7a0b.jpg "Flow meters, kegs and Particle Photon controller.")

Cloud Variables:
Full list: https://api.particle.io/v1/devices/3b0021000447343337373739?access_token=XXX
Example: https://api.particle.io/v1/devices/3b0021000447343337373739/pulseCntT0?access_token=XXX
  * maxPCntTX - the calibrated maximum pulse count
  * instPCntTX - the instantateous pulse count (if a tap is flowing)
  * pulseCntTX - the current pulse count from the sensor (maxPCnt - usage)
  * perFullTX - the percent full (pulseCnt / maxPCnt)

Cloud Functions:
  * post
    curl https://api.particle.io/v1/devices/3b0021000447343337373739/post \
     -d access_token=XXX \
     -d "args=command,arg0,arg1...argN" 
  * commands are: 
    * reset(int tapNum) - Used to reset the pulseCount to the calibration value. Usually used when changing kegs.
      curl https://api.particle.io/v1/devices/3b0021000447343337373739/post -d access_token=XXX -d "args=reset,2"
    * setCalibration(int tapNum, int calibrationValue) - Used to manually set the calibration value
      curl https://api.particle.io/v1/devices/3b0021000447343337373739/post -d access_token=XXX -d "args=setCalibration,2,2400"
    * setPulseCount(int tapNum, int pulseCount) - Used to manually set the pulseCount
      curl https://api.particle.io/v1/devices/3b0021000447343337373739/post -d access_token=XXX -d "args=setPulseCount,2,783"

Cloud Published Events:
Example: https://api.particle.io/v1/devices/3b0021000447343337373739/events?access_token=XXX
  * stop-flow-TX where X is the tap number
    * data is maxPulseCount,pulseCount,percentFull,pulsesConsumed
  * start-flow-TX where X is the tap number
    * data is maxPulseCount,pulseCount,percentFull

Published events can be handled in node.js like this:

      var EventSource = require('eventsource');
      var esInitDict = {rejectUnauthorized: false};

      var url = "https://api.particle.io/v1/devices/3b0021000447343337373739/events/?access_token=XXX";
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
        //do something with the data here
      }
 
If you use New Relic, you can publish this data to Insights with this script: https://github.com/berkley/spark/blob/master/node/beer-insights-writer/beer-insights-writer.js

There is now a GUI to control the tap variables.   Code is here: https://github.com/berkley/spark/tree/master/node/beer-control

![Particle Photon](https://c2.staticflickr.com/6/5681/22281557383_d42163feef.jpg "Particle Photon")
![Flow meters](https://c1.staticflickr.com/1/700/22484342377_5b42a3f49a.jpg "Flow meters")

[Hi Res Photos](https://www.flickr.com/photos/offhegoes/albums/72157660955639056)

