// Reactive Micro Chip Tester
// (c) 2018, Reactive Micro.

#include <SD.h>

//Set to pin number used by your SD shield. (Data logger shield=10)
int SDPIN = 10;

int SDconnected = 0; //will get set to 1 if successful connection
int testCount = 0;
int testFileCount = 0;
String testDirName = "";
char testCmd = ' ';
String testParm = "";
int testState = 0;
int testParmCtr = 0;
String testFileName = "";
//int pinMap[40];
//String pinName[40];
//int pinIO[40];


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

  testDirName = Serial.readStringUntil('\n');

  ReadTestDir(testDirName);
}


void loop() {

  if (SDconnected == 0)
    return;
    
    Serial.println("Enter name of test file to run:");
    while (!Serial.available())
    {
      delay(100);
    }

    testFileName = Serial.readStringUntil('\n');
    ExecuteTestFile( testFileName, testDirName );
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
  File root = SD.open("/");

  Serial.println(root.name());
  Serial.println("Test Directories in root of SD card:");
  testCount = 0;
  while (true) {

    File entry =  root.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    if (entry.isDirectory()) {
      if (entry.name()[0]=='T')
      {
          testCount++;
          Serial.println(entry.name());
      }
    }
    entry.close();
  }
  root.close();
}


void ReadTestDir(String testDirName)
{
  testFileCount = 0;
  File selectedTestDir = SD.open(testDirName);
  if (!selectedTestDir)
  {
    Serial.println("Directory not found");
    return;
  }
  File testSet;
  while (true) {
    testSet = selectedTestDir.openNextFile();
    if (!testSet) {
      break;
    }

    String testSetFile = testSet.name();
    if (testSetFile[0] == 'T')
    {
      Serial.println(testSetFile);
      testFileCount++;
    } 
  }
  testSet.close();
  
  selectedTestDir.close();
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

  File csvFile = SD.open(testFilePath);

  if (csvFile) {
    while (csvFile.available()) {
      csvline = csvFile.readStringUntil('\n');
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
    Serial.println("ERROR opening "+testFilePath);
  }
  csvFile.close();
}

void TestCommand(char c)
{
  testCmd = c;
  testParmCtr = 0;
}

void TestParam(String p)
{
  testParm = p;
  switch (testCmd) {
    case 'M': {
      break;    
    }
    case 'N': {
      break;
    }
      
    case 'O': {
      break;
    }
      
    case 'P': {
     break; 
    }    
  }
   
}

