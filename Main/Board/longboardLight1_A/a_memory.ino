/*----------------------------memory load/save---------------------*/

/*
 * NOTE - all eeprom saves will be wiped when a new sketch is uploaded, but will persist thru normal use.
 * 
 * Settings to save (512 total)
 * 
 * 0=   BLANK
 * 1=  _breathingEnabled          - boolean
 * 2=  _headLightsEnabled         - boolean
 * 3=  _rearLightsEnabled         - boolean
 * 4=  _indicatorsEnabled         - boolean
 * 5=   BLANK
 * 6=   BLANK
 * 7=   system save indicator     = 0/1
 * 8=  _mainLightsSubMode         - 0-255
 * 9=  _ledGlobalBrightnessCur    - 0-255
 * 10= _headLightsBrightness      - 0-255 _headLightsColHSV.val
 * 11= _rearLightsBrightness      - 0-255 _rearLightsColHSV.val
 * 12= _trackLightsFadeAmount ??? - 0-255
 * 13=r
 * 14=g
 * 15=b
 * 16=r2
 * 17=g2
 * 18=b2
 * 19=power
 * 
 * 30=MPU6050AccelOffsetX (prt.0) (uint16_t splits into 4 bytes to save)
 * 31=MPU6050AccelOffsetX (prt.1)
 * 32=MPU6050AccelOffsetX (prt.2)
 * 33=MPU6050AccelOffsetX (prt.3)
 * 34=MPU6050AccelOffsetY (prt.0)
 * 35=MPU6050AccelOffsetY (prt.1)
 * 36=MPU6050AccelOffsetY (prt.2)
 * 37=MPU6050AccelOffsetY (prt.3)
 * 38=MPU6050AccelOffsetZ (prt.0)
 * 39=MPU6050AccelOffsetZ (prt.1)
 * 40=MPU6050AccelOffsetZ (prt.2)
 * 41=MPU6050AccelOffsetZ (prt.3)
 * 
 * 42=MPU6050GyroOffsetX (prt.0)
 * 43=MPU6050GyroOffsetX (prt.1)
 * 44=MPU6050GyroOffsetX (prt.2)
 * 45=MPU6050GyroOffsetX (prt.3)
 * 46=MPU6050GyroOffsetY (prt.0)
 * 47=MPU6050GyroOffsetY (prt.1)
 * 48=MPU6050GyroOffsetY (prt.2)
 * 49=MPU6050GyroOffsetY (prt.3)
 * 50=MPU6050GyroOffsetZ (prt.0)
 * 51=MPU6050GyroOffsetZ (prt.1)
 * 52=MPU6050GyroOffsetZ (prt.2)
 * 53=MPU6050GyroOffsetZ (prt.3)
 * 
 */
void setDefaultSettings() {
  if (DEBUG) { 
    Serial.print(F("Setting default settings"));
    Serial.println();
  }

  _testMode = 0;
  _curTestMode = 0;
  
  //mE = { true, true, true, true, true, false };       // Enabled 
  //mA = { false, false, true, true, false, false };    // Active
  mE = { 1, 1, 1, 1, 1, 0 }; // Enabled
  mA = { 0, 0, 1, 1, 0, 0 }; // Active

  _mainLightsSubMode = 2;  //3
  _ledGlobalBrightnessCur = 255;
  //setHeadLightsBrightness(200);
  //_headLightsBrightness = 255;
  _headLightsColHSV.val = 255;
  //setRearLightsBrightness(200);
  //_rearLightsBrightness = 255;
  _rearLightsColHSV.val = 255;
  _trackLightsFadeAmount = 16;

  //these cant be saved/loaded round here anyway as the library doesnt get initialised till later in setup..  - wrong! gets defaults set in library when declared!
  //offsets not done yet, requires splitting numbers, or just wait till get replacement board with WIFI/Bluetooth and more memory
  //not going to hold these in memory here as that would be duplicating what is in the library
  //_mpu6050AccelOffset[3] = {436, 1956, 1318};       //XYZ accel offsets to write to the MPU6050 - get from full calibration and save to memory
  //_mpu6050GyroOffset[3] = {9, -32, 69};             //XYZ gyro offsets to write to the MPU6050 - get from full calibration and save to memory
}

void loadAllSettings() {
  //this gets called at the beginning of setup.
  
  //if system settings saved flag set at pos 7 is '1', then we have system data to be read from memory.. failsafe
  if (EEPROM.read(7) == 1) {
    if (DEBUG) { 
      Serial.print(F("Loading settings"));
      Serial.println();
    }
    
    mE.breathe = EEPROM.read(1);
    mE.head = EEPROM.read(2);
    mE.rear = EEPROM.read(3);
    mE.indicate = EEPROM.read(4);
    
    _mainLightsSubMode = EEPROM.read(8);  
    _ledGlobalBrightnessCur = EEPROM.read(9);
    //_headLightsBrightness = EEPROM.read(10);
    _headLightsColHSV.val = EEPROM.read(10);
    //_rearLightsBrightness = EEPROM.read(11);
    _rearLightsColHSV.val = EEPROM.read(11);
    _trackLightsFadeAmount = EEPROM.read(12);
    
    byte r = EEPROM.read(13);
    byte g = EEPROM.read(14);
    byte b = EEPROM.read(15);
  
    if (r == 0 && g == 0 && b == 0) { }
    else { solidColor = CRGB(r, g, b); }
    
    byte r2 = EEPROM.read(16);
    byte g2 = EEPROM.read(17);
    byte b2 = EEPROM.read(18);
  
    if (r2 == 0 && g2 == 0 && b2 == 0) { }
    else { solidColor2 = CRGB(r2, g2, b2); }
    
    //power = EEPROM.read(19);

    int16_t ao[3];
    ao[0] = mGetInt(30);
    ao[1] = mGetInt(34);
    ao[2] = mGetInt(38);
    o.SetMPU6050AccelOffset(ao);
    
    int16_t go[3];
    go[0] = mGetInt(42);
    go[1] = mGetInt(46);
    go[2] = mGetInt(50);
    o.SetMPU6050GyroOffset(go);
    
  } else {
    //set defaults then saveSettings
    setDefaultSettings();
    saveAllSettings();
  }
}

void saveAllSettings() {
  if (DEBUG) { 
    Serial.print(F("Saving settings"));
    Serial.println();
  }
    
  EEPROM.write(1, mE.breathe);
  EEPROM.write(2, mE.head);
  EEPROM.write(3, mE.rear);
  EEPROM.write(4, mE.indicate);
    
  EEPROM.write(7, 1); //write a 1 to pos 7 to indicate that system data has been saved and is available for use.

  EEPROM.write(8, _mainLightsSubMode);
  EEPROM.write(9, _ledGlobalBrightnessCur);
  //EEPROM.write(10, _headLightsBrightness);
  EEPROM.write(10, _headLightsColHSV.val);
  //EEPROM.write(11, _rearLightsBrightness);
  EEPROM.write(11, _rearLightsColHSV.val);
  EEPROM.write(12, _trackLightsFadeAmount);

  mWriteInt(30, o.GetMPU6050AccelOffsetX());
  mWriteInt(34, o.GetMPU6050AccelOffsetY());
  mWriteInt(38, o.GetMPU6050AccelOffsetZ());

  mWriteInt(42, o.GetMPU6050GyroOffsetX());
  mWriteInt(46, o.GetMPU6050GyroOffsetY());
  mWriteInt(50, o.GetMPU6050GyroOffsetZ());

  EEPROM.commit();
}

//this is the one to call..
void clearAllSettings() {
  if (DEBUG) { 
    Serial.print(F("Clearing settings"));
    Serial.println();
  }
  for (int i = 0 ; i < 512; i++) {
    EEPROM.write(i, 0);
  } // TEMP
  
  EEPROM.commit();
  
  setDefaultSettings();
  saveAllSettings();
}

void mWriteInt(int pos, int val) {
    byte* p = (byte*) &val;
    EEPROM.write(pos, *p);
    EEPROM.write(pos + 1, *(p + 1));
    EEPROM.write(pos + 2, *(p + 2));
    EEPROM.write(pos + 3, *(p + 3));
    //EEPROM.commit();
}

int mGetInt(int pos) {
  int val;
  byte* p = (byte*) &val;
  *p        = EEPROM.read(pos);
  *(p + 1)  = EEPROM.read(pos + 1);
  *(p + 2)  = EEPROM.read(pos + 2);
  *(p + 3)  = EEPROM.read(pos + 3);
  return val;
}

