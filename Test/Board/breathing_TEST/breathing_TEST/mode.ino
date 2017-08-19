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
  breathRiseFall();
  
//  EVERY_N_MILLISECONDS(20) {                           // FastLED based non-blocking delay to update/display the sequence.
//    ease();
//  }
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

//void loopBreathing() {
//  //'breathing'
//  if (_breathingEnabled == true) { 
//    breathRiseFall();
//  }
//}

void breathRiseFall() {
  //unsigned long breathRiseFallCurrentMillis = millis();  //get current time
  //if ((unsigned long)(breathRiseFallCurrentMillis - _breathRiseFallPrevMillis) >= _breathRiseFallStepIntervalMillis) {
  EVERY_N_MILLISECONDS(250) {
    _leds.fadeToBlackBy(64);    //make any unused pixels fade out
    //ignore first/last 4 (or so) so we get a pause at the bottom and top
    if (_breathRiseFallCounter >= _breathRiseFallSpacer) {
      c.r = _breathRiseFallCounter-_breathRiseFallSpacer;
      c.g = _breathRiseFallCounter-_breathRiseFallSpacer;
      c.b = _breathRiseFallCounter-_breathRiseFallSpacer;
      if (_orientation == 2) {
        _leds(ledSegment[1].first, ledSegment[1].last - _1totalDiv3).fill_gradient_RGB(c, CRGB::Black );
        _leds(ledSegment[2].first, ledSegment[2].last - _2totalDiv3) = _leds(ledSegment[1].first, ledSegment[1].last - _1totalDiv3);
      } else if (_orientation == 3) {
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
//    _breathRiseFallPrevMillis = breathRiseFallCurrentMillis;      //save time - code timing dependant - intervals will be late if code elsewhere gets backed up
    //_breathRiseFallPrevMillis += breathRiseFallCurrentMillis;   //save time - NOT code timing dependant - intervals will try and catch up

  } //END timed loop

  if ( _breathRiseFallCounter >= (_breathMaxBrightness + _breathRiseFallSpacer ) ) {
    _breathRiseFallDirection = false;   //we have reached the end, start falling
  } else if (_breathRiseFallCounter <= 0) {
    _breathRiseFallDirection = true;   //we have reached the beginning, start rising again..
  }

} //END breathRiseFall

//

void ease() {

  static uint8_t easeOutVal = 0;
  static uint8_t easeInVal  = 0;
  static uint8_t lerpVal    = 0;

  easeOutVal = ease8InOutQuad(easeInVal);                     // Start with easeInVal at 0 and then go to 255 for the full easing.
  easeInVal++;

  lerpVal = lerp8by8(ledSegment[1].first, ledSegment[1].last, easeOutVal);                // Map it to the number of LED's you have.

  _leds[lerpVal] = CRGB::Red;
  _leds(ledSegment[2].first, ledSegment[2].last) =  _leds(ledSegment[1].first, ledSegment[1].last);
  //fadeToBlackBy(leds, NUM_LEDS, 16);                          // 8 bit, 1 = slow fade, 255 = fast fade
  _leds.fadeToBlackBy(16);
} // ease()

