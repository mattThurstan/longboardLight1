/*----------------------------util----------------------------*/

String getBatteryLevel() {
  String level = "";
  //have a seperate IC to control battery charging and switching between battery / plug-in power.
  //communicate with IC (prob over I2C)
  //convert value to a string
  //copy to 'level'
  return level;
}

/*----------------------------util - serial----------------------------*/
void setupSerial() {
  Serial.begin(SERIAL_SPEED);
  
  if (DEBUG) {
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
  }
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


