/* DAQ MAIN CODE
 *  subroutines: analogSensors, digitalSensors, saveAndCompile
 */
/* TODO
 *  pull data from I2C bus
 *  pull data from MOTEC/CAN
 *  write functions for manipulating data:
 *    Non testing:
 *      lambdas, wheel speed (DIG) , brake temp, shock potent, brake pressure, throttle pos
 *         manifold ap, intake manifold at, oil pressure, cam angle (DIG)
 *      coolant temp, oil temp, crank angle
 *    Testing:
 *      steering angle, pitot tubes, strain gauges, accelerometer, gyroscope, thermocouple
 */

/*Tom's test comment*/

#include "SD.h"
#include"SPI.h"
#include "MPU9250.h"
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

//LED pin compile time constants
#define dash_r 4
#define dash_g 5
#define dash_y 6
#define lambdas_r 7
#define lambdas_g 8
#define lambdas_y 9
#define braketemp_r 10
#define dash_y 11
#define dash_g 12
#define dash_r 13
#define braketemp_g 14
#define braketemp_y 15
#define brakepress_r 19
#define brakepress_g 22
#define brakepress_y 23
#define intakepress_r 24
#define intakepress_g 25
#define intakepress_y 26
#define intaketemp_r 27
#define intaketemp_g 28
#define intaketemp_y 19
#define coolanttemp_r 30
#define coolanttemp_g 31
#define coolanttemp_y 32
#define oilpress_r 33
#define oilpress_g 34
#define oilpress_y 35
#define oiltemp_r 36
#define oiltemp_g 37
#define oiltemp_y 38
#define spare1_r 39
#define spare1_g 40
#define spare1_y 41
#define spare2_r 42
#define spare2_g 43
#define spare2_y 44
#define daq_r 45
#define daq_g 46
#define daq_y 47

SoftwareSerial xbee(0,1);

Adafruit_ADS1115 ads1115a(0x48);
Adafruit_ADS1115 ads1115b(0x49);
Adafruit_ADS1115 ads1115c(0x4A);

MPU9250 IMU(Wire,0x68);
//GLOBALS
const int CSpin = 49;
String dataString =""; // holds the data to be written to the SD card
String fileName;
File sensorData;
int readFrequency = 100; // CHANGE THIS TO READ FASTER/SLOWER (units in ms)

//Debug and testing
bool enableSerialMessages = false;

unsigned long previousTimeDigital = millis();
unsigned long previousTimeAnalog = millis();
unsigned long currentTime;
unsigned long FL_VSS_LastRead,FR_VSS_LastRead, BL_VSS_LastRead, BR_VSS_LastRead, diff = millis();

// SENSOR ARRAYS
float allSensors[54];

/*// SENSOR GLOBALS
int sensorVoltage = 0;
int systemVoltage = 5;
int resolution = 1024;*/

// Wheel Speed (should use interrupts if possible so that we don't miss pulses)
int wheelCirc = 3.24*2*8;
int wheelSpeed = 0;
int FL_VSS_PIN = 2;
int FR_VSS_PIN = 3;
float FL_VSS, FR_VSS, BL_VSS, BR_VSS;

// Brake Temperature
int FL_BRK_TMP_PIN = A0;
int FR_BRK_TMP_PIN = A1;
//float FL_BRK_TMP, FR_BRK_TMP,BL_BRK_TMP,BR_BRK_TMP;

// Suspension Potentiometer
int FL_SUS_POT_PIN = A2;
int FR_SUS_POT_PIN = A3;
//float FL_SUS_POT, FR_SUS_POT, BL_SUS_POT, BR_SUS_POT;

// Brake Pressure
int F_BRK_PRES_PIN = A4;
//float F_BRK_PRES = 0;
//float B_BRK_PRES = 0;

// Steer Angle
int STEER_ANG_PIN = A5;
//float STEER_ANG = 0;

// Rest of Motec Reads
//float TPS, OIL_PRES,OIL_TEMP,COOL_TEMP, MAP, MAT, NEUT, LAMBDA1, LAMBDA2;

//  Rest of ADC reads
//float ACCEL = 0;
//float GYRO = 0;
//float GPS = 0;
//float STRAIN1, STRAIN2, STRAIN3, STRAIN4;

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
//float PTUBE1, PTUBE2, PTUBE3, PTUBE4, PTUBE5, PTUBE6, PTUBE7, PTUBE8, PTUBE9, PTUBE10, PTUBE11, PTUBE12;


// OFFSETS
float PTUBE_CLB, STRAIN1_CLB, STRAIN2_CLB, STRAIN3_CLB, STRAIN4_CLB, STEER_ANG_CLB, TPS_CLB, F_BRK_PRES_CLB, B_BRK_PRES_CLB, FL_SUS_POT_CLB, FR_SUS_POT_CLB, BL_SUS_POT_CLB, BR_SUS_POT_CLB;


/*float convertSensor(int sensorValue, int calibration=0);
// sensor value from 0 to 2^16 and returns a voltage between 0 and 5 V
float convertSensor(int sensorValue, int calibration=0){
  float sensorVoltage = (sensorValue * ((float)systemVoltage/resolution)) - calibration;
  return sensorVoltage;
}*/ //moved to dimensionalizeSensors

//Function prototypes from dimensionalizeSensors
float dimensionalizeStrainGuage(float raw, float offset = 0);
float dimensionalizeAdsADC(float raw, float offset = 0);
float dimensionalizeMegaADC(float raw, float offset = 0);
float dimensionalizeBrakeTemp(float raw, float offset = 0);
float dimensionalizeSteeringAngle(float raw, float offset = 0);
float dimensionalizeBrakePress(float raw, float offset = 0);


void setup() {
  // Open serial communications
  
  if(enableSerialMessages){
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  } else {
  xbee.begin(9600);
  }
  
  for (int i = 4; i <= 15; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);

  for (int i = 22; i <= 47; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  can_setup();
  
  if(enableSerialMessages){Serial.print("CAN setup complete");}
  pinMode(CSpin, OUTPUT);
  //

  // declare led pins as outputs
  pinMode(spare1_r, OUTPUT);
  pinMode(spare1_g, OUTPUT);
  pinMode(spare1_y, OUTPUT);

  int imuStatus = IMU.begin();

  if (imuStatus < 0) {
    digitalWrite(lambdas_r, LOW);
  }

  //Set external sensor ranges

  //Set IMU accelerometer range to +-4G
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_4G);

  //Set IMU gyroscope range
  IMU.setGyroRange(MPU9250::GYRO_RANGE_500DPS);

  //Set IMU digital low pass filter bandwith (can be 5, 10, 20, 41, 92, or 184 Hz
  //defaults to no filtering
  //IMU.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_20HZ);

  //Set IMU refresh rate (probably defaults to 32kHz for gyro and temp, 4kHz for accel, 100Hz (cannot be changed) for magnetometer)
  //Refresh rate for gyro, temp, and accel = 1000/(1+SRD)
  //IMU.setSrd(19); //50Hz

  //set external ADC for strain guages gain to max
  ads1115b.setGain(GAIN_SIXTEEN);
  ads1115c.setGain(GAIN_SIXTEEN);
  
  //Set other external ADC gain to min
  ads1115a.setGain(GAIN_TWOTHIRDS);
  
  // see if the card is present and can be initialized
  if (!SD.begin(CSpin)) {
  if(enableSerialMessages){Serial.println("Card failed/not found");}
  digitalWrite(spare1_r, LOW);
  digitalWrite(spare1_y, HIGH);
  digitalWrite(spare1_g, HIGH);
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
    digitalWrite(spare1_r, LOW);
    digitalWrite(spare1_y, LOW);
    digitalWrite(spare1_g, HIGH);
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
      sensorData.println("FL_VSS,FR_VSS,BL_VSS,BR_VSS,FL_BRK_TMP,FR_BRK_TMP,BL_BRK_TMP,BR_BRK_TMP,FL_SUS_POT,FR_SUS_POT,BL_SUS_POT,BR_SUS_POT,F_BRK_PRES,B_BRK_PRES,STEER_ANG,TPS,OIL_PRES,OIL_TEMP,COOL_TEMP,MAP,MAT,NEUT,LAMBDA1,LAMBDA2,ACCELX,ACCELY,ACCELZ,STRAIN1,STRAIN2,STRAIN3,STRAIN4,PTUBE1,PTUBE2,PTUBE3,PTUBE4,PTUBE5,PTUBE6,PTUBE7,PTUBE8,PTUBE9,PTUBE10,PTUBE11,PTUBE12,GYROX,GYROY,GYROZ,MAGNETX,MAGNETY,MAGNETZ,DAQTEMP");
      sensorData.close(); // close the file
      if(enableSerialMessages){Serial.println("First line of file written");}
    } else {
    if(enableSerialMessages){Serial.println("Error writing to file !");}
    digitalWrite(spare1_r, LOW);
    digitalWrite(spare1_y, LOW);
    digitalWrite(spare1_g, HIGH);
    delay(2000);
  }

  // set calibration variables ** these are in voltage
  FL_SUS_POT_CLB = dimensionalizeMegaADC(analogRead(FL_SUS_POT_PIN));
  FR_SUS_POT_CLB = dimensionalizeMegaADC(analogRead(FR_SUS_POT_PIN));
  //  BL_SUS_POT_CLB = back l suspot
  //  BR_SUS_POT_CLB = back r suspot
  F_BRK_PRES_CLB = dimensionalizeMegaADC(analogRead(F_BRK_PRES_PIN));
  //  B_BRK_PRES_CLB = back brk pres
  STEER_ANG_CLB = dimensionalizeMegaADC(analogRead(STEER_ANG_PIN));
  //  TPS_CLB = TPS
  STRAIN1_CLB = dimensionalizeStrainGuage(ads1115b.readADC_Differential_0_1());
  STRAIN2_CLB = dimensionalizeStrainGuage(ads1115b.readADC_Differential_2_3());
  STRAIN3_CLB = dimensionalizeStrainGuage(ads1115c.readADC_Differential_0_1());
  STRAIN4_CLB = dimensionalizeStrainGuage(ads1115c.readADC_Differential_2_3());
  PTUBE_CLB = dimensionalizeMegaADC(analogRead(PTUBE1_PIN));
  digitalWrite(spare1_r, HIGH);

  if(enableSerialMessages){Serial.print("Setup complete");}
  //weird bad hacky interrupt fix
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  pinMode(2, INPUT);
  pinMode(2, INPUT_PULLUP);
}

void loop() {
  currentTime = millis();

//  run checks for digital sensors every single loop, check for reading of 0

  digitalSensors();
  if (can_ready()) {
    can_getvalue();
  }
  
//  check for analog reading every second
//  frequency of change of data
  if (currentTime - previousTimeAnalog > readFrequency){
    previousTimeAnalog = currentTime;

    compileCurData();
    digitalWrite(braketemp_r, LOW); // y on
    digitalWrite(braketemp_g, HIGH);
    digitalWrite(braketemp_y, HIGH);
    saveData();
    digitalWrite(braketemp_y, LOW); //g on
    digitalWrite(braketemp_g, HIGH);
    digitalWrite(braketemp_r, HIGH);

    // for now write to xbee every tenth of a second
    writeXbee();
    digitalWrite(braketemp_g, LOW); //y on
    digitalWrite(braketemp_y, HIGH);
    digitalWrite(braketemp_r, HIGH);
    
  }
 
}
