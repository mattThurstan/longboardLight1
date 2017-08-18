/*----------------------------sensors----------------------------*/

void setupSensors() {
  setupWheel();                      //setup wheel tracking
  setupMPU6050();
}

/* sensor input loop
 * called from main loop
 */
void loopSensors() {
  readMPU6050filtered();
  orientation();
  if (_orientation == 0 &&_mainLightsSubMode == 3) { loopWheel(); /*wheel tracking*/ }
}

/*----------------------------wheel sensors----------------------------*/

void setupWheel() {
  //diameter of wheel
  //circumference = 2 * PI * radius
  //
}

void loopWheel() {
#ifdef DATA_LOGGING 
  //timed-loop
  unsigned long wheelSensorReadCurMillis = millis();    //get current time
  //this has the potential to be innacurate, but should be good enough, plus we can correct a bit using other sensors
  //if((unsigned long) (wheelSensorReadCurMillis - _wheelSensorReadPrevMillis) >= _wheelSensorReadInterval) {
  if((unsigned long) (wheelSensorReadCurMillis - _wheelSensorReadPrevMillis) >= 1000) {
    //get direction
    detachInterrupt(0);     //no interrupts whilst we process !!!
    _wheelSpeedRps = _wheelCounter / _wheelMagnetTotal;
    _wheelSpeedMps = _wheelSpeedRps * _wheelCircumference;              //speed in meters per second. distance travelled in 1 second
    
    if (_directionCur == 0) {
      _distTraveledForward = (_distTraveledForward + (unsigned long)_wheelSpeedMps);
    } else if (_directionCur == 1) {
      _distTraveledBackward = (_distTraveledBackward + (unsigned long)_wheelSpeedMps);
    }
    
    _wheelCounter = 0;                                  //reset counter to 0 (why did i put 1? ..was it just 3 in the morning?)
    attachInterrupt(digitalPinToInterrupt(_wheelSensorPin[0]), wheelInterrupt0, CHANGE);  //re-attach the interrupt !!!
    _wheelSensorReadPrevMillis = millis();              //store the current time
    
    #ifdef DEBUG_WHEEL
      Serial.print("RPS=");
      Serial.print(_wheelSpeedRps);
      Serial.print(", MPS=");
      Serial.print(_wheelSpeedMps);
      Serial.print(", Total forward=");
      Serial.print(_distTraveledForward);
      Serial.print(", Total backward=");
      Serial.print(_distTraveledBackward);
      Serial.print(", Orientation=");
      Serial.print(_orientation); 
      Serial.print(", Current direction=");
      Serial.print(_directionCur);
      Serial.println();
    #endif
  } //END timed-loop
#endif  //END if DATA_LOGGING
} //END loopWheel

