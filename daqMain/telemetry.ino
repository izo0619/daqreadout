// writes sensor values to xbee
void writeXbee(){
  digitalWrite(30, HIGH); //r off
  digitalWrite(31, LOW); //g on
  digitalWrite(32, HIGH); //y off
  // excluding strain gauges and pitot tubes
  const int len = 54; // 50 + 4 for non sensor values
  // significant digits of each sensor values, temporary
  // used later if different sig 
  /*sensorSig {1, 1, 1, 1, 1, 1, 1, 1,
                        1, 1, 1, 1, 1, 1, 1, 1,
                      1, 1, 1, 1, 1, 1, 1, 1,
                        1, 1, 1}*/
                         
  short shortSensorData[len];
  short id = -32767;

  // assumes the first 36 elements matches the python side of sensors
  for (int i= 0; i < len; i++){
    // does not round yet
    
    // skip strain and ptubes
    float temp;
    if((i > 26 && i < 43) || i == 49){
      continue;
    //}else if(i > 49){
      //temp = 0;
    }else{
      temp = allSensors[i];
    }
    
    // pow 1 for now, use sensorSig if needed
    shortSensorData[i] = short(temp * pow(10,1)); // may not need to use a list
    xbee.write(highByte(shortSensorData[i]));
    xbee.write(lowByte(shortSensorData[i]));
    digitalWrite(30, HIGH); //r off
    digitalWrite(31, HIGH); //g off
    digitalWrite(32, LOW); //y on
  }
  xbee.write(highByte(id));
  xbee.write(lowByte(id));
}
