/*----------------------------mode testing----------------------------*/
  /*
   * _curTestMode (byte) sent from device
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
  if (_curTestMode == 0) {
    //blank
    _leds.fadeToBlackBy(32);  //do nothing - make sure everything is 'off' at this point in the proceedings
  } else if (_curTestMode == 1) {
    loopMainLights();
  } else if (_curTestMode == 2) {
    loopTrackLights();
  } else if (_curTestMode == 3) {
    loopEmergencyFlash();
  } else if (_curTestMode == 4) {
    loopBreathing();
  } else if (_curTestMode == 5) {
    _leds = CRGB::Black;
    _ledsLeft = CRGB::White;
  } else if (_curTestMode == 6) {
    _leds = CRGB::Black;
    _ledsRight = CRGB::White;
  } else if (_curTestMode == 7) {
    //loopHeadLights();
    _leds = CRGB::Black;
    _ledsFront = _headLightsColHSV;
  } else if (_curTestMode == 8) {
    //loopRearLights();
    _leds = CRGB::Black;
    _ledsRear = _rearLightsColHSV;
  } else if (_curTestMode == 9) {
    //loopIndicatorFlash(); //not implemented yet
  } else if (_curTestMode == 10) {
    //loopIndicatorFlash(); //not implemented yet
  } else if (_curTestMode == 11) {
    showOrientation();
  }
}

void showOrientation() {
  fadeToBlackBy( _leds, _ledNum, 16);
  if (o.GetOrientation() == 0) {
    //flat
    _ledsRear = CRGB::White;              //(midpoint) back  (cannot actually have a midpoint here, as only 2 LEDS)
    _ledsLeft[_orientationTestSideMidpoint] = CRGB::Red;  //midpoint left
    _ledsRight[_orientationTestSideMidpoint] = CRGB::Green;  //midpoint right
    _ledsFront = CRGB::White;             //(midpoint) front
  }
  if (o.GetOrientation() == 1) {
    //upside-down
    //_leds[ledSegment[2].first+1] = CRGB::White;
    _ledsRear = CRGB::Orange;
    _ledsLeft[_orientationTestSideMidpoint] = CRGB::Green;  //midpoint left
    _ledsRight[_orientationTestSideMidpoint] = CRGB::Red;  //midpoint right
    _ledsFront = CRGB::Orange;
  }
  if (o.GetOrientation() == 2) {
    //up
    _ledsRear = CRGB::Red;
    _ledsLeft[_orientationTestSideMidpoint] = CRGB::Green;
    _ledsRight[_orientationTestSideMidpoint] = CRGB::Red;
    _ledsFront = CRGB::White;             //(midpoint) front
  }
  if (o.GetOrientation() == 3) {
    //down
    _ledsRear = CRGB::White;             //(midpoint) back
    _ledsLeft[_orientationTestSideMidpoint] = CRGB::Red;
    _ledsRight[_orientationTestSideMidpoint] = CRGB::Green;
    _ledsFront = CRGB::Red;
  }
  if (o.GetOrientation() == 4) {
    //on its left-side
    _ledsLeft[_orientationTestSideMidpoint] = CRGB::White;  //midpoint left
  }
  if (o.GetOrientation() == 5) {
    //on its right-side
    _ledsRight[_orientationTestSideMidpoint] = CRGB::White;  //midpoint right
  }

} //END showOrientation

