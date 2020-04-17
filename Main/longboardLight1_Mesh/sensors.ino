/*----------------------------sensors----------------------------*/

void setupSensors() {
  //setting up wheel pin using '_wheelSensorPin' because hard to handle interrupts from inside a library.
  
  w.Init(_wheelSensorPin, 0.0345, 8); //currently only using 1 sensor/wheel. (69mm wheel with 8 magnets)
  //w.SetupWheel(1, _wheelSensorPin[1], 0.0345, 8);  
  
  o.Init(); //need to pass values if any available from memory - still use this and set seperately
  //o.InitWithVars(a[3], b[3]);
  
  if (DEBUG_SENSORS) {
	  Serial.print(F("MPU6050 Stored Accel Offset XYZ = "));
	  Serial.print(o.GetMPU6050AccelOffsetX());
	  Serial.print(F(", "));
	  Serial.print(o.GetMPU6050AccelOffsetY());
	  Serial.print(F(", "));
	  Serial.print(o.GetMPU6050AccelOffsetZ());
	  Serial.println();

	  Serial.print(F("MPU6050 Stored Gyro Offset XYZ = "));
	  Serial.print(o.GetMPU6050GyroOffsetX());
	  Serial.print(F(", "));
	  Serial.print(o.GetMPU6050GyroOffsetY());
	  Serial.print(F(", "));
	  Serial.print(o.GetMPU6050GyroOffsetZ());
	  Serial.println();
  }
}

bool _zeroTemp = false;
/* Sensor input loop (called from Main loop) */
void loopSensors() {
  loopOrientation();
  loopWheel();

//  //TEMP
//  EVERY_N_SECONDS(30) {
//    if (_zeroTemp == o.GetZeroMotionStatus()) {
//      if (_zeroTemp = 1) {
//        mA.sleep == 1; //put to sleep
//      } else { 
//        mA.sleep == 0; //wake up again
//      }
//    }
//    Serial.print(F("Zero-motion status = "));
//    Serial.print(o.GetZeroMotionStatus());
//    Serial.println();
//  }
//  _zeroTemp = o.GetZeroMotionStatus();
}

/*----------------------------MPU6050 sensor----------------------------*/
byte _oTemp = 255;
byte _dTemp = 255;

void loopOrientation() {
  EVERY_N_MILLISECONDS(_mpu6050ReadInterval) { o.ReadFiltered(); }
  EVERY_N_MILLISECONDS(_orientationInterval) { 
    o.ReadOrientation(); 
    if (o.GetOrientation() == 0 &&_mainLightsSubMode == 3) { o.ReadDirection(); }   /* main lights sub-mode 3 will always be kept 3 cos of this void */
    if (DEBUG_SENSORS) {
      if (o.GetOrientation() != _oTemp ) {
        Serial.print(F("Orientation = "));
        Serial.print(o.GetOrientation());
        Serial.println();
      }
      _oTemp = o.GetOrientation();
      if (o.GetDirection() != _dTemp ) {
        Serial.print(F("Direction = "));
        Serial.print(o.GetDirection());
        Serial.println();
      }
      _dTemp = o.GetDirection();
    }
  }
}

/*----------------------------wheel sensors----------------------------*/
/* Wheel data logging
 * update the wheels with orientation info 
 * - currently just direction
 * - this is seperate to the straight 1-1 tracking used in sub-modes 
 */
void loopWheel() {
  #ifdef DATA_LOGGING 
    EVERY_N_MILLISECONDS(1000) {                     //FastLED based non-blocking delay to update/display the sequence.
      detachInterrupt(_wheelSensorPin);     //..no interrupts whilst we process !!!
      w.DoUpdate(o.GetDirection()); // !!! need to swap '_directionCur' for a direct line to the mpu6050 library var !!!
      attachInterrupt(digitalPinToInterrupt(_wheelSensorPin), wheelInterrupt0, CHANGE);  //re-attach the interrupt !!!
    } //END timed-loop
  #endif  //END if DATA_LOGGING
} //END loopWheel

