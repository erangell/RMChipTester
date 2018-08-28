# RMChipTester
Arudino sketch for testing chips using CSV files of tests to run.
Commissioned by ReActive Micro, published under MIT License.

# Introduction
There is a need to test the functionality of batches logic chips to determine if any have errors.
This sketch allows creation of test harness files on an SD card that can be used by an Arduino to execute tests.

# Test file format
Each test file consists of lines that start with a letter, then contain parameters separated by commas.  Lines are interpreted in order of the file.

Example:
```
M,22,24,26,28,30
N,VCC,D0,D1,D2,GND
O,O,I,I,O,O
P,VCC,1,GND,0
```

M = pin mappings - the pin numbers on the Arduino that will be used for the tests.
N = pin names - names for each of the pins defined by the 'M command
O = output or input - whether each pin is to be configured as an output or input
P = pin settings - values to be set for specific pins.  In this example the VCC pin is set to logic 1 and the GND pin is set to logic 0.

Test files will typically consist of sequences of the following instructions:

```
T,NOR00,D0,0,D1,0
D,500
E,D2,1
```

T = define a named test and the input conditions to be set
D = delay for a number of microseconds
E = expected results to be verified.  In this case, the D2 pin will be checked for a logical high value.  If any condition on the expected results line fails, the test is considered failed.


