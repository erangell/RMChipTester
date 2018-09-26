# Arduino Timing Test
An attempt to determine whether the Arduino can accurately measure pulses
of a small number of microseconds.  Written for the Arduino MEGA.

# StateChangeDetection.ino
This sketch is a modification of a sample sketch which measures the width of
input pulses on Pin 2.  It captures the micros() value at the following transitions:
LOW to HIGH = start of pulse
HIGH to LOW = end of pulse

A tight loop collects 100 samples of the micros() values in an array.
When all samples are collected, the pulse widths are calculated and written to a file.
The file is stored on an SD card in a Data Logger Shield.

# ArduinoTimingTest.dsk
An AppleSoft basic program pokes the following machine language code at $6000

```
LDA $C059
NOP
LDA $C058
RTS
```

This code turns on Annunciator 0, waits 2 cycles, then turns it off.

The Applesoft program then modifies the code to insert one extra NOP instruction
and runs the test 110 times (to ensure that the data logging code of the Arduino
gets hit).

To execute the test, the Arduino with Data Logger Shield and SD card is hooked
up to the Apple Internal Game I/O socket as follows:

Apple
Pin 1 --> +5V
Pin 8 --> GND
Pin 15 --> Pin 2

Test results from an Apple 2+ are in PULSTEST.TXT

It appears that there is a limit on the resolution and accuracy that can be 
measured from the Arduino C language.  
