/*----------------------------sensors - MPU6050 on GY-521 breakout board----------------------------*/

/*
 * Code contained within this arduino sketch 'tab', 'MPU6050', references code from the following sources:
 * 
 * https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_raw/MPU6050_raw.ino
 * 
 * http://42bots.com/tutorials/arduino-script-for-mpu-6050-auto-calibration/ 
 * - Done by Luis Ródenas <luisrodenaslorda@gmail.com>
 * - Based on the I2Cdev library and previous work by Jeff Rowberg <jeff@rowberg.net>
 * 
 * https://vvvv.org/documentation/arduino02
 * 
 * therefore..
 */
 
/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

/*----------------------------sensors - MPU6050 - setup----------------------------*/
/*
 * place flat
 * get motion data
 * get offsets
 * nudge offsets towards motion data
 * set offsets
 * repeat x times, for a time period, or until within tolerance
 */

void setupMPU6050() {
  
  //initialize device
  #ifdef DEBUG_MPU6050
    Serial.println("Initializing MPU6050 on I2C...");
  #endif
  _mpu6050.initialize();    //this gets stuck sometimes on the mini pro
  blinkStatusLED2();        //so.. we know
  
  //verify connection
  #ifdef DEBUG_MPU6050
    Serial.println("Testing MPU6050 connection...");
    Serial.println(_mpu6050.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  #endif

  //load offsets from memory if they exist
  
  //set offsets
  //these are calculated using the full calibration
  //probably copied from memory
  _mpu6050.setXAccelOffset(436);      //_mpu6050AccelOffset[0]
  _mpu6050.setYAccelOffset(1956);
  _mpu6050.setZAccelOffset(1318);
  _mpu6050.setXGyroOffset(9);       //_mpu6050GyroOffset[0]
  _mpu6050.setYGyroOffset(-32);
  _mpu6050.setZGyroOffset(69);       

  //TEMP
  //_doCalibrateMPU6050 = true;
  //calibrateMPU6050();

  //these are calculated using the quick calibration
  //set here at zero for the start
  for (int i = 0; i < 3; i++) {
     _mpu6050AccelZero[i] = 0;
     _mpu6050GyroZero[i] = 0;
//    _mpu6050AccelZeroX = 0;
//    _mpu6050AccelZeroY = 0;
//    _mpu6050AccelZeroZ = 0;
//    _mpu6050GyroZeroX = 0;
//    _mpu6050GyroZeroY = 0;
//    _mpu6050GyroZeroZ = 0;
  }
  
}

/*----------------------------sensors - MPU6050 - calibrate----------------------------*/

void calibrateMPU6050() {

  if (_doCalibrateMPU6050 == true) {
    
  #ifdef DEBUG_MPU6050
    Serial.print("MPU6050 starting calibration: ");
    Serial.println();
  #endif
  
    // reset offsets
    _mpu6050.setXAccelOffset(0);
    _mpu6050.setYAccelOffset(0);
    _mpu6050.setZAccelOffset(0);
    _mpu6050.setXGyroOffset(0);
    _mpu6050.setYGyroOffset(0);
    _mpu6050.setZGyroOffset(0);
  
    //place flat
  
    //sensor_temperature = (_mpu6050.getTemperature() + 12412) / 340; //do later
  
    long mpu6050CalibrateCurMillis = millis();   //get current time
    
    while (1) {
      int allOk = 0;
      
  //  if((long) (mpu6050CalibrateCurMillis - _mpu6050CalibratePrevMillis) >= _mpu6050CalibrateInterval) {
  //  _mpu6050CalibratePrevMillis = millis();               //store the current time
    
  //  long i = 0;
  //  long accelSampleX = 0;
  //  long accelSampleY = 0;
  //  long accelSampleZ = 0;
  //  long gyroSampleX = 0;
  //  long gyroSampleY = 0;
  //  long gyroSampleZ = 0;
  
  //  //get motion data averages
  //  while (i < (_mpu6050CalibrateSampleTotal + 101)) {
  //    _mpu6050.getMotion6(&_mpu6050AccelCurX, &_mpu6050AccelCurY, &_mpu6050AccelCurZ, &_mpu6050GyroCurX, &_mpu6050GyroCurY, &_mpu6050GyroCurZ);
  //    if (i > 100 && i <= (_mpu6050CalibrateSampleTotal + 100)) { 
  //      //discarded first 100 measures
  //      accelSampleX += _mpu6050AccelCurX;  //add 'em up
  //      ..
  //    }
  //    if (i == (_mpu6050CalibrateSampleTotal + 100)) {
  //      //sum 'em up
  //      _mpu6050AccelAverageX = accelSampleX / _mpu6050CalibrateSampleTotal;
  //      ..
  //    }
  //    //step it up
  //    i++;
  //    //rawhide
  //    delay(2); //Needed so we don't get repeated measures
  //  }
  
      int16_t accelSampleX = 0;
      int16_t accelSampleY = 0;
      int16_t accelSampleZ = 0;
      int16_t gyroSampleX = 0;
      int16_t gyroSampleY = 0;
      int16_t gyroSampleZ = 0;
  
      for (int i=0; i<_mpu6050CalibrateSampleTotal; i++) {
        //_mpu6050.getMotion6(&_mpu6050AccelCurX, &_mpu6050AccelCurY, &_mpu6050AccelCurZ, &_mpu6050GyroCurX, &_mpu6050GyroCurY, &_mpu6050GyroCurZ);
        _mpu6050.getMotion6(&_mpu6050AccelRead[0], &_mpu6050AccelRead[1], &_mpu6050AccelRead[2], &_mpu6050GyroRead[0], &_mpu6050GyroRead[1], &_mpu6050GyroRead[2]);

        accelSampleX += _mpu6050AccelRead[0];
        accelSampleY += _mpu6050AccelRead[1];
        accelSampleZ += _mpu6050AccelRead[2];
        gyroSampleX += _mpu6050GyroRead[0];
        gyroSampleY += _mpu6050GyroRead[1];
        gyroSampleZ += _mpu6050GyroRead[2];
        //delay(2); //Needed so we don't get repeated measures ???
      }

//      _mpu6050AccelCurX = (accelSampleX / _mpu6050CalibrateSampleTotal);
//      _mpu6050AccelCurY = (accelSampleY / _mpu6050CalibrateSampleTotal);
//      _mpu6050AccelCurZ = (accelSampleZ / _mpu6050CalibrateSampleTotal);
//      _mpu6050GyroCurX = (gyroSampleX / _mpu6050CalibrateSampleTotal);
//      _mpu6050GyroCurY = (gyroSampleY / _mpu6050CalibrateSampleTotal);
//      _mpu6050GyroCurZ = (gyroSampleZ / _mpu6050CalibrateSampleTotal);
      _mpu6050AccelRead[0] = (accelSampleX / _mpu6050CalibrateSampleTotal);
      _mpu6050AccelRead[1] = (accelSampleY / _mpu6050CalibrateSampleTotal);
      _mpu6050AccelRead[2] = (accelSampleZ / _mpu6050CalibrateSampleTotal);
      _mpu6050GyroRead[0] = (gyroSampleX / _mpu6050CalibrateSampleTotal);
      _mpu6050GyroRead[1] = (gyroSampleY / _mpu6050CalibrateSampleTotal);
      _mpu6050GyroRead[2] = (gyroSampleZ / _mpu6050CalibrateSampleTotal);
      
      //get motion
      //_mpu6050.getMotion6(&_mpu6050AccelCurX, &_mpu6050AccelCurY, &_mpu6050AccelCurZ, &_mpu6050GyroCurX, &_mpu6050GyroCurY, &_mpu6050GyroCurZ);
      
      //get offsets
      _mpu6050AccelOffset[0] = _mpu6050.getXAccelOffset();
      _mpu6050AccelOffset[1] = _mpu6050.getYAccelOffset();
      _mpu6050AccelOffset[2] = _mpu6050.getZAccelOffset();
      _mpu6050GyroOffset[0] = _mpu6050.getXGyroOffset();
      _mpu6050GyroOffset[1] = _mpu6050.getYGyroOffset();
      _mpu6050GyroOffset[2] = _mpu6050.getZGyroOffset();
      
      //nudge offsets towards motion data
//      if (_mpu6050AccelReadX > 0) _mpu6050AccelOffsetX--;
//      else if (_mpu6050AccelReadX < 0) _mpu6050AccelOffsetX++;
//
//      if (_mpu6050AccelReadY > 0) _mpu6050AccelOffsetY--;
//      else if (_mpu6050AccelReadY < 0) _mpu6050AccelOffsetY++;
//
//      if (_mpu6050AccelReadZ > 0) _mpu6050AccelOffsetZ--;
//      else if (_mpu6050AccelReadZ < 0) _mpu6050AccelOffsetZ++;
//      
//      if (_mpu6050GyroReadX > 0) _mpu6050GyroOffsetX--;
//      else if (_mpu6050GyroReadX < 0) _mpu6050GyroOffsetX++;
//      
//      if (_mpu6050GyroReadY > 0) _mpu6050GyroOffsetY--;
//      else if (_mpu6050GyroReadY < 0) _mpu6050GyroOffsetY++;
//      
//      if (_mpu6050GyroReadZ > 0) _mpu6050GyroOffsetZ--;
//      else if (_mpu6050GyroReadZ < 0) _mpu6050GyroOffsetZ++;

      for (int i = 0; i < 3; i++) {
        if (_mpu6050AccelRead[i] > 0) _mpu6050AccelOffset[i]--;
        else if (_mpu6050AccelRead[i] < 0) _mpu6050AccelOffset[i]++;

        if (_mpu6050GyroRead[i] > 0) _mpu6050GyroOffset[i]--;
        else if (_mpu6050GyroRead[i] < 0) _mpu6050GyroOffset[i]++;
      }
    
      //set offsets
      _mpu6050.setXAccelOffset(_mpu6050AccelOffset[0]);
      _mpu6050.setYAccelOffset(_mpu6050AccelOffset[1]);
      _mpu6050.setZAccelOffset(_mpu6050AccelOffset[2]);
      _mpu6050.setXGyroOffset(_mpu6050GyroOffset[0]);
      _mpu6050.setYGyroOffset(_mpu6050GyroOffset[1]);
      _mpu6050.setZGyroOffset(_mpu6050GyroOffset[2]);

      #ifdef DEBUG_MPU6050
        Serial.print("MPU6050 acceleration XYZ: ");
        Serial.print(_mpu6050AccelRead[0]);
        Serial.print(" - ");
        Serial.print(_mpu6050AccelOffset[0]);
        Serial.print(", ");
        Serial.print(_mpu6050AccelRead[1]);
        Serial.print(" - ");
        Serial.print(_mpu6050AccelOffset[1]);
        Serial.print(", ");
        Serial.print(_mpu6050AccelRead[2]);
        Serial.print(" - ");
        Serial.print(_mpu6050AccelOffset[2]);
        Serial.print(" | MPU6050 gyro XYZ: ");
        Serial.print(_mpu6050GyroRead[0]);
        Serial.print(" - ");
        Serial.print(_mpu6050GyroOffset[0]);
        Serial.print(", ");
        Serial.print(_mpu6050GyroRead[1]);
        Serial.print(" - ");
        Serial.print(_mpu6050GyroOffset[1]);
        Serial.print(", ");
        Serial.print(_mpu6050GyroRead[2]);
        Serial.print(" - ");
        Serial.print(_mpu6050GyroOffset[2]);
        Serial.println();
      #endif
      
      //repeat x times, for a time period, or until within tolerance
//      if (_mpu6050AccelReadX <= _mpu6050CalibrateAccelThreshold && _mpu6050AccelReadX >= -_mpu6050CalibrateAccelThreshold) { allOk++; }
//      //else { ax_offset=ax_offset-mean_ax / __mpu6050CalibrateAccelThreshold; }
//      if (_mpu6050AccelReadY <= _mpu6050CalibrateAccelThreshold && _mpu6050AccelReadY >= -_mpu6050CalibrateAccelThreshold) { allOk++; }
//      if (_mpu6050AccelReadZ <= _mpu6050CalibrateAccelThreshold && _mpu6050AccelReadZ >= -_mpu6050CalibrateAccelThreshold) { allOk++; }
//    
//      if (_mpu6050GyroReadX <= _mpu6050CalibrateGyroThreshold && _mpu6050GyroReadX >= -_mpu6050CalibrateGyroThreshold) { allOk++; }
//      if (_mpu6050GyroReadY <= _mpu6050CalibrateGyroThreshold && _mpu6050GyroReadY >= -_mpu6050CalibrateGyroThreshold) { allOk++; }
//      if (_mpu6050GyroReadZ <= _mpu6050CalibrateGyroThreshold && _mpu6050GyroReadZ >= -_mpu6050CalibrateGyroThreshold) { allOk++; }

      for (int i = 0; i < 3; i++) {
        if (_mpu6050AccelRead[i] <= _mpu6050CalibrateAccelThreshold && _mpu6050AccelRead[i] >= -_mpu6050CalibrateAccelThreshold) { allOk++; }
        if (_mpu6050GyroRead[i] <= _mpu6050CalibrateGyroThreshold && _mpu6050GyroRead[i] >= -_mpu6050CalibrateGyroThreshold) { allOk++; }
      }
      
      if (allOk == 6) {
        _doCalibrateMPU6050 = false;
        break;
      }
      
      if((long) (mpu6050CalibrateCurMillis - _mpu6050CalibratePrevMillis) >= _mpu6050CalibrateTimeout) {
        //timeout
        //save what we have and get out
        _doCalibrateMPU6050 = false;
        break;
      }
      
      _mpu6050CalibratePrevMillis = millis();               //store the current time
    } //END while loop
  
  //
  } //END if (_doCalibrateMPU6050 == true)
  
  #ifdef DEBUG_MPU6050
    Serial.print("MPU6050 calibration complete: ");
    Serial.println();
  #endif
} //END calibrateMPU6050


/*----------------------------sensors - MPU6050 - read----------------------------*/
//see.. 'readMPU6050filtered'  with  'MPU6050buffer'


