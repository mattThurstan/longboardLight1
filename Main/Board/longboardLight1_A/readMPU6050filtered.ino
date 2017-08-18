/*----------------------------sensors - MPU6050 - read with filtering----------------------------*/

/*
 * Code contained within this arduino sketch 'tab', 'readMPU6050filtered', references code from the following source:
 * 
 * 'MPU-6050 Accelerometer + Gyro' by "Krodal" (arduino.cc user ) - June 2012 - Open Source / Public Domain.
 */

// Use the following global variables and access functions to help store the overall
// rotation angle of the sensor
unsigned long last_read_time;

inline unsigned long get_last_time() {return last_read_time;}
inline float get_last_x_angle() {return _mpu6050FilteredPrev[0];}
inline float get_last_y_angle() {return _mpu6050FilteredPrev[1];}
inline float get_last_z_angle() {return _mpu6050FilteredPrev[2];}
inline float get_last_gyro_x_angle() {return _mpu6050GyroPrev[0];}
inline float get_last_gyro_y_angle() {return _mpu6050GyroPrev[1];}
inline float get_last_gyro_z_angle() {return _mpu6050GyroPrev[2];}

void set_last_read_angle_data(unsigned long time, float accel_y, float x, float y, float z, float x_gyro, float y_gyro, float z_gyro) {
  last_read_time = time;
  _mpu6050Accel_yPrev = accel_y;
  _mpu6050FilteredPrev[0] = x;
  _mpu6050FilteredPrev[1] = y;
  _mpu6050FilteredPrev[2] = z;
  _mpu6050GyroPrev[0] = x_gyro;
  _mpu6050GyroPrev[1] = y_gyro;
  _mpu6050GyroPrev[2] = z_gyro;
}

void readMPU6050filtered() {
  
  //timed-loop
  unsigned long mpu6050ReadCurMillis = millis();     //get current time
  if((unsigned long) (mpu6050ReadCurMillis - _mpu6050ReadPrevMillis) >= _mpu6050ReadInterval) {
    
    //read raw values
    _mpu6050.getMotion6(&_mpu6050AccelRead[0], &_mpu6050AccelRead[1], &_mpu6050AccelRead[2], &_mpu6050GyroRead[0], &_mpu6050GyroRead[1], &_mpu6050GyroRead[2]);
    
    // Convert gyro values to degrees/sec
    float FS_SEL = 131;

    float gyro_x = ((float)_mpu6050GyroRead[0] - _mpu6050GyroZero[0])/FS_SEL;
    float gyro_y = ((float)_mpu6050GyroRead[1] - _mpu6050GyroZero[1])/FS_SEL;
    float gyro_z = ((float)_mpu6050GyroRead[2] - _mpu6050GyroZero[2])/FS_SEL;

    // Get raw acceleration values
    //float G_CONVERT = 16384;
    float accel_x = (float)_mpu6050AccelRead[0];
    float accel_y = (float)_mpu6050AccelRead[1];
    float accel_z = (float)_mpu6050AccelRead[2];

    //add to the average for direction calc
    //..this really needs to be a rolling average
    _diAccelSave += accel_y;
    _diDirectionCounter++;
    
    // Get angle values from accelerometer
    float RADIANS_TO_DEGREES = 180/3.14159;
  //  float accel_vector_length = sqrt(pow(accel_x,2) + pow(accel_y,2) + pow(accel_z,2));
    float accel_angle_y = atan(-1*accel_x/sqrt(pow(accel_y,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
    float accel_angle_x = atan(accel_y/sqrt(pow(accel_x,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;

    float accel_angle_z = 0;
    
    // Compute the (filtered) gyro angles
    //float dt =(t_now - get_last_time())/1000.0; 
    float dt =(mpu6050ReadCurMillis - get_last_time())/1000.0;
    float gyro_angle_x = gyro_x*dt + get_last_x_angle();
    float gyro_angle_y = gyro_y*dt + get_last_y_angle();
    float gyro_angle_z = gyro_z*dt + get_last_z_angle();
//    Serial.print("accel_angle_y = ");
//    Serial.print(accel_angle_y);
//    Serial.println();
//    
    // Compute the drifting gyro angles
    float unfiltered_gyro_angle_x = gyro_x*dt + get_last_gyro_x_angle();
    float unfiltered_gyro_angle_y = gyro_y*dt + get_last_gyro_y_angle();
    float unfiltered_gyro_angle_z = gyro_z*dt + get_last_gyro_z_angle();
    
    // Apply the complementary filter to figure out the change in angle - choice of alpha is
    // estimated now.  Alpha depends on the sampling rate...
    float alpha = 0.96; //0.96;  //0.04sec ..erm
    float angle_x = alpha*gyro_angle_x + (1.0 - alpha)*accel_angle_x;
    float angle_y = alpha*gyro_angle_y + (1.0 - alpha)*accel_angle_y;
    float angle_z = gyro_angle_z;  //Accelerometer doesn't give z-angle

    //..this is annoying me ..so ..abracadabra, make minus, plus, stop annoying me..
    //
    //hohum, does effectively turn them from floats to ints, but one does not really require the accuracy
    if (angle_x > 180) { _mpu6050FilteredCur[0] = -abs(angle_x); }
    else if (angle_x < -180) { _mpu6050FilteredCur[0] = abs(angle_x); }
    else { _mpu6050FilteredCur[0] = angle_x; }  //oops, forgot these
    
    if (angle_y > 180) { _mpu6050FilteredCur[1] = -abs(angle_y); }
    else if (angle_y < -180) { _mpu6050FilteredCur[1] = abs(angle_y); }
    else { _mpu6050FilteredCur[1] = angle_y; }  //..
    
    if (angle_z > 180) { _mpu6050FilteredCur[2] = -abs(angle_z); }
    else if (angle_z < -180) { _mpu6050FilteredCur[2] = abs(angle_z); }
    else  {_mpu6050FilteredCur[2] = angle_z; }

    // Update the previous saved data with the latest values - saving un-adjusted values to previous, not minus/plus values..
    set_last_read_angle_data(mpu6050ReadCurMillis, accel_y, angle_x, angle_y, angle_z, unfiltered_gyro_angle_x, unfiltered_gyro_angle_y, unfiltered_gyro_angle_z);

    _mpu6050ReadPrevMillis = millis();               //store the current time
  } //END timed-loop

  //dependant on whether tracking sub-mode is actually running, otherwise waste of processing..
  if (_orientation == 0 &&_mainLightsSubMode == 3) { 
    if (_diDirectionCounter >= _directionSampleTotal) {
      //int average = _diDirectionSave / _directionSampleTotal;
      //..this really needs to be a rolling average
      unsigned int average = (_diAccelSave / _directionSampleTotal);
      if (average > _mpu6050AccelZero[1] + 100) {
        _directionCur = 0;  //going forwards
      } else if (average < _mpu6050AccelZero[1] - 100) {
        _directionCur = 1;  //going backwards
      } else {
        //_directionCur = -1;  //stationary
      }
      _diAccelSave = 0;
      _diDirectionCounter = 0;
  //    #ifdef DEBUG
  //      Serial.print(", Current direction = ");
  //      Serial.print(_directionCur);
  //      Serial.println();
  //    #endif
    }
  } //END dependant

} //END readMPU6050filtered

