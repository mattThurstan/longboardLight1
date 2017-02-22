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
  loopWheel();                       //loop wheel tracking
}

/*----------------------------wheel sensors----------------------------*/

void setupWheel() {
  //diameter of wheel
  //circumference = 2 * PI * radius
  //
}

void loopWheel() {
  //speed = RPM * circumference
  //RPM = speed / circumference
  //speed = distance / time
  //distance = speed x time
  //time = distance / speed
  //pos = map(pos, 0, ?, 0, 18);
  //pos = constrain(pos, 0, 18);
  //pos = constrain(map(pos, 0, ?, 0, 18), 0, 18);
  //how many revolutions does it take to move 1 meter?
  //revs per meter = 1000 / circumference
  //eg. 70mm wheel radius
  //circumference = 2 x 3.142 (pi) x 70 = 439.88
  //revs per meter = 1000 / 439.88 = 2.733(472765)
  //revs per meter = 1000 / 440 = 2.272(727272)
  //pos = map(pos, 0, 4.54, 0, 18);

  //http://www.aqua-calc.com/convert/speed/

  //direction - use first acceleration/deceleration trigger zone to also switch direction

  //_wheelSensorTotal = 1;
  //_wheelMagnetTotal = 4;
  ////int _wheelRadius = 70;                  //radius (dist from center to the edge) of wheels in use in millimeters
  //int _wheelMagnetRadius = 10             //radius of the magnet positioning. prob about 10mm
  //_wheelCounter = 0;
  //double _wheelSpeedMps
  //int _wheelSpeedMpm = 0;                                   //wheel speed in Meters Per Minute
  //int _wheelSpeedKph = 0;                                   //wheel speed in Kilometers Per Hour
  //int _wheelSpeedMph = 0;                                   //wheel speed in Miles Per Hour
  //_wheelSensorReadInterval = 1000;        //read loop interval in milliseconds   1000
  //_wheelSensorReadPrevMillis = 0;         //previous time for reference
  //_wheelCircumference
  
  //timed-loop
  unsigned long wheelSensorReadCurMillis = millis();    //get current time
  //this has the potential to be innacurate, but should be good enough, plus we can correct a bit using other sensors
  if((unsigned long) (wheelSensorReadCurMillis - _wheelSensorReadPrevMillis) >= _wheelSensorReadInterval) {
    //get direction
    detachInterrupt(0);     //no interrupts whilst we process !!!

    //..dividing int(s) by float(s) ..not good. prob move some calculations to app/web end when wireless communication is implemented.
    //..hmm. use double, just don't go over 99 miles an hour!? ..better check with memory management on that one..
    //how many times has the sensor triggered in 1 second
    //divide by number of magnets
    //_wheelCounter within this timed loop is 'revolutions per second' * 4
    //int rps = _wheelCounter / _wheelMagnetTotal;
    //int rpsXthousand = (_wheelCounter  / _wheelMagnetTotal) * 1000; //gets rid of decimal point
    double rps = _wheelCounter / _wheelMagnetTotal;
    //_wheelSpeedRps = _wheelCounter / _wheelMagnetTotal;
    //int rpm = rps * 60;                                 //multiply by 60 (secs in a minute)
    //or.. int rpm = _wheelCounter * 15;                  //quarter minute

    //speed = RPM * circumference
    _wheelSpeedMps = rps * _wheelCircumference;              //speed in meters per second - distance travelled in 1 second
    // _wheelSpeedMpm = rpm * _wheelCircumference;              //speed in meters per minute - distance travelled in 1 minute

    //if (direction == forward)..
    _distTraveledForward = (_distTraveledForward + (unsigned long)_wheelSpeedMps);
/*    
    //kph
    _wheelSpeedKph = 3.6 * _wheelSpeedMps;              //kilometers per hour = 3.6 * (meters per second)
    //_wheelSpeedKph = 0.06 * _wheelSpeedMpm;             //kilometers per hour = 0.06 * (meters per minute)
    //mph
    //_wheelSpeedMph = 0.0372822715 * _wheelSpeedMpm;     //miles per hour = 0.0372822715 * (meters per minute)
    _wheelSpeedMph = 0.621371192 * _wheelSpeedKph;      //miles per hour = 0.621371192 * (kilometers per hour)
*/    
//    #ifdef DEBUG
//      Serial.print("RPS=");
//      Serial.print(rps);
//      Serial.print(", MPS=");
//      Serial.print(_wheelSpeedMps);
//      Serial.print(", Total Distance=");
//      Serial.print(_distTraveledForward);
//      Serial.print(", Orientation=");
//      Serial.print(_orientation); 
//      Serial.println();
//    #endif
//  
    _wheelCounter = 1;                                  //reset counter to 1
    _wheelSensorReadPrevMillis = millis();              //store the current time
    attachInterrupt(digitalPinToInterrupt(_wheelSensorPin[0]), wheelInterrupt0, CHANGE);  //re-attach the interrupt !!!
  } //END timed-loop

}


