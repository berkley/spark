rm firmware*
spark compile spark-ws-pixels.ino neopixel.h neopixel.cpp Spark-Websockets.cpp Spark-Websockets.h 
spark flash --usb firmware*.bin


