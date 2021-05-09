// saves to sd card
void saveData(){
  if (SD.exists(fileName)) { // check the card is still there
  // now append new data file
    sensorData = SD.open(fileName, FILE_WRITE);
    if (sensorData){
      sensorData.println(dataString);
      sensorData.close(); // close the file
    }
    digitalWrite(spare1_r, HIGH);
    digitalWrite(spare1_y, HIGH);
    digitalWrite(spare1_g, LOW);
  } else {
    Serial.println("Error writing to file !");
    digitalWrite(spare1_r, LOW);
    digitalWrite(spare1_y, LOW);
    digitalWrite(spare1_g, HIGH);
  }
}

// pulls all analog values and compiles into CSV string
void compileCurData(){
  digitalWrite(27, HIGH); // y off
  digitalWrite(28, LOW); //g on
  analogSensors();
  // convert to CSV
  dataString = "";
  for (int i = 0; i < sizeof(allSensors)/sizeof(float); i++) {
    dataString = dataString + String(allSensors[i]) + ",";
  }
}
