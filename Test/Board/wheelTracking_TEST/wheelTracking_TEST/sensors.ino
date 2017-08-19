/*----------------------------sensors----------------------------*/

void setupSensors() {
  setupWheel();                      //setup wheel tracking
}

/* Sensor input loop (called from Main loop) */
void loopSensors() {
  loopWheel();
}

/*----------------------------wheel sensors----------------------------*/

void setupWheel() {
  //diameter of wheel
  //circumference = 2 * PI * radius
  //
}

/* Wheel tracking */
void loopWheel() {
  //increment/decrement of led pos is done straight from interrupt
  //timed-loop
  unsigned long wheelSensorReadCurMillis = millis();    //get current time
  //this has the potential to be innacurate, but should be good enough, plus we can correct a bit using other sensors
  //if((unsigned long) (wheelSensorReadCurMillis - _wheelSensorReadPrevMillis) >= _wheelSensorReadInterval) {
  //'_wheelSensorReadInterval' needs to be hardcoded to 1 sec, otherwise we don't get 'revolutions per SECOND'
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
    
//      Serial.print(F("RPS="));
//      Serial.print(_wheelSpeedRps);
//      Serial.print(F(", MPS="));
//      Serial.print(_wheelSpeedMps);
//      Serial.print(F(", Total forward="));
//      Serial.print(_distTraveledForward);
//      Serial.print(F(", Total backward="));
//      Serial.print(_distTraveledBackward);
//      Serial.print(F(", Orientation="));
//      Serial.print(_orientation); 
//      Serial.print(F(", Current direction="));
//      Serial.print(_directionCur);
//      Serial.println();

  } //END timed-loop

} //END loopWheel

