# nightlight
A [RaspIO nightlight](https://github.com/raspitv/nl) based sensor/light.

It reads numerical values over serial to change colors.

usage:

```
python brightness.py --port /dev/ttyUSB0 --baudrate 9600 --output /sys/class/backlight/10-0045/brightness
```
