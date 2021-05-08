#include <can.h>

/* DAQ MAIN CODE
 *  subroutines: analogSensors, digitalSensors, saveAndCompile
 */
/* TODO
 *  pull data from I2C bus
 *  pull data from MOTEC/CAN
 *  write functions for manipulating data:
 *    Non testing:
 *      lambdas, wheel speed (DIG) , brake temp, shock potent, brake pressure, throttle pos
 *      intake manifold ap, intake manifold at, oil pressure, cam angle (DIG)
 *      coolant temp, oil temp, crank angle
 *    Testing:
 *      steering angle, pitot tubes, strain gauges, accelerometer, gyroscope, thermocouple
 */

#include "SD.h"
#include"SPI.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <SoftwareSerial.h>
#include <can.h>
#include <global.h>
#include <Canbus.h>
#include <mcp2515_defs.h>
#include <mcp2515.h>
#include <defaults.h>

/*
led behavior

setup:
dash led - (4,5,6) - r on
breaktemp led - y on (14,15)
shockpot led - g on (16, 17, 18)
intake pressure led - g on (24, 25, 26)
intake temperature led - y on (27, 28)
coolant temp led - r on (30,31,32)

after can_setup: r on - dash led

after compileCurData = r on (breaktemp led)
after saveData = g on
after writeXbee - y on

before analog sensors - intake pressure green
enters analog sensors - intake pressure yellow

enters digital sensors - intake temperature yellow

telemetry
enters writeXbee - green on (coolant temp led)
after xbee.write for short sensor data - yellow on (cooland temp led)
*/

SoftwareSerial xbee(0,1);

Adafruit_ADS1115 ads1115a(0x48);
Adafruit_ADS1115 ads1115b(0x49);
Adafruit_ADS1115 ads1115c(0x4A);
//GLOBALS
const int CSpin = 10;
String dataString =""; // holds the data to be written to the SD card
String fileName;
File sensorData;
int readFrequency = 1000; // CHANGE THIS TO READ FASTER/SLOWER (units in ms)

//Debug and testing
bool enableSerialMessages = false;



// leds for detecting sd card
int led_r = 39;
int led_g = 40;
int led_y = 41;

unsigned long previousTimeDigital = millis();
unsigned long previousTimeAnalog = millis();
unsigned long currentTime;
unsigned long FL_VSS_LastRead,FR_VSS_LastRead, BL_VSS_LastRead, BR_VSS_LastRead, diff = millis();

// SENSOR ARRAYS
float allSensors[43];

// SENSOR GLOBALS
int sensorVoltage = 0;
int systemVoltage = 5;
int resolution = 1024;

// Wheel Speed
int wheelCirc = 3.24*2*8;
int wheelSpeed = 0;
int FL_VSS_PIN = 2;
int FR_VSS_PIN = 3;
float FL_VSS, FR_VSS, BL_VSS, BR_VSS;

// Brake Temperature
int FL_BRK_TMP_PIN = A0;
int FR_BRK_TMP_PIN = A1;
float FL_BRK_TMP, FR_BRK_TMP,BL_BRK_TMP,BR_BRK_TMP;

// Suspension Potentiometer
int FL_SUS_POT_PIN = A2;
int FR_SUS_POT_PIN = A3;
float FL_SUS_POT, FR_SUS_POT, BL_SUS_POT, BR_SUS_POT;

// Brake Pressure
int F_BRK_PRES_PIN = A4;
float F_BRK_PRES = 0;
float B_BRK_PRES = 0;

// Steer Angle
int STEER_ANG_PIN = A5;
float STEER_ANG = 0;

// Rest of Motec Reads
float TPS, OIL_PRES,OIL_TEMP,COOL_TEMP, MAP, MAT, NEUT, LAMBDA1, LAMBDA2;

//  Rest of ADC reads
float ACCEL = 0;
float GYRO = 0;
float GPS = 0;
float STRAIN1, STRAIN2, STRAIN3, STRAIN4;

// PTUBES
int PTUBE1_PIN = A6;
int PTUBE2_PIN = A7;
int PTUBE3_PIN = A8;
int PTUBE4_PIN = A9;
int PTUBE5_PIN = A10;
int PTUBE6_PIN = A11;
int PTUBE7_PIN = A12;
int PTUBE8_PIN = A13;
int PTUBE9_PIN = A14;
int PTUBE10_PIN = A15;
float PTUBE1, PTUBE2, PTUBE3, PTUBE4, PTUBE5, PTUBE6, PTUBE7, PTUBE8, PTUBE9, PTUBE10, PTUBE11, PTUBE12;


// OFFSETS
float PTUBE_CLB, STRAIN1_CLB, STRAIN2_CLB, STRAIN3_CLB, STRAIN4_CLB, STEER_ANG_CLB, TPS_CLB, F_BRK_PRES_CLB, B_BRK_PRES_CLB, FL_SUS_POT_CLB, FR_SUS_POT_CLB, BL_SUS_POT_CLB, BR_SUS_POT_CLB;


float convertSensor(int sensorValue, int calibration=0);
// sensor value from 0 to 2^16 and returns a voltage between 0 and 5 V
float convertSensor(int sensorValue, int calibration=0){
  float sensorVoltage = (sensorValue * ((float)systemVoltage/resolution)) - calibration;
  return sensorVoltage;
}


void setup() {
  // Open serial communications
  if(enableSerialMessages){
  Serial.begin(9600);
  Serial.print("Initializing SD card...");}
  //xbee.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);   //dash LED
  
  pinMode(14, OUTPUT); //breaktemp LED
  pinMode(15, OUTPUT); //
  
  pinMode(16, OUTPUT); 
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT); //shockpot LED
  
  digitalWrite(14, HIGH); // g off
  digitalWrite(15, LOW); //y on
  
  digitalWrite(16, HIGH); //r off
  digitalWrite(17, LOW); // g on
  digitalWrite(18, HIGH); //y off

  
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT); 
  pinMode(26, OUTPUT); //intake pressure LED

  digitalWrite(24, LOW); // g on
  digitalWrite(25, HIGH); //y off
  digitalWrite(26, HIGH); //r off

  pinMode(27, OUTPUT); 
  pinMode(28, OUTPUT); //intake temperature LED

  digitalWrite(27, LOW); //y on
  digitalWrite(28, HIGH); //r off

  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(32, OUTPUT); // coolant temperature

  digitalWrite(30, HIGH); //r on
  digitalWrite(31, LOW); //g off
  digitalWrite(32, LOW); //y off

  digitalWrite(4, LOW); //r on
  digitalWrite(5, HIGH); //g off
  digitalWrite(6, HIGH); //y off
  
  can_setup();
  
  digitalWrite(4, HIGH); //r off
  digitalWrite(5, HIGH); //g off
  digitalWrite(6, LOW); //y on
  
  
  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_y, OUTPUT);
  
  if(enableSerialMessages){Serial.print("CAN setup complete");}
  pinMode(CSpin, OUTPUT);
  //

  // declare led pins as outputs
  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_y, OUTPUT);

  
  // see if the card is present and can be initialized
  if (!SD.begin(CSpin)) {
  if(enableSerialMessages){Serial.println("Card failed/not found");}
  digitalWrite(led_r, LOW);
  digitalWrite(led_y, HIGH);
  digitalWrite(led_g, HIGH);
  // stop
  return;
  }
  if(enableSerialMessages){Serial.println("card initialized.");}

  pinMode(FL_VSS_PIN, INPUT);
  pinMode(FR_VSS_PIN, INPUT);

  //   get current version for file name and then update version
  int dataVer;
  File sensorDataVer = SD.open("VerTrack.txt");
  if (sensorDataVer) {
        dataVer = sensorDataVer.read();
  } else {
    if(enableSerialMessages){Serial.println("File unavailable");}
    digitalWrite(led_r, LOW);
    digitalWrite(led_y, LOW);
    digitalWrite(led_g, HIGH);
    delay(2000);
  }
  sensorDataVer.close();
  sensorDataVer = SD.open("VerTrack.txt", O_RDWR);
  sensorDataVer.write(dataVer+1);
  sensorDataVer.close();
  fileName = "data" + String(dataVer) + ".csv";
 
  // write headers
  sensorData = SD.open(fileName, FILE_WRITE);
  if (sensorData){
      sensorData.println("FL_VSS,FR_VSS,BL_VSS,BR_VSS,FL_BRK_TMP,FR_BRK_TMP,BL_BRK_TMP,BR_BRK_TMP,FL_SUS_POT,FR_SUS_POT,BL_SUS_POT,BR_SUS_POT,F_BRK_PRES,B_BRK_PRES,STEER_ANG,TPS,OIL_PRES,OIL_TEMP,COOL_TEMP,MAP,MAT,NEUT,LAMBDA1,LAMBDA2,ACCEL,GYRO,GPS,STRAIN1,STRAIN2,STRAIN3,STRAIN4,PTUBE1,PTUBE2,PTUBE3,PTUBE4,PTUBE5,PTUBE6,PTUBE7,PTUBE8,PTUBE9,PTUBE10,PTUBE11,PTUBE12");
      sensorData.close(); // close the file
      if(enableSerialMessages){Serial.println("First line of file written");}
    } else {
    if(enableSerialMessages){Serial.println("Error writing to file !");}
    digitalWrite(led_r, LOW);
    digitalWrite(led_y, LOW);
    digitalWrite(led_g, HIGH);
    delay(2000);
  }

  // set calibration variables ** these are in voltage
  FL_SUS_POT_CLB = convertSensor(analogRead(FL_SUS_POT_PIN));
  FR_SUS_POT_CLB = convertSensor(analogRead(FR_SUS_POT_PIN));
  //  BL_SUS_POT_CLB = back l suspot
  //  BR_SUS_POT_CLB = back r suspot
  F_BRK_PRES_CLB = convertSensor(analogRead(F_BRK_PRES_PIN));
  //  B_BRK_PRES_CLB = back brk pres
  STEER_ANG_CLB = convertSensor(analogRead(STEER_ANG_PIN));
  //  TPS_CLB = TPS
  STRAIN1_CLB = convertSensor(ads1115b.readADC_Differential_0_1());
  STRAIN2_CLB = convertSensor(ads1115b.readADC_Differential_2_3());
  STRAIN3_CLB = convertSensor(ads1115c.readADC_Differential_0_1());
  STRAIN4_CLB = convertSensor(ads1115c.readADC_Differential_2_3());
  PTUBE_CLB = convertSensor(analogRead(PTUBE1_PIN));
  digitalWrite(led_r, HIGH);

  if(enableSerialMessages){Serial.print("Setup complete");}
}

void loop() {
  currentTime = millis();

//  run checks for digital sensors every single loop, check for reading of 0

  digitalSensors();

 
  
//  check for analog reading every second
//  frequency of change of data
  if (currentTime - previousTimeAnalog > readFrequency){
    previousTimeAnalog = currentTime;

    compileCurData();
    digitalWrite(14, LOW); // r on
    digitalWrite(15, HIGH);
    digitalWrite(16, HIGH);
    saveData();
    digitalWrite(15, LOW); //g on
    digitalWrite(14, HIGH);
    digitalWrite(16, HIGH);

    // for now write to xbee every second, may shorten interval
    writeXbee();
    digitalWrite(16, LOW); //y on
    digitalWrite(14, HIGH);
    digitalWrite(15, HIGH);
    
  }
 
}
