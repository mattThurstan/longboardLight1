/*----------------------------user input----------------------------*/

void setupUserInputs() {
  setupButtons();
  //setup communications to phone/tablet/internet here??? - no
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
    //bt0 - WIFI On - ESP8266 pin 2 (D4)- internal pullup
    pinMode(_buttonPin[0], INPUT_PULLUP); //activate internal pullup resistor on pin
    _button[0].attach(_buttonPin[0]);
    _button[0].interval(_buttonDebounceTime);

    //bt1 - ??? - ESP8266 pin 16 (D0) - external pullup
//    _button[1].attach(_buttonPin[1]);
//    _button[1].interval(_buttonDebounceTime);
}

//void checkStartupButtons() {
//  _button[0].update(); //update buttons (this handles everything to do with reading the pins)
//    //doing the bare minimum here, don't need fast and acurate, just want to hold the button 'for a sec', then it will do something
//    int btRead = _button[0].read();
//    if (btRead == LOW) {
//      //_doFullCalibration = true;  //if held when power on, do full calibration
//      _doQuickCalibration = true;
//    }
//}

void loopButtons() {

  if (!_wifiActive) {
    EVERY_N_MILLISECONDS(_loopButtonsInterval) {                     //FastLED based non-blocking delay to update/display the sequence.

      //if ( _button[i].update() ) { }; //returns a 1 if changed
      _button[0].update(); //update buttons (this handles everything to do with reading the pins)
      //doing the bare minimum here
      if ( _button[0].fell() ) {
        _buttonToggled[0] = true;
        //setupSerial();  //this has to be seperated due to startup order
        //startComms();
        if (DEBUG_GEN) { Serial.println(F("Button 0 fell - WIFI on")); }
      }
      
    }
  }
  
  //loopButton1();
  
//  unsigned long loopButtonsCurMillis = millis();    //get current time
//  if((unsigned long) (loopButtonsCurMillis - _loopButtonsPrevMillis) >= _loopButtonsInterval) {
  //EVERY_N_MILLISECONDS(_loopButtonsInterval) {                     //FastLED based non-blocking delay to update/display the sequence.

//    for (byte i = 0; i < _buttonTotal; i++) {
//      //sub-loop - reads all buttons (digital pins)
//      //if ( _button[i].update() ) { }; //returns a 1 if changed
//      _button[i].update(); //update buttons (this handles everything to do with reading the pins)
//      //doing the bare minimum here
//      if ( _button[i].fell() ) {
//        _buttonToggled[i] = true;
//        if (i == 0) { 
//          //_doQuickCalibration = true;
//          mE.head = !mE.head;
//          mE.rear = !mE.rear;
//        }  //use same bt at startup eg. if held when power on, do full calibration
//        if (i == 1) { 
//          incrementMainLightsSubMode(); 
//          //TEMP HIJACK !!!
//          //  if (_sleepActive == true) { _sleepActive = false; } 
//          //  else if (_sleepActive == false) { _sleepActive = true; }
//        } 
//        #ifdef DEBUG_GEN
//          Serial.print(F("Button fell "));
//          Serial.print(i);
//          Serial.println();
//        #endif
//      }
//    } //end button for loop
  
//    _loopButtonsPrevMillis = millis();              //store the current time
//  } //END timed-loop
  
} //end loopButtons

