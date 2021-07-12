@echo off
title code_burnATMega16
color 07
echo !!Welcome to usbasp programmer!!
echo Please enter the hex file path
avrdude -c usbasp -p m16 -u -U flash:w:Blink.ino.hex
pause