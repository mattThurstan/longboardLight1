/*----------------------------(main lights) sub-mode - patterns (called from subMode)----------------------------*/

void fadeall() { for(int i = 0; i < _ledNum; i++) { _leds[i].nscale8(250); } }

/* sub-mode 0 - blank */
void blank() {
  _leds.fadeToBlackBy(32);  //do nothing - make sure everything is 'off' at this point in the proceedings
}

/* sub-mode 1 - glow */
void glow() {
  _leds(ledSegment[1].first, ledSegment[2].last) = CRGB(16, 16, 16);
}

/* sub-mode 2 - gradient  from end to end */
void gradient() {
  _leds(ledSegment[1].first, ledSegment[1].last).fill_gradient(_rearLightsColHSV, _headLightsColHSV);
  _leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
}

/* sub-mode 3 - wheel tracked running lights (position from wheel data combined with direction from MPU6050)
 * always 3 cos of sensors loopOrientation()
 */
void loopTrackLights() {  
  _leds(ledSegment[1].first, ledSegment[2].last).fadeToBlackBy(_trackLightsFadeAmount);            //_trackLightsFadeAmount
  
  //wrap-around for segments 1 and 2  
  if (_ledMovePos > ledSegment[1].total) { _ledMovePos = _ledMovePos - ledSegment[1].total; } 
  else if (_ledMovePos < 1) { _ledMovePos = _ledMovePos + ledSegment[1].total; }
  
  _leds[ledSegment[1].last - _ledMovePos + 1] = CRGB::White;
  _leds[ledSegment[2].last - _ledMovePos + 1] = CRGB::White;
  
  //FastLED.show(); //not here.. this would show before the headlights and rearlights
}

void mattKspTest()
{
  //eg. equivalant to 'mode X'
  //fill_solid(leds, _ledNum, solidColor2);
  _leds(ledSegment[1].first, ledSegment[1].last) = solidColor2;
  _leds(ledSegment[2].first, ledSegment[2].last) = solidColor2;
}

void showSolidColor()
{
  //fill_solid(leds, _ledNum, solidColor);
  _leds(ledSegment[1].first, ledSegment[1].last) = solidColor;
  _leds(ledSegment[2].first, ledSegment[2].last) = solidColor;
}

/* gradient (end to end) with adjustable colours */
void gradientCol() {
  _leds(ledSegment[1].first, ledSegment[1].last).fill_gradient_RGB(solidColor2, solidColor);
  _leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
}

/* wheel tracking (basic) with adjustable colour */
void loopTrackLightsCol() {  
  _leds(ledSegment[1].first, ledSegment[2].last).fadeToBlackBy(_trackLightsFadeAmount);            //_trackLightsFadeAmount
  
  //wrap-around for segments 1 and 2  
  if (_ledMovePos > ledSegment[1].total) { _ledMovePos = _ledMovePos - ledSegment[1].total; } 
  else if (_ledMovePos < 1) { _ledMovePos = _ledMovePos + ledSegment[1].total; }
  
  _leds[ledSegment[1].last - _ledMovePos + 1] = solidColor;
  _leds[ledSegment[2].last - _ledMovePos + 1] = solidColor2;
  
  //FastLED.show(); //not here.. this would show before the headlights and rearlights
}

/* https://github.com/FastLED/FastLED/blob/master/examples/DemoReel100/DemoReel100.ino  */
void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( _leds, _ledNum, gHue, 7);
}


/* don't use this - doesn't like converting fract8 into a void for the pattern list - not worth it, walk away slowly */
void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    _leds[ random16(_ledNum) ] += CRGB::White;
  }
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( _leds, _ledNum, 10);
  int pos = random16(_ledNum);
  _leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( _leds, _ledNum, 20);
  int pos = beatsin16( 13, 0, _ledNum-1 );
  _leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < _ledNum; i++) { //9948
    _leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( _leds, _ledNum, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    _leds[beatsin16( i+7, 0, _ledNum-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

//..and now with colour
void rainbowCol() 
{
  // FastLED's built-in rainbow generator
  //fill_rainbow( _leds, _ledNum, gHue, 7);
  _leds(ledSegment[1].first, ledSegment[1].last).fill_rainbow(solidColor, 7);
  _leds(ledSegment[2].first, ledSegment[2].last).fill_rainbow(solidColor2, 7);
}

void rainbowWithGlitterCol() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbowCol();
  addGlitter(80);
}

void confettiCol() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( _leds, _ledNum, 10);
  int pos = random16(_ledNum);
  //_leds[pos] += CHSV( solidColor. + random8(64), 200, 255);
  _leds[pos] += CRGB( solidColor.r + random8(64), solidColor.g, solidColor.b );
}

void sinelonCol()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( _leds, _ledNum, 20);
  int pos = beatsin16( 13, 0, ledSegment[1].total-1 );  //_ledNum-1 );
  int pos2 = beatsin16( 13, 0, ledSegment[2].total-1 );
  //_leds[pos] += CHSV( gHue, 255, 192);
  _leds[ledSegment[1].first + pos] += solidColor;
  //_leds[pos] += CHSV( gHue, 255, 192);
  _leds[ledSegment[1].total + pos2] += solidColor2;
}

/* https://github.com/FastLED/FastLED/blob/master/examples/Cylon/Cylon.ino */
void cylon() 
{
  //_leds(ledSegment[1].first, ledSegment[1].last).fill_gradient_RGB(solidColor2, solidColor);
  //_leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
  
  static uint8_t cHue = 0;
  
  // First slide the led in one direction
  for(int i = 0; i < ledSegment[1].total; i++) {
    // Set the i'th led to red 
    _leds[ledSegment[1].first + i] = CHSV(cHue++, 255, 255);
    _leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
    // Show the leds
    FastLED.show();  //special case, cos its cylons
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    //delay(10);
    FastLED.delay(10);
  }

  // Now go in the other direction.  
  for(int i = (ledSegment[1].total)-1; i >= 0; i--) {
    // Set the i'th led to red 
    _leds[ledSegment[1].first + i] = CHSV(cHue++, 255, 255);
    _leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
    // Show the leds
    FastLED.show(); //special case, cos its cylons
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    //delay(10);
    FastLED.delay(10);
  }
  
}

void cylonCol() 
{
  //_leds(ledSegment[1].first, ledSegment[1].last).fill_gradient_RGB(solidColor2, solidColor);
  //_leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
  
  //static uint8_t cHue = 0;
  
  // First slide the led in one direction
  for(int i = 0; i < ledSegment[1].total; i++) {
    // Set the i'th led to red 
    _leds[ledSegment[1].first + i] = solidColor; //CHSV(cHue++, 255, 255);
    //_leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
    _leds[ledSegment[2].first + i] = solidColor2; //CHSV(cHue++, 255, 255);
    // Show the leds
    FastLED.show();  //special case, cos its cylons
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    //delay(10);
    FastLED.delay(10);
  }

  // Now go in the other direction.  
  for(int i = (ledSegment[1].total)-1; i >= 0; i--) {
    // Set the i'th led to red 
    _leds[ledSegment[1].first + i] = solidColor; //CHSV(cHue++, 255, 255);
    //_leds(ledSegment[2].first, ledSegment[2].last) = _leds(ledSegment[1].first, ledSegment[1].last);
    _leds[ledSegment[2].first + i] = solidColor2; //CHSV(cHue++, 255, 255);
    // Show the leds
    FastLED.show(); //special case, cos its cylons
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    //delay(10);
    FastLED.delay(10);
  }
  
}

