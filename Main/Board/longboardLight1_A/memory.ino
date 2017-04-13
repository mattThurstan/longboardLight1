/*----------------------------memory load/save---------------------*/

/*
 * NOTE - all eeprom saves will be wiped when a new sketch is uploaded, but will persist thru normal use.
 * 
 * Settings to save
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
 * 10= _headLightsBrightness      - 0-255
 * 11= _rearLightsBrightness      - 0-255
 * 12= _trackLightsFadeAmount ??? - 0-255
 * 
 * 
 */
void setDefaultSettings() {
  #ifdef DEBUG
    Serial.print(F("Setting default settings"));
    Serial.println();
  #endif
  
  _breathingEnabled = true;    //true
  _headLightsEnabled = true;   //true
  _rearLightsEnabled = true;   //true
  _indicatorsEnabled = true;   //true - not done yet

  _mainLightsSubMode = 3;
  _ledGlobalBrightnessCur = 255;
  _headLightsBrightness = 200;
  _rearLightsBrightness = 200;
  _trackLightsFadeAmount = 64;
}

void loadAllSettings() {
  //this gets called at the beginning of setup.
  
  //if system settings saved flag set at pos 7 is '1', then we have system data to be read from memory.. failsafe
  if (EEPROM.read(7) == 1) {
    #ifdef DEBUG
      Serial.print(F("Loading settings"));
      Serial.println();
    #endif
    //redo these later into 1 bit mask
    if (EEPROM.read(1) == 255) { _breathingEnabled = true; }
    else { _breathingEnabled = false; }
    if (EEPROM.read(2) == 255) { _headLightsEnabled = true; }
    else { _headLightsEnabled = false; }
    if (EEPROM.read(3) == 255) { _rearLightsEnabled = true; }
    else { _rearLightsEnabled = false; }
    if (EEPROM.read(4) == 255) { _indicatorsEnabled = true; }
    else { _indicatorsEnabled = false; }
    
    _mainLightsSubMode = EEPROM.read(8);  
    _ledGlobalBrightnessCur = EEPROM.read(9);
    _headLightsBrightness = EEPROM.read(10);
    _rearLightsBrightness = EEPROM.read(11);
    _trackLightsFadeAmount = EEPROM.read(12);
  } else {
    //set defaults then saveSettings
    setDefaultSettings();
    saveAllSettings();
  }
}

void saveAllSettings() {
  #ifdef DEBUG
    Serial.print(F("Saving settings"));
    Serial.println();
  #endif
    
  //redo these later into 1 bit mask
  if (_breathingEnabled == true) { EEPROM.write(1, 255); }
  else { EEPROM.write(1, 0); }
  if (_headLightsEnabled == true) { EEPROM.write(2, 255); }
  else { EEPROM.write(2, 0); }
  if (_rearLightsEnabled == true) { EEPROM.write(3, 255); }
  else { EEPROM.write(3, 0); }
  if (_indicatorsEnabled == true) { EEPROM.write(4, 255); }
  else { EEPROM.write(4, 0); }
    
  EEPROM.write(7, 1); //write a 1 to pos 7 to indicate that system data has been saved and is available for use.

  EEPROM.write(8, _mainLightsSubMode);
  EEPROM.write(9, _ledGlobalBrightnessCur);
  EEPROM.write(10, _headLightsBrightness);
  EEPROM.write(11, _rearLightsBrightness);
  EEPROM.write(12, _trackLightsFadeAmount);
  
}

void clearAllSettings() {
  #ifdef DEBUG
    Serial.print(F("Clearing settings"));
    Serial.println();
  #endif
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  } // TEMP
  setDefaultSettings();
}


