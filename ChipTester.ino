// Reactive Micro Chip Tester
// (c) 2018, Reactive Micro.

#include <SPI.h>
#include <SD.h>

// 2018-09-20: Code modified to work with Arduino Mega, Adafruit SD library, and Data Logger Shield that doesn't have SPI bus header.

//For Arduino Mega, follow these instructions: https://learn.adafruit.com/adafruit-data-logger-shield/for-the-mega-and-leonardo
//(reverted to original SD libaray)
const int SSPIN = 53;
const int spi1 = 10;  // pins needed for SD.begin()
const int spi2 = 11;
const int spi3 = 12;
const int spi4 = 13;

int SDconnected = 0; //will get set to 1 if successful connection


String _testDirName= "";
char _testCmd = ' ';
int _pinCfgDone = 0;
int _testParmCtr = 0;
int _savePinNum = 0;
int _testNameFound = 0;
int _delayval = 1000;

//bit maps: 8 bytes of 8 bits = max 64 pin numbers for arduino mega 
byte pinMap[ 8 ] = {0,0,0,0,0,0,0,0}; // bit (pin-1) set to 1 if pin is being used

char pinName[ 192 ] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
}; //64 pins * 3 character label for each 

byte pinIO[ 8 ] = {0,0,0,0,0,0,0,0}; // bit (pin-1): 0=input 1=output


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Send one line of text to begin");
  establishContact();  // send a byte to establish contact until receiver responds

  Serial.println("Connecting to SD card");

  pinMode(SSPIN, OUTPUT); // required for MEGA
  
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);  //LED will be lit if any error occurs
    
  if (!SD.begin(spi1, spi2, spi3, spi4)) {
    Serial.println("initialization failed!  Check wiring, if card inserted, and pin to use for your SD shield");
    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(LED_BUILTIN,HIGH);
    // don't do anything more:
    while (1) ;
  }
  SDconnected = 1;
  Serial.println("initialization done");

  printDirectory();

  Serial.println ("Enter name of directory containing tests to run");  

  while (!Serial.available())
  {
    delay(100);
  }

  _testDirName = Serial.readStringUntil('\n');

/*
  for (int i=0; i<5; i++)
  {
    pinMap[i] = 0;
    pinIO[i] = 0;
    pinName[i * 3] = ' ';
    pinName[i *3 + 1] = ' ';
    pinName[i * 3 + 2] = ' ';
  }
  pinMap[5] = 0;
  pinMap[6] = 0;
  pinMap[7] = 0;
  */

  ReadTestDir(_testDirName);
}


void loop() {

  if (SDconnected == 0)
    return;
    
    Serial.println("Enter name of test file to run");
    while (!Serial.available())
    {
      delay(100);
    }

    String testFileName = Serial.readStringUntil('\n');
    ExecuteTestFile( testFileName, _testDirName );
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print(".");   // send an initial string
    delay(300);
  }

  Serial.readStringUntil('\n');
  
  //String garbage = Serial.readStringUntil('\n');
  //Serial.println("Received:"+garbage);
  //Serial.println();
}

void printDirectory() {
  File fDir = SD.open("/",FILE_READ);

  Serial.println(fDir.name());
  Serial.println("Test Directories in root of SD card");
  while (true) {

    File entry =  fDir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    if (entry.isDirectory()) {
      if (entry.name()[0]=='T')
      {
          Serial.println(entry.name());
      }
    }
    entry.close();
  }
  fDir.close();
}


void ReadTestDir(String testDirName)
{
  int testFileCount = 0;
  char testDirNameStr[255];
  for (int ix=0; ix < testDirName.length(); ix++)
  {
    testDirNameStr[ix] = testDirName[ix];
  }
  testDirNameStr[testDirName.length()]='\0';

  File fDir = SD.open(testDirNameStr, FILE_READ);
  File fFile;
  if (!fDir)
  {
    Serial.println("Directory not found");
    return;
  }
  
  while (true) {
    fFile = fDir.openNextFile();
    if (!fFile) {
      break;
    }

    String testSetFile = fFile.name();
    if (testSetFile[0] == 'T')
    {
      Serial.println(testSetFile);
      testFileCount++;
    } 
    fFile.close();
  }
  
  fDir.close();
  if (testFileCount == 0)
  {
    Serial.println("No test files found");
  }
}

void ExecuteTestFile(String testFile, String testFileDir)
{
  String csvline;
  char testFilePath[255];
  Serial.print("Opening: /");
  testFilePath[0] = '/';
  int strpos=0;
  for (int ix=0; ix<testFileDir.length(); ix++)
  {
      testFilePath[ix+1] = testFileDir[ix];
      Serial.print(testFilePath[ix+1]);
  }
  strpos = testFileDir.length() + 1;
  testFilePath[strpos] = '/';
  Serial.print(testFilePath[strpos]);

  for (int ix=0; ix<testFile.length(); ix++)
  {
      testFilePath[strpos+ix+1] = testFile[ix];
      Serial.print(testFilePath[strpos+ix+1]);
  }
  strpos += testFile.length()+1;
  testFilePath[strpos] = '\0';
  Serial.println(testFilePath[strpos]);

  //DEBUG:
  //for (int ix=0; ix <= strpos; ix++)
  //  Serial.print(testFilePath[ix]);
  //Serial.println("|");
  
  File fFile = SD.open(testFilePath, FILE_READ);

  if (fFile) {
    while (fFile.available()) {
      csvline = fFile.readStringUntil('\n');
      Serial.println(csvline);
      
      char command = csvline[0];
      //Serial.println(command);
      TestCommand(command);
      
      int commapos = csvline.indexOf(',',0);
      int lastcomma = csvline.lastIndexOf(',');
      while (commapos < lastcomma)
      {
        int nextcomma = csvline.indexOf(',',commapos+1);
        String param = csvline.substring(commapos+1,nextcomma);      
        //Serial.println(param); 
        TestParam(param);
        commapos = nextcomma;
      }
      String lastparam = csvline.substring(lastcomma+1,csvline.length());
      //Serial.println(lastparam);
      TestParam(lastparam);

      //removed to save space:
      /*
      if (command=='P')
      {
        //Serial.println("OK to connect power to breadboard now");
        Serial.println("OK to connect power now");
        establishContact();
      }*/
      _testNameFound = 0;
      
    }
  }
  else {
    Serial.println("ERROR opening "+testFile);
  }
  fFile.close();
}

void TestCommand(char c)
{
  _testCmd = c;
  _testParmCtr = 0;

  //upon receiving first P command, set Pin I/O configuration
  
  if (_testCmd == 'P')
  {
    if (_pinCfgDone == 0)
    {
      int pinNum = 0;
      for (int i=0; i<8; i++) 
      {
        int b = pinMap[i];
        int mode = pinIO[i];
        for (int m=0; m<8; m++)
        {
          int pinInUse = b % 2;
          int pinInOut = mode % 2;
          if (pinInUse)
          {
            //Serial.print("Pin in use: ");
            //Serial.println(pinNum);
            //Serial.print("I/O = ");
            //Serial.println(pinInOut);
            if (pinInOut == 0)
            {
              pinMode(pinNum,INPUT);
            }
            else
            {
              pinMode(pinNum,OUTPUT);
            }
          }
          pinNum ++;
          b = b >> 1;
          mode = mode >> 1;        
        }
      }
      _pinCfgDone = 1;
    }
  }    
}

int getPinNumFromName (String p)
{
  int pnum = -1;

  for (int i=0; i< 64; i++)
  {
    if ((p[0] == pinName[i*3])  && (p[1] == pinName[i*3+1]) && (p[2] == pinName[i*3+2]))
    {
      pnum = i;
      break;
    }
  }
  return pnum;
}

void setPinValue (String p)
{
  //Serial.print("PinNumber=");
  //Serial.println(_savePinNum);

  //Serial.print("PinValue=");
  //Serial.println(p);     

  if (p[0]=='1')
  {
    digitalWrite(_savePinNum, HIGH);
  }
  else
  {
    digitalWrite(_savePinNum, LOW);
  }
}

void passTest()
{
  Serial.println(" PASS");
}

void failTest()
{
  Serial.println(" FAIL <<<");
}

void TestParam(String p)
{
  _testParmCtr++;

  int mapPhase = (_testParmCtr - 1) % 3;
  int pinPhase = (_testParmCtr - 1) % 2;
  int tstPhase = (_testParmCtr) % 2; 
  
  switch (_testCmd) {
    case 'M': {
      if (mapPhase == 0) // pin number
      {
        int parm = p.toInt();
        int q = parm / 8;
        int r = parm % 8;
        int bitmask = 1 << r;
        pinMap[q] |= bitmask;
        _savePinNum = parm;
      }
      else if (mapPhase == 1) // pin name
      {
        int namepos = _savePinNum * 3;
        pinName[namepos] = p[0];
        pinName[namepos+1] = p[1];
        pinName[namepos+2] = p[2];
      }
      else // phase == 2 // pin I/O
      {
        int q = _savePinNum / 8;
        int r = _savePinNum % 8;
       
        int posbitmask = 1 << r;
        int negbitmask = posbitmask ^ 255;
        if (p[0]=='I')
          pinIO[q] &= negbitmask;
        else if (p[0]=='O')       
          pinIO[q] |= posbitmask;
      }
      break;    
    }      
    case 'P': { //commands to execute before applying power (preconditions)
      //for each active pin, set pinmode based on PinIO array.
      if (pinPhase == 0)
      {       
        _savePinNum = getPinNumFromName(p);
      }
      else // (pinPhase == 1)
      {
        setPinValue(p);
      }
      
      /*
      Serial.println(p);

      for (int i=0; i<8; i++) {
        int b = pinMap[i];
        for (int m=0; m<8; m++)
        {
          Serial.print(b%2);
          b = b >> 1;
        }
      }
      Serial.println();

      for (int j=0; j<192; j++) {
        Serial.print(pinName[j]);
      }
      Serial.println();
 
      for (int k=0; k<8; k++) {
        int l=pinIO[k];
        for (int n=0; n<8; n++)
        {
          Serial.print(l%2);
          l = l >> 1;        
        }
      }
      Serial.println();
      */
     break; 
    }    
    case 'T':
    {
      if (tstPhase == 0)
      {
        //Serial.println("Phase 0:"+p);
        _savePinNum = getPinNumFromName(p);
        //Serial.print("Pin:");
        //Serial.println(_savePinNum);
      }
      else // tstPhase == 1
      {
        if (_testNameFound == 0)
        {
          _testNameFound = 1;
          //Serial.println("TEST NAME: "+p);
        }
        else
        {
          setPinValue(p);
        }
      }
      break;
    }
    case 'D':
    {
      if (tstPhase == 1)
      {
        _delayval = p.toInt();
        //Serial.print("delay=");
        //Serial.println(delayval);
        delay(_delayval);
      }
    }
    case 'E':
    {
      if (tstPhase == 0)
      {
        Serial.print(" P");
        Serial.print(_savePinNum);
        Serial.print("=");

        //Serial.println("Expect:"+p);
        int result = digitalRead(_savePinNum);
        Serial.print(result);
        if (p[0]=='1') // expect HIGH
        {
          if (result == 1)
          {
            passTest();
          }
          else
          {
            failTest();
          }
        }
        else // expect LOW
        {
          if (result == 0)
          {
            passTest();
          }
          else
          {
            failTest();
          }
        }
        delay(_delayval);
      }
      else // tstPhase != 0
      {
        _savePinNum = getPinNumFromName(p);
        Serial.print(p+":");
      }
      break;
    }
    default:
    {
      //Serial.println(" *COMMENT*");
      break;
    }
  }   
}

