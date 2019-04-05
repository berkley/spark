rm *firmware*
particle compile photon *.ino 
particle flash $1 *firmware*.bin
