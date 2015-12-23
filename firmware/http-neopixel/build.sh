rm *firmware*
particle compile photon *.ino *.cpp *.h
particle flash 1c002a000b47343432313031 *firmware*.bin
