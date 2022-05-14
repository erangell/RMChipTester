//AY PSG tester
//Note: uses timer logic to generates pulses within the frequency range of 1 MHz to 1 Hz using Timer1 PWM on pin 9:

#include <TimerOne.h>
#define pwmRegister OCR1A // the logical pin, can be set to OCR1B

//Wiring of pins from AY-3-8913 to Arduino Mega:
//AY pin 1 = GND
const int bdir=241; //AY pin 2
const int bc1=26;  //pin 3
const int da7=28;  //pin 4
const int da6=30;  //pin 5
const int da5=32;  //pin 6
const int da4=34;  //pin 7
const int da3=36;  //pin 8
const int da2=38;  //pin 9
const int da1=40;  //pin 10
const int da0=42;  //pin 11
//pin 12 = unconnected

//pin 13 = +5V, 0.1 uf cap between pin 13 (VCC) and 19 (GND)
//pin 14 = unconnected
//pin 15 = Channel B analog out --> pin 17
//pin 16 = unconnected
//pin 17 = Channel A analog out --> Speaker/Amplifier --> GND
//pin 18 = Channel C analog out --> pin 17
//pin 19 = GND
const int clk=9;  //pin20 - 2Mhz pulses generated using PWM on Arduino pin 9
const int res=29;  //pin21
//pin 22=A8 - wire to +5V
//pin 23=~A9 - wire to GND
//pin 24=~CS - wire to GND

bool clock=0;
int numevents=20;
//events are register/value pairs
int event[] = {0x00, 0xf0, 0x01, 0x00, 0x02, 0xE0, 0x03, 0x00, 0x04, 0xD0, 0x05, 0x00, 0x07, 0x38, 0x08, 0x0F, 0x09, 0x0F, 0x0A, 0x0F};
int eventct = 0;

int dly=1; //delay in milliseconds between sending messages to chip

const int outPin = 9; // the physical pin for clock output
long period = 10000; // the period in microseconds
long pulseWidth = 1000; // width of a pulse in microseconds
int prescale[] = {0,1,8,64,256,1024}; // the range of prescale values

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Sending RESET");

  pinMode(clk,OUTPUT); 
  pinMode(res,OUTPUT);
  pinMode(bdir,OUTPUT);
  pinMode(bc1,OUTPUT);
  digitalWrite(res,LOW);
  digitalWrite(clk,LOW);
  digitalWrite(bdir,LOW);
  digitalWrite(bc1,LOW);
  
  pinMode(da0,OUTPUT);
  pinMode(da1,OUTPUT);
  pinMode(da2,OUTPUT);
  pinMode(da3,OUTPUT);
  pinMode(da4,OUTPUT);
  pinMode(da5,OUTPUT);
  pinMode(da6,OUTPUT);
  pinMode(da7,OUTPUT);

  //Set up PWM pulse on pin 9
  pinMode(outPin, OUTPUT);
  Timer1.initialize(period); // initialize timer1, 1000 microseconds
  setPulseWidth(pulseWidth);

  delay(dly);
  
  Serial.println("Sending 4 (Inactive)");
  digitalWrite(res,HIGH); 
  digitalWrite(bdir,LOW);
  digitalWrite(bc1,LOW);
  
  delay(dly);
  
}

bool setPulseWidth(long microseconds)
{
  bool ret = false;
  int prescaleValue = 8;
  // calculate time per counter tick in nanoseconds
  long precision = (F_CPU / 128000) * prescaleValue ;
  period = precision * ICR1 / 1000; // period in microseconds
  if( microseconds < period)
  {
    int duty = map(microseconds, 0,period, 0,1024);
    if( duty < 1)
    duty = 1;
    if (microseconds > 0)
    {
      Timer1.pwm(outPin, duty);
      ret = true;
    }
  }
  return ret;
}

void loop() {
   
  if (eventct < numevents)
  {
    Serial.print("Eventct=");
    Serial.println(eventct);

    if ((eventct % 2) == 0)
    {
      int reg = event[eventct];
      Serial.print("Reg:");
      Serial.println(reg);
       
      digitalWrite(da7,LOW);
      Serial.print("0");
      digitalWrite(da6,LOW);
      Serial.print("0");
      digitalWrite(da5,LOW);
      Serial.print("0");
      digitalWrite(da4,LOW);
      Serial.print("0");
      
      int a3 = (int)((reg & 0x0F) / 8);
      Serial.print(a3);
      digitalWrite(da3,a3);
      
      reg = reg - a3 * 8;     
      int a2 = (int)(reg / 4);
      Serial.print(a2);
      digitalWrite(da2,a2);
      
      reg = reg - a2 * 4;      
      int a1 = (int)(reg/2);
      Serial.print(a1);
      digitalWrite(da1,a1);
      
      reg = reg - a1 * 2;   
      int a0 = reg;
      Serial.println(a0);
      digitalWrite(da0,a0);      

      Serial.println("Sending 7");
  
      digitalWrite(res,HIGH);
      digitalWrite(bdir,HIGH);
      digitalWrite(bc1,HIGH);
      delay(dly);

      Serial.println("Sending 4");
      
      digitalWrite(res,HIGH);
      digitalWrite(bdir,LOW);
      digitalWrite(bc1,LOW);
      delay(dly);
  
    }
    else
    { 
      int data = event[eventct];
      Serial.print("Data:");
      Serial.println(data);      

      int d7 = (int)(data / 128);
      Serial.print(d7);
      digitalWrite(da7,d7);

      data = data - d7 * 128;
      int d6 = (int)(data / 64);
      Serial.print(d6);
      digitalWrite(da6,d6);
      
      data = data - d6 * 64;
      int d5 = (int)(data / 32);
      Serial.print(d5);
      digitalWrite(da5,d5);
      
      data = data - d5 * 32;
      int d4 = (int)(data / 16);
      Serial.print(d4);
      digitalWrite(da4,d4);  
      
      data = data - d4 * 16;
      int d3 = (int)(data / 8);
      Serial.print(d3);
      digitalWrite(da3,d3);
      
      data = data - d3 * 8;
      int d2 = (int)(data / 4);
      Serial.print(d2);
      digitalWrite(da2,d2);  
      
      data = data - d2 * 4;
      int d1 = (int)(data / 2);
      Serial.print(d1);
      digitalWrite(da1,d1);  

      data = data - d1 * 2;
      int d0 = data;
      Serial.println(d0);
      digitalWrite(da0,d0);  

      Serial.println("Sending 6");
      
      digitalWrite(res,HIGH);
      digitalWrite(bdir,HIGH);
      digitalWrite(bc1,LOW);
      delay(dly);

      Serial.println("Sending 4");
      
      digitalWrite(res,HIGH);
      digitalWrite(bdir,LOW);
      digitalWrite(bc1,LOW);
      delay(dly);
    }
        
    eventct++;
  }

}

