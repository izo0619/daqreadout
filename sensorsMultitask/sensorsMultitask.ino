#include <TimedAction.h>

/*
Sensor Multitasking
- checking for multiple sensors at different time frames
- digital sensors should be checked in the main loop/using timed action with a very small time
- analog sensors can be called every 1000ms?
*/


//THIRD-PARTY LIBRARIES
//these must be manually added to your Arduino IDE installation

//TimedAction
//allows us to set actions to perform on separate timed intervals
//http://playground.arduino.cc/Code/TimedAction
//http://wiring.uniandes.edu.co/source/trunk/wiring/firmware/libraries/TimedAction

#include <TimedAction.h>
//NOTE: This library has an issue on newer versions of Arduino. After
//      downloading the library you MUST go into the library directory and
//      edit TimedAction.h. Within, overwrite WProgram.h with Arduino.h


//GLOBALS
//int backlightPin = 9;   // used for backlight fading
//
//int timerCounter = 0;   // incrementing counter. will crash eventually.
//int stringNo = 0;       //which text string to show



int digitalCounter = 0;
int analogCounter1 = 0;
int analogCounter2 = 0;
////                   "16 CHARACTER MAX"
//char* stringArray[]={"Check it out... ",
//                     "I have 3 threads",
//                     "going at once...",
//                     "Cool, huh?! :D  "};
//                     
//FUNCTIONS

////this is our first task, print an incrementing number to the LCD
//void incrementNumber(){
//   // set the cursor to column 0, line 1
//  // (note: line 1 is the second row, since counting begins with 0):
//  lcd.setCursor(0, 1);
//  // add one to the counter, then display it.
//  timerCounter = timerCounter + 1;  
//  lcd.print(timerCounter);
//}
//
////our second task, fires every few seconds and rotates text strings
//void changeText(){  
//  // Print a message to the LCD.
//  lcd.setCursor(0, 0);
//  lcd.print(stringArray[stringNo]);
//
//  //nasty hack to get number of Array elements
//  if (stringNo >= sizeof(stringArray)/sizeof(char *)){  
//    stringNo = 0;
//    changeText();
//  }
//  else{
//    stringNo = stringNo + 1;  
//  }
//}

// fake digital sensor
void digitalSensor(){
  digitalCounter = digitalCounter + 1;
  Serial.print("Digital Output:");
  Serial.println(digitalCounter);
}

// fake analog sensors
void analogSensor1(){
  analogCounter1 = analogCounter1 + 1;
  Serial.print("Analog 1 Output:");
  Serial.println(analogCounter1);
}
void analogSensor2(){
  analogCounter2 = analogCounter2 + 1;
  Serial.print("Analog 2 Output:");
  Serial.println(analogCounter2);
}

//Create a couple timers that will fire repeatedly every x ms

//edit: these lines used to be in front of incrementNumber and changeText
//      functions. this didn't work because the functions weren't defined yet!
//TimedAction numberThread = TimedAction(700,incrementNumber);
//TimedAction textThread = TimedAction(3000,changeText);

TimedAction analogThread1 = TimedAction(2000,analogSensor1);
TimedAction analogThread2 = TimedAction(3000,analogSensor2);

// where's our third task? well, it's the main loop itself :) the task
// which repeats most often should be used as the loop. other
// tasks are able to "interrupt" the fastest repeating task.


void setup() {
//  //define the LCD's number of columns and rows:
//  lcd.begin(16, 2);
//  //fire changeText once to paint the initial string [0]
//  changeText();
}


void loop() {
  
  //check on our threads. based on how long the system has been
  //running, do they need to fire and do work? if so, do it!
  digitalSensor();
  analogThread1.check();
  analogThread2.check();
  
  
}
