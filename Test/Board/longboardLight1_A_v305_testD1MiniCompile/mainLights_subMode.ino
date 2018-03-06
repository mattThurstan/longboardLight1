/*----------------------------main lights sub-modes----------------------------*/

/* Wheel tracked running lights (position from wheel data combined with direction from MPU6050) */
void loopTrackLights() {  
  _leds(ledSegment[1].first, ledSegment[2].last).fadeToBlackBy(_trackLightsFadeAmount);            //_trackLightsFadeAmount
  
  //wrap-around for segments 1 and 2  
  if (_ledMovePos > ledSegment[1].total) { _ledMovePos = _ledMovePos - ledSegment[1].total; } 
  else if (_ledMovePos < 1) { _ledMovePos = _ledMovePos + ledSegment[1].total; }
  
  _leds[ledSegment[1].last - _ledMovePos + 1] = CRGB::White;
  _leds[ledSegment[2].last - _ledMovePos + 1] = CRGB::White;
  
  //FastLED.show(); //not here.. this would show before the headlights and rearlights
}

