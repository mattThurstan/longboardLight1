/*----------------------------user input----------------------------*/

//void setupUserInputs() {
//  setupButtons();
//  //setup communications to phone/tablet/internet here??? - no
//}

/* user inputs loop
 * called from main loop
 */
//void loopUserInputs() {
//  loopButtons();
//  //
//}

/*----------------------------buttons----------------------------*/

//void setupButtons() {
//  for (byte i = 0; i < _buttonTotal; i++) {
//    pinMode(_buttonPin[i], INPUT_PULLUP); //activate internal pullup resistor on pin
//    _button[i].attach(_buttonPin[i]);
//    _button[i].interval(_buttonDebounceTime);
//  }
//}
//
//void checkStartupButtons() {
//  _button[0].update(); //update buttons (this handles everything to do with reading the pins)
//    //doing the bare minimum here, don't need fast and acurate, just want to hold the button 'for a sec', then it will do something
//    int btRead = _button[0].read();
//    if (btRead == LOW) {
//      //_doFullCalibration = true;  //if held when power on, do full calibration
//      _doQuickCalibration = true;
//    }
//}
//
//void loopButtons() {
////  unsigned long loopButtonsCurMillis = millis();    //get current time
////  if((unsigned long) (loopButtonsCurMillis - _loopButtonsPrevMillis) >= _loopButtonsInterval) {
//  EVERY_N_MILLISECONDS(_loopButtonsInterval) {                     //FastLED based non-blocking delay to update/display the sequence.
//    
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
//        #ifdef DEBUG
//          Serial.print(F("Button fell "));
//          Serial.print(i);
//          Serial.println();
//        #endif
//      }
//    } //end button for loop
//  
////    _loopButtonsPrevMillis = millis();              //store the current time
//  } //END timed-loop
//  
//} //end loopButtons


