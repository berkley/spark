rm *firmware*
particle compile photon *.ino *.cpp *.h
particle flash 3b0031001447343432313031 *firmware*.bin
