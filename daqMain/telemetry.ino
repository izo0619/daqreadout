// writes sensor values to xbee
void writeXbee(){
  // excluding strain gauges and pitot tubes
  const int len = 27;
  // significant digits of each sensor values, temporary
  int sensorSig[len] = {1, 1, 1, 1, 1, 1, 1, 1,
                        1, 1, 1, 1, 1, 1, 1, 1,
                        1, 1, 1, 1, 1, 1, 1, 1,
                        1, 1, 1}; 
  short shortSensorData[len];
  short id = -32767;

  // assumes the first 36 elements matches the python side of sensors
  for (int i= 0; i < len; i++){
    // does not round yet
    shortSensorData[i] = short(allSensors[i] * pow(10,sensorSig[i])); // may not need to use a list
    xbee.write(highByte(shortSensorData[i]));
    xbee.write(lowByte(shortSensorData[i]));
  }
  xbee.write(highByte(id));
  xbee.write(lowByte(id));
}
