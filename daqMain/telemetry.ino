// writes sensor values to xbee
void writeXbee(){
  const int len = 36;
  // significant digits of each sensor values
  int sensorSig[len] = {0, 0, 1, 2, 0, 0, 1, 2,
                        1, 1, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 3, 3, 3, 2, 1,
                        0, 0, 0, 0, 0, 0, 0, 0,
                        4, 1, 2, 1}; 
  short shortSensorData[len];
  short id = -32767;

  // assumes the first 36 elements matches the python side of sensors
  for (int i= 0; i < len; i++){
    // does not round yet
    shortSenData[i] = short(allSensors[i] * pow(10,sigList[i])); // may not need to use a list
    xbee.write(highByte(shortSenData[i]));
    xbee.write(lowByte(shortSenData[i]));
  }
  xbee.write(highByte(id));
  xbee.write(lowByte(id));
}
