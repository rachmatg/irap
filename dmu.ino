#include <EEPROM.h>

#define LED 9
#define CALIB 1
#define RUN 0
volatile byte state = LOW;
const byte ledPin = 13;
const byte interruptPin = 2;
const byte secondEncoderPin = 3;
const byte calibrationPin = 4;
volatile byte counter = 0;
volatile long count = 0;
volatile int lastState = 1;
const byte address = 0;
volatile unsigned int ppm = 0;
volatile byte mode = RUN;
int ledState = LOW;  


void setup() {
  ppm = EEPROMReadInt(address);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only3
  }
  //ppm = 1024;
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(secondEncoderPin, INPUT_PULLUP);
  pinMode(calibrationPin, INPUT_PULLUP);
  Serial.begin(115200);
  delay(2000);
  Serial.println("Siaaaap... " + String(ppm) + " ppm");
  attachInterrupt(digitalPinToInterrupt(interruptPin), toggle, CHANGE);
  mode = RUN;
}

void loop() { 
  //digitalWrite(ledPin, state); 
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil(0x0d);
    data.trim();
    Serial.println(">> " + data);
    if (data.substring(0,3) == "cal") {
      Serial.println("Calibration Mode");
      mode = CALIB;
      count = 0;
    } else if (data.substring(0,3) == "run") {
      Serial.println("RunTime Mode");
      mode = RUN;
      count = 0;
    } else if (data.substring(0,3) == "set") {
      String val = data.substring(4);
      unsigned int setPoint = val.toInt();
      Serial.println("Write " + val + " to EEPROM");
      EEPROMWriteInt(0,setPoint);
      ppm = setPoint;
    } else if (data.substring(0,3) == "get") {
      unsigned int setPoint = EEPROMReadInt(0);
      Serial.println("Read " + String(setPoint) + " from EEPROM");
      ppm = setPoint;
    } else {
      Serial.println("Unknown Command");
    }
  }
}

void calibMode(byte c) {
//  Serial.print(c);
  Serial.print(count);
  if ((counter == 3) && (c == 0)) // maju
  {
    Serial.println(",1");
    count++;
    lastState = 1;
    counter = c;
    return;
  }
  if ((counter == 0) && (c == 3)) // maju
  {
    Serial.println(",-1");
    count--;
    lastState = -1;
    counter = c;
    return;
  }
  if (c > counter) {
    Serial.println(",1");
    lastState = 1;
    count++;
    counter = c;
    return;
  }
  if (c< counter) {
    Serial.println(",-1");
    lastState = -1;
    count--;
    counter = c;
    return;
  }
  if (c == counter) {
    Serial.println(","+lastState);
    counter = c;
    return;
  }
}

void runMode(byte c) {
 // Serial.println(count);
  if (count >=ppm) {
    count = 0;
    Serial.println("1");
  }
  if ((counter == 3) && (c == 0)) // maju
  {
   // Serial.println(",1");
    count++;
    lastState = 1;
    counter = c;
    return;
  }
  if ((counter == 0) && (c == 3)) // maju
  {
   // Serial.println(",-1");
    count--;
    lastState = -1;
    counter = c;
    return;
  }
  if (c > counter) {
   // Serial.println(",1");
    lastState = 1;
    count++;
    counter = c;
    return;
  }
  if (c< counter) {
   // Serial.println(",-1");
    lastState = -1;
    count--;
    counter = c;
    return;
  }
  if (c == counter) {
   // Serial.println(","+lastState);
    counter = c;
    return;
  }
}

void toggle() { 
  //state = !state; 
  
 // ledState = !ledState;

    // control LED arccoding to the toggled state
 // digitalWrite(ledPin, ledState); 
  byte a=0;
  byte b=0;
  if (digitalRead(interruptPin)==HIGH) {
    a = 1;
  }
  if (digitalRead(secondEncoderPin)==HIGH) {
    b = 1;
  }
  byte c = a + b*2;
 // Serial.println(String(c, HEX));
  if (mode == CALIB) {
    calibMode(c);  
  } else {
    runMode(c);
  }
}

void EEPROMWriteInt(int p_address, unsigned int p_value)
{
  byte Byte1 = ((p_value >> 0) & 0xFF);
  byte Byte2 = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, Byte1);
  EEPROM.write(p_address + 1, Byte2);
}

unsigned int EEPROMReadInt(int p_address)
{
  byte Byte1 = EEPROM.read(p_address);
  byte Byte2 = EEPROM.read(p_address + 1);

  unsigned int firstTwoBytes = ((Byte1 << 0) & 0xFF) + ((Byte2 << 8) & 0xFF00);
  return (firstTwoBytes);
}
