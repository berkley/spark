#!/bin/bash
rm *.bin
/usr/local/bin/particle compile photon *.ino *.cpp *.h
/usr/local/bin/particle flash 40002f000d47343233323032 *.bin
echo "build done"