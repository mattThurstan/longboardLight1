/*----------------------------calibration----------------------------*/
void quickCalibration() {
  if (DEBUG_GEN) { 
    Serial.print(F("Quick calibration started"));
    Serial.println();
  }
  delay(3000);      //..it's all ok, we really do want to halt the thread!
  o.QuickCalibration();
  if (DEBUG_GEN) { 
    Serial.print(F("Quick calibration done"));
    Serial.println();
  }
  _doQuickCalibration = 0;
  //broadcastInt("quickCalibration", _doQuickCalibration);  //reset button on device
}

void fullCalibration() {
  if (DEBUG_GEN) { 
    Serial.println(F("Full calibration started"));
    Serial.println(F("Current.."));
	printCalibrationOffsets();
  }
  delay(9000);
  o.FullCalibration();

  //here we need to handle saving results to memory
  //..but there isn't enough memory on the Arduino Pro Mini board we are using
  //..ahah!!! - now on esp8266!!!
  mWriteInt(30, o.GetMPU6050AccelOffsetX());
  mWriteInt(34, o.GetMPU6050AccelOffsetY());
  mWriteInt(38, o.GetMPU6050AccelOffsetZ());
  mWriteInt(42, o.GetMPU6050GyroOffsetX());
  mWriteInt(46, o.GetMPU6050GyroOffsetY());
  mWriteInt(50, o.GetMPU6050GyroOffsetZ());
  EEPROM.commit();
  
  if (DEBUG_GEN) 
  {
	Serial.println(F("Results.."));
	printCalibrationOffsets();
    Serial.println(F("Full calibration done"));
  }
  _doFullCalibration = 0;
  //broadcastInt("fullCalibration", _doFullCalibration);  //reset button on device
}

void printCalibrationOffsets() 
{
	Serial.print(F("MPU6050 Accel Offset XYZ = "));
	Serial.print(o.GetMPU6050AccelOffsetX());
	Serial.print(F(", "));
	Serial.print(o.GetMPU6050AccelOffsetY());
	Serial.print(F(", "));
	Serial.print(o.GetMPU6050AccelOffsetZ());
	Serial.println();

	Serial.print(F("MPU6050 Gyro Offset XYZ = "));
	Serial.print(o.GetMPU6050GyroOffsetX());
	Serial.print(F(", "));
	Serial.print(o.GetMPU6050GyroOffsetY());
	Serial.print(F(", "));
	Serial.print(o.GetMPU6050GyroOffsetZ());
	Serial.println();
}

