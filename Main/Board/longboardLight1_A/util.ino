/*----------------------------util----------------------------*/

void incrementMainLightsSubMode() {
  if ( _mainLightsSubMode >= (_mainLightsSubModeTotal - 1) ) {
    _mainLightsSubMode = 0;    //wrap-around at end back to 0
  } else {
    _mainLightsSubMode += 1;   //increase by 1
  }
  EEPROM.write(8, _mainLightsSubMode);
}
void decrementMainLightsSubMode() {
  if ( _mainLightsSubMode <= 0 ) {
    _mainLightsSubMode = (byte)(_mainLightsSubModeTotal - 1);    //wrap-around at beginning to the end (1 less than the total)
  } else {
    _mainLightsSubMode -= 1;   //decrease by 1
  }
  EEPROM.write(8, _mainLightsSubMode);
}

void setupSerial() {
  #ifdef DEBUG
    Serial.begin(9600);                     //usb
    //Serial.begin(38400);                    //for doing MPU6050 stuff - don't really need it this fast cos not controlling it over serial
    _inputString.reserve(200);              // reserve 200 bytes for the inputString:
    Serial.println();
    Serial.print(_progName);
    Serial.print(" ");
    Serial.print(_progVers);
    Serial.println();
    Serial.print("..");
    Serial.println();
  #endif
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  #ifdef DEBUG
    while (Serial.available()) {
      char inChar = (char)Serial.read();      // get the new byte:
      _inputString += inChar;                 // add it to the inputString:
      // if the incoming character is a newline, set a flag
      // so the main loop can do something about it:
      if (inChar == '\n') {
        _stringComplete = true;
      }
    }
  #endif
}

/*----------------------------util - send data----------------------------*/
//..see '_longboardLight1_A_SerialTestDataViewer.v4p' for the first receiving end

/*
 * Calibration data sent as dump
 * 
 * XYZ_
 * _mpu6050AccelZero[0]
 * _mpu6050AccelZero[1]
 * _mpu6050AccelZero[2]
 * _mpu6050GyroZero[0]
 * _mpu6050GyroZero[1]
 * _mpu6050GyroZero[2]
 * _mpu6050AccelReadAverage[0]
 * _mpu6050AccelReadAverage[1]
 * _mpu6050AccelReadAverage[2]
 * _mpu6050GyroReadAverage[0]
 * _mpu6050GyroReadAverage[1]
 * _mpu6050GyroReadAverage[2]
 */

void sendCalibrationDataDump() {
  
      Serial.print(_mpu6050AccelZero[0]);
      Serial.print(", ");
      Serial.print(_mpu6050AccelZero[1]);
      Serial.print(", ");
      Serial.print(_mpu6050AccelZero[2]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroZero[0]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroZero[1]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroZero[2]);
      Serial.print(", ");
      Serial.print(_mpu6050AccelReadAverage[0]);
      Serial.print(", ");
      Serial.print(_mpu6050AccelReadAverage[1]);
      Serial.print(", ");
      Serial.print(_mpu6050AccelReadAverage[2]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroReadAverage[0]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroReadAverage[1]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroReadAverage[2]);
      
      //linebreak
      Serial.write(10);
      Serial.write(13);
}

/*
 * Movement data sent as stream
 * 
 * RAW XYZ_
 * _mpu6050AccelRead[0]
 * _mpu6050AccelRead[1]
 * _mpu6050AccelRead[2]
 * _mpu6050GyroRead[0]
 * _mpu6050GyroRead[1]
 * _mpu6050GyroRead[2]

 * FINAL XYZ_
 * _mpu6050FilteredCur[0]
 * _mpu6050FilteredCur[1]
 * _mpu6050FilteredCur[2]

 * MOVEMENT_
 * _wheelSpeedRps
 * _wheelSpeedMps
 * _distTraveledForward
 * _distTraveledBackward
 * _orientation
 * _directionCur
 */
void sendMovementDataStream() {
  
  unsigned long sendMovementDataStreamCurMillis = millis();     //get current time
  
  if((unsigned long) (sendMovementDataStreamCurMillis - _sendMovementDataStreamPrevMillis) >= _sendMovementDataStreamInterval) {
  
      //Serial.print("M");  //29 inc this one and last
      //Serial.print(", ");
      Serial.print(_mpu6050AccelRead[0]);
      Serial.print(", ");
      Serial.print(_mpu6050AccelRead[1]);
      Serial.print(", ");
      Serial.print(_mpu6050AccelRead[2]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroRead[0]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroRead[1]);
      Serial.print(", ");
      Serial.print(_mpu6050GyroRead[2]);
      Serial.print(", ");
      Serial.print(_mpu6050FilteredCur[0]);
      Serial.print(", ");
      Serial.print(_mpu6050FilteredCur[1]);
      Serial.print(", ");
      Serial.print(_mpu6050FilteredCur[2]);
      Serial.print(", ");
      Serial.print(_wheelSpeedRps); 
      Serial.print(", ");
      Serial.print(_wheelSpeedMps); 
      Serial.print(", ");
      Serial.print(_distTraveledForward); 
      Serial.print(", ");
      Serial.print(_distTraveledBackward); 
      Serial.print(", ");
      Serial.print(_orientation); 
      Serial.print(", ");
      Serial.print(_directionCur); 
      //Serial.print(", ");
      //Serial.print("W");  //29 inc this one and last
      
      //linebreak
      Serial.write(10);
      Serial.write(13);
      
    _sendMovementDataStreamPrevMillis = millis();               //store the current time
  } //END timed-loop
} //END sendMovementDataStream

/*
 * Settings sent as requested dump
 * 
 * _progName
 * _progVers
  
 * _wheelRadius
 * _ledGlobalBrightnessCur
 * _headLightsBrightness
 * _rearLightsBrightness
 * _trackLightsFadeAmount
  
 * _doFullCalibration
 * _doQuickCalibration
  
 * _sleepActive
 * _breathingActive
 * _headLightsActive
 * _rearLightsActive
 * _mainLightsSubMode
 * _brakeActive
 * _indicatorsEnabled
 * _indicatorLeftActive
 * _indicatorRightActive
 */
void sendSettingsDataDump() {
  
}


