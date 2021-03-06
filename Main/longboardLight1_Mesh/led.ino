/*----------------------------setup led----------------------------*/
void setupLEDs()
{
    
  //uint8_t volts = 0;
  uint32_t milliamps = 0;
  
  if (_batteryPowered == 0) {
    milliamps = 450;  //USB powered
  } else {
    if (_curBoardProfile == 1) { 
      milliamps = 1800; //Dervish Sama board
    } else if (_curBoardProfile == 2) {
      milliamps = 1800; //Drop-down board
    } else {
      milliamps = 450;  //plugged into the computer //board TEST
    }
  }
  FastLED.setMaxPowerInVoltsAndMilliamps(5, milliamps);
  
  //FastLED doesn't like an array being used for the pins eg. _ledDOutPin[0]
  //head lights and left side need to be combined! ..might also split right side and brake lights.
  //FastLED.addLeds<WS2812B, _ledDOutPin0, GRB>(_leds, ledSegment[0].first, ledSegment[0].total).setCorrection( TypicalSMD5050 );  //rear lights
  //FastLED.addLeds<WS2812B, _ledDOutPin1, GRB>(_leds, ledSegment[1].first, ledSegment[1].total).setCorrection( TypicalSMD5050 );  //left strip
  //FastLED.addLeds<WS2812B, _ledDOutPin2, GRB>(_leds, ledSegment[2].first, (ledSegment[2].total + ledSegment[3].total)).setCorrection( TypicalSMD5050 );  //right strip + head lights
  FastLED.addLeds<WS2812B, _ledDOutPin0, GRB>(_leds, 0, _ledNum).setCorrection( TypicalSMD5050 );  //all lights 1 loop
  

  FastLED.setBrightness(_ledGlobalBrightnessCur);       //set global brightness
  FastLED.setTemperature(UncorrectedTemperature);       //set first temperature

  //calculateBreathRiseFallRates();                       //re-calculate if user changes max breath brightness
  //_ledMovePos = ledSegment[1].total/2;                  //set LED tracking start point in the middle of the board.
  _ledMovePos = 15;
}

void flashLED(byte led)
{
  if (led == 0) {
    //
  } else if (led == 1) {
    _leds[1] = CRGB::Yellow;
  } else if (led == 2) {
    _leds[2] = CRGB::Fuchsia;
  } else if (led == 3) {
    _leds[3] = CRGB::Orange;
  } else if (led == 4) {
    _leds[4] = CRGB::Violet;
  } else if (led == 5) {
    _leds[5] = CRGB::Teal;
  } else if (led == 6) {
    _leds[6] = CRGB::Pink;
  } else if (led == 7) {
    _leds[7] = CRGB::Green;
  }
  FastLED.show();
  delay(400);
}

/*----------------------------loop led----------------------------*/


