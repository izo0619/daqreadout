void digitalSensors(){
  // wheel speed
  FL_VSS = digitalRead(FL_VSS_PIN);
  FR_VSS = digitalRead(FR_VSS_PIN);
  if (FL_VSS == 0){
      diff = currentTime - FL_VSS_LastRead;
      wheelSpeed = wheelCirc/diff;
      allSensors[0] = wheelSpeed;
      FL_VSS_LastRead = currentTime;
  }
  if (FR_VSS == 0){
      diff = currentTime - FR_VSS_LastRead;
      wheelSpeed = wheelCirc/diff;
      allSensors[1] = wheelSpeed;
      FR_VSS_LastRead = currentTime;
  }
  // REPEAT WITH BACK WHEEL SPEEDS FROM MOTEC
  if (BL_VSS == 0){
      diff = currentTime - BL_VSS_LastRead;
      wheelSpeed = wheelCirc/diff;
      allSensors[2] = wheelSpeed;
      BL_VSS_LastRead = currentTime;
  }
  if (BR_VSS == 0){
      diff = currentTime - BR_VSS_LastRead;
      wheelSpeed = wheelCirc/diff;
      allSensors[3] = wheelSpeed;
      BR_VSS_LastRead = currentTime;
  }
}
