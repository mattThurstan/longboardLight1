/* This code shows an easy way to smooth readings from a sensor subject to
 * high frequency noise.
 * It uses a low pass filter on a circular buffer.
 * This circular buffer always contains the last BUFFER_SIZE-1 readings from
 * the sensor.
 * The new reading is then added to this buffer, from which wecompute the
 * mean value by simply dividing the sum of the readings in the buffer by the
 * number of readings in the buffer.
  
 * http://theboredengineers.com/2012/09/the-quadcopter-get-its-orientation-from-sensors/
 */

//int indexBuffer = 0;
const int _bufferVectorTotal = 3;   //how many vectors do we want to store?
int _indexBuffer[_bufferVectorTotal];
const int BUFFER_SIZE = 5;      // Number of samples you want to filter on.
int16_t circularBuffer[_bufferVectorTotal][BUFFER_SIZE];
//float sensorDataCircularSum;
int16_t sensorDataCircularSum[_bufferVectorTotal];
//int16_t filteredOutput;
//float sensorRawData;            // typically the value you read from your sensor in your loop() function

/*
 * bufferSmoothInsert(int cbIndex, int16_t a)
 * 
 * eg.
 * _mpu6050FilteredCurX = bufferSmoothInsert(0, (int16_t) angle_x);
 * 
 * cbIndex = the 1st array index of the circular buffer
 * indexBuffer = the 2nd array index of the circular buffer
 * a = the int16_t value to be buffered and smoothed
 */
int16_t bufferSmoothInsert(int cbIndex, int16_t a) {
  //cbIndex should not exceed _bufferVectorTotal

  //remove the oldest value from the buffer
  sensorDataCircularSum[cbIndex] = sensorDataCircularSum[cbIndex] - circularBuffer[cbIndex][_indexBuffer[cbIndex]];
  
  // The new input from the sensor is placed in the buffer
  circularBuffer[cbIndex][_indexBuffer[cbIndex]] = a;
  
  // It is also added to the total sum of the last  BUFFER_SIZE readings
  // This method avoids to sum all the elements every time this function is called.
  sensorDataCircularSum[cbIndex] += a;

  // We increment the cursor
  _indexBuffer[cbIndex]++;

  // We test if we arrived to the end of the buffer, in which case we start again from index 0
  if (_indexBuffer[cbIndex] >= BUFFER_SIZE) {
    _indexBuffer[cbIndex] = 0;
  }

  a = (sensorDataCircularSum[cbIndex] / BUFFER_SIZE);             //running averaged total
  //int16_t b = (sensorDataCircularSum[cbIndex] / BUFFER_SIZE);

  return a;
}

/*
//void smoothSensorReadings(float sensorRawData){
void bufferSmoothMPU6050Read() {
  // We remove the oldest value from the buffer
 sensorDataCircularSum[0] = sensorDataCircularSum[0] - circularBuffer[0][indexBuffer];
 sensorDataCircularSum[1] = sensorDataCircularSum[1] - circularBuffer[1][indexBuffer];
 sensorDataCircularSum[2] = sensorDataCircularSum[2] - circularBuffer[2][indexBuffer];
 sensorDataCircularSum[3] = sensorDataCircularSum[3] - circularBuffer[3][indexBuffer];
 sensorDataCircularSum[4] = sensorDataCircularSum[4] - circularBuffer[4][indexBuffer];
 sensorDataCircularSum[5] = sensorDataCircularSum[5] - circularBuffer[5][indexBuffer];
 
  // The new input from the sensor is placed in the buffer
 //circularBuffer[indexBuffer] = sensorRawData;
 circularBuffer[0][indexBuffer] = _mpu6050AccelReadX;
 circularBuffer[1][indexBuffer] = _mpu6050AccelReadY;
 circularBuffer[2][indexBuffer] = _mpu6050AccelReadZ;
 circularBuffer[3][indexBuffer] = _mpu6050GyroReadX;
 circularBuffer[4][indexBuffer] = _mpu6050GyroReadY;
 circularBuffer[5][indexBuffer] = _mpu6050GyroReadZ;

 
  // It is also added to the total sum of the last  BUFFER_SIZE readings
  // This method avoids to sum all the elements every time this function is called.
 //sensorDataCircularSum += sensorRawData;
 sensorDataCircularSum[0] += _mpu6050AccelReadX;
 sensorDataCircularSum[1] += _mpu6050AccelReadY;
 sensorDataCircularSum[2] += _mpu6050AccelReadZ;
 sensorDataCircularSum[3] += _mpu6050GyroReadX;
 sensorDataCircularSum[4] += _mpu6050GyroReadY;
 sensorDataCircularSum[5] += _mpu6050GyroReadZ;
 
  // We increment the cursor
 indexBuffer++;

 if (indexBuffer >= BUFFER_SIZE) {
  // We test if we arrived to the end of the buffer, in which case we start again from index 0
  indexBuffer = 0;
 }
 
 //filteredOutput = (SensorDataCircularSum / BUFFER_SIZE); // The output is the the mean value of the circular buffer.
 _mpu6050AccelReadX = (sensorDataCircularSum[0] / BUFFER_SIZE);
 _mpu6050AccelReadY = (sensorDataCircularSum[1] / BUFFER_SIZE);
 _mpu6050AccelReadZ = (sensorDataCircularSum[2] / BUFFER_SIZE);
 _mpu6050GyroReadX = (sensorDataCircularSum[3] / BUFFER_SIZE);
 _mpu6050GyroReadY = (sensorDataCircularSum[4] / BUFFER_SIZE);
 _mpu6050GyroReadZ = (sensorDataCircularSum[5] / BUFFER_SIZE);
 
}
*/

