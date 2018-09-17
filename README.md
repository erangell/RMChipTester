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
* D = delay for a number of milliseconds
* E = expected results to be verified.  In this case, the D2 pin will be checked for a logical high value.  If any condition on the expected results line fails, the test is considered failed.

Note: Multiple expected results needs to be tested - see issue #1

## Counting Pulses
For future development:
* C = count pulses on specific pins during a specific delay

# Sample Test Run in Serial Monitor:
```
Send........Send one line of text to begin
.Connecting to SD card
initialization done
/
Test Directories in root of SD card
T4001
T6522
T7404
T4011
Enter name of directory containing tests to run
T000STRT.CSV
T10NAND1.CSV
T20NAND2.CSV
T30NAND3.CSV
T40NAND4.CSV
Enter name of test file to run
Opening: /t4011/t000strt.csv
M,22,D0,O,24,D1,O,26,D2,I
M,28,D3,O,30,D4,O,32,D5,I
M,34,D6,O,36,D7,O,38,D8,I
M,40,D9,O,42,D10,O,44,D11,I
P,D0,0,D1,0,D3,0,D4,0
P,D6,0,D7,0,D9,0,D10,0
Enter name of test file to run
Opening: /t4011/t10nand1.csv
T,NAND00,D0,0,D1,0
D,500
E,D2,1
***FAIL***
T,NAND01,D0,0,D1,1
D,500
E,D2,1
***FAIL***
T,NAND10,D0,1,D0,0
D,500
E,D2,1
***FAIL***
T,NAND11,D0,1,D1,1
D,500
E,D2,0
PASS
Enter name of test file to run
Opening: /t4011/t20nand2.csv
T,NAND00,D3,0,D4,0
D,500
E,D5,1
***FAIL***
T,NAND01,D3,0,D4,1
D,500
E,D5,1
***FAIL***
T,NAND10,D3,1,D3,0
D,500
E,D5,1
***FAIL***
T,NAND11,D3,1,D4,1
D,500
E,D5,0
PASS
Enter name of test file to run
Opening: /t4011/t30nand3.csv
T,NAND00,D6,0,D7,0
D,500
E,D8,1
***FAIL***
T,NAND01,D6,0,D7,1
D,500
E,D8,1
***FAIL***
T,NAND10,D6,1,D6,0
D,500
E,D8,1
***FAIL***
T,NAND11,D6,1,D7,1
D,500
E,D8,0
PASS
Enter name of test file to run
Opening: /t4011/t40nand.csv
ERROR opening /t4011/t40nand.csv
Enter name of test file to run
Opening: /t4011/t40nand4.csv
T,NAND00,D9,0,D10,0
D,500
E,D11,1
***FAIL***
T,NAND01,D9,0,D10,1
D,500
E,D11,1
***FAIL***
T,NAND10,D9,1,D9,0
D,500
E,D11,1
***FAIL***
T,NAND11,D9,1,D10,1
D,500
E,D11,0
PASS
Enter name of test file to run
```
