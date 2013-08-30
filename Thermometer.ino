#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 8 on the Arduino
#define ONE_WIRE_BUS 8

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

const int segPinOffs = 0;
const int decPinOffs = 9;

const int segA = 0;
const int segB = 1;
const int segC = 2;
const int segD = 3;
const int segE = 4;
const int segF = 5;
const int segG = 6;
const int segDP = 7;

const int period = 1000;
const int activeDecs = 4;
const int updateFreq = 60;

const int periodSlice = period / activeDecs;
const int updatePeriod = periodSlice / updateFreq;
const int updatesPerPeriod = period / updatePeriod;

const byte seven_seg_digits[10][7] = { { 1,1,1,1,1,1,0 },  // = 0
                                 { 0,1,1,0,0,0,0 },  // = 1
                                 { 1,1,0,1,1,0,1 },  // = 2
                                 { 1,1,1,1,0,0,1 },  // = 3
                                 { 0,1,1,0,0,1,1 },  // = 4
                                 { 1,0,1,1,0,1,1 },  // = 5
                                 { 1,0,1,1,1,1,1 },  // = 6
                                 { 1,1,1,0,0,0,0 },  // = 7
                                 { 1,1,1,1,1,1,1 },  // = 8
                                 { 1,1,1,0,0,1,1 }   // = 9
                               };
                               
byte dec_array[5][4] = {
  {0, 0, 0, 1},
  {0, 0, 1, 0},
  {0, 1, 0, 0},
  {1, 0, 0, 0},
  {0, 0, 0, 0}
};
                            
void writeNbrDec(int nbr, int dec, boolean lightDp) {
  writeNbr(nbr, lightDp);
  for (int i = 0; i < activeDecs; i++) {
    digitalWrite(i + decPinOffs, dec_array[dec][i]);
  }
}

void writeNbr(int nbr, boolean lightDp) {
  if (nbr < 0 || nbr > 9) {
    return;
  }
 
  //Using digital pins 2 to 8
  for (int i = 0; i < 7; i++) {
     digitalWrite(i + segPinOffs, seven_seg_digits[nbr][i]); 
  }
  if (lightDp == true) {
     digitalWrite(segDP, HIGH); 
  } else {
    digitalWrite(segDP, LOW);
  }
}

void setup() {
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);  
  pinMode(segG, OUTPUT);
  pinMode(segDP, OUTPUT);
  
  for (int i = decPinOffs; i < 4; i++) {
    pinMode(i, OUTPUT);
  }
}

const byte display[4] = { 1, 3, 3, 7 };

void loop() {
  sensors.requestTemperatures();
  float tempFloat = sensors.getTempCByIndex(0);
  int tempInt = (int) (tempFloat * 100);

  for (int i = 0; i < updatesPerPeriod / 2; i++) {
      writeNbrDec(tempInt - ((tempInt / 10) * 10), 3, false);
      delay(updatePeriod);
      
      writeNbrDec((tempInt - ((tempInt / 100) * 100)) / 10, 2, false);
      delay(updatePeriod);
      
      writeNbrDec((tempInt - ((tempInt / 1000) * 1000)) / 100, 1, true);
      delay(updatePeriod);
      
      writeNbrDec((tempInt - ((tempInt / 10000) * 10000)) / 1000, 0, false);
      delay(updatePeriod);   
  }
  writeNbrDec(0, 4, false);
}
