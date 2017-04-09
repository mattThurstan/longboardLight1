/*----------------------------mode----------------------------*/

//_orientation: 0=flat, 1=upside-down, 2=up, 3=down, 4=left-side, 5=right-side

/* modes loop
 * called from main loop
 */
void loopModes() {
  boolean orientationTest = false;
  if (orientationTest == true) {
    fadeToBlackBy( _leds, _ledNum, 10);
    if (_orientation == 0) { _leds[ledSegment[2].first] = CRGB::White;  } //_leds[ledSegment[0].first]
    if (_orientation == 1) { _leds[ledSegment[2].first+1] = CRGB::White; }
    if (_orientation == 2) { _leds[ledSegment[2].first+2] = CRGB::White; }
    if (_orientation == 3) { _leds[ledSegment[2].first+3] = CRGB::White; }
    if (_orientation == 4) { _leds[ledSegment[2].first+4] = CRGB::White; }
    if (_orientation == 5) { _leds[ledSegment[2].first+5] = CRGB::White; }
  } else {
    if (_sleepActive == false) {
      //NOTICE: fadeToBlackBy is annoying the customers down the street. implement only when needed
      //..oops, this will have bust a few things, like breathing
      //fadeToBlackBy( _leds, _ledNum, 64);   //anything not used gets set to fade off
      if (_orientation == 0) { loopMainLights(); }
      //if (_orientation == 1) { fadeToBlackBy( _leds, _ledNum, 64); loopEmergencyFlash(); }  //upside-down not working yet
      if (_orientation == 2) { fadeToBlackBy( _leds, _ledNum, 64); loopBreathing(); _headLightsActive = false; } //breathing here is overlaid by rear lights. turn off headlights when you pickup the board so they don't blind you.
      else { _headLightsActive = true; }  //turn the headlights back on when you put the board down.
      if (_orientation == 4 || _orientation == 5) { fadeToBlackBy( _leds, _ledNum, 64); loopSideLight(); }
      loopHeadLights(); //..after the main bits
      loopRearLights(); //..
      loopIndicatorFlash();
    } else {
      //sleep - defined by a period of no movement, a switch, or a wireless command.
      //fadeToBlackBy( _leds, _ledNum, 64); //(array, count, percent) eg. 192/256 = 75% (use 192), 64 = 25%
      //fill_solid(_leds, _ledNum, CRGB::Black);  //TEMP colour
      loopBreathing(); //breathing here is not overlaid by rear lights
    }
  } //END test
}

void loopHeadLights() {
  if (_headLightsActive == true) {
    //fill_solid( leds, _ledNum, CRGB::White);
    fill_gradient_RGB(_leds, ledSegment[0].first, CRGB::White, ledSegment[0].last, CRGB::White );
  }
}

void loopRearLights() {
  if (_rearLightsActive == true) { 
    fill_gradient_RGB(_leds, ledSegment[3].first, CRGB::Red, ledSegment[3].last, CRGB::Red );
  }
}

void loopMainLights() {
  //normal running (flat) - with sub-modes: none, full, track, pretty patterns, etc.
  //0=none/blank, 1= , 2= , 3=
  if (_mainLightsSubMode == 0) {
    //do nothing - 'off'
    fadeToBlackBy( _leds, _ledNum, 30);  //
  } else if (_mainLightsSubMode == 1) {
    fill_gradient_RGB(_leds, ledSegment[1].first, CRGB(16, 16, 16), ledSegment[2].last, CRGB(16, 16, 16) );
  } else if (_mainLightsSubMode == 2) {
    fill_gradient_RGB(_leds, ledSegment[1].first, CRGB::White, ledSegment[1].last, CRGB::Red );
    fill_gradient_RGB(_leds, ledSegment[2].first, CRGB::White, ledSegment[2].last, CRGB::Red );
  } else if (_mainLightsSubMode == 3) {
    loopTrackLights();
  }
}

void loopEmergencyFlash() {
  //emergency flash (upside-down)
    fill_gradient_RGB(_leds, ledSegment[1].first, CRGB::Orange, ledSegment[2].last, CRGB::Orange );
}

void loopSideLight() {
  //emergency light (stood on a side)
  if (_orientation == 4) {
    //left
    fill_gradient_RGB(_leds, ledSegment[2].first, CRGB::White, ledSegment[2].last, CRGB::White );
  } else if (_orientation == 5) {
    //right
    fill_gradient_RGB(_leds, ledSegment[1].first, CRGB::White, ledSegment[1].last, CRGB::White );
  }
}

void loopIndicatorFlash() {
  //indicator flash (turn left/right)
  if (_indicatorsEnabled == true) { 
    if (_indicatorLeftActive == true) {
      //
    } else if (_indicatorRightActive == true) {
      // 
    }
  }
}

void loopTrackLights() {
  //wheel tracked lights
  if (_directionCur == 0) {
    //
  } else if (_directionCur == 1) {
    //
  } else {
    //
  }
  
  //make any unused pixels fade out
  fadeToBlackBy( _leds, _ledNum, _trackLightsFadeAmount);  //2
  //_leds(ledSegment[1].first, ledSegment[1].total).fadeToBlackBy(_trackLightsFadeAmount);
  //_leds(ledSegment[2].first, ledSegment[2].total).fadeToBlackBy(_trackLightsFadeAmount);
  
  //wrap-around for segments 1 and 2  
  if (_ledMovePos > ledSegment[1].total) {
    _ledMovePos = _ledMovePos - ledSegment[1].total;
  } else if (_ledMovePos < 1) {
    _ledMovePos = _ledMovePos + ledSegment[1].total;
  }
  _leds[ledSegment[1].last - _ledMovePos] = CRGB::White;
  _leds[ledSegment[2].last - _ledMovePos] = CRGB::White;
}

/*----------------------------Breathing----------------------------*/
//would eventually like a simple timeline screen on an app where you can draw curves in and see the results in realtime
const unsigned long _breathRiseFallStepIntervalMillis = 250;  //156  62  139  2 //breath rise/fall step interval in milliseconds
const int _breathRiseFallSpacer = 4;  //just 4 at begin.. eg. 4 each end.. //8   //_breathMaxBrightness / (4*2)     //counts to pause/ignore at the bottom and top - 42
const int _breathMaxBrightness = 16;  //32
const unsigned long _breathRiseFallHoldIntervalMillis = 1;    //breath rise/fall hold interval in milliseconds
unsigned long _breathRiseFallPrevMillis = 0;                  //previous time for reference
int _breathRiseFallCounter = 0;                               //eg. 0-17
boolean _breathRiseFallDirection = true;                     //direction true-rise, false-fall
CRGB c;

void loopBreathing() {
  //'breathing'
  if (_breathingActive == true) { 
    breathRiseFall();
  }
}

void breathRiseFall() {
  unsigned long breathRiseFallCurrentMillis = millis();  //get current time
  if ((unsigned long)(breathRiseFallCurrentMillis - _breathRiseFallPrevMillis) >= _breathRiseFallStepIntervalMillis) {
    //ignore first/last 4 (or so) so we get a pause at the bottom and top
    if (_breathRiseFallCounter >= _breathRiseFallSpacer) { //&& _breathRiseFallCounter <= (_breathMaxBrightness + _breathRiseFallSpacer) ) {
      c.r = _breathRiseFallCounter-_breathRiseFallSpacer;
      c.g = _breathRiseFallCounter-_breathRiseFallSpacer;
      c.b = _breathRiseFallCounter-_breathRiseFallSpacer;
      fill_gradient_RGB(_leds, ledSegment[0].first, CRGB::Black, ledSegment[3].last, CRGB::Black );
      if (_orientation == 2) {
        fill_gradient_RGB(_leds, ledSegment[1].first, c, ledSegment[1].last - (ledSegment[1].total / 3), CRGB::Black );
        fill_gradient_RGB(_leds, ledSegment[2].first, c, ledSegment[2].last - (ledSegment[2].total / 3), CRGB::Black );
      } else if (_orientation == 3) {
        fill_gradient_RGB(_leds, ledSegment[1].first + (ledSegment[1].total / 3), CRGB::Black, ledSegment[1].last, c );
        fill_gradient_RGB(_leds, ledSegment[2].first + (ledSegment[2].total / 3), CRGB::Black, ledSegment[2].last, c );
      } else {
        fill_gradient_RGB(_leds, ledSegment[1].first, c, ledSegment[1].last, c );
        fill_gradient_RGB(_leds, ledSegment[2].first, c, ledSegment[2].last, c );
      }
    }
    if (_breathRiseFallDirection == true) {
      _breathRiseFallCounter++;       //we are rising, increase the counter
    } else if (_breathRiseFallDirection == false) {
      _breathRiseFallCounter--;       //we are falling, decrease the counter
    }
    _breathRiseFallPrevMillis = breathRiseFallCurrentMillis;      //save time - code timing dependant - intervals will be late if code elsewhere gets backed up
    //_breathRiseFallPrevMillis += breathRiseFallCurrentMillis;   //save time - NOT code timing dependant - intervals will try and catch up

  } //END timed loop

  if ( _breathRiseFallCounter >= (_breathMaxBrightness + _breathRiseFallSpacer ) ) {
    _breathRiseFallDirection = false;   //we have reached the end, start falling
  } else if (_breathRiseFallCounter <= 0) {
    _breathRiseFallDirection = true;   //we have reached the beginning, start rising again..
  }

} //END breathRiseFall

