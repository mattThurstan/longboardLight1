/*
    'longboardLight1_A' by Thurstan. LED longboard lights with motion tracking.

    -- TESTING IF V0.305 COMPILES FOR A D1 MINI (ESP8266) --
    
    Copyright (C) 2016  MTS Standish (mattThurstan)

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


#define DEBUG 1                                   //comment/un-comment
#ifdef DEBUG
  //#define DEBUG_INTERRUPT 1
#endif
#define DATA_LOGGING 1                            //turn data logging on or off eg. rps/mps, dist travelled, etc.


/*----------------------------libraries----------------------------*/
#include <EEPROM.h>                               //WeMos D1 (R2 &) mini, 80 MHz, 115200 baud, 4M, (3M SPIFFS)
//#include <Bounce2.h>                              //buttons with de-bounce
#include <FastLED.h>                              //WS2812B LED strip control and effects
#include <MT_BlinkStatusLED.h>                    //my basic status LED blink library
#include <MT_BoardWheel.h>                        //(single version) attempt to move all board wheel related items to a single library
#include <MT_BoardOrientation.h>                  //attempt to move all board mpu6050 sensor related items to a single library

/*----------------------------system----------------------------*/
const String _progName = "longboardLight1_A_testD1MiniCompile";
const String _progVers = "0.305";                   //test
const boolean _batteryPowered = true; //take away const if power charge sensing ever gets implemented  //are we running on battery or plugged into the computer?
//const int _mainLoopDelay = 0;                     //just in case  - using FastLED.delay instead..
#define UPDATES_PER_SECOND 0           //120      //main loop FastLED show delay - 1000/120
boolean _firstTimeSetupDone = false;              //starts false
/*
 * Serial baud speeds:
 * 9600   - Arduino Pro Mini ATmega328 5V 16MHz
 * 38400  - for controlling MPU6050 over serial (don't really need it this fast so not doing it this way)
 * 115200 - D1 Mini (ESP8266)
 */
#define SERIAL_SPEED 115200                         //Serial coms baud speed
//#ifdef DEBUG
//String _inputString = "";                         //a string to hold incoming data
//boolean _stringComplete = false;                  //whether the string is complete
//#endif

boolean _testModes = false;                       //used as an override to test all the modes
byte _curTestModes = 0;                           //the current mode being tested in isolation - sent from device

//3-axis accelerometer  calibration (these will be moved and integrated later when have communications)
boolean _doFullCalibration = false;               //set to true to run full calibration. it will reset itself to false when finished.
//a quick calibration could normally be done, unless it gets turned off at the end of a full calibration.
boolean _doQuickCalibration = false;              //set to true to run quick calibration. it will reset itself to false when finished.
//boolean _orientationTest = false;                 //used as a test override. not during normal operation. will prob remove later

//const byte _buttonTotal = 2;                      //how many butons are there? - cannot set Bounce using this unfortunately!
//const byte _wheelSensorTotal = 1;                 //how many wheels have we mounted sensors on?

/*----------------------------arduino pins----------------------------*/
//const byte _wheelSensorPin[_wheelSensorTotal] = { 0 };  //array of wheel sensor inputs (!!all interrupt pins!!!) - uses _wheelSensorTotal
const byte _wheelSensorPin = 0;   //D3
const byte _ledDOutPin0 = 14;     //D5            //rear lights
const byte _ledDOutPin1 = 12;     //D6            //left
const byte _ledDOutPin2 = 13;     //D7            //right and head lights
//const byte _buttonPin[_buttonTotal] = { 11, 12 }; //array of user input buttons - uses _buttonTotal - opps.. think I burned out pin 10 by mistake - nope, just wired button 90deg wrong way
MT_BlinkStatusLED statusLED(2);                   //setup status LED on pin 13
/*
 * Pinout D1 Mini (ESP-8266)
 * RX  3
 * TX  1
 * A0  Analog input, max 3.3V input  A0
 * D0  16  IO  GPIO16   - no PWM or I2C
 * D1  5  IO, SCL GPIO5  I2C
 * D2  4  IO, SDA GPIO4  I2C
 * D3  0  IO, 10k Pull-up GPIO0
 * D4  2  IO, 10k Pull-up, BUILTIN_LED  GPIO2
 * D5  14  IO, SCK GPIO14
 * D6  12  IO, MISO  GPIO12
 * D7  13  IO, MOSI  GPIO13
 * D8  15  IO, 10k Pull-down, SS GPIO15
 */
/*----------------------------modes----------------------------*/
/* In standby the board 'breathes' (glows gently) at 12 times a minute (average breathing rate of sleeping adult).
 * Main lights on/off is controlled using the blank sub-mode.
 */
typedef struct {
  boolean sleep;    //doesn't use Enabled
  boolean breathe;  //doesn't use Active
  boolean head;
  boolean rear;
  boolean brake;    //give the brake lights a slight brightness boost when decelerating
  boolean indicate;
} MODES_ENABLED_ACTIVE;

MODES_ENABLED_ACTIVE mE = { true, true, true, true, true, false };    // Enabled
MODES_ENABLED_ACTIVE mA = { false, false, true, true, false, false }; // Active

const byte _mainLightsSubModeTotal = 4;             //never gonna have more than 256 lighting modes..
/*memory\void setDefaultSettings*/byte _mainLightsSubMode = 3;  //sub-mode for main lights loop: 0=none/blank, 1= , 2= , 3=

/*----------------------------buttons----------------------------*/
//const unsigned long _buttonDebounceTime = 5;      //5 milli-seconds debounce time
//Bounce *_button = new Bounce[_buttonTotal];       //pointer to new array of N buttons
//const unsigned long _loopButtonsInterval = 250;   //read loop interval in milliseconds. press button, count '1 (mabye 2)',  then let go.
//unsigned long _loopButtonsPrevMillis = 0;         //previous time for reference
//boolean _buttonToggled[_buttonTotal] = { false }; //array of button toggle states

/*----------------------------sensors - wheels----------------------------*/
//latching bipolar hall effect sensor mounted on chassis, with 8 magents mounted on wheel
MT_BoardWheel w;

/*----------------------------sensors - MPU6050 6-axis----------------------------*/
/* MPU6050:             X=Right/Left, Y=Forward/Backward, Z=Up/Down
 * orientation (byte):  0=flat, 1=upside-down, 2=up, 3=down, 4=left-side, 5=right-side
 * direction (byte):    -1=stationary, 0=forward, 1=back, 2=up, 3=down, 4=left, 5=right
 */
const unsigned long _mpu6050ReadInterval = 20;    //read loop interval in milliseconds
const unsigned long _orientationInterval = 100;   //main orientation read loop interval in milliseconds
byte _orientationTestSideMidpoint = 0;            //side LED strip midpoint, calculated in startup
MT_BoardOrientation o;

/*----------------------------LED----------------------------*/
//..see up top.. #define UPDATES_PER_SECOND 120   //main loop FastLED show delay - 1000/120
//mabye struct should contain int16_t instead of byte. eg. fill_gradient asks for it..
typedef struct {
  byte first;
  byte last;
  byte total;                                     //byte ok as haven't got more than 256 LEDs in a segment
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
CRGBArray<_ledNum> _leds;                         //CRGBArray means can do multiple '_leds(0, 2).fadeToBlackBy(40);' as well as single '_leds[0].fadeToBlackBy(40);'

/*memory\void setDefaultSettings*/byte _ledGlobalBrightnessCur = 255;               //current global brightness
/*memory\void setDefaultSettings*/byte _headLightsBrightness = 200;                 //use function to set
/*memory\void setDefaultSettings*/byte _rearLightsBrightness = 200;                 //use function to set
/*memory\void setDefaultSettings*/byte _trackLightsFadeAmount = 16;          //64   //division of 256 eg. fadeToBlackBy( _leds, _ledNum, _trackLightsFadeAmount);
volatile byte _ledMovePos = 0;                    //center point for tracking LEDs to wheels

CHSV _headLightsColHSV( 0, 0, 200);               //fixed - white @ 200
CHSV _rearLightsColHSV( 0, 255, 200);             //fixed - red @ 200

/*----------------------------MAIN----------------------------*/
void setup() {

  //clearAllSettings();   // TEMP
  
  statusLED.Blink1();
  
  setupSerial();                                  //..see 'util'
  loadAllSettings();                              //load any saved settings eg. save state when turn board power off. - not fully implemented yet !!!
  setupInterrupts();                              //set any interrupts..
  delay(3000);                                    //..after setting interrupts, give the power, LED strip, etc. a couple of secs to stabilise
  setupLEDs();                                    //setup LEDs first and then use as setup indicator lights
  delay(400);
    _leds[ledSegment[1].first] = CRGB::Yellow;
    _leds[ledSegment[2].first] = CRGB::Yellow;
    FastLED.show();
  setupSensors();                                 //setup all sensor inputs (note: sensors on wheels use interrupt pins)
  delay(400);
    _leds[ledSegment[1].first+2] = CRGB::Fuchsia;
    _leds[ledSegment[2].first+2] = CRGB::Fuchsia;
    FastLED.show();
  //setupUserInputs();                              //setup any user inputs - buttons, WIFI, bluetooth etc.
  delay(400);
    _leds[ledSegment[1].first+4] = CRGB::Green;
    _leds[ledSegment[2].first+4] = CRGB::Green;
    FastLED.show();
  _orientationTestSideMidpoint = ledSegment[1].total / 2; //change it later, easier for 2 segments
  //checkStartupButtons();  //check to see if any button has been held down during startup eg. full calibration
  //
  #ifdef DEBUG
  //everything done? ok then..
    Serial.print(F("Setup done"));
    Serial.println();
  #endif
  statusLED.Blink1();
  delay(400);
    _leds[ledSegment[1].first+6] = CRGB::MediumTurquoise;
    _leds[ledSegment[2].first+6] = CRGB::MediumTurquoise;
    FastLED.show();
  delay(400);
  FastLED.clear();
}

void loop() {
  if(_firstTimeSetupDone == false) {
    #ifdef DEBUG
    //
    #endif
    _firstTimeSetupDone = true;
  }

  //cut everything out of the loop and do calibrations - put board flat and press button to start these loops
  //check for full calibration bt first
  if (_doFullCalibration == true) { 
    fullCalibration(); 
    _doQuickCalibration = false; //if we have just done a full one, we don't need to do a quick one.
  }
  else if (_doQuickCalibration == true) { quickCalibration(); }   //..if not, try for a quick one - this has issues!
  else {
    //run the loop normally
    //loopUserInputs();   //if a board with more interrupts for the buttons, that would cut out this whole loop!
    loopSensors();
    loopModes();
  }
  
  FastLED.show();
  //delay(_mainLoopDelay);
  //FastLED.delay(1000 / UPDATES_PER_SECOND);
  FastLED.delay(UPDATES_PER_SECOND);  //currently '0'
}

