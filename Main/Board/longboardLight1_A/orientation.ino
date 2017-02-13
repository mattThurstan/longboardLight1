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


  //timed-loop
  unsigned long orientationCurMillis = millis();     //get current time
  if((unsigned long) (orientationCurMillis - _orientationPrevMillis) >= _orientationInterval) {
  
    if ( _mpu6050FilteredCurX < ( _mpu6050AccelZeroX - (90/2) ) ) {
      if (orFlag[0] == false) { orCounter[0] = millis(); orFlag[0] = true; }  //if already true then don't reset the counter
    }
    else if ( _mpu6050FilteredCurX < ( _mpu6050AccelZeroX + (90/2) ) && _mpu6050FilteredCurX > ( _mpu6050AccelZeroX - (90/2) ) ) {
      if (orFlag[1] == false) { orCounter[1] = millis(); orFlag[1] = true; }
    } 
    else if ( _mpu6050FilteredCurX > ( _mpu6050AccelZeroX + (90/2) ) ) { 
      if (orFlag[2] == false) { orCounter[2] = millis(); orFlag[2] = true; }
    }
  
    if ( _mpu6050FilteredCurY < ( _mpu6050AccelZeroY - (90/2) ) ) {
      if (orFlag[3] == false) { orCounter[3] = millis(); orFlag[3] = true; }  //if already true then don't reset the counter
    }
    else if ( _mpu6050FilteredCurY < ( _mpu6050AccelZeroY + (90/2) ) && _mpu6050FilteredCurY > ( _mpu6050AccelZeroY - (90/2) ) ) {
      if (orFlag[4] == false) { orCounter[4] = millis(); orFlag[4] = true; }
    } 
    else if ( _mpu6050FilteredCurY > ( _mpu6050AccelZeroY + (90/2) ) ) { 
      if (orFlag[5] == false) { orCounter[5] = millis(); orFlag[5] = true; }
    }
  
    if ( _mpu6050FilteredCurZ < ( _mpu6050AccelZeroZ - (90/2) ) ) {
      if (orFlag[6] == false) { orCounter[6] = millis(); orFlag[6] = true; }  //if already true then don't reset the counter
    }
    else if ( _mpu6050FilteredCurZ < ( _mpu6050AccelZeroZ + (90/2) ) && _mpu6050FilteredCurZ > ( _mpu6050AccelZeroZ - (90/2) ) ) {
      if (orFlag[7] == false) { orCounter[7] = millis(); orFlag[7] = true; }
    } 
    else if ( _mpu6050FilteredCurZ > ( _mpu6050AccelZeroZ + (90/2) ) ) { 
      if (orFlag[8] == false) { orCounter[8] = millis(); orFlag[8] = true; }
    }

    unsigned long orGetMillis = millis();
    for (int i = 0; i < 9; i++) {
      if (orFlag[i] == true) {
        if ( (unsigned long) (orGetMillis - orCounter[i]) >= orInterval) {
          int m;  //matrix XYZ switch
          int n;  //value for matrix
          if (i >= 0 && i <= 2) {
            //first 3 flags
            m = 0;
            n = i;
          }
          else if (i >= 3 && i <= 5) { 
            m = 1;
            n = i-3;
          }
          else if (i >= 6 && i <= 8) { 
            m = 2;
            n = i-6;
          }
          orMatrix[m] = n;
          orFlag[i] = false; //reset
        }
      }
    }

    if (orMatrix[0] == 1
          && orMatrix[1] == 1
          //&& orMatrix[2] == 1
          ) { _orientation = 0; }  //1, 1, 1 - flat
    else if (orMatrix[0] == 1
          && (orMatrix[1] == 0 || orMatrix[1] == 2)
          && orMatrix[2] == 0) { _orientation = 1; }  //1, 0 or 2, 1 - upside-down
    else if (orMatrix[0] == 2
          && orMatrix[1] == 1
          //&& orMatrix[2] == 1
          ) { _orientation = 2; }  //2, 1, 1 - up
    else if (orMatrix[0] == 0
          && orMatrix[1] == 1
          //&& orMatrix[2] == 1
          ) { _orientation = 3; }  //0, 1, 1 - down
    else if (orMatrix[0] == 1
          && orMatrix[1] == 0
          //&& orMatrix[2] == 1
          ) { _orientation = 4; }  //1, 0, 1 - left  
    else if (orMatrix[0] == 1
          && orMatrix[1] == 2
          //&& orMatrix[2] == 1
          ) { _orientation = 5; }  //1, 2, 1 - right
  
    #ifdef DEBUG_MPU6050
      Serial.print("0");
      Serial.print(", ");
      Serial.print("0");
      Serial.print(", ");
      Serial.print(_mpu6050FilteredCurX);  //orMatrix[0]
      Serial.print(", ");
      Serial.print("0");
      Serial.print(", ");
      Serial.print("0");
      Serial.print(", ");
      Serial.print(_mpu6050FilteredCurY);
      Serial.print(", ");
      Serial.print("0");
      Serial.print(", ");
      Serial.print("0");
      Serial.print(", ");
      Serial.print(_mpu6050FilteredCurZ);
      Serial.print(", ");
      Serial.print(_orientation); 
      //linebreak
      Serial.write(10);
      Serial.write(13);
    #endif

    _orientationPrevMillis = millis();               //store the current time
  } //END timed-loop
  
} //END orientation





