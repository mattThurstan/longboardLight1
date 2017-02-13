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
      fadeToBlackBy( _leds, _ledNum, 10);   //anything not used gets set to fade off
      if (_orientation == 0) { loopMainLights(); }
      if (_orientation == 1) { loopEmergencyFlash(); }
      if (_orientation == 2) { loopBreathing(); _headLightsActive = false; } //breathing here is overlaid by rear lights. turn off headlights when you pickup the board so they don't blind you.
      else { _headLightsActive = true; }  //turn the headlights back on when you put the board down.
      if (_orientation == 4 || _orientation == 5) { loopSideLight(); }
      loopHeadLights(); //..after the main bits
      loopRearLights(); //..
      loopIndicatorFlash();
    } else { 
      //sleep - defined by a period of no movement, a switch, or a wireless command.
      fadeToBlackBy( _leds, _ledNum, 30); 
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
    //
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

void loopBreathing() {
  //'breathing'
  if (_breathingActive == true) { 
    if (_orientation == 2) {
      //glow from the bottom
      fill_gradient_RGB(_leds, ledSegment[2].first+((ledSegment[2].last-ledSegment[2].first)/2), CRGB::Black, (ledSegment[2].last), CRGB(16, 16, 16) );
      fill_gradient_RGB(_leds, ledSegment[1].first+((ledSegment[1].last-ledSegment[1].first)/2), CRGB::Black, (ledSegment[1].last), CRGB(16, 16, 16) );
    } else {
      //general all-over glow (even if it is upside-down because it will act as a reminder to "put it away and not leave it lying around the living room floor")
    }
  }
}


