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
 * https://www.arduino.cc/en/Guide/Troubleshooting#upload
 * http://forum.arduino.cc/index.php?topic=140494.0
 * http://bildr.org/2011/04/sensing-orientation-with-the-adxl335-arduino/
 */

/* 
 use if defined statements and comment out to disable debug mode 
 this will remove all debug code when compiling rather than just switching off
 ..for now only use serial when in debug. this will be changed when wireless communication is implemented.
*/
#define DEBUG 1                           //comment/un-comment
#define DEBUG_MPU6050 1                   //..
//#ifdef DEBUG     
////
//#endif
const int _buttonTotal = 1;               //how many butons are there? - cannot set Bounce using this unfortunately!
const int _wheelSensorTotal = 1;          //how many wheels have we mounted sensors on?
const int _wheelMagnetTotal = 4;          //how many magnets are mounted on each wheel?

/*----------------------------arduino pins----------------------------*/

/* 
 * Arduino Prom Mini pins:
 * 
 * serial 0,1
 * interrupts 2,3
 * PWM 3,5,6,9,10,11
 * SPI 10(SS),11(MOSI),13(SCK)
 * I2C A4(SDA),A5(SCL) 
 */
 
//ArduinoMiniPro A4 -> GY-521 (MPU6050) SDA
//ArduinoMiniPro A5 -> GY-521 (MPU6050) SCL
//GY-521 (MPU6050) INT (interrupt) pin optional
 
const int _wheelSensorPin[_wheelSensorTotal] = { 2 };     //array of wheel sensor inputs (!!all interrupt pins!!!) - uses _wheelSensorTotal
//const int _mpu6050InterruptPin = 2;                       //??? - don't think will need interrupt stuff, even with wireless. use it for wheels
//DOut -> LED strip DIn (0 = rear break lights, 1 = left strip + front head lights, 2 = right strip)
//FastLED doesn't like an array being used for the pins eg. _ledDOutPin[0]
const int _ledDOutPin0 = 5; //6
const int _ledDOutPin1 = 6; //7
const int _ledDOutPin2 = 9; //8
const int _buttonPin[_buttonTotal] = { 10 };  //array of user input buttons - uses _buttonTotal
const int _ledPin = 13;                         //built-in LED


/*----------------------------libraries----------------------------*/
#include <EEPROM.h>                       //Arduino Pro Mini ATmega328 5V 16MHz - 1kBs EEPROM
#include <Bounce2.h>                      //buttons with de-bounce
#include <FastLED.h>                      //WS2812B LED strip control and effects
#include <I2Cdev.h>                       //I2C devices
#include <MPU6050.h>                      //MPU6050 6-axis motion sensor
#include <Wire.h>

/*----------------------------system----------------------------*/
const String _progName = "longboardLight1_A";
const String _progVers = "0.1";
//const int _mainLoopDelay = 0;               //just in case  - using FastLED.delay instead..
boolean _firstTimeSetupDone = false;        //starts false
#ifdef DEBUG
String _inputString = "";                   // a string to hold incoming data
boolean _stringComplete = false;             // whether the string is complete
#endif

/*----------------------------modes----------------------------*/
boolean _sleepActive = false;                       //init false at power-up
boolean _breathingActive = true;                      //the board 'breathes' (glows gently) at 12 times a minute (average breathing rate of sleeping adult)
boolean _headLightsActive = false;
boolean _rearLightsActive = false;
//main lights on/off is controlled using the blank sub-mode
int _mainLightsSubMode = 0;                   //sub-mode for main lights loop: 0=none/blank, 1= , 2= , 3=
boolean _brakeActive = false;                 //give the brake lights a slight brightness boost when decelerating
boolean _indicatorsEnabled = false;            //indicators for turning left/right
boolean _indicatorLeftActive = false;
boolean _indicatorRightActive = false;

/*----------------------------buttons----------------------------*/
const unsigned long debounceTime = 5;           //5 milli-seconds debounce time
/* !!! remember to change bounce number to match '_buttonTotal' !!! */
Bounce button[1] = {
  Bounce(_buttonPin[0], debounceTime),
  //Bounce(_wheelSensorPin[1], debounceTime),
  //Bounce(_wheelSensorPin[2], debounceTime),
  //Bounce(_wheelSensorPin[3], debounceTime),
  };
//boolean _buttonToggled[_butttonTotal] = { false };        //array of button toggle states

/*----------------------------sensors----------------------------*/
//hall effect sensor mounted on chassis, with 4 magents mounted on wheel
//3-axis accelerometer

 //eg. 'place board on flat ground and press calibrate'  ..will get saved here - '_orientationCalibrationXYZ'
int _orientationCalibrationXYZ[6][3] = {
  { 400, 398, 488 },
  { 408, 404, 328 },
  { 410, 490, 410 },
  { 420, 330, 420 },
  { 450, 372, 375 },
  { 290, 365, 370 }
  };
//then the following is filled-in by the computer for XYZ
int _orientationCalibrationXYZminMaxDist[3][3] = {
  { 0, 0, 0 },  //eg. 302, 462, 40
  { 0, 0, 0 },
  { 0, 0, 0 }
};
int _orientationXYZdistTriggerDivide = 3; //xt = (xm-xmi)/_orientationXYZdistTriggerDivide;
//
int _orientation = 0;                           //0=flat, 1=upside-down, 2=up, 3=down, 4=left-side, 5=right-side


/*----------------------------sensors - MPU6050 6-axis----------------------------*/
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  // Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation is used in I2Cdev.h - ???
  #include "Wire.h"
#endif
MPU6050 _mpu6050;  //accel gyro;
//int16_t _mpu6050AccelCurX, _mpu6050AccelCurY, _mpu6050AccelCurZ;
//int16_t _mpu6050GyroCurX, _mpu6050GyroCurY, _mpu6050GyroCurZ;
//int _mpu6050AccelAverageX, _mpu6050AccelAverageY, _mpu6050AccelAverageZ, _mpu6050GyroAverageX, _mpu6050GyroAverageY, _mpu6050GyroAverageZ;
int _mpu6050AccelOffsetX, _mpu6050AccelOffsetY, _mpu6050AccelOffsetZ, _mpu6050GyroOffsetX, _mpu6050GyroOffsetY, _mpu6050GyroOffsetZ;
const int _mpu6050CalibrateSampleTotal = 100;     //how many samples to take at once when calibrating
const int _mpu6050CalibrateAccelThreshold = 10;  //threshold tolerance for 'dead zone' at center of readings
const int _mpu6050CalibrateGyroThreshold = 3;     //..for gyro
long _mpu6050CalibratePrevMillis = 0;             //previous time for reference
const long _mpu6050CalibrateInterval = 1000;      //sampling interval in milliseconds
const long _mpu6050CalibrateTimeout = 30000;      //sampling interval in milliseconds
boolean _doCalibrateMPU6050 = false;              //set to true to run MPU6050 calibration. it will reset itself to false when finished.

//stuff for filtering
const unsigned long _mpu6050ReadInterval = 40;                                        //read loop interval in milliseconds   1000
unsigned long _mpu6050ReadPrevMillis = 0;                                            //previous time for reference
int16_t _mpu6050AccelReadX, _mpu6050AccelReadY, _mpu6050AccelReadZ;           //the current raw accel reading
int16_t _mpu6050GyroReadX, _mpu6050GyroReadY, _mpu6050GyroReadZ;           //the current raw gyro reading
float _mpu6050AccelZeroX, _mpu6050AccelZeroY, _mpu6050AccelZeroZ;
float _mpu6050GyroZeroX, _mpu6050GyroZeroY, _mpu6050GyroZeroZ;              //calibrating zero average for gyro
float _mpu6050FilteredCurX, _mpu6050FilteredCurY, _mpu6050FilteredCurZ;     //final filtered combined gyro readings for then calculating orientation
float _mpu6050FilteredPrevX, _mpu6050FilteredPrevY, _mpu6050FilteredPrevZ;  //previous final filtered combined yadayada.. = last_x_angle
float _mpu6050GyroPrevX, _mpu6050GyroPrevY, _mpu6050GyroPrevZ;              //last_gyro_x_angle; 


/*----------------------------orientation----------------------------*/
int orMatrix[9] = { 0, 0, 0 }; //TEMP x =  0(low) / 1(mid) / 2(hi)       - wanted to use -1, 0, 1 but too convoluted    -- XYZ timed
int orMatrixPrev[3] = { 0, 0, 0 };
boolean orFlag[9] = { false }; //flag 0 x
unsigned long orCounter[9] = { 0 };  //TEMP time
const unsigned long orInterval = 50;  //interval at which to check whether flags have changed
const unsigned long _orientationInterval = 100;                                        //read loop interval in milliseconds   1000
unsigned long _orientationPrevMillis = 0;                                            //previous time for reference

/*----------------------------LED----------------------------*/
#define UPDATES_PER_SECOND 120                  //main loop FastLED show delay  //100
typedef struct {
  byte first;
  byte last;
  byte total;
} LED_SEGMENT;
const int _ledNum = 40;                         //18 LED strip each side and 2 each end = 40 LEDs (2280mA max)
const int _segmentTotal = 4;                    //2 sides, 2 ends
const int _ledGlobalBrightness = 255;           //global brightness
int _ledGlobalBrightnessCur = 255;              //current global brightness - adjust this
int _ledBrightnessIncDecAmount = 10;            //the brightness amount to increase or decrease
int _headLightsBrightness = 200;
int _rearLightsBrightness = 200;
LED_SEGMENT ledSegment[_segmentTotal] = { 
  { 0, 1, 2 },      //front head lights
  { 2, 19, 18 },    //left side
  { 20, 37, 18 },   //right side
  { 38, 39, 2 },     //rear brake lights
  //can only utilise the following if can get loop to wrap-around using _ledNum
  //if use remember to change _segmentTotal
  //{ 1, 38, 20 },     //left half (split down centre of board)
  //{ 39, 0, 20 },     //right half
  //{ 1, 0, 40 }      //loop round board from back to front
};
CRGB _leds[_ledNum];                         //global RGB array
int _ledState = LOW;                        //use to toggle LOW/HIGH (ledState = !ledState)


/*----------------------------MAIN----------------------------*/
void setup() {
  blinkStatusLED1();
  
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
  setupSerial();                          //..see 'util'
  //loadSettings();                         //load any saved settings eg. save state when turn board power off. - not implemented yet !!!
  setupSensors();                         //setup all sensor inputs (note: sensors on wheels use interrupt pins)
  delay(3000);                            //..after setting interrupts, give the power, LED strip, etc. a couple of secs to stabilise
  setupLEDs();
  setupUserInputs();                      //setup any user inputs - buttons, WIFI, bluetooth etc.
  //
  #ifdef DEBUG
  //everything done? ok then..
    Serial.print(F("Setup done"));
    Serial.println();
  #endif
  blinkStatusLED();

  //TEMP for testing
  _headLightsActive = true;
  _rearLightsActive = true;
  _indicatorsEnabled = true;
  _mainLightsSubMode = 1;
}

void loop() {
  if(_firstTimeSetupDone == false) {
    #ifdef DEBUG
    //
    #endif
    _firstTimeSetupDone = true;
  }
  //
  loopUserInputs();
  loopSensors();
  loopModes();
  //
  FastLED.show();                           //send all the data to the strips
  FastLED.delay(1000 / UPDATES_PER_SECOND);
  //
  //delay(_mainLoopDelay);                  //using FastLED.delay instead..
}

