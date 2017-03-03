HTTP Neopixel and solenoid/relay control

This firmware is meant for a Particle Photon (https://store.particle.io/collections/photon) MCU.  It has two basic functions: 1) It will control 1 (eventually 3) strip of Neopixels of an arbitrary length via pin D2 (future strips will be on D0 and D1).  2) It will control up to 6 on/off devices (relays, solenoids, etc) via the A0-A5 pins.  

The state of the strip(s) and pins is retained via the EEPROM if power to the MCU is lost or if the internet connection resets.

It can be used with the node web-based controller here: https://github.com/berkley/spark/tree/master/node
