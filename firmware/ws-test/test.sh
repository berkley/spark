rm firmware*
spark compile ws-test.ino Spark-Websockets.cpp Spark-Websockets.h 
spark flash --usb firmware*.bin


