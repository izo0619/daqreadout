/*
SD CARD SHIELD TESTING CODE

IN ORDER FOR THIS CODE TO WORK, THERE MUST BE A FILE CALLED "VerTrack.txt" with some number in it.

WHAT THE CODE SHOULD DO:
Write to a CSV file like so:

Digital  Analog1 Analog2
0.00  2.00  100.00
1.00  4.00  102.00
58945.00  6.00  104.00
125357.00 8.00  106.00
190720.00 10.00 108.00
255796.00 12.00 110.00
319031.00 14.00 112.00
381989.00 16.00 114.00
445006.00 18.00 116.00
507953.00 20.00 118.00



*/

#include "SD.h"
#include"SPI.h"

//GLOBALS
const int CSpin = 53;
String dataString ="";
String fileName;
File sensorData;
float digitalReading = 0;
unsigned long previousTimeDigital = millis();
unsigned long previousTimeAnalog = millis();

// leds for detecting sd card
int led_r = 39;
int led_g = 40;
int led_y = 41;

// initial values for sensor values
float allSensors[3]={digitalReading, 2, 100};
       
//FUNCTIONS

// fake digital sensor
void digitalSensor(){
  digitalReading = digitalReading + 1.0;
  allSensors[0] = digitalReading;
}


void analogSensors(){
  for (int i = 1; i < sizeof(allSensors)/sizeof(float); i = i + 1) {
    allSensors[i] = allSensors[i]+2;
    }
  };


// saves to sd card
void saveData(){
  // reopen file
  sensorData = SD.open(fileName, FILE_WRITE);
    if (sensorData){
     // print line into csv
      sensorData.println(dataString);
      sensorData.close();
      Serial.println(dataString);
    }
  } else {
    Serial.println("Error saving values to file !");
    digitalWrite(led_r, LOW);
    digitalWrite(led_y, LOW);
    digitalWrite(led_g, HIGH);
  }
}

// pulls all analog values and compiles into CSV string
void compileCurData(){
  analogSensors();
  // convert to CSV
  dataString = "";
  for (int i = 0; i < sizeof(allSensors)/sizeof(float); i = i + 1) {
    dataString = dataString + String(allSensors[i]) + ",";
  }
}


void setup() {
  // Open serial communications
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  pinMode(CSpin, OUTPUT);
  //
  
  // declare led pins as outputs
  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_y, OUTPUT);
  
  // see if the card is present and can be initialized
  if (!SD.begin(CSpin)) {
    Serial.println("Card failed/not found");
    digitalWrite(led_r, LOW);
    digitalWrite(led_y, HIGH);
    digitalWrite(led_g, HIGH);
    
    // stop the program
    return;
  
  } else {
    Serial.println("card initialized.");
    // stop for a second and set leds to green
    delay(1000);
    digitalWrite(led_r, HIGH);
    digitalWrite(led_y, HIGH);
    digitalWrite(led_g, LOW);
    
    //   get current version # for file name and then update version #
    int dataVer;
    File sensorDataVer = SD.open("VerTrack.txt");
    if (sensorDataVer) {
          dataVer = sensorDataVer.read();
    } else {
      // if version file is available, light orange leds and set the dataVer to 0 to log something
      dataVer = 0;
      Serial.println("File unavailable");
      digitalWrite(led_r, LOW);
      digitalWrite(led_y, LOW);
      digitalWrite(led_g, HIGH);
      delay(2000);
    }
    sensorDataVer.close();

    // open the file again and write a new number over the original
    sensorDataVer = SD.open("VerTrack.txt", O_RDWR);
    sensorDataVer.write(dataVer+1);
    sensorDataVer.close();

    // set our new file name to data#.csv
    fileName = "data" + String(dataVer) + ".csv";
    
    // write headers
    sensorData = SD.open(fileName, FILE_WRITE);
     if (sensorData){
        Serial.println("Digital, Analog1, Analog2");
        sensorData.println("Digital, Analog1, Analog2");
      } else {
      Serial.println("Error writing headers to file !");
      digitalWrite(led_r, LOW);
      digitalWrite(led_y, LOW);
      digitalWrite(led_g, HIGH);
      delay(2000);
    }
    sensorData.close();

    // save initial values
    sensorData = SD.open(fileName, FILE_WRITE);
    for (int i = 0; i < sizeof(allSensors)/sizeof(float); i = i + 1) {
      dataString = dataString + String(allSensors[i]) + ",";
    }
    saveData();
    sensorData.close(); // close the file
  }
}

void loop() {
  unsigned long currentTime = millis();
  
  // run checks for digital sensors every single loop, check for reading of 0
  if (currentTime < 10000){
    
  digitalSensor();

  // check for analog reading every second
  if (currentTime - previousTimeAnalog > 1000){
    previousTimeAnalog = currentTime;
    compileCurData();
    saveData();
  }
  }
  
}
