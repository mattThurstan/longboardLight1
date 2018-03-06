/*----------------------------calibration----------------------------*/

void quickCalibration() {
  if (DEBUG) { 
    Serial.print(F("Quick calibration started"));
    Serial.println();
  }
  delay(3000);      //..it's all ok, we really do want to halt the thread!
  o.QuickCalibration();
  _doQuickCalibration = 0;
  if (DEBUG) { 
    Serial.print(F("Quick calibration done"));
    Serial.println();
  }
}

void fullCalibration() {
  if (DEBUG) { 
    Serial.print(F("Full calibration started"));
    Serial.println();

    Serial.print(F("Current.."));
    Serial.println();
    
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
  delay(9000);
  o.FullCalibration();

  //here we need to handle saving results to memory
  //..but there isn't enough memory on the Arduino Pro Mini board we are using
  
  _doFullCalibration = 0;
  if (DEBUG) { 
    Serial.print(F("Results.."));
    Serial.println();
    
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
    
    Serial.print(F("Full calibration done"));
    Serial.println();
  }
}

