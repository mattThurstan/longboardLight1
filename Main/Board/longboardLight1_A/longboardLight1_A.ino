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

    https://github.com/mattKsp/
*/

/*
 * Reference:
 * 
 * https://www.arduino.cc/en/Main/ArduinoBoardProMini
 * https://www.arduino.cc/en/Guide/Troubleshooting#upload
 * https://github.com/FastLED/FastLED/wiki/Pixel-reference
 * http://forum.arduino.cc/index.php?topic=140494.0
 * http://bildr.org/2011/04/sensing-orientation-with-the-adxl335-arduino/
 * http://42bots.com/tutorials/arduino-script-for-mpu-6050-auto-calibration/
 * https://www.i2cdevlib.com/devices/mpu6050#source
 * https://www.i2cdevlib.com/docs/html/class_m_p_u6050.html
 * http://www.geekmomprojects.com/gyroscopes-and-accelerometers-on-a-chip/
 * http://engineering.stackexchange.com/questions/3348/calculating-pitch-yaw-and-roll-from-mag-acc-and-gyro-data
 * http://www.nxp.com/assets/documents/data/en/application-notes/AN3461.pdf
 * http://theboredengineers.com/2012/09/the-quadcopter-get-its-orientation-from-sensors/
 * https://forum.arduino.cc/index.php?topic=383064.0
 * 
 * byte   = unsigned 8-bit value, 0-255
 * uint8_t   = same as byte
 * 
 * short   = 16-bit (2-byte) value, -32,768 to 32,767
 * int   = 16-bit (2-byte) value, -32,768 to 32,767
 * int16_t   = 16-bit (2-byte) value, -32,768 to 32,767
 * unsigned int  = 16-bit (2-byte) value, 0 to 65,535
 * uint16_t  = 16-bit (2-byte) value, 0 to 65,535
 * 
 * long    = 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647
 *     eg. long speedOfLight = 186000L;   //must declare the 'L'
 * unsigned long = 32 bits (4 bytes), from 0 to 4,294,967,295
 * 
 */

/* 
 use if defined statements and comment out to disable debug mode 
 this will remove all debug code when compiling rather than just switching off
 ..for now only use serial when in debug. this will be changed when wireless communication is implemented.
*/
//#define DEBUG 1                                   //comment/un-comment
//#define DEBUG_MPU6050 1                           //..
//#define DEBUG_ORIENTATION 1                       //
//#define DEBUG_WHEEL 1                             //DEUBUG wheel sensor(s)
//#define DEBUG_INTERRUPT 1

//3-axis accelerometer  calibration (these will be moved and integrated later when have communications)
boolean _doFullCalibration = false;               //set to true to run full calibration. it will reset itself to false when finished.
//a quick calibration will normally be done, unless it gets turned off at the end of a full calibration.
boolean _doQuickCalibration = false;              //set to true to run quick calibration. it will reset itself to false when finished.
boolean _orientationTest = false;                 //used as a test override. not during normal operation. will prob remove later
/*
 * black marked side of magents are North)
 * my wheels 1 - shark wheels 70mm diameter / 78A (about 69mm dues to wear etc.)
 * - 2 * 3.14 (PI) * half diameter (radius) = 219.8
 * - 2 * 3.14 * 34.5 = 216.66
 * LED spacing on strips - 33.3mm (3 LEDs in 100mm)
 * circumference = 2 * PI * radius
*/
//#ifdef DEBUG     
////
//#endif
boolean _batteryPowered = true;                   //are we running on battery or plugged into the computer?
const byte _buttonTotal = 2;                      //how many butons are there? - cannot set Bounce using this unfortunately!
const float _wheelRadius = 0.0345;                //half of diameter, my wheels are (worn to) 69mm diameter (70mm from the factory, give or take a bit) 
const byte _wheelSensorTotal = 1;                 //how many wheels have we mounted sensors on?
const byte _wheelMagnetTotal = 8; //4;            //how many magnets are mounted on each wheel?


/*----------------------------arduino pins----------------------------*/

/* 
 * Arduino Prom Mini pins:
 * 
 * serial 0,1+
 * interrupts 2,3
 * PWM 3,5,6,9,10,11
 * SPI 10(SS),11(MOSI),13(SCK)
 * I2C A4(SDA),A5(SCL) 
 */
 
//ArduinoMiniPro A4 -> GY-521 (MPU6050) SDA
//ArduinoMiniPro A5 -> GY-521 (MPU6050) SCL
//GY-521 (MPU6050) INT (interrupt) pin optional - needs interrupt pin to check its fancy onboard calculations such as no-motion status
 
const byte _wheelSensorPin[_wheelSensorTotal] = { 2 };     //array of wheel sensor inputs (!!all interrupt pins!!!) - uses _wheelSensorTotal
//const int _mpu6050InterruptPin = 2;   //??? - don't think will need MPU6050 interrupt stuff, even with wireless. ..just use interrupts for wheels
//DOut -> LED strip DIn (0 = rear break lights, 1 = left strip + front head lights, 2 = right strip)
//FastLED doesn't like an array being used for the pins eg. _ledDOutPin[0]  ..am i addressing it correctly?
const byte _ledDOutPin0 = 5;                      //head lights
const byte _ledDOutPin1 = 6;                      //left
const byte _ledDOutPin2 = 9;                      //right and rear lights
const byte _buttonPin[_buttonTotal] = { 10, 11 }; //array of user input buttons - uses _buttonTotal
const byte _ledPin = 13;                          //built-in LED


/*----------------------------libraries----------------------------*/
#include <EEPROM.h>                               //Arduino Pro Mini ATmega328 5V 16MHz - 1kBs EEPROM
#include <Bounce2.h>                              //buttons with de-bounce
#include <FastLED.h>                              //WS2812B LED strip control and effects
#include <I2Cdev.h>                               //I2C devices
#include <MPU6050.h>                              //MPU6050 6-axis motion sensor
#include <Wire.h>

/*----------------------------system----------------------------*/
const String _progName = "longboardLight1_A";
const String _progVers = "0.292";                 //testing and adjusting timings
//const int _mainLoopDelay = 0;                   //just in case  - using FastLED.delay instead..
boolean _firstTimeSetupDone = false;              //starts false
#ifdef DEBUG
String _inputString = "";                         // a string to hold incoming data
boolean _stringComplete = false;                  // whether the string is complete
#endif
const unsigned long _sendMovementDataStreamInterval = 1000;//100   //send loop interval in milliseconds   1000
unsigned long _sendMovementDataStreamPrevMillis = 0;         //previous time for reference

/*----------------------------modes----------------------------*/
/*
 * ..the following example line is wrong, and will then be explained..
 * 'the user could put the board to sleep, then turn off, then expect the same when turned on later'
 * 
 * 'the user puts the board to sleep' - all good
 * 'then turns off' - still good
 * 'then expect the same when turned on later' - wrong, cos handling the board to turn it on would result in waking it from sleep
 * ..therefore _sleep is 'Active', NOT 'Enabled' and not saved to memory
 */
boolean _sleepActive = false;                     //init false at power-up
boolean _breathingEnabled = true;                 //the board 'breathes' (glows gently) at 12 times a minute (average breathing rate of sleeping adult)
boolean _headLightsEnabled = false;               //have we switched on the headlights?
boolean _headLightsActive = true;                 //start true
boolean _rearLightsEnabled = false;               //have we switched on the rearlights?
boolean _rearLightsActive = true;                 //start true
boolean _brakeActive = false;                     //give the brake lights a slight brightness boost when decelerating
boolean _indicatorsEnabled = false;               //indicators for turning left/right
boolean _indicatorLeftActive = false;
boolean _indicatorRightActive = false;
//main lights on/off is controlled using the blank sub-mode
const byte _mainLightsSubModeTotal = 9;           //9 (0-8)       //4 (0-3)     //never gonna have more than 256 lighting modes..
byte _mainLightsSubMode = 3;                      //sub-mode for main lights loop: 0=none/blank, 1= , 2= , 3=

/*----------------------------buttons----------------------------*/
const unsigned long _buttonDebounceTime = 5;//5;  //5 milli-seconds debounce time
Bounce *_button = new Bounce[_buttonTotal];       //pointer to new array of N buttons
const unsigned long _loopButtonsInterval = 500;   //read loop interval in milliseconds   500= press button, count '1 (mabye 2',  then let go.
unsigned long _loopButtonsPrevMillis = 0;         //previous time for reference
boolean _buttonToggled[_buttonTotal] = { false }; //array of button toggle states

/*----------------------------sensors----------------------------*/
//latching bipolar hall effect sensor mounted on chassis, with 4/8 magents mounted on wheel
volatile byte _wheelCounter = 0;                  //byte may not be large enough (0-255), might have to use an int - starts at 0, uses 1-4
//'_wheelSensorReadInterval' needs to be hardcoded to 1 sec, otherwise we don't get 'revolutions per SECOND'
//const unsigned long _wheelSensorReadInterval = 1000;      //read loop interval in milliseconds   1000
unsigned long _wheelSensorReadPrevMillis = 0;            //previous time for reference
double _wheelSpeedRps = 0;                        //wheel revolutions per second
double _wheelSpeedMps = 0;                        //wheel speed in Meters Per Second
unsigned long _distTraveledForward = 0;           //total distance traveled forward in meters
unsigned long _distTraveledBackward = 0;          //total distance traveled backward in meters
//_distanceTraveledLeft                           //gotta get these in later..
//_distanceTraveledRight
//_distanceTraveledUp
//_distanceTraveledDown
//_distanceTraveledOtherWays
//int _wheelRadius = 0.035;                         //radius (dist from center to the edge) of wheels in use (meters ..cos) eg. 70mm diameter
//const float _wheelDiameter = 0.07;
//const float _wheelDiameter = 0.069;               //my wheels are worn to about 69mm
//..moved up top.. const float _wheelRadius = 0.0345;                          //half of diameter
//const int _wheelMagnetRadius = 0.01;                        //radius of the magnet positioning (millimeters). prob about 10mm
//const int _wheelCircumference = 2 * PI * _wheelRadius;      //circumference = 2 * PI * radius (meters)
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
//int _diDirectionSave = 0;                         //used to hold the direction during comparison - NOT USED NOW
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
const unsigned long orInterval = 400; //250 //450 //400 //interval at which to check whether flags have changed - are we still in the same orientation - how long to trigger
unsigned long _orientationPrevMillis = 0;         //previous time for reference
byte _orientationTestSideMidpoint = 0;            //side LED strip midpoint, calculated in startup

/*----------------------------LED----------------------------*/
#define UPDATES_PER_SECOND 120                    //main loop FastLED show delay  //100
typedef struct {
  byte first;
  byte last;
  byte total;                                     //haven't got more than 256 LEDs in a segment
} LED_SEGMENT;
const byte _ledNum = 40;                          //18 LED strip each side and 2 each end = 40 LEDs (2280mA max)
const byte _segmentTotal = 4;                     //2 sides, 2 ends
//const byte _ledGlobalBrightness = 255;            //global brightness - used to cap - might remove..
byte _ledGlobalBrightnessCur = 255;               //current global brightness - adjust this
byte _ledBrightnessIncDecAmount = 10;             //the brightness amount to increase or decrease
byte _headLightsBrightness = 200;
byte _rearLightsBrightness = 200;
byte _trackLightsFadeAmount = 64;                 //division of 256 eg. fadeToBlackBy( _leds, _ledNum, _trackLightsFadeAmount);
LED_SEGMENT ledSegment[_segmentTotal] = { 
  { 0, 1, 2 },      //front head lights
  { 2, 19, 18 },    //left side
  { 20, 37, 18 },   //right side
  { 38, 39, 2 },     //rear brake lights
};
//CRGB _leds[_ledNum];                              //global RGB array
//CRGBSet _ledsSideSet(_leds, 36);                  //set made from segements 1 and 2 combined
CRGBArray<_ledNum> _leds;                         //CRGBArray means can do multiple '_leds(0, 2).fadeToBlackBy(40);' as well as single '_leds[0].fadeToBlackBy(40);'
int _ledState = LOW;                              //use to toggle LOW/HIGH (ledState = !ledState)
volatile byte _ledMovePos = 0;                    //center point for tracking LEDs to wheels


/*----------------------------MAIN----------------------------*/
void setup() {

  clearAllSettings();   // TEMP
  
  blinkStatusLED1();
  
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
  setupSerial();                          //..see 'util'
  loadAllSettings();                         //load any saved settings eg. save state when turn board power off. - not implemented yet !!!
  setupInterrupts();                      //set any interrupts..
  delay(3000);                            //..after setting interrupts, give the power, LED strip, etc. a couple of secs to stabilise
  setupLEDs();                            //setup LEDs first and then use as setup indicator lights
  fill_gradient_RGB(_leds, ledSegment[1].first, CRGB::Yellow, ledSegment[1].first+1, CRGB::Yellow);
  fill_gradient_RGB(_leds, ledSegment[2].first, CRGB::Yellow, ledSegment[2].first+1, CRGB::Yellow);
  //_leds(ledSegment[1].first, 2) = CRGB::Yellow;
  delay(2);
  FastLED.show();
  setupSensors();                         //setup all sensor inputs (note: sensors on wheels use interrupt pins)
  delay(2);
  fill_gradient_RGB(_leds, ledSegment[1].first+2, CRGB::Fuchsia, ledSegment[1].first+3, CRGB::Fuchsia);
  fill_gradient_RGB(_leds, ledSegment[2].first+2, CRGB::Fuchsia, ledSegment[2].first+3, CRGB::Fuchsia);
  //_leds(ledSegment[1].first+3, 2) = CRGB::Fuchsia;
  FastLED.show();
  setupUserInputs();                      //setup any user inputs - buttons, WIFI, bluetooth etc.
  delay(2);
  fill_gradient_RGB(_leds, ledSegment[1].first+4, CRGB::Green, ledSegment[1].first+5, CRGB::Green);
  fill_gradient_RGB(_leds, ledSegment[2].first+4, CRGB::Green, ledSegment[2].first+5, CRGB::Green);
  //_leds(ledSegment[1].first+6, 2) = CRGB::Green;
  FastLED.show();
  //
  #ifdef DEBUG
  //everything done? ok then..
    Serial.print(F("Setup done"));
    Serial.println();
    blinkStatusLED();
  #endif
  delay(2);
  fill_gradient_RGB(_leds, ledSegment[1].first+6, CRGB::MediumTurquoise, ledSegment[1].first+7, CRGB::MediumTurquoise);
  fill_gradient_RGB(_leds, ledSegment[2].first+6, CRGB::MediumTurquoise, ledSegment[2].first+7, CRGB::MediumTurquoise);
  //_leds(ledSegment[1].first+9, 2) = CRGB::MediumTurquoise;
  FastLED.show();

  //TEMP for testing. these will get saved as settings later
//  _sleepActive = false;
//  _headLightsEnabled = true;
//  _headLightsActive = true;
//  _rearLightsEnabled = true;
//  _rearLightsActive = true;
//  _indicatorsEnabled = true;
//  _mainLightsSubMode = 3;

  //_orientationTestSideMidpoint = ledSegment[1].first + (ledSegment[1].total/2);
  _orientationTestSideMidpoint = ledSegment[1].total / 2; //add it later, easier for 2 segments
  checkStartupButtons();  //check to see if any button has been held down during startup eg. full calibration

  delay(2);
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
  else if (_doQuickCalibration == true) { quickCalibration(); }   //..if not, try for a quick one
  else {
    //run the loop normally
    loopUserInputs();
    loopSensors();
    loopModes();
  }
  
  FastLED.show();                           //send all the data to the strips
  #ifdef DEBUG
    sendMovementDataStream();               //send readings to PC/app
  #endif
  FastLED.delay(1000 / UPDATES_PER_SECOND);
  //delay(_mainLoopDelay);
}

