rm firmware*
particle compile spark-ws-pixels.ino neopixel.h neopixel.cpp Spark-Websockets.cpp Spark-Websockets.h 
particle flash --usb firmware*.bin


