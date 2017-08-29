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
  statusLED.Blink2();
  
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
  _mpu6050.setYGyroOffset(_mpu6050GyroOffset[1]);
  _mpu6050.setZGyroOffset(_mpu6050GyroOffset[2]);      

} //END setupMPU6050


