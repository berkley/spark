rm firmware*
spark compile *.ino
spark flash --usb firmware*.bin
