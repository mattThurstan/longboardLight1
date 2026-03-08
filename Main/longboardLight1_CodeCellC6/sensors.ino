/*----------------------------sensors----------------------------*/

void setupSensors() {
  //o.Init(); //need to pass values if any available from memory - still use this and set seperately
  //o.InitWithVars(a[3], b[3]);
  //o.InitWithVars(uint8_t accelYAverageSize = 10, float accelThreshold = 100.00, uint8_t indYAverageSize = 10, float indThreshold = 200.00);
  o.InitWithVars(10, 100.00, 10, 200.00);

  //setting up wheel pin using '_wheelSensorPin' because hard to handle interrupts from inside a library.
  
  w.Init(_wheelSensorPin, 0.0345, 8); //currently only using 1 sensor/wheel. (69mm wheel with 8 magnets)
  //w.SetupWheel(1, _wheelSensorPin[1], 0.0345, 8);  
  
  if (DEBUG_SENSORS) { }
}

/*----------------------------loop----------------------------*/
//bool _zeroTemp = false;

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

//byte _oTemp = 255;
//byte _dTemp = 255;

void loopOrientation() {
  //EVERY_N_MILLISECONDS(_mpu6050ReadInterval) { o.ReadFiltered(); }
  //EVERY_N_MILLISECONDS() {  //_orientationInterval
    //o.ReadOrientation(); 
  //}
  
  if (myCodeCell.Run(10)) {                   // Run service loop at 10 Hz (every 100 ms)
    myCodeCell.Motion_AccelerometerRead(accelX, accelY, accelZ);  // Read acceleration values
    Serial.print("X: "); Serial.print(accelX);     // Print X value
    Serial.print("  Y: "); Serial.print(accelY);   // Print Y value
    Serial.print("  Z: "); Serial.println(accelZ); // Print Z value
    
    myCodeCell.Motion_RotationRead(Roll, Pitch, Yaw); // Read latest rotation values
    Serial.printf("Roll: %.2f°, Pitch: %.2f°, Yaw: %.2f°\n", Roll, Pitch, Yaw); // Print  output
    
    Serial.print("State: ");
    switch (myCodeCell.Motion_StateRead()) {
      case MOTION_STATE_ONTABLE:
        Serial.println("On Table");
        //also this..
        break;
      case MOTION_STATE_STATIONARY:
        Serial.println("Stationary");
        //use this and timer for triggering sleep.
        //start with the code from up the page.
        break;
      default:
        Serial.println("Unknown");
        //..and this.
        break;
    }

    o.AddToAverage(accelY, roll);
    
    /* I may move all of the following to a Second loop (1000ms), or Half Second (500ms). */

    o.DoUpdateOrientation(Roll, Pitch, Yaw);
	  o.DoUpdateIndicator();

    // Always 3 cos of sensors loopOrientation()
    // Pattern 3 - loopTrackLights, "Track (basic)"
    // main lights sub-mode 3 will always be kept 3 because of the following conditions:
    if (o.GetOrientation() == 0 &&_mainLightsSubMode == 3) { o.DoUpdateDirection(); }   /* 3 cos of sub-mode 3 */
    
    if (DEBUG_SENSORS) {
      Serial.print(F("Orientation = "));
      Serial.print(o.GetOrientation());
      Serial.println();
      Serial.print(F("Direction = "));
      Serial.print(o.GetDirection());
      Serial.println();
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
  //#ifdef DATA_LOGGING 
    EVERY_N_MILLISECONDS(1000) {                     //FastLED based non-blocking delay to update/display the sequence.
      detachInterrupt(_wheelSensorPin);     //..no interrupts whilst we process !!!
      //w.DoUpdate(o.GetDirection()); // !!! need to swap '_directionCur' for a direct line to the mpu6050 library var !!!
      w.DoUpdateEverySec(o.GetDirection());
      attachInterrupt(digitalPinToInterrupt(_wheelSensorPin), wheelInterrupt0, CHANGE);  //re-attach the interrupt !!!
    } //END timed-loop
  //#endif  //END if DATA_LOGGING
} //END loopWheel
