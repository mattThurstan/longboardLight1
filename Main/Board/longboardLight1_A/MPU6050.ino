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
  _mpu6050.setXAccelOffset(_mpu6050AccelOffset[0]);
  _mpu6050.setYAccelOffset(_mpu6050AccelOffset[1]);
  _mpu6050.setZAccelOffset(_mpu6050AccelOffset[2]);
  _mpu6050.setXGyroOffset(_mpu6050GyroOffset[0]);
  _mpu6050.setYGyroOffset(_mpu6050GyroOffset[0]);
  _mpu6050.setZGyroOffset(_mpu6050GyroOffset[0]);      

} //END setupMPU6050

/*----------------------------sensors - MPU6050 - calibrate----------------------------*/

void doMPU6050ReadAverage() {
  //this does not get used for the main filtered loop, only for calibration porpoises
  
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
}

void calibrateMPU6050() {
  //full calibration - place flat
  
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
      break;
    }
    
    //timeout
    if((long) (mpu6050CalibrateCurMillis - _mpu6050CalibratePrevMillis) >= _mpu6050CalibrateTimeout) {
      //save what we have and get out
      //_doFullCalibration = false;
      break;
    }
      
    _mpu6050CalibratePrevMillis = millis();               //store the current time
  } //END while loop
  
  #ifdef DEBUG_MPU6050
    Serial.print("MPU6050 calibration complete: ");
    Serial.println();
  #endif
  
} //END calibrateMPU6050

