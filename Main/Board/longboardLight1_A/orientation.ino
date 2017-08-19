/*----------------------------orientation----------------------------*/

void orientation() {

//  unsigned long orientationCurMillis = millis();     //get current time
//  if((unsigned long) (orientationCurMillis - _orientationPrevMillis) >= _orientationInterval) {
  EVERY_N_MILLISECONDS(_orientationInterval) {                     //FastLED based non-blocking delay to update/display the sequence.

    float cutoff = 45;  //starting at zero calibration, we need to know 90deg either way, so 45 is halfway point anywhere from 0
    for (int i = 0; i < 3; i++) {
      if ( _mpu6050FilteredCur[i] < ( _mpu6050AccelZero[i] - cutoff ) ) { orMatrix[i] = 0; }
      else if ( _mpu6050FilteredCur[i] < ( _mpu6050AccelZero[i] + cutoff ) && _mpu6050FilteredCur[i] > ( _mpu6050AccelZero[i] - cutoff ) ) { orMatrix[i] = 1; }
      else if ( _mpu6050FilteredCur[i] > ( _mpu6050AccelZero[i] + cutoff ) ) { orMatrix[i] = 2; }
    }

    //compare 3-matrix, set orientation temp
    if (orMatrix[0] == 1 && orMatrix[1] == 1 ) {
            _orOrientationTemp = 0;
            if (orFlag == false) { _orOrientationSave = 0; orCounter = millis(); orFlag = true; }
          }  //1, 1, 1 - flat
//upside-down - have to work on this one..
//    else if (orMatrix[0] == 1
//          && (orMatrix[1] == 0 || orMatrix[1] == 2)
//          && orMatrix[2] == 0) {
//            _orOrientationTemp = 1;
//            if (orFlag == false) { _orOrientationSave = 1; orCounter = millis(); orFlag = true; }
//          }  //1, 0 or 2, 1 - upside-down - have to work on this one..
    else if (orMatrix[0] == 2 && orMatrix[1] == 1 ) { 
            _orOrientationTemp = 2;
            if (orFlag == false) { _orOrientationSave = 2; orCounter = millis(); orFlag = true; } 
          }  //2, 1, 1 - up
    else if (orMatrix[0] == 0 && orMatrix[1] == 1 ) { 
            _orOrientationTemp = 3;
            if (orFlag == false) { _orOrientationSave = 3; orCounter = millis(); orFlag = true; }
          }  //0, 1, 1 - down
    else if (orMatrix[0] == 1 && orMatrix[1] == 0 ) { 
            _orOrientationTemp = 4;
            if (orFlag == false) { _orOrientationSave = 4; orCounter = millis(); orFlag = true; }
          }  //1, 0, 1 - left  
    else if (orMatrix[0] == 1 && orMatrix[1] == 2 ) { 
            _orOrientationTemp = 5;
            if (orFlag == false) { _orOrientationSave = 5; orCounter = millis(); orFlag = true; }
          }  //1, 2, 1 - right

    unsigned long orGetMillis = millis();
    if (orFlag == true) {
      if ( (unsigned long) (orGetMillis - orCounter) >= orInterval) {
        if (_orOrientationSave == _orOrientationTemp) {
          //is the orientation still the same as when we took a sample and set the timer?
          //if so, set the actual orientation
          _orientation = _orOrientationTemp;
        }
        orFlag = false; //either way, reset
      }
    }
  
    #ifdef DEBUG_ORIENTATION
      //stil sending 10 values here cos outputing to same VVVV test patch as readMPU6050filtered
      Serial.print(F("0"));
      Serial.print(F(", "));
      Serial.print(F("0"));
      Serial.print(F(", "));
      Serial.print(_mpu6050FilteredCur[0]);  //orMatrix[0]
      Serial.print(F(", "));
      Serial.print(F("0"));
      Serial.print(F(", "));
      Serial.print(F("0"));
      Serial.print(F(", "));
      Serial.print(_mpu6050FilteredCur[1]);
      Serial.print(F(", "));
      Serial.print(F("0"));
      Serial.print(F(", "));
      Serial.print(F("0"));
      Serial.print(F(", "));
      Serial.print(_mpu60500FilteredCur[2]);
      Serial.print(F(", "));
      Serial.print(_orientation); 
      
      //linebreak
      Serial.write(10);
      Serial.write(13);
    #endif

//    _orientationPrevMillis = millis();               //store the current time
  } //END timed-loop
  
} //END orientation


void showOrientation() {
  fadeToBlackBy( _leds, _ledNum, 16);
  if (_orientation == 0) {
    //flat
    _leds(ledSegment[0].first, ledSegment[0].last) = CRGB::White;             //(midpoint) back
    _leds[ledSegment[1].first + _orientationTestSideMidpoint] = CRGB::White;  //midpoint left
    _leds[ledSegment[2].first + _orientationTestSideMidpoint] = CRGB::White;  //midpoint right
    _leds(ledSegment[3].first, ledSegment[3].last) = CRGB::White;             //(midpoint) front
  }
  if (_orientation == 1) {
    //upside-down
    //_leds[ledSegment[2].first+1] = CRGB::White;
  }
  if (_orientation == 2) {
    //up
    _leds(ledSegment[3].first, ledSegment[3].last) = CRGB::White;             //(midpoint) front
  }
  if (_orientation == 3) {
    //down
    _leds(ledSegment[0].first, ledSegment[0].last) = CRGB::White;             //(midpoint) back
  }
  if (_orientation == 4) {
    //on its left-side
    _leds[ledSegment[1].first + _orientationTestSideMidpoint] = CRGB::White;  //midpoint left
  }
  if (_orientation == 5) {
    //on its right-side
    _leds[ledSegment[2].first + _orientationTestSideMidpoint] = CRGB::White;  //midpoint right
  }
  
} //END showOrientation


