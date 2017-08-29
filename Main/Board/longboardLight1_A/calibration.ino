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
  //..mabye wait for a bit to make sure board is flat. but this might not be a problem if you do it wirelessly.
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
//  doMPU6050ReadAverage();         //..see 'MPU6050' tab..
//  for (int i = 0; i < 3; i++) {
//     _mpu6050AccelZero[i] = _mpu6050AccelReadAverage[i];  //used in orientation
//     _mpu6050GyroZero[i] = _mpu6050GyroReadAverage[i];    //used at start of MPU6050 filtering loop
//  }
  //these then feed back to the start of the MPU6050 filtering stack to correct the initial readings.
}

/*----------------------------calibration - bits----------------------------*/

void doMPU6050ReadAverage() {
  //this does not get used for the main filtered loop, only for calibration porpoises
/*  
  _mpu6050.getMotion6(&_mpu6050AccelRead[0], &_mpu6050AccelRead[1], &_mpu6050AccelRead[2], &_mpu6050GyroRead[0], &_mpu6050GyroRead[1], &_mpu6050GyroRead[2]);
  delay(100);  //trying to iron out instability at the beginning
  
  unsigned long accelSampleX = 0;
  unsigned long accelSampleY = 0;
  unsigned long accelSampleZ = 0;
  unsigned long gyroSampleX = 0;
  unsigned long gyroSampleY = 0;
  unsigned long gyroSampleZ = 0;

  //get motion data averages
  for (int i=0; i < _mpu6050CalibrateSampleTotal; i++) {
    _mpu6050.getMotion6(&_mpu6050AccelRead[0], &_mpu6050AccelRead[1], &_mpu6050AccelRead[2], &_mpu6050GyroRead[0], &_mpu6050GyroRead[1], &_mpu6050GyroRead[2]);

    accelSampleX += _mpu6050AccelRead[0];
    accelSampleY += _mpu6050AccelRead[1];
    accelSampleZ += _mpu6050AccelRead[2];
    gyroSampleX += _mpu6050GyroRead[0];
    gyroSampleY += _mpu6050GyroRead[1];
    gyroSampleZ += _mpu6050GyroRead[2];

    delay(2); //..so we don't get repeated measures ???
  }

  _mpu6050AccelReadAverage[0] = (accelSampleX / _mpu6050CalibrateSampleTotal);
  _mpu6050AccelReadAverage[1] = (accelSampleY / _mpu6050CalibrateSampleTotal);
  _mpu6050AccelReadAverage[2] = (accelSampleZ / _mpu6050CalibrateSampleTotal);
  _mpu6050GyroReadAverage[0] = (gyroSampleX / _mpu6050CalibrateSampleTotal);
  _mpu6050GyroReadAverage[1] = (gyroSampleY / _mpu6050CalibrateSampleTotal);
  _mpu6050GyroReadAverage[2] = (gyroSampleZ / _mpu6050CalibrateSampleTotal);

  #ifdef DEBUG_MPU6050
    Serial.print(_mpu6050AccelReadAverage[1]);
    Serial.println();
  #endif

*/
}

void calibrateMPU6050() {
  //called from full calibration - place flat
/*  
  boolean thresholdVers = 1;  //which version of trying to nudge the offsets so everything zeros out do we want to use right now..?
  
  #ifdef DEBUG_MPU6050
    Serial.print("MPU6050 starting calibration: ");
    Serial.println();
  #endif
  
  //sensor_temperature = (_mpu6050.getTemperature() + 12412) / 340; //do later
  
  doMPU6050ReadAverage(); //first averaging read

  if (thresholdVers == 1) {
    //work out initial starting point for offsets
    _mpu6050AccelOffset[0] = -_mpu6050AccelReadAverage[0] / 8;
    _mpu6050AccelOffset[1] = -_mpu6050AccelReadAverage[1] / 8;
    _mpu6050AccelOffset[2] = (16384-_mpu6050AccelReadAverage[2]) / 8;
    _mpu6050GyroOffset[0] = -_mpu6050GyroReadAverage[0] / 4;
    _mpu6050GyroOffset[1] = -_mpu6050GyroReadAverage[1] / 4;
    _mpu6050GyroOffset[2] = -_mpu6050GyroReadAverage[2] / 4;
  }
    
  long mpu6050CalibrateCurMillis = millis();   //get current time
  int allOk = 0;
  
  while (1) {
      
    //set offsets
    _mpu6050.setXAccelOffset(_mpu6050AccelOffset[0]);
    _mpu6050.setYAccelOffset(_mpu6050AccelOffset[1]);
    _mpu6050.setZAccelOffset(_mpu6050AccelOffset[2]);
    _mpu6050.setXGyroOffset(_mpu6050GyroOffset[0]);
    _mpu6050.setYGyroOffset(_mpu6050GyroOffset[1]);
    _mpu6050.setZGyroOffset(_mpu6050GyroOffset[2]);
    
    doMPU6050ReadAverage(); //do an averaged read
      
    if (thresholdVers == 0) {
      
      //nudge read towards motion data using offsets
      for (int i = 0; i < 3; i++) {
        if (_mpu6050AccelReadAverage[i] > 0) _mpu6050AccelOffset[i]--;
        else if (_mpu6050AccelReadAverage[i] < 0) _mpu6050AccelOffset[i]++;
  
        if (_mpu6050GyroReadAverage[i] > 0) _mpu6050GyroOffset[i]--;
        else if (_mpu6050GyroReadAverage[i] < 0) _mpu6050GyroOffset[i]++;
      }
    
      //are we all within thresholds?
      for (int i = 0; i < 3; i++) {
        if (_mpu6050AccelReadAverage[i] <= _mpu6050CalibrateAccelThreshold && _mpu6050AccelReadAverage[i] >= -_mpu6050CalibrateAccelThreshold) { allOk++; }
        if (_mpu6050GyroReadAverage[i] <= _mpu6050CalibrateGyroThreshold && _mpu6050GyroReadAverage[i] >= -_mpu6050CalibrateGyroThreshold) { allOk++; }
      }
      
    } else if (thresholdVers == 1) {
    
      ////are we all within thresholds? ..alt vers
      if (abs(_mpu6050AccelReadAverage[0]) <= _mpu6050CalibrateAccelThreshold) allOk++;
      else _mpu6050AccelOffset[0] = _mpu6050AccelOffset[0] - _mpu6050AccelReadAverage[0] / _mpu6050CalibrateAccelThreshold;
  
      if (abs(_mpu6050AccelReadAverage[1]) <= _mpu6050CalibrateAccelThreshold) allOk++;
      else _mpu6050AccelOffset[1] = _mpu6050AccelOffset[1] - _mpu6050AccelReadAverage[1]/_mpu6050CalibrateAccelThreshold;
  
      if (abs(16384 - _mpu6050AccelReadAverage[2]) <= _mpu6050CalibrateAccelThreshold) allOk++;
      else _mpu6050AccelOffset[2] = _mpu6050AccelOffset[2] + (16384 - _mpu6050AccelReadAverage[2]) / _mpu6050CalibrateAccelThreshold;
  
      if (abs(_mpu6050GyroReadAverage[0])<=_mpu6050CalibrateGyroThreshold) allOk++;
      else _mpu6050GyroOffset[0] = _mpu6050GyroOffset[0] - _mpu6050GyroReadAverage[0] / (_mpu6050CalibrateGyroThreshold + 1);
  
      if (abs(_mpu6050GyroReadAverage[1]) <= _mpu6050CalibrateGyroThreshold) allOk++;
      else _mpu6050GyroOffset[1] = _mpu6050GyroOffset[1] - _mpu6050GyroReadAverage[1] / (_mpu6050CalibrateGyroThreshold + 1);
  
      if (abs(_mpu6050GyroReadAverage[2])<=_mpu6050CalibrateGyroThreshold) allOk++;
      else _mpu6050GyroOffset[2] = _mpu6050GyroOffset[2] - _mpu6050GyroReadAverage[2] / (_mpu6050CalibrateGyroThreshold + 1);
      
    } //END thresholdVers

    #ifdef DEBUG_MPU6050
      Serial.print("MPU6050 acceleration XYZ: ");
      Serial.print(_mpu6050AccelReadAverage[0]);
      Serial.print(" - ");
      Serial.print(_mpu6050AccelOffset[0]);
      Serial.print(", ");
      Serial.print(_mpu6050AccelReadAverage[1]);
      Serial.print(" - ");
      Serial.print(_mpu6050AccelOffset[1]);
      Serial.print(", ");
      Serial.print(_mpu6050AccelReadAverage[2]);
      Serial.print(" - ");
      Serial.print(_mpu6050AccelOffset[2]);
      Serial.print(" | MPU6050 gyro XYZ: ");
      Serial.print(_mpu6050GyroReadAverage[0]);
      Serial.print(" - ");
      Serial.print(_mpu6050GyroOffset[0]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroReadAverage[1]);
      Serial.print(" - ");
      Serial.print(_mpu6050GyroOffset[1]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroReadAverage[2]);
      Serial.print(" - ");
      Serial.print(_mpu6050GyroOffset[2]);
      Serial.println();
    #endif
    
    //all ok?
    if (allOk == 6) {
      //_doFullCalibration = false;
      break;  //break out of the while 1 loop
    }
    
    //timeout
    if((long) (mpu6050CalibrateCurMillis - _mpu6050CalibratePrevMillis) >= _mpu6050CalibrateTimeout) {
      //save what we have and get out
      //_doFullCalibration = false;
      break;  //break out of the while 1 loop
    }
      
    _mpu6050CalibratePrevMillis = millis();               //store the current time
  } //END while loop

//..save calibration data for '_mpu6050AccelOffset[]' and '_mpu6050GyroOffset[]'
  
  #ifdef DEBUG_MPU6050
    Serial.print("MPU6050 calibration complete: ");
    Serial.println();
  #endif

*/
} //END calibrateMPU6050


