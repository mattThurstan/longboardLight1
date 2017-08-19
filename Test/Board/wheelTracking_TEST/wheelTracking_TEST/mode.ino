/*----------------------------mode----------------------------*/

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
      loopTrackLights();
}

/* Wheel tracked running lights (position from wheel data combined with direction from MPU6050) */
void loopTrackLights() {  
  _leds.fadeToBlackBy(_trackLightsFadeAmount);            
  
  //wrap-around for segments 1 and 2  
  if (_ledMovePos > ledSegment[1].total) { _ledMovePos = _ledMovePos - ledSegment[1].total; } 
  else if (_ledMovePos < 1) { _ledMovePos = _ledMovePos + ledSegment[1].total; }
  
  _leds[ledSegment[1].last - _ledMovePos + 1] = CRGB::White;
  _leds[ledSegment[2].last - _ledMovePos + 1] = CRGB::White;
  
  FastLED.show();
}

