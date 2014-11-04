firmware
========

This is the control firmware for the spark core device(s).  You'll need to use the spark web IDE to flash each of your devices with this firmware before using the node server.

This file provides a simple POST route to control the neopixels attached to the device.  You'll need to set the number of pixels and the neopixel chip type (ex: WS2812) at the top of the firmware file.  