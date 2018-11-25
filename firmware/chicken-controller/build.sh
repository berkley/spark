rm *firmware*
particle compile particle *.ino
particle flash 380042000447343337373738 *firmware*.bin
