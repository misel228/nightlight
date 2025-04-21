upload: compile
	arduino-cli upload nightlight.ino -p /dev/ttyUSB0 -b arduino:avr:nano:cpu=atmega328old

compile:
	arduino-cli compile nightlight.ino -b arduino:avr:nano

libs:
	arduino-cli lib install FastLED

