/*----------------------------orientation----------------------------*/

/*_orientation: 0=flat, 1=upside-down, 2=up, 3=down, 4=left-side, 5=right-side
 * flat 0, 0, 0
 * upside-down 0, 180/-180, -45 ???
 * up 90, 0, 0
 * down -90, 0, 0
 * left 0, -90, 0
 * right 0, 90, 0
 */

void orientation() {

  //orientation matrix: 9 -> 3 -> 1
  //err.. think all this is wrong

  //timed-loop
  unsigned long orientationCurMillis = millis();     //get current time
  if((unsigned long) (orientationCurMillis - _orientationPrevMillis) >= _orientationInterval) {

    //set the 9-matrix
//    if ( _mpu6050FilteredCurX < ( _mpu6050AccelZeroX - 45 ) ) {
//      if (orFlag[0] == false) { orCounter[0] = millis(); orFlag[0] = true; }  //if already true then don't reset the counter
//    }
//    else if ( _mpu6050FilteredCurX < ( _mpu6050AccelZeroX + 45 ) && _mpu6050FilteredCurX > ( _mpu6050AccelZeroX - 45 ) ) {
//      if (orFlag[1] == false) { orCounter[1] = millis(); orFlag[1] = true; }
//    } 
//    else if ( _mpu6050FilteredCurX > ( _mpu6050AccelZeroX + 45 ) ) { 
//      if (orFlag[2] == false) { orCounter[2] = millis(); orFlag[2] = true; }
//    }
//  
//    if ( _mpu6050FilteredCurY < ( _mpu6050AccelZeroY - 45 ) ) {
//      if (orFlag[3] == false) { orCounter[3] = millis(); orFlag[3] = true; }  //if already true then don't reset the counter
//    }
//    else if ( _mpu6050FilteredCurY < ( _mpu6050AccelZeroY + 45 ) && _mpu6050FilteredCurY > ( _mpu6050AccelZeroY - 45 ) ) {
//      if (orFlag[4] == false) { orCounter[4] = millis(); orFlag[4] = true; }
//    } 
//    else if ( _mpu6050FilteredCurY > ( _mpu6050AccelZeroY + 45 ) ) { 
//      if (orFlag[5] == false) { orCounter[5] = millis(); orFlag[5] = true; }
//    }
//  
//    if ( _mpu6050FilteredCurZ < ( _mpu6050AccelZeroZ - 45 ) ) {
//      if (orFlag[6] == false) { orCounter[6] = millis(); orFlag[6] = true; }  //if already true then don't reset the counter
//    }
//    else if ( _mpu6050FilteredCurZ < ( _mpu6050AccelZeroZ + 45 ) && _mpu6050FilteredCurZ > ( _mpu6050AccelZeroZ - 45 ) ) {
//      if (orFlag[7] == false) { orCounter[7] = millis(); orFlag[7] = true; }
//    } 
//    else if ( _mpu6050FilteredCurZ > ( _mpu6050AccelZeroZ + 45 ) ) { 
//      if (orFlag[8] == false) { orCounter[8] = millis(); orFlag[8] = true; }
//    }
    //set the 3-matrix directly
//    if ( _mpu6050FilteredCurX < ( _mpu6050AccelZeroX - 45 ) ) { orMatrix[0] = 0; }
//    else if ( _mpu6050FilteredCurX < ( _mpu6050AccelZeroX + 45 ) && _mpu6050FilteredCurX > ( _mpu6050AccelZeroX - 45 ) ) { orMatrix[0] = 1; }
//    else if ( _mpu6050FilteredCurX > ( _mpu6050AccelZeroX + 45 ) ) { orMatrix[0] = 2; }
//  
//    if ( _mpu6050FilteredCurY < ( _mpu6050AccelZeroY - 45 ) ) { orMatrix[1] = 0; }
//    else if ( _mpu6050FilteredCurY < ( _mpu6050AccelZeroY + 45 ) && _mpu6050FilteredCurY > ( _mpu6050AccelZeroY - 45 ) ) { orMatrix[1] = 1; }
//    else if ( _mpu6050FilteredCurY > ( _mpu6050AccelZeroY + 45 ) ) { orMatrix[1] = 2; }
//  
//    if ( _mpu6050FilteredCurZ < ( _mpu6050AccelZeroZ - 45 ) ) { orMatrix[2] = 0; }
//    else if ( _mpu6050FilteredCurZ < ( _mpu6050AccelZeroZ + 45 ) && _mpu6050FilteredCurZ > ( _mpu6050AccelZeroZ - 45 ) ) { orMatrix[2] = 1; }
//    else if ( _mpu6050FilteredCurZ > ( _mpu6050AccelZeroZ + 45 ) ) { orMatrix[2] = 2; }

    float cutoff = 45;  //starting at zero calibration, we need to know 90deg either way, so 45 is halfway point anywhere from 0
    for (int i = 0; i < 3; i++) {
      if ( _mpu6050FilteredCur[i] < ( _mpu6050AccelZero[i] - cutoff ) ) { orMatrix[i] = 0; }
      else if ( _mpu6050FilteredCur[i] < ( _mpu6050AccelZero[i] + cutoff ) && _mpu6050FilteredCur[i] > ( _mpu6050AccelZero[i] - cutoff ) ) { orMatrix[i] = 1; }
      else if ( _mpu6050FilteredCur[i] > ( _mpu6050AccelZero[i] + cutoff ) ) { orMatrix[i] = 2; }
    }

    //compare 9-matrix and set 3-matrix
//    unsigned long orGetMillis = millis();
//    for (int i = 0; i < 9; i++) {
//      if (orFlag[i] == true) {
//        if ( (unsigned long) (orGetMillis - orCounter[i]) >= orInterval) {
//          int m;  //matrix XYZ switch
//          int n;  //value for matrix
//          if (i >= 0 && i <= 2) {
//            //first 3 flags
//            m = 0;
//            n = i;
//          }
//          else if (i >= 3 && i <= 5) { 
//            m = 1;
//            n = i-3;
//          }
//          else if (i >= 6 && i <= 8) { 
//            m = 2;
//            n = i-6;
//          }
//          orMatrix[m] = n;
//          orFlag[i] = false; //reset
//        }
//      }
//    }

    //compare 3-matrix, set orientation, save prev, start timer
    //unsigned long orGetMillis = millis();
    if (orMatrix[0] == 1
          && orMatrix[1] == 1
          //&& orMatrix[2] == 1
          ) {
            _orOrientationTemp = 0;
            if (orFlag == false) { _orOrientationSave = 0; orCounter = millis(); orFlag = true; }
          }  //1, 1, 1 - flat
//    else if (orMatrix[0] == 1
//          && (orMatrix[1] == 0 || orMatrix[1] == 2)
//          && orMatrix[2] == 0) {
//            _orOrientationTemp = 1;
//            if (orFlag == false) { _orOrientationSave = 1; orCounter = millis(); orFlag = true; }
//          }  //1, 0 or 2, 1 - upside-down - have to work on this one..
    else if (orMatrix[0] == 2
          && orMatrix[1] == 1
          //&& orMatrix[2] == 1
          ) { 
            _orOrientationTemp = 2;
            if (orFlag == false) { _orOrientationSave = 2; orCounter = millis(); orFlag = true; } 
          }  //2, 1, 1 - up
    else if (orMatrix[0] == 0
          && orMatrix[1] == 1
          //&& orMatrix[2] == 1
          ) { 
            _orOrientationTemp = 3;
            if (orFlag == false) { _orOrientationSave = 3; orCounter = millis(); orFlag = true; }
          }  //0, 1, 1 - down
    else if (orMatrix[0] == 1
          && orMatrix[1] == 0
          //&& orMatrix[2] == 1
          ) { 
            _orOrientationTemp = 4;
            if (orFlag == false) { _orOrientationSave = 4; orCounter = millis(); orFlag = true; }
          }  //1, 0, 1 - left  
    else if (orMatrix[0] == 1
          && orMatrix[1] == 2
          //&& orMatrix[2] == 1
          ) { 
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
      Serial.print("0");
      Serial.print(", ");
      Serial.print("0");
      Serial.print(", ");
      Serial.print(_mpu6050FilteredCur[0]);  //orMatrix[0]
      Serial.print(", ");
      Serial.print("0");
      Serial.print(", ");
      Serial.print("0");
      Serial.print(", ");
      Serial.print(_mpu6050FilteredCur[1]);
      Serial.print(", ");
      Serial.print("0");
      Serial.print(", ");
      Serial.print("0");
      Serial.print(", ");
      Serial.print(_mpu6050FilteredCur[2]);
      Serial.print(", ");
      Serial.print(_orientation); 
      
      //linebreak
      Serial.write(10);
      Serial.write(13);
    #endif

    _orientationPrevMillis = millis();               //store the current time
  } //END timed-loop
  
} //END orientation





