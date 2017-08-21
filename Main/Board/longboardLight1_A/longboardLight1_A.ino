/*
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


//#define DEBUG 1                                   //comment/un-comment
#ifdef DEBUG
  //#define DEBUG_MPU6050 1                           //..
  //#define DEBUG_ORIENTATION 1                       //
  //#define DEBUG_WHEEL 1                             //DEUBUG wheel sensor(s)
  //#define DEBUG_INTERRUPT 1
#endif
#define DATA_LOGGING 1                            //turn data logging on or off eg. rps/mps, dist travelled, etc.


/*----------------------------libraries----------------------------*/
#include <EEPROM.h>                               //Arduino Pro Mini ATmega328 5V 16MHz - 1kBs EEPROM
#include <Bounce2.h>                              //buttons with de-bounce
#include <FastLED.h>                              //WS2812B LED strip control and effects
#include <I2Cdev.h>                               //I2C devices
#include <MPU6050.h>                              //MPU6050 6-axis motion sensor
#include <Wire.h>

/*----------------------------system----------------------------*/
const String _progName = "longboardLight1_A";
const String _progVers = "0.301";                   //pre-re-build. moved over to using CRGBArray. only 4 modes. WORKING
const boolean _batteryPowered = true; //take away const if power charge sensing ever gets implemented  //are we running on battery or plugged into the computer?
//const int _mainLoopDelay = 0;                     //just in case  - using FastLED.delay instead..
#define UPDATES_PER_SECOND 0           //120      //main loop FastLED show delay - 1000/120
boolean _firstTimeSetupDone = false;              //starts false

#ifdef DEBUG
String _inputString = "";                         //a string to hold incoming data
boolean _stringComplete = false;                  //whether the string is complete
#endif
//const unsigned long _sendMovementDataStreamInterval = 1000;//100   //send loop interval in milliseconds   1000
//unsigned long _sendMovementDataStreamPrevMillis = 0;         //previous time for reference

//3-axis accelerometer  calibration (these will be moved and integrated later when have communications)
boolean _doFullCalibration = false;               //set to true to run full calibration. it will reset itself to false when finished.
//a quick calibration could normally be done, unless it gets turned off at the end of a full calibration.
boolean _doQuickCalibration = false;              //set to true to run quick calibration. it will reset itself to false when finished.
boolean _orientationTest = false;                 //used as a test override. not during normal operation. will prob remove later

const byte _buttonTotal = 2;                      //how many butons are there? - cannot set Bounce using this unfortunately!
const float _wheelRadius = 0.0345;                //half of diameter, my wheels are (worn to) 69mm diameter (70mm from the factory, give or take a bit) 
const byte _wheelSensorTotal = 1;                 //how many wheels have we mounted sensors on?
const byte _wheelMagnetTotal = 8; //4;            //how many magnets are mounted on each wheel?

/*----------------------------arduino pins----------------------------*/
const byte _wheelSensorPin[_wheelSensorTotal] = { 2 };     //array of wheel sensor inputs (!!all interrupt pins!!!) - uses _wheelSensorTotal
const byte _ledDOutPin0 = 9;                      //rear lights
const byte _ledDOutPin1 = 5;                      //left
const byte _ledDOutPin2 = 6;                      //right and head lights
const byte _buttonPin[_buttonTotal] = { 11, 12 }; //array of user input buttons - uses _buttonTotal - opps.. think I burned out pin 10 by mistake - nope, just wired button 90deg wrong way
const byte _ledPin = 13;                          //built-in LED

/*----------------------------modes----------------------------*/
boolean _sleepActive = false;                     //init false at power-up
/*memory\void setDefaultSettings*/boolean _breathingEnabled = true;                 //the board 'breathes' (glows gently) at 12 times a minute (average breathing rate of sleeping adult)
/*memory\void setDefaultSettings*/boolean _headLightsEnabled = false;               //have we switched on the headlights?
boolean _headLightsActive = true;                 //start true
/*memory\void setDefaultSettings*/boolean _rearLightsEnabled = false;               //have we switched on the rearlights?
boolean _rearLightsActive = true;                 //start true
boolean _brakeActive = false;                     //give the brake lights a slight brightness boost when decelerating
/*memory\void setDefaultSettings*/boolean _indicatorsEnabled = false;               //indicators for turning left/right
boolean _indicatorLeftActive = false;
boolean _indicatorRightActive = false;
//main lights on/off is controlled using the blank sub-mode
const byte _mainLightsSubModeTotal = 4;           //9 (0-8)       //4 (0-3)     //never gonna have more than 256 lighting modes..
/*memory\void setDefaultSettings*/byte _mainLightsSubMode = 3;                      //sub-mode for main lights loop: 0=none/blank, 1= , 2= , 3=

/*----------------------------buttons----------------------------*/
const unsigned long _buttonDebounceTime = 5;//5;  //5 milli-seconds debounce time
Bounce *_button = new Bounce[_buttonTotal];       //pointer to new array of N buttons
const unsigned long _loopButtonsInterval = 250; //750;   //read loop interval in milliseconds   500= press button, count '1 (mabye 2',  then let go.
unsigned long _loopButtonsPrevMillis = 0;         //previous time for reference
boolean _buttonToggled[_buttonTotal] = { false }; //array of button toggle states

/*----------------------------sensors - wheels----------------------------*/
//latching bipolar hall effect sensor mounted on chassis, with 4/8 magents mounted on wheel
volatile byte _wheelCounter = 0;                  //byte may not be large enough (0-255), might have to use an int
//'_wheelSensorReadInterval' needs to be hardcoded to 1 sec, otherwise we don't get 'revolutions per SECOND' - see 'sensors/void loopWheel'
//const unsigned long _wheelSensorReadInterval = 1000;      //read loop interval in milliseconds   1000
unsigned long _wheelSensorReadPrevMillis = 0;     //previous time for reference
double _wheelSpeedRps = 0;                        //wheel revolutions per second
double _wheelSpeedMps = 0;                        //wheel speed in Meters Per Second
unsigned long _distTraveledForward = 0;           //total distance traveled forward in meters
unsigned long _distTraveledBackward = 0;          //total distance traveled backward in meters
//_distanceTraveledLeft                           //gotta get these in later..
//_distanceTraveledRight
//_distanceTraveledUp
//_distanceTraveledDown
//_distanceTraveledOtherWays
const float _wheelCircumference = 2 * PI * _wheelRadius;


/*----------------------------sensors - MPU6050 6-axis----------------------------*/
//X=Right/Left, Y=Forward/Backward, Z=Up/Down
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  // Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation is used in I2Cdev.h - ???
  #include "Wire.h"
#endif
MPU6050 _mpu6050;  //accel gyro;
int16_t _mpu6050AccelOffset[3] = {436, 1956, 1318};       //XYZ accel offsets to write to the MPU6050 - get from full calibration and save to memory
int16_t _mpu6050GyroOffset[3] = {9, -32, 69};             //XYZ gyro offsets to write to the MPU6050 - get from full calibration and save to memory
const int _mpu6050CalibrateSampleTotal = 100; //100;      //how many samples to take at once when calibrating
const int _mpu6050CalibrateAccelThreshold = 10;           //threshold tolerance for 'dead zone' at center of readings
const int _mpu6050CalibrateGyroThreshold = 3;     //..for gyro
long _mpu6050CalibratePrevMillis = 0;             //previous time for reference
const long _mpu6050CalibrateInterval = 1000;      //sampling interval in milliseconds
const long _mpu6050CalibrateTimeout = 30000;      //sampling interval in milliseconds

//stuff for filtering
const unsigned long _mpu6050ReadInterval = 20; //40 //read loop interval in milliseconds   1000
unsigned long _mpu6050ReadPrevMillis = 0;         //previous time for reference
float _mpu6050AccelZero[3] = {0, 0, 0};           //XYZ quick calibration zero average save for accel - quick offsets to use whilst running
float _mpu6050GyroZero[3] = {0, 0, 0};            //XYZ quick calibration zero average save for gyro
int16_t _mpu6050AccelRead[3];                     //XYZ Current accel reading
int16_t _mpu6050GyroRead[3];                      //XYZ Current gyro reading
int16_t _mpu6050AccelReadAverage[3];              //XYZ averaged current accel reading. see calibration
int16_t _mpu6050GyroReadAverage[3];               //XYZ averaged current gyro reading. see calibration
float _mpu6050GyroPrev[3];                        //XYZ last_gyro_x_angle;
float _mpu6050Accel_yPrev = 0;                    //Y previous raw accleration y value

//FINAL calculated numbers
float _mpu6050FilteredCur[3];                     //XYZ FINAL filtered combined gyro/accel readings for use in calculating orientation
float _mpu6050FilteredPrev[3];                    //XYZ Previous FINAL readings..

//prob won't use 'stationary' cos the calculations will need something to get started, otherwise they will be a frame behind. better to have wrong direction for a split second, than have more complicated code
//also might try this as the average of a rolling buffer cos for 10 samples we wait 200 or 400ms..
const byte _directionSampleTotal = 10;            //how many times to sample direction before making a decision on whether it is true or not
unsigned int _diAccelSave = 0;
byte _diDirectionCounter = 0;                     //restricted by '_directionSampleTotal'
byte _directionCur = 0;                           // -1 = stationary, 0 = forward, 1=back, 2=up, 3=down, 4=left, 5=right


/*----------------------------orientation----------------------------*/
byte _orientation = 0;                            //0=flat, 1=upside-down, 2=up, 3=down, 4=left-side, 5=right-side
byte orMatrix[3] = { 0, 0, 0 };                   //TEMP x =  0(low) / 1(mid) / 2(hi)       - wanted to use -1, 0, 1 but too convoluted    -- XYZ timed
byte _orOrientationSave = 0;                      //used to hold the orientation during comparison
byte _orOrientationTemp = 0;                      //used to hold the orientation (then convert to _orientation)
boolean orFlag = false;                           //flag 0 x
unsigned long orCounter = 0;                      //TEMP time
const unsigned long _orientationInterval = 100; //200 //100   //main orientation read loop interval in milliseconds   1000
const unsigned long orInterval = 450; //250 //450 //400 //interval at which to check whether flags have changed - are we still in the same orientation - how long to trigger
unsigned long _orientationPrevMillis = 0;         //previous time for reference
byte _orientationTestSideMidpoint = 0;            //side LED strip midpoint, calculated in startup


/*----------------------------LED----------------------------*/
//..see up top.. #define UPDATES_PER_SECOND 120                    //main loop FastLED show delay - 1000/120
//mabye struct should contain int16_t instead of byte. eg. fill_gradient asks for it..
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

//const byte _ledGlobalBrightness = 255;            //global brightness - used to cap - might remove..
/*memory\void setDefaultSettings*/byte _ledGlobalBrightnessCur = 255;               //current global brightness - adjust this
byte _ledBrightnessIncDecAmount = 10;             //the brightness amount to increase or decrease
/*memory\void setDefaultSettings*/byte _headLightsBrightness = 200;                 //use function to set
/*memory\void setDefaultSettings*/byte _rearLightsBrightness = 200;                 //use function to set
/*memory\void setDefaultSettings*/byte _trackLightsFadeAmount = 16;          //64   //division of 256 eg. fadeToBlackBy( _leds, _ledNum, _trackLightsFadeAmount);
int _ledState = LOW;                              //use to toggle LOW/HIGH (ledState = !ledState)
volatile byte _ledMovePos = 0;                    //center point for tracking LEDs to wheels

//CRGB _headLightsCol;                              //colour of the head lights. restricted by 'headLightsBrightness', use functions to set. 
/*memory\void setDefaultSettings*/CHSV _headLightsColHSV( 0, 0, 200);               //default - white @ 200
//CRGB _rearLightsCol;                              //colour of the rear lights. restricted by 'rearLightsBrightness', use functions to set.
/*memory\void setDefaultSettings*/CHSV _rearLightsColHSV( 0, 255, 200);               //default - red @ 200

/*----------------------------MAIN----------------------------*/
void setup() {

  //clearAllSettings();   // TEMP
  
  blinkStatusLED1();
  
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
  setupSerial();                          //..see 'util'
  loadAllSettings();                         //load any saved settings eg. save state when turn board power off. - not fully implemented yet !!!
  setupInterrupts();                      //set any interrupts..
  delay(3000);                            //..after setting interrupts, give the power, LED strip, etc. a couple of secs to stabilise
  setupLEDs();                            //setup LEDs first and then use as setup indicator lights
  _leds[ledSegment[1].first] = CRGB::Yellow;
  _leds[ledSegment[2].first] = CRGB::Yellow;
  delay(400);
  FastLED.show();
  setupSensors();                         //setup all sensor inputs (note: sensors on wheels use interrupt pins)
  delay(400);
  _leds[ledSegment[1].first+2] = CRGB::Fuchsia;
  _leds[ledSegment[2].first+2] = CRGB::Fuchsia;
  FastLED.show();
  setupUserInputs();                      //setup any user inputs - buttons, WIFI, bluetooth etc.
  delay(400);
  _leds[ledSegment[1].first+4] = CRGB::Green;
  _leds[ledSegment[2].first+4] = CRGB::Green;
  FastLED.show();
  _orientationTestSideMidpoint = ledSegment[1].total / 2; //change it later, easier for 2 segments
  checkStartupButtons();  //check to see if any button has been held down during startup eg. full calibration
  //
  #ifdef DEBUG
  //everything done? ok then..
    Serial.print(F("Setup done"));
    Serial.println();
    blinkStatusLED();
  #endif
  delay(400);
  _leds[ledSegment[1].first+6] = CRGB::MediumTurquoise;
  _leds[ledSegment[2].first+6] = CRGB::MediumTurquoise;
  FastLED.show();

  //TEMP for testing. these will get saved as settings later
//  _sleepActive = false;
//  _headLightsEnabled = true;
//  _headLightsActive = true;
//  _rearLightsEnabled = true;
//  _rearLightsActive = true;
//  _indicatorsEnabled = true;
//  _mainLightsSubMode = 3;

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
    loopUserInputs();   //if a board with more interrupts for the buttons, that would cut out this whole loop!
    loopSensors();
    loopModes();
  }
  
  FastLED.show();                           //send all the data to the strips
  #ifdef DEBUG
    //sendMovementDataStream();               //send readings to PC/app
  #endif
  //delay(_mainLoopDelay);
  //FastLED.delay(1000 / UPDATES_PER_SECOND); //handles power management stuff from v3.1.1
  FastLED.delay(UPDATES_PER_SECOND);  //currently '0'
}

