#!/bin/bash
rm *.bin
/usr/local/bin/particle compile photon *.ino *.cpp *.h
/usr/local/bin/particle flash 3b0021000447343337373739 *.bin
echo "build done"