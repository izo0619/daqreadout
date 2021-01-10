


/*
Sensor Multitasking
- checking for multiple sensors at different time frames
- uses millis and is ugly asf bc of if statements but it works
- may b shitty but no 3rd party lieberry = simulator friendly: 
https://www.tinkercad.com/things/kvzXWspMo0g-stunning-snaget-duup/editel?sharecode=lyl-Zan2RpXMJFdnIAeN28VRvoHp2dthZG_x7TVTIyk
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

Adafruit_ADS1115 ads1115a(0x48);
Adafruit_ADS1115 ads1115b(0x49);
Adafruit_ADS1115 ads1115c(0x4A);
//GLOBALS
const int CSpin = 10;
String dataString =""; // holds the data to be written to the SD card
String fileName;
File sensorData;


unsigned long previousTimeDigital = millis();
unsigned long previousTimeAnalog = millis();
unsigned long currentTime;
unsigned long lastDigReadTime = millis();

// SENSOR ARRAYS
float allSensors[43];

// SENSOR GLOBALS
int sensorVoltage = 0;
int systemVoltage = 5;
int resolution = 65535;

// Wheel Speed
int wheelCirc = 0;
int wheelSpeed = 0;
int FL_VSS_PIN = 6;
int FR_VSS_PIN = 7;
float FL_VSS, FR_VSS, BL_VSS, BR_VSS;

// Brake Temperature
int FL_BRK_TMP_PIN = 97;
int FR_BRK_TMP_PIN = 96;
float FL_BRK_TMP, FR_BRK_TMP,BL_BRK_TMP,BR_BRK_TMP;

// Suspension Potentiometer
int FL_SUS_POT_PIN = 95;
int FR_SUS_POT_PIN = 94;
float FL_SUS_POT, FR_SUS_POT, BL_SUS_POT, BR_SUS_POT;

// Brake Pressure
int F_BRK_PRES_PIN = 93;
float F_BRK_PRES = 0;
float B_BRK_PRES = 0;

// Steer Angle
int STEER_ANG_PIN = 92;
float STEER_ANG = 0;

// Rest of Motec Reads
float TPS, OIL_PRES,OIL_TEMP,COOL_TEMP, MAP, MAT, NEUT, LAMBDA1, LAMBDA2;

//  Rest of ADC reads
float ACCEL = 0;
float GYRO = 0;
float GPS = 0;
float STRAIN1, STRAIN2, STRAIN3, STRAIN4;

// PTUBES
int PTUBE1_PIN = 91;
int PTUBE2_PIN = 90;
int PTUBE3_PIN = 89;
int PTUBE4_PIN = 88;
int PTUBE5_PIN = 87;
int PTUBE6_PIN = 86;
int PTUBE7_PIN = 85;
int PTUBE8_PIN = 84;
int PTUBE9_PIN = 83;
int PTUBE10_PIN = 82;
float PTUBE1, PTUBE2, PTUBE3, PTUBE4, PTUBE5, PTUBE6, PTUBE7, PTUBE8, PTUBE9, PTUBE10, PTUBE11, PTUBE12;

// OFFSETS
                
//FUNCTIONS

void digitalSensors(int diff){
  // wheel speed
  FL_VSS = digitalRead(FL_VSS_PIN);
  FR_VSS = digitalRead(FR_VSS_PIN);
  if (FL_VSS == 0){
      wheelSpeed = wheelCirc/diff;
      allSensors[1] = wheelSpeed;
      lastDigReadTime = currentTime;
  }
  if (FR_VSS == 0){
      wheelSpeed = wheelCirc/diff;
      allSensors[2] = wheelSpeed;
      lastDigReadTime = currentTime;
  }
  // REPEAT WITH BACK WHEEL SPEEDS FROM MOTEC
  if (BL_VSS == 0){
      wheelSpeed = wheelCirc/diff;
      allSensors[3] = wheelSpeed;
      lastDigReadTime = currentTime;
  }
  if (BR_VSS == 0){
      wheelSpeed = wheelCirc/diff;
      allSensors[4] = wheelSpeed;
      lastDigReadTime = currentTime;
  }
}


// sensor value from 0 to 2^16 and returns a voltage between 0 and 5 V
int convertSensor(int sensorValue){
  sensorVoltage = sensorValue * (systemVoltage/resolution);
  return sensorVoltage;
}

void analogSensors(){
  
  allSensors[5] = (convertSensor(analogRead(FL_BRK_TMP_PIN)) - 0.50)/0.005;
  allSensors[6] = (convertSensor(analogRead(FR_BRK_TMP_PIN)) - 0.50)/0.005;
  //  allSensors[7] = back l brake temp
  //  allSensors[8] = back r brake temp
  allSensors[9] = convertSensor(analogRead(FL_SUS_POT_PIN));
  allSensors[10] = convertSensor(analogRead(FR_SUS_POT_PIN));
  //  allSensors[11] = back l suspot
  //  allSensors[12] = back r suspot
  allSensors[13] = (convertSensor(analogRead(F_BRK_PRES_PIN)) - 0.50)/ (4/7500);
  //  allSensors[14] = back brk pres
  allSensors[15] = convertSensor(analogRead(STEER_ANG_PIN));
  // motec
  //  allSensors[16] = TPS
  //  allSensors[17] = OIL_PRES
  //  allSensors[18] = OIL_TEMP
  //  allSensors[19] = COOL_TEMP
  //  allSensors[20] = MAP
  //  allSensors[21] = MAT
  //  allSensors[22] = NEUT
  //  allSensors[23] = LAMBDA1
  //  allSensors[24] = LAMBDA2

  // i2c
  //  allSensors[25] = accel;
  //  allSensors[26] = gyro;
  //  allSensors[27] = gps;
  allSensors[28] = convertSensor(ads1115b.readADC_Differential_0_1());
  allSensors[29] = convertSensor(ads1115b.readADC_Differential_2_3());
  allSensors[30] = convertSensor(ads1115c.readADC_Differential_0_1());
  allSensors[31] = convertSensor(ads1115c.readADC_Differential_2_3());
  allSensors[32] = convertSensor(analogRead(PTUBE1_PIN));
  allSensors[33] = convertSensor(analogRead(PTUBE2_PIN));
  allSensors[34] = convertSensor(analogRead(PTUBE3_PIN));
  allSensors[35] = convertSensor(analogRead(PTUBE4_PIN));
  allSensors[36] = convertSensor(analogRead(PTUBE5_PIN));
  allSensors[37] = convertSensor(analogRead(PTUBE6_PIN));
  allSensors[38] = convertSensor(analogRead(PTUBE7_PIN));
  allSensors[39] = convertSensor(analogRead(PTUBE8_PIN));
  allSensors[40] = convertSensor(analogRead(PTUBE9_PIN));
  allSensors[41] = convertSensor(analogRead(PTUBE10_PIN));
  allSensors[42] = convertSensor(ads1115a.readADC_SingleEnded(0));
  allSensors[43] = convertSensor(ads1115a.readADC_SingleEnded(0));
}
// saves to sd card
void saveData(){
  if (SD.exists(fileName)) { // check the card is still there
  // now append new data file
    sensorData = SD.open(fileName, FILE_WRITE);
    if (sensorData){
      sensorData.println(dataString);
      sensorData.close(); // close the file
    }
  } else {
    Serial.println("Error writing to file !");
  }
}

// pulls all analog values and compiles into CSV string
void compileCurData(){
  analogSensors();
  // convert to CSV
  dataString = "";
  for (int i = 0; i < sizeof(allSensors)/sizeof(int); i = i + 1) {
    dataString = dataString + "," + String(allSensors[i]);
  } 
}


void setup() {
  // Open serial communications
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  pinMode(CSpin, OUTPUT);
  //
  // see if the card is present and can be initialized
  if (!SD.begin(CSpin)) {
  Serial.println("Card failed/not found");
  // stop
  return;
  }
  Serial.println("card initialized.");

  pinMode(FL_VSS_PIN, INPUT);
  pinMode(FR_VSS_PIN, INPUT);

  //   get current version for file name and then update version
  int dataVer;
  File sensorDataVer = SD.open("VerTrack.txt");
  if (sensorDataVer) {
        dataVer = sensorDataVer.read();
  } else {
    Serial.println("file unavailable");
  }
  sensorDataVer.close();
  sensorDataVer = SD.open("VerTrack.txt", O_RDWR);
  sensorDataVer.write(dataVer+1);
  sensorDataVer.close();
  fileName = "data" + String(dataVer);

}


void loop() {
  currentTime = millis();
  // this is kinda ugly but i think it will do the job lul 
//
//  // check for digital reading every millisecond
//  if (currentTime - previousTimeDigital > 1){
//    previousTimeDigital = currentTime;
//    // get digital reading and manipulate it in main loop (can be abstracted into digitalSensor() )
//    digitalSensor();
//    if (digitalReading == 0){
//      int diff = currentTime - lastDigReadTime;
//      wheelSpeed = wheelCirc/diff;
//      // do something with this value 
//    }
//  }
  
//  run checks for digital sensors every single loop, check for reading of 0
  digitalSensors(currentTime - lastDigReadTime);

//  check for analog reading every second
  if (currentTime - previousTimeAnalog > 1000){
    previousTimeAnalog = currentTime;
    compileCurData();
    saveData();
  }
  
}
