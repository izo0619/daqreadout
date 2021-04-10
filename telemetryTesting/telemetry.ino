// writes sensor values to xbee
void writeXbee(){
  // excluding strain gauges and pitot tubes
  // significant digits of each sensor values
  //int sensorSig[sensorsLen] = {0, 3, 1, 2, 0, 0, 1, 2,
  //                      1, 1, 0, 0, 0, 0, 0, 0,
  //                      0, 0, 0, 3, 3, 3, 2, 1,
  //                      0, 0}; 
  int sensorSig[sensorsLen] = {0,0,0};
  short shortSensorData[sensorsLen];
  short id = -32767;
  Serial.println(allSensors[1]);
  // assumes the first 36 elements matches the python side of sensors
  for (int i= 0; i < sensorsLen; i++){
    // does not round yet
    shortSensorData[i] = short(allSensors[i] * pow(10,sensorSig[i])); // may not need to use a list
    xbee.write(highByte(shortSensorData[i]));
    xbee.write(lowByte(shortSensorData[i]));
  }
  xbee.write(highByte(id));
  xbee.write(lowByte(id));
}
