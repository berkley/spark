rm *firmware*
particle compile photon *.ino *.cpp *.h
particle flash $1 *firmware*.bin
