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
inline float get_last_x_angle() {return _mpu6050FilteredPrevX;}
inline float get_last_y_angle() {return _mpu6050FilteredPrevY;}
inline float get_last_z_angle() {return _mpu6050FilteredPrevZ;}
inline float get_last_gyro_x_angle() {return _mpu6050GyroPrevX;}
inline float get_last_gyro_y_angle() {return _mpu6050GyroPrevY;}
inline float get_last_gyro_z_angle() {return _mpu6050GyroPrevZ;}

void set_last_read_angle_data(unsigned long time, float x, float y, float z, float x_gyro, float y_gyro, float z_gyro) {
  last_read_time = time;
  _mpu6050FilteredPrevX = x;
  _mpu6050FilteredPrevY = y;
  _mpu6050FilteredPrevZ = z;
  _mpu6050GyroPrevX = x_gyro;
  _mpu6050GyroPrevY = y_gyro;
  _mpu6050GyroPrevZ = z_gyro;
}


void readMPU6050filtered() {
  
  //timed-loop
  unsigned long mpu6050ReadCurMillis = millis();     //get current time
  if((unsigned long) (mpu6050ReadCurMillis - _mpu6050ReadPrevMillis) >= _mpu6050ReadInterval) {
    
    //read raw values
    _mpu6050.getMotion6(&_mpu6050AccelReadX, &_mpu6050AccelReadY, &_mpu6050AccelReadZ, &_mpu6050GyroReadX, &_mpu6050GyroReadY, &_mpu6050GyroReadZ);
    
    // Convert gyro values to degrees/sec
    float FS_SEL = 131;

    float gyro_x = ((float)_mpu6050GyroReadX - _mpu6050GyroZeroX)/FS_SEL;
    float gyro_y = ((float)_mpu6050GyroReadY - _mpu6050GyroZeroY)/FS_SEL;
    float gyro_z = ((float)_mpu6050GyroReadZ - _mpu6050GyroZeroZ)/FS_SEL;
  
    // Get raw acceleration values
    //float G_CONVERT = 16384;
    float accel_x = (float)_mpu6050AccelReadX;
    float accel_y = (float)_mpu6050AccelReadY;
    float accel_z = (float)_mpu6050AccelReadZ;
    
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

    //..this is annoying me ..so ..abracadabra, make minus, plus, stop annoying me
    if (angle_x > 180) { _mpu6050FilteredCurX = -abs(angle_x); }
    else if (angle_x < -180) { _mpu6050FilteredCurX = abs(angle_x); }
    else { _mpu6050FilteredCurX = (int16_t) angle_x; }  //oops, forgot these
    
    if (angle_y > 180) { _mpu6050FilteredCurY = -abs(angle_y); }
    else if (angle_y < -180) { _mpu6050FilteredCurY = abs(angle_y); }
    else { _mpu6050FilteredCurY = (int16_t) angle_y; }  //..
    
    if (angle_z > 180) { _mpu6050FilteredCurZ = -abs(angle_z); }
    else if (angle_z < -180) { _mpu6050FilteredCurZ = abs(angle_z); }
    else  {_mpu6050FilteredCurZ = (int16_t) angle_z; }

    // Update the saved data with the latest values - saving un-adjusted values to previous, not minus/plus values..
    set_last_read_angle_data(mpu6050ReadCurMillis, angle_x, angle_y, angle_z, unfiltered_gyro_angle_x, unfiltered_gyro_angle_y, unfiltered_gyro_angle_z);
    
/*    #ifdef DEBUG_MPU6050
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
      
      //Serial.print(", ");
      //Serial.print(_orientation);   //this will be 1 frame behind, but outweighs other stuff for debug..
      //int oM0;
      //oM0 = orMatrix[0];
      //Serial.print(oM0);
      
      //linebreak
      Serial.write(10);
      Serial.write(13);
    #endif
*/
    
    _mpu6050ReadPrevMillis = millis();               //store the current time
  } //END timed-loop
}






