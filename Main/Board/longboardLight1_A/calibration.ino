/*----------------------------calibration----------------------------*/

void quickCalibration() {
  #ifdef DEBUG
    Serial.print(F("Quick calibration started"));
    Serial.println();
  #endif
  quickCalibrateMPU6050Gyro();
  _doQuickCalibration = false;
  #ifdef DEBUG
    Serial.print(F("Quick calibration done"));
    Serial.println();
  //sendCalibrationDataDump();
  #endif
}

void fullCalibration() {
  #ifdef DEBUG
    Serial.print(F("Full calibration started"));
    Serial.println();
  #endif
  calibrateMPU6050();
  _doFullCalibration = false;
  #ifdef DEBUG
    Serial.print(F("Full calibration done"));
    Serial.println();
  //sendSettingsDataDump();
  #endif
}

//

void quickCalibrateMPU6050Gyro() {
  /* gets average current reading and saves as zero point
   *  we only need to do the gyro
   * ..so it ideally needs to have been running for a few seconds
   * but as this quick save is intended just to compensate for slight 
   * drift whilst out riding, it will have been running a while anyway.
   * prob 100 samples
   * ..could loop a few times and average that aswell.. 
   * still wouldn't notice the time
   */
  doMPU6050ReadAverage();         //..see 'MPU6050' tab..
  for (int i = 0; i < 3; i++) {
     _mpu6050AccelZero[i] = _mpu6050AccelReadAverage[i];  //used in orientation
     _mpu6050GyroZero[i] = _mpu6050GyroReadAverage[i];    //used at start of MPU6050 filtering loop
  }
  //these then feed back to the start of the MPU6050 filtering stack to correct the initial readings.
}



