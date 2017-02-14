/*----------------------------calibration----------------------------*/

void quickCalibration() {
  if (_doQuickCalibration == true) {
    quickCalibrateMPU6050Gyro();
    _doQuickCalibration = false;
  }
}

void fullCalibration() {
  if (_doFullCalibration == true) {
    calibrateMPU6050();
    _doFullCalibration = false;
  }
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
  doMPU6050ReadAverage();
  for (int i = 0; i < 3; i++) {
     _mpu6050AccelZero[i] = _mpu6050AccelReadAverage[i];  //used in orientation
     _mpu6050GyroZero[i] = _mpu6050GyroReadAverage[i];    //used at start of MPU6050 filtering loop
  }
  //these then feed back to the start of the MPU6050 filtering stack to correct the initial readings.
}
