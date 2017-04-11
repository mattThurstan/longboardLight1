/*----------------------------user input----------------------------*/

void setupUserInputs() {
  setupButtons();
  //setup communications to phone/tablet/internet
}

/* user inputs loop
 * called from main loop
 */
void loopUserInputs() {
  loopButtons();
  //
}

/*----------------------------buttons----------------------------*/

void setupButtons() {
  for (byte i = 0; i < _buttonTotal; i++) {
    pinMode(_buttonPin[i], INPUT_PULLUP); //activate internal pullup resistor on pin
    _button[i].attach(_buttonPin[i]);
    _button[i].interval(_buttonDebounceTime);
  }
}

void checkStartupButtons() {
  _button[0].update(); //update buttons (this handles everything to do with reading the pins)
    //doing the bare minimum here, don't need fast and acurate, just want to hold the button 'for a sec', then it will do something
    int btRead = _button[0].read();
    if (btRead == LOW) {
      //_doFullCalibration = true;  //if held when power on, do full calibration
      _doQuickCalibration = true;
    }
}

void loopButtons() {
  unsigned long loopButtonsCurMillis = millis();    //get current time
  
  if((unsigned long) (loopButtonsCurMillis - _loopButtonsPrevMillis) >= _loopButtonsInterval) {
    
    for (byte i = 0; i < _buttonTotal; i++) {
      //sub-loop - reads all buttons (digital pins)
      _button[i].update(); //update buttons (this handles everything to do with reading the pins)
      //doing the bare minimum here, don't need fast and acurate, just want to hold the button 'for a sec', then it will do something
      int btRead = _button[i].read();
      if (btRead == LOW) {
        _buttonToggled[i] = true;
        if (i == 0) { 
          _doQuickCalibration = true; 
          //incrementMainLightsSubMode();   //TEMP hijacking !!!
          }  //use same bt at startup eg. if held when power on, do full calibration
        if (i == 1) { incrementMainLightsSubMode(); } 
        #ifdef DEBUG
          Serial.print(F("Button pressed "));
          Serial.print(i);
          Serial.println();
        #endif
      }
    } //end button for loop
  
    _loopButtonsPrevMillis = millis();              //store the current time
  } //END timed-loop
  
} //end loopButtons

/*----------------------------WIFI/bluetooth----------------------------*/


