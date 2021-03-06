/*----------------------------util----------------------------*/

/*----------------------------util - led----------------------------*/
void setGlobalBrightness(byte b) {
  _ledGlobalBrightnessCur = b;
  FastLED.setBrightness(_ledGlobalBrightnessCur);
}

void setHeadLightsBrightness(byte j) {
  _headLightsBrightness = j;
  _headLightsColHSV.val = j;
}

void setRearLightsBrightness(byte k) {
  _rearLightsBrightness = k;
  _rearLightsColHSV.val = k;
}

/*----------------------------util main lights sub-mode----------------------------*/
void setMainLightsSubMode(byte m) {
  if ( m < 0 || m >= (_mainLightsSubModeTotal - 1) ) {
    m = 0;    //wrap-around at end/start to 0
  } else {
    _mainLightsSubMode = m;
  }
  EEPROM.write(8, _mainLightsSubMode);
}

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

/*----------------------------util - serial----------------------------*/
void setupSerial() {
  #ifdef DEBUG
    Serial.begin(SERIAL_SPEED);                     //usb
    delay(100);
    //Serial.setDebugOutput(true);    // ???
    //_inputString.reserve(200);              // reserve 200 bytes for the inputString:
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
//void serialEvent() {
//  #ifdef DEBUG
//    while (Serial.available()) {
//      char inChar = (char)Serial.read();      // get the new byte:
//      _inputString += inChar;                 // add it to the inputString:
//      // if the incoming character is a newline, set a flag
//      // so the main loop can do something about it:
//      if (inChar == '\n') {
//        _stringComplete = true;
//      }
//    }
//  #endif
//}

/*----------------------------util - send data----------------------------*/
//..see '_longboardLight1_A_SerialTestDataViewer.v4p' for the first receiving end
//void sendCalibrationDataDump() { }
//void sendMovementDataStream() { }
//void sendSettingsDataDump() { }


