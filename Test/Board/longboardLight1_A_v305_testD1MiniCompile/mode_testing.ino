/*----------------------------mode testing----------------------------*/
  /*
   * _curTestModes (byte) sent from device
   * 0 blank
   * 1 main lights (sub-modes) = loopMainLights();
   * 2 tracked lights (basic) = loopTrackLights();
   * 3 emergency flash = loopEmergencyFlash(); // (used for upside down)
   * 4 breathing = loopBreathing();
   * 5 left side lights = loopSideLight();
   * 6 right side lights
   * 7 head lights = loopHeadLights();
   * 8 rear lights = loopRearLights();
   * 9 left indicator flash = loopIndicatorFlash(); //not implemented yet
   * 10 right indicator flash = loopIndicatorFlash(); //not implemented yet
   * 11 orientation = showOrientation();
   */
   
void testModes() {
  if (_curTestModes == 0) {
    //
  } else if (_curTestModes == 1) {
    loopMainLights();
  } else if (_curTestModes == 2) {
    loopTrackLights();
  } else if (_curTestModes == 3) {
    loopEmergencyFlash();
  } else if (_curTestModes == 4) {
    loopBreathing();
  } else if (_curTestModes == 5) {
    _leds(ledSegment[1].first, ledSegment[1].last) = CRGB::White; /* left */
  } else if (_curTestModes == 6) {
    _leds(ledSegment[2].first, ledSegment[2].last) = CRGB::White; /* right */
  } else if (_curTestModes == 7) {
    loopHeadLights();
  } else if (_curTestModes == 8) {
    loopRearLights();
  } else if (_curTestModes == 9) {
    //loopIndicatorFlash(); //not implemented yet
  } else if (_curTestModes == 10) {
    //loopIndicatorFlash(); //not implemented yet
  } else if (_curTestModes == 11) {
    showOrientation();
  }
}

void showOrientation() {
  fadeToBlackBy( _leds, _ledNum, 16);
  if (o.GetOrientation() == 0) {
    //flat
    _leds(ledSegment[0].first, ledSegment[0].last) = CRGB::White;             //(midpoint) back
    _leds[ledSegment[1].first + _orientationTestSideMidpoint] = CRGB::Orange;  //midpoint left
    _leds[ledSegment[2].first + _orientationTestSideMidpoint] = CRGB::Blue;  //midpoint right
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

