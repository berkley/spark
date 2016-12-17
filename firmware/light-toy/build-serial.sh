rm *firmware*
particle compile photon *.ino *.cpp *.h
particle flash --usb *firmware*.bin
