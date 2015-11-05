This firmware runs on a particle photon connected to neopixel LED strips and pulsing flow meters
to generate metrics on beer (or other liquid) usage and display it visually.  This
system is tended for a 3 tap beer system but could be expanded.

Fun volumetric facts about beer kegs:
  * 1/2 barrel = 15.5 gallons = 124 pints = 165 12oz bottles - (Full Size Keg)
  * 50 Liter = 13.2 gallons = 105 pints = 140 12oz bottles
  * 1/4 barrel (30 Ltr) = 7.75-8.1 gallons = 62-66 pints = 83-88 12oz bottles (Pony Keg)
  * 1/6 barrel (20 Ltr) = 5.2 gallons = 41 pints = 55 12oz bottles (Sixtel)

Note: At FlightStats, we use 6th barrels!  
      Approx 16 pulses per pint = 656 pulses per 6th barrel

Published Events:
Example: https://api.particle.io/v1/devices/3b0021000447343337373739/events?access_token=XXX
  * stop-flow-TX where X is the tap number
  * data is maxPulseCount,pulseCount,percentFull

Published Variables:
Full list: https://api.particle.io/v1/devices/3b0021000447343337373739?access_token=XXX
Example: https://api.particle.io/v1/devices/3b0021000447343337373739/pulseCntT0?access_token=XXX
  * maxPCntTX - the calibrated maximum pulse count
  * instPCntTX - the instantateous pulse count (if a tap is flowing)
  * pulseCntTX - the current pulse count from the sensor (maxPCnt - usage)
  * perFullTX - the percent full (pulseCnt / maxPCnt)

Published Functions:
  * post
    curl https://api.particle.io/v1/devices/3b0021000447343337373739/post \
     -d access_token=XXX \
     -d "args=<command>,<arg0>,<arg1>...<argN>" 
  * commands are: 
    * reset(int tapNum) 
      curl https://api.particle.io/v1/devices/3b0021000447343337373739/post -d access_token=XXX -d "args=reset,2"
    * setCalibration(int tapNum, int calibrationValue)
      curl https://api.particle.io/v1/devices/3b0021000447343337373739/post -d access_token=XXX -d "args=setCalibration,2,2400"

  Note: Don't call reset unless you really want to reset the pulseCount.  There is no way to undo this!