/*
    breathing TEST sketch. Parity with longboardLight1_A v0.3
    
    'longboardLight1_A' by Thurstan. LED longboard lights with motion tracking.
    Copyright (C) 2016  MTS Standish (Thurstan|mattKsp)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be usefull,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    https://github.com/mattThurstan/
*/


/*----------------------------libraries----------------------------*/
#include <EEPROM.h>                               //Arduino Pro Mini ATmega328 5V 16MHz - 1kBs EEPROM
//#include <Bounce2.h>                              //buttons with de-bounce
#include <FastLED.h>                              //WS2812B LED strip control and effects
//#include <I2Cdev.h>                               //I2C devices
//#include <MPU6050.h>                              //MPU6050 6-axis motion sensor
//#include <Wire.h>

/*----------------------------system----------------------------*/
const String _progName = "breathing_TEST";
//const String _progVers = "0.3";                   //pre-re-build. moved over to using CRGBArray. only 4 modes
const boolean _batteryPowered = false; //take away const if power charge sensing ever gets implemented  //are we running on battery or plugged into the computer?
const int _mainLoopDelay = 0;                     //just in case  -- NOT using FastLED.delay instead.. --
//#define UPDATES_PER_SECOND 120                    //main loop FastLED show delay - 1000/120

String _inputString = "";                         //a string to hold incoming data
boolean _stringComplete = false;                  //whether the string is complete

/*----------------------------arduino pins----------------------------*/
const byte _ledDOutPin0 = 9;                      //rear lights
const byte _ledDOutPin1 = 5;                      //left
const byte _ledDOutPin2 = 6;                      //right and head lights
const byte _ledPin = 13;                          //built-in LED

/*----------------------------sensors - MPU6050 6-axis----------------------------*/
byte _directionCur = 0;                           // -1 = stationary, 0 = forward, 1=back, 2=up, 3=down, 4=left, 5=right

/*----------------------------orientation----------------------------*/
byte _orientation = 2;      //0                      //0=flat, 1=upside-down, 2=up, 3=down, 4=left-side, 5=right-side

/*----------------------------LED----------------------------*/
typedef struct {
  byte first;
  byte last;
  byte total;                                     //haven't got more than 256 LEDs in a segment
} LED_SEGMENT;
const byte _ledNum = 40;                          //18 LED strip each side and 2 each end = 40 LEDs (2280mA max)
const byte _segmentTotal = 4;                     //2 sides, 2 ends
LED_SEGMENT ledSegment[_segmentTotal] = { 
  { 0, 1, 2 },      //rear brake lights
  { 2, 19, 18 },    //left side
  { 20, 37, 18 },   //right side
  { 38, 39, 2 },     //front head lights
};
const uint16_t _1totalDiv3 = (ledSegment[1].total / 3); //used in 'mode/void breathRiseFall'
const uint16_t _2totalDiv3 = (ledSegment[2].total / 3); //used in 'mode/void breathRiseFall'
//CRGB _leds[_ledNum];                              //global RGB array
//CRGBSet _ledsSideSet(_leds, 36);                  //set made from segements 1 and 2 combined
CRGBArray<_ledNum> _leds;                         //CRGBArray means can do multiple '_leds(0, 2).fadeToBlackBy(40);' as well as single '_leds[0].fadeToBlackBy(40);'

byte _ledGlobalBrightnessCur = 255;               //current global brightness - adjust this
byte _ledBrightnessIncDecAmount = 10;             //the brightness amount to increase or decrease
byte _headLightsBrightness = 200;                 //use function to set
byte _rearLightsBrightness = 200;                 //use function to set
byte _trackLightsFadeAmount = 16;          //64   //division of 256 eg. fadeToBlackBy( _leds, _ledNum, _trackLightsFadeAmount);
int _ledState = LOW;                              //use to toggle LOW/HIGH (ledState = !ledState)
volatile byte _ledMovePos = 0;                    //center point for tracking LEDs to wheels

//CRGB _headLightsCol;                              //colour of the head lights. restricted by 'headLightsBrightness', use functions to set. 
CHSV _headLightsColHSV( 0, 0, 200);               //default - white @ 200
//CRGB _rearLightsCol;                              //colour of the rear lights. restricted by 'rearLightsBrightness', use functions to set.
CHSV _rearLightsColHSV( 0, 255, 200);               //default - red @ 200


/*----------------------------MAIN----------------------------*/
void setup() {
  Serial.print(F("Setup starting"));
  Serial.println();
  blinkStatusLED1();
  setupSerial();                          //..see 'util'
  loadAllSettings();                         //load any saved settings eg. save state when turn board power off. - not fully implemented yet !!!
  delay(3000);                            //..after setting interrupts, give the power, LED strip, etc. a couple of secs to stabilise
  setupLEDs();                            //setup LEDs first and then use as setup indicator lights
  delay(400);
  //everything done? ok then..
  Serial.print(F("Setup done"));
  Serial.println();
  blinkStatusLED3();
  delay(400);
}

void loop() {
  loopModes();
  
  FastLED.show();                           //send all the data to the strips
  //sendMovementDataStream();               //send readings to PC/app
  //FastLED.delay(1000 / UPDATES_PER_SECOND); //doesn't handle the power management stuff yet
  delay(_mainLoopDelay);
}

  
