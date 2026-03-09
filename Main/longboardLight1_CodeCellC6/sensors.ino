/*----------------------------sensors----------------------------*/

void setupSensors() {
  //o.InitWithVars(uint8_t accelYAverageSize = 10, float accelThreshold = 100.00, uint8_t indYAverageSize = 10, float indThreshold = 200.00);
  o.InitWithVars(_codeCellRunInterval, 100.00, _codeCellRunInterval, 200.00);
  //I have taken out a lot of smoothing code based on the idea that BNO085 has far cleaner data than MPU6050. 
  //Can put back if still needed.

  //setting up wheel pin using '_wheelSensorPin' because hard to handle interrupts from inside a library.
  w.Init(_wheelSensorPin, 0.0345, 8); //currently only using 1 sensor/wheel. (69mm wheel with 8 magnets)
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

void loopOrientation() {
  
  if (myCodeCell.Run(_codeCellRunInterval)) {                   // Run service loop at 10 Hz (every 100 ms)

    myCodeCell.Motion_AccelerometerRead(accelX, accelY, accelZ);  // Read acceleration values
    myCodeCell.Motion_RotationRead(Roll, Pitch, Yaw); // Read latest rotation values
/*
    if (DEBUG_SENSORS) { 
      Serial.print("X: "); Serial.print(accelX);     // Print X value
      Serial.print("  Y: "); Serial.print(accelY);   // Print Y value
      Serial.print("  Z: "); Serial.println(accelZ); // Print Z value
      Serial.printf("Roll: %.2f°, Pitch: %.2f°, Yaw: %.2f°\n", Roll, Pitch, Yaw); // Print  output
      Serial.print("State: ");
    }
*/
    //uint16_t stateRead = myCodeCell.Motion_StateRead();
    //if (stateRead != 4) {}

    switch (myCodeCell.Motion_StateRead()) {
      default:
        //if (DEBUG_SENSORS) { Serial.println("0 - Unknown"); } //0
        //..and this.
        break;
      case MOTION_STATE_ONTABLE:
        //if (DEBUG_SENSORS) { Serial.println("1 - On Table"); }  //1
        //also this..
        break;
      case MOTION_STATE_STATIONARY:
        //if (DEBUG_SENSORS) { Serial.println("2 - Stationary"); }  //2
        //use this and timer for triggering sleep.
        //start with the code from up the page.
        break;
      case MOTION_STATE_STABLE:
        //Serial.println("3 - Motion Stopped - Stabilizing"); //3
        break;
      case MOTION_STATE_MOTION:
        //Serial.println("4 - In Motion");  //4
        break;
    }

    o.AddToAverage(accelY, Pitch);
  }

  EVERY_N_MILLISECONDS(_orDirUpdateInterval) {   

    o.DoUpdateOrientation(Roll, Pitch, Yaw);
	  o.DoUpdateIndicator();

    // Always 3 cos of sensors loopOrientation()
    // Pattern 3 - loopTrackLights, "Track (basic)"
    // main lights sub-mode 3 will always be kept 3 because of the following conditions:
    if (o.GetOrientation() == 0 &&_mainLightsSubMode == 3) { o.DoUpdateDirection(); }   /* 3 cos of sub-mode 3 */
    
    if (DEBUG_SENSORS) {
      Serial.print(F("Orientation = "));
      Serial.print(o.GetOrientation());
      Serial.print(F(" | Direction = "));
      Serial.print(o.GetDirection()); 
      Serial.printf(" | Roll: %.2f°, Pitch: %.2f°, Yaw: %.2f°\n", Roll, Pitch, Yaw); // Print  output
      Serial.println();
    }
  }

}

/*----------------------------wheel sensors----------------------------*/
/* Wheel data logging
 * update the wheels with orientation info 
 * update every second
 * - currently just direction
 * - this is seperate to the straight 1-1 tracking used in sub-modes 
 */
void loopWheel() {
  EVERY_N_MILLISECONDS(1000) {                      //FastLED based non-blocking delay to update/display the sequence.
    detachInterrupt(_wheelSensorPin);               //..no interrupts whilst we process !!!
    w.DoUpdateEverySec(o.GetDirection());
    attachInterrupt(digitalPinToInterrupt(_wheelSensorPin), wheelInterrupt0, CHANGE);  //re-attach the interrupt !!!
  } 
} 
