/*
  Adapted from Arduino Samples: State Change Detection (edge detection)
  and DataLogger. 
   
  To get Data Logger shield to work with MEGA, follow these instructions:
  https://learn.adafruit.com/adafruit-data-logger-shield/for-the-mega-and-leonardo

  Comments from Datalogger.ino - but I think SS is pin #53, not 52.
  ** Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 4 (CS pin can be changed)
  and pin #52 (SS) must be an output
  
*/

#include <SPI.h>
#include "SD.h"

const int  annc0Pin = 2;    // Connect to Apple Annunciator 0 on internal game socket
const int megaSlaveSelect = 53; //This pin must be set to OUTPUT for Mega to work with datalogger
const int spi1 = 10;  // pins needed for SD.begin()
const int spi2 = 11;
const int spi3 = 12;
const int spi4 = 13;
const int ledPin =  LED_BUILTIN;

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

long usecPulse = 0;
long pulseStart[100];
long pulseStop[100];

char outFileName[] = "PULSTEST.TXT";
File dataFile;

void setup() {
  // initialize the button pin as a input:
  pinMode(annc0Pin, INPUT);

  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin,HIGH);
  
  // initialize serial communication:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println();
  Serial.println("This sketch measures pulse width of HIGH to LOW transition on pin 2");
  Serial.println("Data will be written to file PULSTEST.TXT on the SD card in the Data Logger shield.");
  
  Serial.print("Initializing SD card...");
  pinMode(megaSlaveSelect, OUTPUT);
  
  // see if the card is present and can be initialized:
  //if (!SD.begin(chipSelect)) {
  if (!SD.begin(spi1, spi2, spi3, spi4)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1) ;
  }
  Serial.println("card initialized.");
  
  // Open up the file we're going to log to!
  dataFile = SD.open(outFileName, FILE_WRITE);
  if (! dataFile) {
    Serial.println("error opening datalog.txt");
    // Wait forever since we cant write data
    while (1) ;
  }

  dataFile.print("LOG STARTED AT microsec() = ");
  dataFile.println(micros()); 
  dataFile.flush();
  Serial.println("Logging Data Now...");
  digitalWrite(ledPin,LOW);

  while(1)
  {
    // read the pushbutton input pin:
    buttonState = digitalRead(annc0Pin);
  
    // compare the buttonState to its previous state
    if (buttonState != lastButtonState) {
      // if the state has changed, increment the counter
      if (buttonState == HIGH) {
        pulseStart[buttonPushCounter] = micros();
      } else {
        pulseStop[buttonPushCounter] = micros();
        buttonPushCounter++;
  
        if (buttonPushCounter == 100)
        {
  
          dataFile.print("LOW->HIGH->LOW: Pulse widths recorded");
  
          for (int i=0 ; i<100; i++)
          {
            dataFile.print("[");
            dataFile.print(i);
            dataFile.print("]=");
  
            usecPulse = pulseStop[i] - pulseStart[i];
            
            dataFile.println(usecPulse);
          }    
          dataFile.print("END OF DATA");
          dataFile.flush();
          dataFile.close();
          digitalWrite(ledPin,HIGH);
          while (1) ; // wait forever when done    
        }
      }
    }
    // save the current state as the last state, for next time through the loop
    lastButtonState = buttonState;
  }
} 

void loop() {
}
