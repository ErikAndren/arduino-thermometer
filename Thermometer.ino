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

const byte seven_seg_digits_port[10] = {0b00111111, // 0
                                        0b00000110, // 1
                                        0b01011011, // 2
                                        0b01001111, // 3
                                        0b01100110, // 4
                                        0b01101101, // 5
                                        0b01111101, // 6
                                        0b00000111, // 7
                                        0b01111111, // 8
                                        0b01100111  // 9
                                      };

byte dec_array_port[5] = {
   0b00000010,
   0b00000100,
   0b00001000,
   0b00010000,
   0b00000000
};

void writeNbrDec(int nbr, int dec, boolean lightDp) {
  writeNbr(nbr, lightDp);

  PORTB = dec_array_port[dec];
}

DeviceAddress deviceAddress;

void writeNbr(int nbr, boolean lightDp) {
  if (nbr < 0 || nbr > 9) {
    return;
  }
 
  PORTD = seven_seg_digits_port[nbr];

  if (lightDp == true) {
    PORTD = 0b10000000 | PORTD;
  } else {
    //Clear dp bit
    PORTD &= ~(1 << 7);
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
  
  sensors.requestTemperatures();
  sensors.setWaitForConversion(false);
  sensors.getAddress(deviceAddress, 0);
}

void loop() {
  //Display the old value, to avoid needing to block for the conversion to take place
  float tempFloat = sensors.getTempC(deviceAddress);
  sensors.requestTemperatures();
  int tempInt = (int) (tempFloat * 100);

  for (int i = 0; i < updatesPerPeriod / 2; i++) {
      writeNbrDec(tempInt - ((tempInt / 10) * 10), 0, false);
      delay(updatePeriod);
      
      writeNbrDec((tempInt - ((tempInt / 100) * 100)) / 10, 1, false);
      delay(updatePeriod);
      
      writeNbrDec((tempInt - ((tempInt / 1000) * 1000)) / 100, 2, true);
      delay(updatePeriod);
      
      writeNbrDec((tempInt - ((tempInt / 10000) * 10000)) / 1000, 3, false);
      delay(updatePeriod);   
  }
}
