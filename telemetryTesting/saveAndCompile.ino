// saves to sd card
void saveData(){
//  if (SD.exists(fileName)) { // check the card is still there
//  // now append new data file
//    sensorData = SD.open(fileName, FILE_WRITE);
//    if (sensorData){
//      sensorData.println(dataString);
//      sensorData.close(); // close the file
//    }
//    digitalWrite(led_r, HIGH);
//    digitalWrite(led_y, HIGH);
//    digitalWrite(led_g, LOW);
//  } else {
//    Serial.println("Error writing to file !");
//    digitalWrite(led_r, LOW);
//    digitalWrite(led_y, LOW);
//    digitalWrite(led_g, HIGH);
//  }
//}
//
//// pulls all analog values and compiles into CSV string
//void compileCurData(){
//  analogSensors();
//  // convert to CSV
//  dataString = "";
//  for (int i = 0; i < sizeof(allSensors)/sizeof(int); i = i + 1) {
//    dataString = String(allSensors[i]) + " ,";
//  } 
}
