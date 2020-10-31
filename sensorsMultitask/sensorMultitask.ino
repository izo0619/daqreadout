
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

//GLOBALS
const int CSpin = 10;
String dataString =""; // holds the data to be written to the SD card
String fileName = "data.csv";
File sensorData;
int digitalReading = 0;
int lastDigReadTime = 0;
int analogReading1 = 0;
int analogReading2 = 0;
unsigned long previousTimeDigital = millis();
unsigned long previousTimeAnalog = millis();
int allSensors[24];

// SENSOR GLOBALS
int sensorVoltage = 0;
int systemVoltage = 0;
int resolution = 65535;
int brakeTempReading = 0;
int brakePressureReading = 0;
int intakeAirReading = 0;
int oilPressureReading = 0;
                
//FUNCTIONS

// fake digital sensor
void digitalSensor(){
  digitalReading = digitalReading + 1;
  Serial.print("Digital Output:");
  Serial.println(digitalReading);
}
// fake analog sensors
void analogSensor1(){
  analogReading1 = analogReading1 + 1;
  Serial.print("Analog 1 Output:");
  Serial.println(analogReading1);
}
void analogSensor2(){
  analogReading2 = analogReading2 + 1;
  Serial.print("Analog 2 Output:");
  Serial.println(analogReading2);
}

// real sensors
int convertSensor(int sensorValue){
  sensorVoltage = sensorValue * (systemVoltage/resolution);
  return sensorVoltage;
}
void analogSensors(){
  // arbituary values to be received thru i2c
  brakeTempReading = 30;
  
  
  allSensors[2] = (convertSensor(brakeTempReading)-0.4)/0.03;
  allSensors[4] = (convertSensor(brakePressureReading)-0.5)*1875;
  allSensors[6] = 11 + ((convertSensor(intakeAirReading)-0.5)*74);
  allSensors[8] = (convertSensor(oilPressureReading)-0.5)*25;
  
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
  analogSensor1();
  analogSensor2();
  // convert to CSV
  for (int i = 0; i < sizeof(allSensors)/sizeof(int); i = i + 1) {
    dataString = dataString + "," + String(allSensors[i]) + ",";
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
}


void loop() {
  unsigned long currentTime = millis();
  // this is kinda ugly but i think it will do the job lul 

  // check for digital reading every millisecond
  if (currentTime - previousTimeDigital > 1){
    previousTimeDigital = currentTime;
    // get digital reading and manipulate it in main loop (can be abstracted into digitalSensor() )
    digitalSensor();
    if (digitalReading == 1){
      int diff = currentTime - lastDigReadTime;
      // do something with this value 
      compileCurData();
      saveData();
    }
  }

  // check for analog reading every second
  if (currentTime - previousTimeAnalog > 1000){
    previousTimeAnalog = currentTime;
    compileCurData();
    saveData();
  }
  
}
