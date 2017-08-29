/*----------------------------mode----------------------------*/
bool _fadeOut = false;

/* Modes loop (called from Main loop) 
 *  _orientation: 
 *  0=flat, 
 *  1=upside-down, 
 *  2=up, 
 *  3=down, 
 *  4=left-side, 
 *  5=right-side
 */
void loopModes() {
  if (_orientationTest == true) { showOrientation(); } 
  else {
    //if (modesA.sleep == false) {
    if (_sleepActive == false) {
      if (o.GetOrientation() == 0) { loopMainLights(); }
      if (o.GetOrientation() == 1) { /* fadeToBlackBy( _leds, _ledNum, 64); loopEmergencyFlash(); */ }  //upside-down is not working yet
      if (o.GetOrientation() == 2) { loopBreathing(); _headLightsActive = false; /* modesA.head = false; */ } //breathing here is overlaid by rear lights. turn off headlights when you pickup the board so they don't blind you.
      else { _fadeOut == false; _headLightsActive = true; /* modesA.head = true; */ }  //turn the headlights back on when you put the board down.  ..this is a bad place to put this, potential future conflicts..
      if (o.GetOrientation() == 3) { }  //down is not working yet
      else { }
      if (o.GetOrientation() == 4 || o.GetOrientation() == 5) { _leds.fadeToBlackBy(32); loopSideLight(); }
      loopHeadLights(); //..overlay AFTER the main bits ..these come last in the stack for safety reasons.
      loopRearLights(); //..
      loopIndicatorFlash();
    } else {
//TODO
      //sleep - can be defined by a period of no movement, a switch, or a wireless command.
      loopBreathing(); //breathing here is not overlaid by rear lights
    } //END if _sleepActive
  } //END if orientationTest
}

void loopHeadLights() {
  //if (modesE.head == true) {  //modes Enabled - head lights
  if (_headLightsEnabled == true) {
    //if (modesA.head == true) {  //modes Active - head lights
    if (_headLightsActive == true) {
      _leds(ledSegment[3].first, ledSegment[3].last) = _headLightsColHSV;
    } else {
      _leds(ledSegment[3].first, ledSegment[3].last).fadeToBlackBy(32);
    }
  }
}

void loopRearLights() {
  if (_rearLightsEnabled == true) { 
    if (_rearLightsActive == true) { 
      _leds(ledSegment[0].first, ledSegment[0].last) = _rearLightsColHSV;
    } else {
      _leds(ledSegment[0].first, ledSegment[0].last).fadeToBlackBy(32);
    }
  }
}

/* Normal running (flat) - with 4 sub-modes:
 * 0= none (black), 
 * 1= glow, 
 * 2= gradient (end to end), 
 * 3= wheel tracking
 */
void loopMainLights() {
  if (_mainLightsSubMode == 0) {
    _leds.fadeToBlackBy(32);  //do nothing - make sure everything is 'off' at this point in the proceedings
  } else if (_mainLightsSubMode == 1) {
    _leds(ledSegment[1].first, ledSegment[2].last) = CRGB(16, 16, 16);
  } else if (_mainLightsSubMode == 2) {
    _leds(ledSegment[1].first, ledSegment[1].last).fill_gradient(_rearLightsColHSV, _headLightsColHSV);
    _leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
  } else if (_mainLightsSubMode == 3) {
    loopTrackLights();
  }
}

/* Emergency flash (upside-down) */
void loopEmergencyFlash() {
  _leds( ledSegment[1].first, ledSegment[2].last).fill_gradient_RGB(CRGB::Orange, CRGB::Orange );
  FastLED.show();
}

/* Emergency light (stood on a side) */
void loopSideLight() {
  if (o.GetOrientation() == 4) {  _leds(ledSegment[1].first, ledSegment[1].last) = CRGB::White; /* left */ } 
  else if (o.GetOrientation() == 5) { _leds(ledSegment[2].first, ledSegment[2].last) = CRGB::White; /* right */ }
}

/* Indicator flash (turn left/right) */
void loopIndicatorFlash() {
  if (_indicatorsEnabled == true) { 
    //
  }
}

/* Wheel tracked running lights (position from wheel data combined with direction from MPU6050) */
void loopTrackLights() {  
  _leds(ledSegment[1].first, ledSegment[2].last).fadeToBlackBy(_trackLightsFadeAmount);            //_trackLightsFadeAmount
  
  //wrap-around for segments 1 and 2  
  if (_ledMovePos > ledSegment[1].total) { _ledMovePos = _ledMovePos - ledSegment[1].total; } 
  else if (_ledMovePos < 1) { _ledMovePos = _ledMovePos + ledSegment[1].total; }
  
  _leds[ledSegment[1].last - _ledMovePos + 1] = CRGB::White;
  _leds[ledSegment[2].last - _ledMovePos + 1] = CRGB::White;
  
  FastLED.show();
}

void showOrientation() {
  fadeToBlackBy( _leds, _ledNum, 16);
  //if (_orientation == 0) {
  if (o.GetOrientation() == 0) {
    //flat
    _leds(ledSegment[0].first, ledSegment[0].last) = CRGB::White;             //(midpoint) back
    _leds[ledSegment[1].first + _orientationTestSideMidpoint] = CRGB::White;  //midpoint left
    _leds[ledSegment[2].first + _orientationTestSideMidpoint] = CRGB::White;  //midpoint right
    _leds(ledSegment[3].first, ledSegment[3].last) = CRGB::White;             //(midpoint) front
  }
  if (o.GetOrientation() == 1) {
    //upside-down
    //_leds[ledSegment[2].first+1] = CRGB::White;
  }
  if (o.GetOrientation() == 2) {
    //up
    _leds(ledSegment[3].first, ledSegment[3].last) = CRGB::White;             //(midpoint) front
  }
  if (o.GetOrientation() == 3) {
    //down
    _leds(ledSegment[0].first, ledSegment[0].last) = CRGB::White;             //(midpoint) back
  }
  if (o.GetOrientation() == 4) {
    //on its left-side
    _leds[ledSegment[1].first + _orientationTestSideMidpoint] = CRGB::White;  //midpoint left
  }
  if (o.GetOrientation() == 5) {
    //on its right-side
    _leds[ledSegment[2].first + _orientationTestSideMidpoint] = CRGB::White;  //midpoint right
  }

} //END showOrientation

/*----------------------------Breathing----------------------------*/
//would eventually like a simple timeline screen on an app where you can draw curves in and see the results in realtime
const unsigned long _breathRiseFallStepIntervalMillis = 250;  //156  62  139  2 //breath rise/fall step interval in milliseconds
const int _breathRiseFallSpacer = 4;  //just 4 at begin.. eg. 4 each end.. //8   //_breathMaxBrightness / (4*2)     //counts to pause/ignore at the bottom and top - 42
const int _breathMaxBrightness = 16;  //32
const unsigned long _breathRiseFallHoldIntervalMillis = 1;    //breath rise/fall hold interval in milliseconds
unsigned long _breathRiseFallPrevMillis = 0;                  //previous time for reference
int _breathRiseFallCounter = 0;                               //eg. 0-17
boolean _breathRiseFallDirection = true;                      //direction true-rise, false-fall
CRGB c;

void loopBreathing() {
  //'breathing'
  if (_breathingEnabled == true) {
    breathRiseFall();
  }
}

void breathRiseFall() {
  if (o.GetOrientation() == 2) {
    _leds((ledSegment[1].last - _1totalDiv3)+1, ledSegment[1].last).fadeToBlackBy(32);
    _leds(ledSegment[2].last - _2totalDiv3+1, ledSegment[2].last) = _leds((ledSegment[1].last - _1totalDiv3)+1, ledSegment[1].last);
  } else if (o.GetOrientation() == 3) {
    _leds(ledSegment[1].first, (ledSegment[1].first + _1totalDiv3)-1).fadeToBlackBy(32);
    _leds(ledSegment[2].first, (ledSegment[2].first + _2totalDiv3)-1) = _leds(ledSegment[1].first, (ledSegment[1].first + _1totalDiv3)-1);
  }
  
  EVERY_N_MILLISECONDS(_breathRiseFallStepIntervalMillis) {                     //FastLED based non-blocking delay to update/display the sequence.
    //ignore first/last 4 (or so) so we get a pause at the bottom and top
    if (_breathRiseFallCounter >= _breathRiseFallSpacer) {
      c.r = _breathRiseFallCounter-_breathRiseFallSpacer;
      c.g = _breathRiseFallCounter-_breathRiseFallSpacer;
      c.b = _breathRiseFallCounter-_breathRiseFallSpacer;
      if (o.GetOrientation() == 2) {
        _leds(ledSegment[1].first, ledSegment[1].last - _1totalDiv3).fill_gradient_RGB(c, CRGB::Black );
        _leds(ledSegment[2].first, ledSegment[2].last - _2totalDiv3) = _leds(ledSegment[1].first, ledSegment[1].last - _1totalDiv3);
      } else if (o.GetOrientation() == 3) {
        _leds(ledSegment[1].first + _1totalDiv3, ledSegment[1].last).fill_gradient_RGB(CRGB::Black, c );
        _leds(ledSegment[2].first + _2totalDiv3, ledSegment[2].last) = _leds(ledSegment[1].first + _1totalDiv3, ledSegment[1].last);
      } else {
        _leds(ledSegment[1].first, ledSegment[1].last).fill_gradient_RGB(c, c );
        _leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
      }
    }
    FastLED.show();
    if (_breathRiseFallDirection == true) {
      _breathRiseFallCounter++;       //we are rising, increase the counter
    } else if (_breathRiseFallDirection == false) {
      _breathRiseFallCounter--;       //we are falling, decrease the counter
    }
  } //END timed loop

  if ( _breathRiseFallCounter >= (_breathMaxBrightness + _breathRiseFallSpacer ) ) {
    _breathRiseFallDirection = false;   //we have reached the end, start falling
  } else if (_breathRiseFallCounter <= 0) {
    _breathRiseFallDirection = true;   //we have reached the beginning, start rising again..
  }

} //END breathRiseFall

