/*----------------------------mode----------------------------*/
bool _fadeOut = false;  //this is a hack!!!

/* Modes loop (called from Main loop) 
 * orientation (byte):  0=flat, 1=upside-down, 2=up, 3=down, 4=left-side, 5=right-side
 */
void loopModes() {
  if (_testMode == 1) { testModes(); } 
  else {
    if (mA.sleep == 0) {
      if (o.GetOrientation() == 0) { loopMainLights(); }
      if (o.GetOrientation() == 1) { /* fadeToBlackBy( _leds, _ledNum, 64); loopEmergencyFlash(); */ }  //upside-down is not working yet
      if (o.GetOrientation() == 2) { loopBreathing(); mA.head = 0; mA.rear = 1; } //breathing here is overlaid by rear lights. turn off headlights when you pickup the board so they don't blind you.
      else { _fadeOut == false; mA.head = 1; mA.rear = 1; }  //turn the headlights back on when you put the board down.  ..this is a bad place to put this, potential future conflicts..
      if (o.GetOrientation() == 3) { loopBreathing(); mA.head = 0; mA.rear = 0; }  //down is not working properly yet (should be opposite of 2)
      else { _fadeOut == false; mA.head = 1; mA.rear = 1; }
      if (o.GetOrientation() == 4 || o.GetOrientation() == 5) { _leds.fadeToBlackBy(32); loopSideLight(); }
      loopHeadLights(); //..overlay AFTER the main bits ..these come last in the stack for safety reasons.
      loopRearLights(); //..
      loopIndicatorFlash();
    } else {
//TODO
      //sleep - can be defined by a period of no movement, a switch, or a wireless command.
      //after a set period of time it will power down (completely - eventually).
      //can be woken from sleep bye movement. (can also go to sleep from lack of movement)
      loopBreathing(); //breathing here is not overlaid by rear lights
    } //END if _sleepActive
  } //END if orientationTest
}

/*----------------------------mode(s)----------------------------*/

/* Main Lights. Normal running (flat) - with (4) sub-modes:
 * 0= none (black), 
 * 1= glow, 
 * 2= gradient (end to end), 
 * 3= wheel tracking
 */
void loopMainLights() {
  // Call the current pattern function once, updating the 'leds' array
  patterns[_mainLightsSubMode].pattern();
}

void loopHeadLights() {
  if (mE.head == 1) {
    if (mA.head == 1) {
      _ledsFront = _headLightsColHSV;
    } else {
      //if we do nothing, then the sub-modes can run all around the board
    }
  }
}

void loopRearLights() {
  if (mE.rear == 1) { 
    if (mA.rear == 1) {
      _ledsRear = _rearLightsColHSV;
    }
  }
}

/* Emergency flash (upside-down) */
void loopEmergencyFlash() {
  
  EVERY_N_MILLISECONDS(_emergencyFlashInterval) {
      _emergencyFlashFlip = !_emergencyFlashFlip;
  }
  
  if (_emergencyFlashFlip) {
    _ledsLeft.fill_gradient_RGB(CRGB::Orange, CRGB::Orange);
    _ledsRight = _ledsLeft;
  } else {
    _ledsLeft = CRGB::Black;
    _ledsRight = CRGB::Black;
  }
  //FastLED.show();
}

/* Emergency light (stood on a side) */
void loopSideLight() {
  if (o.GetOrientation() == 4) {  
    //left (turn on right)
    _ledsRight = CRGB::White;
  } 
  else if (o.GetOrientation() == 5) { 
    //right (turn on left
    _ledsLeft = CRGB::White;
  }
}

/* Indicator flash (turn left/right) */
void loopIndicatorFlash() {
  if (mE.indicate == 1) { 
    //
  }
}

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
  if (mE.breathe == 1) { breathRiseFall2(); } 
  else { _leds.fadeToBlackBy(32); }
}

void breathRiseFall() {
  //_leds.fadeToBlackBy(32);
  //_leds = CRGB::Black;
//  _ledsRear.fadeToBlackBy(32);
//  _ledsFront.fadeToBlackBy(32);
//  if (o.GetOrientation() == 2) {
//    //_leds((ledSegment[1].last - _1totalDiv)+1, ledSegment[1].last).fadeToBlackBy(32);
//    //_leds(ledSegment[2].last - _2totalDiv+1, ledSegment[2].last) = _leds((ledSegment[1].last - _1totalDiv)+1, ledSegment[1].last);
//    _ledsLeft( (ledSegment[1].total - _1totalDiv), (ledSegment[1].total - 1) ).fadeToBlackBy(32);
//    _ledsRight = _ledsLeft;
//  } else if (o.GetOrientation() == 3) {
//    //_leds(ledSegment[1].first, (ledSegment[1].first + _1totalDiv)-1).fadeToBlackBy(32);
//    //_leds(ledSegment[2].first, (ledSegment[2].first + _2totalDiv)-1) = _leds(ledSegment[1].first, (ledSegment[1].first + _1totalDiv)-1);
//    _ledsLeft( 0, (_1totalDiv - 1) ).fadeToBlackBy(32);
//    _ledsRight = _ledsLeft;
//  }
  //this really needs a state machine. fade out all : bottom : rising : top : falling
  //and a mathematical curve to fade up and down.
  EVERY_N_MILLISECONDS(_breathRiseFallStepIntervalMillis) {                     //FastLED based non-blocking delay to update/display the sequence.
    _leds = CRGB::Black;
    //ignore first/last 4 (or so) so we get a pause at the bottom and top
    if (_breathRiseFallCounter >= _breathRiseFallSpacer) {
      c.r = _breathRiseFallCounter-_breathRiseFallSpacer;
      c.g = _breathRiseFallCounter-_breathRiseFallSpacer;
      c.b = _breathRiseFallCounter-_breathRiseFallSpacer;
      if (o.GetOrientation() == 2) {
        //_leds(ledSegment[1].first, ledSegment[1].last - _1totalDiv).fill_gradient_RGB(c, CRGB::Black );
        //_leds(ledSegment[2].first, ledSegment[2].last - _2totalDiv) = _leds(ledSegment[1].first, ledSegment[1].last - _1totalDiv);
        _ledsLeft( 0, ( (ledSegment[1].total - 1) - _1totalDiv ) ).fill_gradient_RGB(c, CRGB::Black );
        _ledsRight = _ledsLeft;
        _ledsRear = _ledsLeft[0];
      } else if (o.GetOrientation() == 3) {
        //_leds(ledSegment[1].first + _1totalDiv, ledSegment[1].last).fill_gradient_RGB(CRGB::Black, c );
        //_leds(ledSegment[2].first + _2totalDiv, ledSegment[2].last) = _leds(ledSegment[1].first + _1totalDiv, ledSegment[1].last);
        _ledsLeft( _1totalDiv, (ledSegment[1].total - 1)  ).fill_gradient_RGB(c, CRGB::Black );
        _ledsRight = _ledsLeft;
        _ledsRear = _ledsLeft[ledSegment[1].total - 1];
      } else {
        //_leds(ledSegment[1].first, ledSegment[1].last).fill_gradient_RGB(c, c );
        //_leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
        _ledsLeft( 0, (ledSegment[1].total - 1) ).fill_gradient_RGB(c, c );
        _ledsRight = _ledsLeft;
        _ledsRear = _ledsLeft[0];
      }
    }
    //FastLED.show();
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


/*
 * The gradient function here is trying to smoothly turn on each led as it clocks up the row.
 * I think what is really needed is each led to fade on, then the next, etc.
 * This would require doing something like a 16bit fade number, the length of the led strip, 
 * multiplied by 256 (steps), so we get a 256 fade on each led before moving onto the next.
 * ...but this seems processor intensive and over-complicated for a little effect that you 
 * only see out of the corner of you eye.
 */

uint8_t bBpm = 12;
uint8_t bMax;
uint8_t bPeak;

void breathRiseFall2() {
  _leds = CRGB::Black;
  
  if (o.GetOrientation() == 2 || o.GetOrientation() == 3) {
    bMax = ( (ledSegment[1].total - 1) - _1totalDiv );
  } else {
    //fade all
    bMax = 255; //here we are fading all leds so using this for the brightness instead of led position
  }
  
  bPeak = beatsin8( bBpm, 0, bMax); //bpm, min, max
  
  if (o.GetOrientation() == 2) {
    //fade bot to top
    _ledsLeft( 0, bPeak ).fill_gradient_RGB( CRGB::White, CRGB::Black );
    _ledsRight = _ledsLeft;
    _ledsRear = _ledsLeft[0];
  } else if (o.GetOrientation() == 3) {
    //fade top to bot
    //use right leds CRGBSet here as the array has already been reversed in setup
    _ledsRight( 0, bPeak ).fill_gradient_RGB( CRGB::White, CRGB::Black );
    _ledsLeft = _ledsRight;
    _ledsFront = _ledsRight[0];
  } else {
    //fade all
    c.r = bPeak;
    c.g = bPeak;
    c.b = bPeak;
    _leds.fill_solid(c);  //use full CRGBArray
  }
  
}

