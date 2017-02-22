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
  for (int i = 0; i < _buttonTotal; i++) {
    pinMode(_buttonPin[i], INPUT_PULLUP); //activate internal pullup resistor on pin    
  }
}

void loopButtons() {
  //sub-loop - reads all buttons (digital pins)
  //fallingEdge/risingEdge may be wrong way round cos using NC switches ?
  for (int i = 0; i < _buttonTotal; i++) {
    button[i].update(); //update buttons (this handles everything to do with reading the pins)
    if (button[i].fallingEdge()) {
      //button pressed (connected to ground) - NC ON STATE
      //..do something
      //_buttonToggled[i] = true;     //toggle the button state so we can use it elswhere
      //if (i == 0) { swap sub-modes }
      //if (i == 1) { _doQuickCalibration = true; }
    } //end fallingEdge
    
    if (button[i].risingEdge()) {
      //button released (internal pullup to HIGH) - NC OFF STATE
      //..do something
    } //end risingEdge
    
  } //end button for loop
} //end loopButtons

/*----------------------------WIFI/bluetooth----------------------------*/


