/*----------------------------display----------------------------*/

/* WS2812B  
 * power/mA per LED = 60
 * total _leds = 40
 * total power/mA = 2400 (full white) ( 75% duty cycle/mA = 1800 )
 * total power per colour/mA = 800  ( 75% duty cycle/mA = 600 )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
 */

void setupLEDs() {
  
  //system _leds
  pinMode(_ledPin, OUTPUT);                 //Setup the internal LED on pin 13
  digitalWrite(_ledPin, _ledState);

  //FastLED
  //FastLED.setMaxPowerInVoltsAndMilliamps(5, 1800);  //limit power draw to 1.8A at 5v (with 7.4V 2700mAhours power supply this gives us a bit of head room for board, lights etc.)
FastLED.setMaxPowerInVoltsAndMilliamps(5, 450);  //board TEST
  //FastLED doesn't like an array being used for the pins eg. _ledDOutPin[0]
/*  { 0, 1, 2 },      //front head lights
  { 2, 19, 18 },    //left side
  { 20, 37, 18 },   //right side
  { 38, 39, 2 },     //rear brake lights
  ledSegment
   first;
  last;
  total; 
*/
  FastLED.addLeds<WS2812B, _ledDOutPin0, GRB>(_leds, ledSegment[0].first, ledSegment[0].total).setCorrection( TypicalSMD5050 );  //head lights
  FastLED.addLeds<WS2812B, _ledDOutPin1, GRB>(_leds, ledSegment[1].first, ledSegment[1].total).setCorrection( TypicalSMD5050 );  //left strip
  FastLED.addLeds<WS2812B, _ledDOutPin2, GRB>(_leds, ledSegment[2].first, (ledSegment[2].total + ledSegment[3].total)).setCorrection( TypicalSMD5050 );  //right strip + rear lights
//  FastLED.addLeds<WS2812B, _ledDOutPin0, GRB>(_leds, 0, 2).setCorrection( TypicalSMD5050 );  //head lights
//  FastLED.addLeds<WS2812B, _ledDOutPin1, GRB>(_leds, 2, 18).setCorrection( TypicalSMD5050 );  //left strip
//  FastLED.addLeds<WS2812B, _ledDOutPin2, GRB>(_leds, 20, 18).setCorrection( TypicalSMD5050 );  //right strip + rear lights
  
  FastLED.setBrightness(_ledGlobalBrightness);      //set global brightness
  FastLED.setTemperature(UncorrectedTemperature);   //set first temperature

}

/*-----------display - status------------*/

void blinkStatusLED2() {
  //this delays the whole script. use sparingly
  digitalWrite(_ledPin, HIGH);
  delay(250);
  digitalWrite(_ledPin, LOW);
  delay(250);
  digitalWrite(_ledPin, HIGH);
  delay(250);
  digitalWrite(_ledPin, LOW);
}

void blinkStatusLED() {
  //this delays the whole script. use sparingly
  digitalWrite(_ledPin, HIGH);
  delay(250);
  digitalWrite(_ledPin, LOW);
  delay(250);
  digitalWrite(_ledPin, HIGH);
  delay(250);
  digitalWrite(_ledPin, LOW);
  delay(250);
  digitalWrite(_ledPin, HIGH);
  delay(250);
  digitalWrite(_ledPin, LOW);
}
