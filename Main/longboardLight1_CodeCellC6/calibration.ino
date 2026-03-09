/*----------------------------calibration----------------------------*/

void quickCalibration() {
  if (DEBUG_GEN) { 
    Serial.print(F("Quick calibration started"));
    Serial.println();
  }
  delay(3000);      //..it's all ok, we really do want to halt the thread!
  //o.QuickCalibration();
  if (DEBUG_GEN) { 
    Serial.print(F("Quick calibration done"));
    Serial.println();
  }
  _doQuickCalibration = 0;
  broadcastInt("quickCalibration", _doQuickCalibration);  //reset button on device
}

void fullCalibration() {
  if (DEBUG_GEN) { 
    Serial.println(F("Full calibration started"));
    Serial.println(F("Current.."));
	  printCalibrationOffsets();
  }
  delay(9000);
  //o.FullCalibration();

  //here we need to handle saving results to memory
  //mWriteInt(30, o.GetMPU6050AccelOffsetX());
  //EEPROM.commit();
  
  if (DEBUG_GEN) 
  {
	  Serial.println(F("Results.."));
	  printCalibrationOffsets();
    Serial.println(F("Full calibration done"));
  }
  _doFullCalibration = 0;
  broadcastInt("fullCalibration", _doFullCalibration);  //reset button on device
}

void printCalibrationOffsets() { }

