void analogSensors(){

  digitalWrite(27, LOW); // y on
  digitalWrite(28, HIGH); //g off
  
  allSensors[4] = dimensionalizeBrakeTemp(analogRead(FL_BRK_TMP_PIN));
  allSensors[5] = dimensionalizeBrakeTemp(analogRead(FR_BRK_TMP_PIN));
  allSensors[6] = can_read(4); // back l brake temp
  allSensors[7] = can_read(5); // back r brake temp
  allSensors[8] = dimensionalizeMegaADC(analogRead(FL_SUS_POT_PIN), FL_SUS_POT_CLB);
  allSensors[9] = dimensionalizeMegaADC(analogRead(FR_SUS_POT_PIN), FR_SUS_POT_CLB);
  //  allSensors[10] = back l suspot
  //  allSensors[11] = back r suspot
  allSensors[12] = dimensionalizeMegaADC(analogRead(F_BRK_PRES_PIN), F_BRK_PRES_CLB);
  allSensors[13] = can_read(1); // back brk pres
  allSensors[14] = dimensionalizeSteeringAngle(analogRead(STEER_ANG_PIN), STEER_ANG_CLB);
  // motec
  allSensors[15] = can_read(31); // TPS
  allSensors[16] = can_read(20); // OIL_PRES
  allSensors[17] = can_read(21); // OIL_TEMP
  allSensors[18] = can_read(53); // COOL_TEMP
  allSensors[19] = can_read(51); // MAP
  allSensors[20] = can_read(52); // MAT
  allSensors[21] = can_read(30); // NEUT
  allSensors[22] = can_read(40); // LAMBDA1
  allSensors[23] = can_read(41); // LAMBDA2

  // imu
  IMU.readSensor();
  allSensors[24] = IMU.getAccelX_mss()/9.81;
  allSensors[25] = IMU.getAccelY_mss()/9.81;
  allSensors[26] = IMU.getAccelZ_mss()/9.81;

  // strain gauges
  allSensors[27] = dimensionalizeStrainGuage(ads1115b.readADC_Differential_0_1(), STRAIN1_CLB);
  allSensors[28] = dimensionalizeStrainGuage(ads1115b.readADC_Differential_2_3(), STRAIN2_CLB);
  allSensors[29] = dimensionalizeStrainGuage(ads1115c.readADC_Differential_0_1(), STRAIN3_CLB);
  allSensors[30] = dimensionalizeStrainGuage(ads1115c.readADC_Differential_2_3(), STRAIN4_CLB);

  allSensors[31] = dimensionalizeMegaADC(analogRead(PTUBE1_PIN), PTUBE_CLB);
  allSensors[32] = dimensionalizeMegaADC(analogRead(PTUBE2_PIN), PTUBE_CLB);
  allSensors[33] = dimensionalizeMegaADC(analogRead(PTUBE3_PIN), PTUBE_CLB);
  allSensors[34] = dimensionalizeMegaADC(analogRead(PTUBE4_PIN), PTUBE_CLB);
  allSensors[35] = dimensionalizeMegaADC(analogRead(PTUBE5_PIN), PTUBE_CLB);
  allSensors[36] = dimensionalizeMegaADC(analogRead(PTUBE6_PIN), PTUBE_CLB);
  allSensors[37] = dimensionalizeMegaADC(analogRead(PTUBE7_PIN), PTUBE_CLB);
  allSensors[38] = dimensionalizeMegaADC(analogRead(PTUBE8_PIN), PTUBE_CLB);
  allSensors[39] = dimensionalizeMegaADC(analogRead(PTUBE9_PIN), PTUBE_CLB);
  allSensors[40] = dimensionalizeMegaADC(analogRead(PTUBE10_PIN), PTUBE_CLB);
  allSensors[41] = dimensionalizeAdsADC(ads1115a.readADC_SingleEnded(0));
  allSensors[42] = dimensionalizeAdsADC(ads1115a.readADC_SingleEnded(0));
  allSensors[43] = IMU.getGyroX_rads();
  allSensors[44] = IMU.getGyroY_rads();
  allSensors[45] = IMU.getGyroZ_rads();
  allSensors[46] = IMU.getMagX_uT();
  allSensors[47] = IMU.getMagY_uT();
  allSensors[48] = IMU.getMagZ_uT();
  allSensors[49] = IMU.getTemperature_C();
  allSensors[51] = can_read(50);
}
