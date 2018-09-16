// Reactive Micro Chip Tester
// (c) 2018, Reactive Micro.

#include <SD.h>

//Set to pin number used by your SD shield. (Data logger shield=10)
int SDPIN = 10;
int SDconnected = 0; //will get set to 1 if successful connection

String _testDirName;
char _testCmd = ' ';
int _testParmCtr = 0;
int _savePinNum = 0;

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

File fDir;
File fFile;

int _pinCfgDone = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Send one line of text to begin:");
  establishContact();  // send a byte to establish contact until receiver responds

  Serial.print("Connecting to SD card...");

  if (!SD.begin(SDPIN)) {
    Serial.println("initialization failed!  Check wiring, if card inserted, and pin to use for your SD shield");
    return;
  }
  SDconnected = 1;
  Serial.println("initialization done.");

  printDirectory();

  Serial.println ("Enter name of directory containing tests to run:");  

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
    
    Serial.println("Enter name of test file to run:");
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
  String garbage = Serial.readStringUntil('\n');
  Serial.println("Received:"+garbage);
  Serial.println();
}

void printDirectory() {
  fDir = SD.open("/");

  Serial.println(fDir.name());
  Serial.println("Test Directories in root of SD card:");
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
  fDir = SD.open(testDirName);
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
  String testFilePath = "/" + testFileDir + "/" + testFile;
  Serial.println("Opening: "+testFilePath);

  fFile = SD.open(testFilePath);

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
    }
  }
  else {
    Serial.println("ExecuteTestFile: ERROR opening "+testFilePath);
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
            Serial.print("Pin in use: ");
            Serial.println(pinNum);
            Serial.print("I/O = ");
            Serial.println(pinInOut);
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

void TestParam(String p)
{
  _testParmCtr++;

  int mapPhase = (_testParmCtr - 1) % 3;
  int pinPhase = (_testParmCtr - 1) % 2;
  
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
          Serial.print("PinName=");
          Serial.println(p);
          
          int pnum = -1;

          for (int i=0; i< 64; i++)
          {
            if ((p[0] == pinName[i*3])  && (p[1] == pinName[i*3+1]) && (p[2] == pinName[i*3+2]))
            {
              pnum = i;
              break;
            }
          }
          
          Serial.print("PinNumber=");
          Serial.println(pnum);

      }
      else // (pinPhase == 1)
      {
          Serial.print("PinValue=");
          Serial.println(p);        
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
  }
   
}

