void analogSensors(){
  
  allSensors[0] = (convertSensor(analogRead(FL_BRK_TMP_PIN)) - 0.50)/0.005;
  allSensors[1] = (convertSensor(analogRead(FR_BRK_TMP_PIN)) - 0.50)/0.005;
  allSensors[2] = (convertSensor(analogRead(F_BRK_PRES_PIN), F_BRK_PRES_CLB) - 0.50)/ (4/7500);
}
