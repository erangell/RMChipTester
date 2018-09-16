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
M,22,D0,O,24,D1,O,26,D2,I
P,D0,0,D1,0
```

* M = pin mappings - sequences of 3 parameters
  * pin number
  * pin name to be used in further statements
  * I or O (input or output pin mode)
In this example, the Arduino will send two digital outputs from pins 22 and 24 to a logic gate and get a digital input from pin 26 which will be connected to the output of the gate.

* P = pin settings to be done before applying power to your breadboard
  *  In this example the D0 and D1 outputs (pins 22 and 24) are set to LOW

Test files will typically consist of sequences of the following instructions:

```
T,NOR00,D0,0,D1,0
D,500
E,D2,1
```

* T = define a named test and the input conditions to be set
* D = delay for a number of microseconds
* C = count pulses on specific pins during a specific delay
* E = expected results to be verified.  In this case, the D2 pin will be checked for a logical high value.  If any condition on the expected results line fails, the test is considered failed.

## Counting Pulses
to be continued...

