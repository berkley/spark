rm *firmware*
particle compile particle *.ino
particle flash 1c002a000b47343432313031 *firmware*.bin
