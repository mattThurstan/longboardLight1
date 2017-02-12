/*----------------------------sensors - MPU6050 - read with filtering----------------------------*/

/*
 * Code contained within this arduino sketch 'tab', 'readMPU6050filtered', references code from the following source:
 * 
 * 'MPU-6050 Accelerometer + Gyro' by "Krodal" (arduino.cc user ) - June 2012 - Open Source / Public Domain.
 */

// Use the following global variables and access functions to help store the overall
// rotation angle of the sensor
unsigned long last_read_time;
//float         last_x_angle;  // These are the filtered angles = _mpu6050FilteredPrevX
//float         last_y_angle;
//float         last_z_angle;  
//float         last_gyro_x_angle;  // Store the gyro angles to compare drift = _mpu6050GyroPrevX
//float         last_gyro_y_angle;
//float         last_gyro_z_angle;

inline unsigned long get_last_time() {return last_read_time;}
//inline float get_last_x_angle() {return last_x_angle;}
//inline float get_last_y_angle() {return last_y_angle;}
//inline float get_last_z_angle() {return last_z_angle;}
inline float get_last_x_angle() {return _mpu6050FilteredPrevX;}
inline float get_last_y_angle() {return _mpu6050FilteredPrevY;}
inline float get_last_z_angle() {return _mpu6050FilteredPrevZ;}
//inline float get_last_gyro_x_angle() {return last_gyro_x_angle;}
//inline float get_last_gyro_y_angle() {return last_gyro_y_angle;}
//inline float get_last_gyro_z_angle() {return last_gyro_z_angle;}
inline float get_last_gyro_x_angle() {return _mpu6050GyroPrevX;}
inline float get_last_gyro_y_angle() {return _mpu6050GyroPrevY;}
inline float get_last_gyro_z_angle() {return _mpu6050GyroPrevZ;}

void set_last_read_angle_data(unsigned long time, float x, float y, float z, float x_gyro, float y_gyro, float z_gyro) {
  last_read_time = time;
//  last_x_angle = x;
//  last_y_angle = y;
//  last_z_angle = z;
  _mpu6050FilteredPrevX = x;
  _mpu6050FilteredPrevY = y;
  _mpu6050FilteredPrevZ = z;
//  last_gyro_x_angle = x_gyro;
//  last_gyro_y_angle = y_gyro;
//  last_gyro_z_angle = z_gyro;
  _mpu6050GyroPrevX = x_gyro;
  _mpu6050GyroPrevY = y_gyro;
  _mpu6050GyroPrevZ = z_gyro;
}


void readMPU6050filtered() {
  //accel_t_gyro.value.x_gyro = _mpu6050GyroCurX
  //base_x_gyro (calibration average zero point save) = _mpu6050GyroZeroX

  //timed-loop
  long mpu6050ReadCurMillis = millis();     //get current time
  if((long) (mpu6050ReadCurMillis - _mpu6050ReadPrevMillis) >= _mpu6050ReadInterval) {
    
    //read raw values
    //_mpu6050.getMotion6(&_mpu6050AccelCurX, &_mpu6050AccelCurY, &_mpu6050AccelCurZ, &_mpu6050GyroCurX, &_mpu6050GyroCurY, &_mpu6050GyroCurZ);
    _mpu6050.getMotion6(&_mpu6050AccelReadX, &_mpu6050AccelReadY, &_mpu6050AccelReadZ, &_mpu6050GyroReadX, &_mpu6050GyroReadY, &_mpu6050GyroReadZ);
    bufferSmoothMPU6050Read();
  
    // Get the time of reading for rotation computations
    unsigned long t_now = millis();
    
    // Convert gyro values to degrees/sec
    float FS_SEL = 131;
    
//    float gyro_x = (accel_t_gyro.value.x_gyro - base_x_gyro)/FS_SEL;
//    float gyro_y = (accel_t_gyro.value.y_gyro - base_y_gyro)/FS_SEL;
//    float gyro_z = (accel_t_gyro.value.z_gyro - base_z_gyro)/FS_SEL;
    float gyro_x = ((float)_mpu6050GyroReadX - _mpu6050GyroZeroX)/FS_SEL;
    float gyro_y = (_mpu6050GyroReadY - _mpu6050GyroZeroY)/FS_SEL;
    float gyro_z = (_mpu6050GyroReadZ - _mpu6050GyroZeroZ)/FS_SEL;
  
    // Get raw acceleration values
    //float G_CONVERT = 16384;
    float accel_x = (float)_mpu6050AccelReadX;  //_mpu6050AccelCurX
    float accel_y = _mpu6050AccelReadY;
    float accel_z = _mpu6050AccelReadZ;
    
    // Get angle values from accelerometer
    float RADIANS_TO_DEGREES = 180/3.14159;
  //  float accel_vector_length = sqrt(pow(accel_x,2) + pow(accel_y,2) + pow(accel_z,2));
    float accel_angle_y = atan(-1*accel_x/sqrt(pow(accel_y,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
    float accel_angle_x = atan(accel_y/sqrt(pow(accel_x,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
  
    float accel_angle_z = 0;
  
    // Compute the (filtered) gyro angles
    float dt =(t_now - get_last_time())/1000.0;
    float gyro_angle_x = gyro_x*dt + get_last_x_angle();
    float gyro_angle_y = gyro_y*dt + get_last_y_angle();
    float gyro_angle_z = gyro_z*dt + get_last_z_angle();
    
    // Compute the drifting gyro angles
    float unfiltered_gyro_angle_x = gyro_x*dt + get_last_gyro_x_angle();
    float unfiltered_gyro_angle_y = gyro_y*dt + get_last_gyro_y_angle();
    float unfiltered_gyro_angle_z = gyro_z*dt + get_last_gyro_z_angle();
    
    // Apply the complementary filter to figure out the change in angle - choice of alpha is
    // estimated now.  Alpha depends on the sampling rate...
    float alpha = 0.98; //0.96;
    float angle_x = alpha*gyro_angle_x + (1.0 - alpha)*accel_angle_x;
    float angle_y = alpha*gyro_angle_y + (1.0 - alpha)*accel_angle_y;
    float angle_z = gyro_angle_z;  //Accelerometer doesn't give z-angle
//    _mpu6050FilteredCurX = alpha*gyro_angle_x + (1.0 - alpha)*accel_angle_x;
//    _mpu6050FilteredCurY = alpha*gyro_angle_y + (1.0 - alpha)*accel_angle_y;
//    _mpu6050FilteredCurZ = gyro_angle_z;      //Accelerometer doesn't give z-angle
    _mpu6050FilteredCurX = (int16_t) angle_x; //mabye make this use '_mpu6050AccelCurX' ???
    _mpu6050FilteredCurY = (int16_t) angle_y;
    _mpu6050FilteredCurZ = (int16_t) angle_z;
    
    // Update the saved data with the latest values
    set_last_read_angle_data(t_now, angle_x, angle_y, angle_z, unfiltered_gyro_angle_x, unfiltered_gyro_angle_y, unfiltered_gyro_angle_z);
    //set_last_read_angle_data(t_now, _mpu6050FilteredCurX, _mpu6050FilteredCurY, _mpu6050FilteredCurZ, unfiltered_gyro_angle_x, unfiltered_gyro_angle_y, unfiltered_gyro_angle_z);

    //accel_angle_x //Accelerometer angle
    //unfiltered_gyro_angle_x //Gyroscope angle
    //angle_x //combined final Filtered angle
    #ifdef DEBUG_MPU6050;
      Serial.print(accel_angle_x);
      Serial.print(", ");
      Serial.print(unfiltered_gyro_angle_x);
      Serial.print(", ");
      Serial.print(angle_x); //_mpu6050FilteredCurX

      Serial.print(", ");
      
      Serial.print(accel_angle_y);
      Serial.print(", ");
      Serial.print(unfiltered_gyro_angle_y);
      Serial.print(", ");
      Serial.print(angle_y);
      
      Serial.print(", ");
      
      Serial.print(accel_angle_z);
      Serial.print(", ");
      Serial.print(unfiltered_gyro_angle_z);
      Serial.print(", ");
      Serial.print(angle_z);
      
      //linebreak
      Serial.write(10);
      Serial.write(13);
    #endif
    
    _mpu6050ReadPrevMillis = millis();               //store the current time
  } //END timed-loop
}






