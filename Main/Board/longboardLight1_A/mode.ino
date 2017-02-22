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
      fadeToBlackBy( _leds, _ledNum, 64);   //anything not used gets set to fade off
      if (_orientation == 0) { loopMainLights(); }
      //if (_orientation == 1) { loopEmergencyFlash(); }  //upside-down not working yet
      if (_orientation == 2) { loopBreathing(); _headLightsActive = false; } //breathing here is overlaid by rear lights. turn off headlights when you pickup the board so they don't blind you.
      else { _headLightsActive = true; }  //turn the headlights back on when you put the board down.
      if (_orientation == 4 || _orientation == 5) { loopSideLight(); }
      loopHeadLights(); //..after the main bits
      loopRearLights(); //..
      loopIndicatorFlash();
    } else { 
      //sleep - defined by a period of no movement, a switch, or a wireless command.
      //fadeToBlackBy( _leds, _ledNum, 64); //(array, count, percent) eg. 192/256 = 75% (use 192), 64 = 25%
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
  //Fire2012();
  //confetti();
  //sinelon();
  //bpm();
  //juggle();
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

/* 12 times a sec
 * 1000/12 = 83.333 (millis) = total rise and fall time per 'breath'
 * 83.333 (millis)
 * 12*83 = 996
 * 12*84 = 1008 (rounding up)
 * 84/2 = 42 = rise/fall time (millis)
 * 42/18 (eg. left segment total) = 2.33 (millis) - can't have decimal place!
 * 
 * (always rounding down)
 * 1000/12 = 83/2 = 41 (41.5)
 * 41/18(ledSegment[1].total) = 2 (2.3)
 * 18(ledSegment[1].total)/2 + 18(ledSegment[1].total) = 27
 * 41/27 = 1 (1.5)
 * 
 * 2ms * 18 = 36
 * 36 * 2 * 12 = 864
 * 3ms * 18 = 54
 * 54 * 2 * 18 = 1296
 * 
 * 83,333 (micros)
 * 12 * 83,333 = 999,996
 * 83,333 / 2 / 18 = 2314 (millis) (2314.8055555555)
 * 2314 * 18 = 41652
 * 41652*2*12 = 999648 (micros)
 * 
 * too many rounding errors either way. gonna use 3ms per step
 * mabye 2 to rise, 1 to hold, 2 to fall, 1 to hold, etc
 * 
 * damn.. 1 minute, not second..
 * "Note: there are 1,000 microseconds in a millisecond and 1,000,000 microseconds in a second."
 * 
 * 60*1000 = 60,000 (millis)
 * 60,000/12 = 5000 full
 * 5000/2 = 2500 rise/fall
 * 2500/18(ledSegment[1].total) = 138.88 step
 * 
 * 2500/255 = 9.80
 * 10ms per brightness step
 * 5000/18 = 277.77
 * 5000/255 = 19.60
 * 5000/256 = 19.56
 * 
 * 5000 / 298(256+42) =  16.77
 * 
 */ 
// (62 8 32)    156 8 32    312 4 16
//60000/12=5000 12 breaths per min
//5000
//5000/24=208       208 4 16
//5000/48=104       104 8 32
//60000/6=10000     6 breaths per min
//10000/18=555.55   556 2 16
//10000/24=416      416 4 16
//10000/48=208      208 8 32
//
//60000/12=5000
//60000/11=5454
//60000/10=6000
//60000/9=6666  
//60000/8=7500
//60000/7=8571
//
//6000/18=333.33
//6000/24=250       250 4 16 (bot&top)
//6000/20=300       300 4 16 (bot)
//
//6666/18=370
//6666/24=277.75
//
const unsigned long _breathRiseFallStepIntervalMillis = 250;  //156  62  139  2 //breath rise/fall step interval in milliseconds
const int _breathRiseFallSpacer = 4;  //just 4 at begin.. eg. 4 each end.. //8   //_breathMaxBrightness / (4*2)     //counts to pause/ignore at the bottom and top - 42
const int _breathMaxBrightness = 16;  //32
//
//const unsigned long _breathRiseFallIntervalMillis = 42;     //breath rise/fall interval in milliseconds
const unsigned long _breathRiseFallHoldIntervalMillis = 1;    //breath rise/fall hold interval in milliseconds
unsigned long _breathRiseFallPrevMillis = 0;                  //previous time for reference
//const unsigned long _breathRiseFallIntervalMicros = 3;        //breath rise/fall interval in micros
//unsigned long _breathRiseFallPrevMicros = 0;                  //previous time for reference
int _breathRiseFallCounter = 0;                               //eg. 0-17
boolean _breathRiseFallDirection = true;                     //direction true-rise, false-fall
CRGB c;
//int breathsPerMin = 12;   //12
//60000/12=5000
//5000/2=2500
//2500/32=62.5
//2500/64=39.06
//5000/32=156.25
//5000/64=78.12
/*
 * fill gradient, then increase last by 1 each division of time
 * 
 */

//void calculateBreathRiseFallRates() {
//  //_breathMaxBrightness
//  //_breathRiseFallStepIntervalMillis
//  //12 times a min
//  //60*1000 = 60,000 (millis) 1 min
//  //60,000/12 = 5000 full
//  //5000/2 = 2500 rise/fall
//  //2500/18(ledSegment[1].total) = 138.88 step
//  //5000 / 298(256+42) =  16.77
//  int a = (60000/breathsPerMin)/2;  //5000
//  _breathRiseFallSpacer = _breathMaxBrightness / 4; //..this first !!!
//  int b;
//  int c = _breathMaxBrightness + _breathRiseFallSpacer;
//  b = a / c;
//  _breathRiseFallStepIntervalMillis = (unsigned long) a;
//  Serial.print(b);
//  Serial.println();
//}
 
void breathRiseFall() {
  //byte k1 = ledSegment[1].last - (ledSegment[1].total / 2);
  //byte k2 = ledSegment[2].last - (ledSegment[2].total / 2);
  unsigned long breathRiseFallCurrentMillis = millis();  //get current time
  if ((unsigned long)(breathRiseFallCurrentMillis - _breathRiseFallPrevMillis) >= _breathRiseFallStepIntervalMillis) {
    //ignore first/last 4 (or so) so we get a pause at the bottom and top
    //if (_breathRiseFallCounter >= _breathRiseFallSpacer && _breathRiseFallCounter <= (_breathMaxBrightness + _breathRiseFallSpacer) ) {
    if (_breathRiseFallCounter >= _breathRiseFallSpacer) { //&& _breathRiseFallCounter <= (_breathMaxBrightness + _breathRiseFallSpacer) ) {
      c.r = _breathRiseFallCounter-_breathRiseFallSpacer;
      c.g = _breathRiseFallCounter-_breathRiseFallSpacer;
      c.b = _breathRiseFallCounter-_breathRiseFallSpacer;
      fill_gradient_RGB(_leds, ledSegment[0].first, CRGB::Black, ledSegment[3].last, CRGB::Black );
      if (_orientation == 2) {
       // fill_gradient_RGB(_leds, ledSegment[0].first, CRGB::Black, ledSegment[0].last, CRGB::Black );
        //fill_gradient_RGB(_leds, ledSegment[0].first, CRGB::Black, ledSegment[0].last, CRGB::Black );
        fill_gradient_RGB(_leds, ledSegment[1].first, c, ledSegment[1].last - (ledSegment[1].total / 3), CRGB::Black );
        //fill_gradient_RGB(_leds, k1+1, CRGB::Black, ledSegment[1].last, CRGB::Black );
        fill_gradient_RGB(_leds, ledSegment[2].first, c, ledSegment[2].last - (ledSegment[2].total / 3), CRGB::Black );
        //fill_gradient_RGB(_leds, k2+1, c, ledSegment[3].last, CRGB::Black );
      } else if (_orientation == 3) {
        //fill_gradient_RGB(_leds, ledSegment[0].first, CRGB::Black, k1-1, CRGB::Black );
        fill_gradient_RGB(_leds, ledSegment[1].first + (ledSegment[1].total / 3), CRGB::Black, ledSegment[1].last, c );
        //fill_gradient_RGB(_leds, ledSegment[2].first, CRGB::Black, k2-1, c );
        fill_gradient_RGB(_leds, ledSegment[2].first + (ledSegment[2].total / 3), CRGB::Black, ledSegment[2].last, c );
        //fill_gradient_RGB(_leds, ledSegment[3].first, CRGB::Black, ledSegment[3].last, CRGB::Black );
      } else {
        //fill_gradient_RGB(_leds, ledSegment[0].first, CRGB::Black, ledSegment[0].last, CRGB::Black );
        fill_gradient_RGB(_leds, ledSegment[1].first, c, ledSegment[1].last, c );
        fill_gradient_RGB(_leds, ledSegment[2].first, c, ledSegment[2].last, c );
        //fill_gradient_RGB(_leds, ledSegment[3].first, CRGB::Black, ledSegment[3].last, CRGB::Black );
      }
    }
    if (_breathRiseFallDirection == true) {
      _breathRiseFallCounter++;       //we are rising, increase the counter
    } else if (_breathRiseFallDirection == false) {
      _breathRiseFallCounter--;       //we are falling, decrease the counter
    }
    _breathRiseFallPrevMillis = breathRiseFallCurrentMillis;      //save time - code timing dependant - intervals will be late if code elsewhere gets backed up
    //_breathRiseFallPrevMillis += breathRiseFallCurrentMillis;   //save time - NOT code timing dependant - intervals will try and catch up
  
//    #ifdef DEBUG
//      Serial.print(c.r);
//      Serial.print(" - ");
//      Serial.print(_breathRiseFallCounter);
//      Serial.println();
//    #endif
  
  } //END timed loop

  //if ( _breathRiseFallCounter >= (_breathMaxBrightness + (_breathRiseFallSpacer * 2) ) ) {
  if ( _breathRiseFallCounter >= (_breathMaxBrightness + _breathRiseFallSpacer ) ) {
    _breathRiseFallDirection = false;   //we have reached the end, start falling
  } else if (_breathRiseFallCounter <= 0) {
    _breathRiseFallDirection = true;   //we have reached the beginning, start rising again..
  }

} //END breathRiseFall

/* // Test if a color is lit at all (versus pure black)
 * if( leds[i] ) {
 *  // it is somewhat lit (not pure black)
 * } else {
 *  // it is completely black
 * }
 */
void loopBreathing() {
  //'breathing'
  if (_breathingActive == true) { 
    breathRiseFall();
  }
}



