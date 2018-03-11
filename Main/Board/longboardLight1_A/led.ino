/*----------------------------led display----------------------------*/
void setupLEDs() {
  
  //FastLED
  if (_batteryPowered == 0) {
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 450);  //USB powered
  } else {
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 450);  //plugged into the computer //board TEST
    //FastLED.setMaxPowerInVoltsAndMilliamps(5, 1800);  //limit power draw to 1.8A at 5v (with 7.4V 2700mAhour power supply this gives us a bit of head room for board, lights etc.)
  } 
  
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
  _ledMovePos = 13;
}

