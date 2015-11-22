rm *firmware*
particle compile core *.ino
particle flash 54ff6b066678574927170667 *firmware*.bin
