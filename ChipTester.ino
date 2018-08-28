// Reactive Micro Chip Tester
// (c) 2018, Reactive Micro.

int verboseDebug = 0; //set to 1 for more log messages

#include <SD.h>
File root;
int SDconnected = 0; //will get set to 1 if successful connection
int testCount = 0;
int testFileCount = 0;
const int MAXTESTS = 5;
const int MAXTFILES = 5;
String testDirs[MAXTESTS];
String testFiles[MAXTFILES];


//Set to pin number used by your SD shield. (Data logger shield=10)
int SDPIN = 10;

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

  root = SD.open("/");
  printDirectory(root, 0);
  root.close();
    
}


void loop() {
  if (SDconnected == 0)
    return;

  Serial.println ("Enter number of directory containing tests to run:");  

  while (!Serial.available())
  {
    delay(100);
  }

  String testDirNum = Serial.readStringUntil('\n');
  int iDirNum = testDirNum.toInt();
  Serial.println(iDirNum);
  if ((iDirNum <= 0) || (iDirNum > testCount)) 
  {
    Serial.println("Invalid option");
    testDirNum = Serial.readStringUntil('\n');
    return;
  }
  
  Serial.print("Selected test set: ");
  Serial.println(testDirNum);
  ReadTests(iDirNum-1);
  ExecTests(iDirNum-1);


  printTestDirectoryArray();
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

void printDirectory(File dir, int numTabs) {
  Serial.println(root.name());
  Serial.println("Test Directories in root of SD card:");
  testCount = 0;
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    //for (uint8_t i = 0; i < numTabs; i++) {
    //  Serial.print('\t');
    //}
    if (entry.isDirectory()) {
      if (entry.name()[0]=='T')
      {
        if (testCount < MAXTESTS)
        {
          testCount++;
          Serial.print (testCount);
          Serial.print (" : ");
          Serial.println(entry.name());
          testDirs[testCount - 1] = entry.name();          
        }
      }
    }
    entry.close();
  }
}

void printTestDirectoryArray()
{
  for (int i=0; i < testCount; i++)
  {
    Serial.print (i+1);
    Serial.print (" : ");
    Serial.println(testDirs[i]);
  }
}


void printTestFileArray()
{
  for (int i=0; i < testFileCount; i++)
  {
    Serial.print (i+1);
    Serial.print (" : ");
    Serial.println(testFiles[i]);
  }
}

void ReadTests(int testDirNum)
{
  File selectedTestDir;
  testFileCount = 0;
  String dir2open = testDirs[testDirNum];
  Serial.println("Opening: "+dir2open);

  selectedTestDir = SD.open(dir2open);
  while (true) {
    File testSet = selectedTestDir.openNextFile();
    if (!testSet) {
      break;
    }

    String testSetFile = testSet.name();
    if (testSetFile[0] == 'T')
    {
      if (verboseDebug) Serial.println("Test File="+testSetFile);
    
      testFiles[testFileCount] = testSetFile;  
      testSet.close();
      testFileCount++;
    
      //INSERTION SORT - determine if this file needs to bubble up
      int currTestIndex = testFileCount - 1;
      String currTest = testFiles[currTestIndex];
      String currTestNum = currTest.substring(1,4);
      if (verboseDebug) Serial.println("currTestNum="+currTestNum);

      if (testFileCount > 1)
      {
        int newFileNum = currTestNum.toInt();
        if (verboseDebug) {
          Serial.print ("newFileNum=");
          Serial.println(newFileNum);
        }
        
        int prevFileIndex = testFileCount - 2;
        while (prevFileIndex >= 0)
        {
          String prevFileNumStr = testFiles[prevFileIndex].substring(1,4);
          int prevFileNum = prevFileNumStr.toInt();
          if (newFileNum < prevFileNum)
          {
            if (verboseDebug) Serial.println("***SWAPPING***");

            testFiles[currTestIndex] = testFiles[prevFileIndex];
            testFiles[prevFileIndex] = currTest;
            currTestIndex = prevFileIndex;
            currTest = testFiles[currTestIndex];
            currTestNum = currTest.substring(1,4);
            newFileNum = currTestNum.toInt();
          }
          
          prevFileIndex--;
        }
        if (verboseDebug) printTestFileArray();

      }
    }
  }
  selectedTestDir.close();
  if (testFileCount == 0)
  {
    Serial.println("No test files found");
  }
  else
  {
    Serial.println("TEST FILES TO BE RUN:");
    printTestFileArray();
  }
  Serial.println();
}


void ExecTests(int testDirIndex)
{
  for (int i=0; i < testFileCount; i++)
  {
    Serial.print ("Enter any text and press <CR> to execute TEST file: ");
    Serial.print (i+1);
    Serial.print (" : ");
    Serial.println(testFiles[i]);
    establishContact();
    ExecTestFile(testFiles[i], testDirIndex);
  }
}

void ExecTestFile(String testFile, int testDirIndex)
{
  String csvline;
  String testFilePath = "/" + testDirs[testDirIndex] + "/" + testFile;
  Serial.println("Opening: "+testFilePath);

  File csvFile = SD.open(testFilePath);

  if (csvFile) {
    while (csvFile.available()) {
      csvline = csvFile.readStringUntil('\n');
      Serial.println(csvline);
    }
   csvFile.close();
  }
  else {
    Serial.println("ERROR opening "+testFilePath);
  }
}

