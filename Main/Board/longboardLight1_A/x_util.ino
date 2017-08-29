/*----------------------------util----------------------------*/

void setHeadLightsBrightness(byte l) {
  _headLightsBrightness = l;
  _headLightsColHSV.val = l;
}
//void setHeadLightsRGB(byte r, byte g, byte b) {
//  _headLightsCol.r = r; //= CRGB(r, g, b);
//  _headLightsCol.g = g;
//  _headLightsCol.b = b;
//}
void setHeadLightsHSV(byte h, byte s, byte v) {
  _headLightsColHSV.hue = h;
  _headLightsColHSV.sat = s;
  //byte vv = constrain(v, 0, 255, 0, _headLightsBrightness);
  _headLightsColHSV.val = v;
}
void setHeadLightsHS(byte h, byte s) {
  _headLightsColHSV.hue = h;
  _headLightsColHSV.sat = s;
  _headLightsColHSV.val = _headLightsBrightness;
}
void setRearLightsBrightness(byte k) {
  _rearLightsBrightness = k;
  _rearLightsColHSV.val = k;
}
void setRearLightsHSV(byte h, byte s, byte v) {
  _rearLightsColHSV.hue = h;
  _rearLightsColHSV.sat = s;
  _rearLightsColHSV.val = v;
}
void setRearLightsHS(byte h, byte s) {
  _rearLightsColHSV.hue = h;
  _rearLightsColHSV.sat = s;
  _rearLightsColHSV.val = _rearLightsBrightness;
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
//void sendCalibrationDataDump() { }
//void sendMovementDataStream() { }
//void sendSettingsDataDump() { }


