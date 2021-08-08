/*
    'longboardLight1_Mesh' by Thurstan. LED longboard lights with motion tracking.

    Copyright (C) 2021 MTS Standish (mattThurstan)

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

    Implements fields array selection method by Jason Coon, from his ESP8266 + FastLED webserver (Copyright (C) 2015-2016 GNU GPL-3.0)
    https://github.com/jasoncoon/esp8266-fastled-webserver
*/


/*----------------------------libraries----------------------------*/
#include <MT_LightControlDefines.h>               // my network defines
#include <EEPROM.h>                               // ESP8266 style EEPROM (using 512 bytes) - WeMos D1 (R2 &) mini, 80 MHz, 115200 baud, 4M, (3M SPIFFS)
#include <FastLED.h>                              // WS2812B LED strip control and effects
#include "painlessMesh.h"                         // painless mesh network
#include <Bounce2.h>                              // buttons with de-bounce
#include <MT_BoardWheel.h>                        // (single version) attempt to move all board wheel related items to a single library
#include <MT_BoardOrientation.h>                  // attempt to move all board mpu6050 sensor related items to a single library

/*----------------------------system----------------------------*/
const String _progName = "longboardLight1_Mesh";
const String _progVers = "0.411";                 // moving toward combined mesh/standalone implementation
const uint8_t _batteryPowered = 1; //take away const if power charge sensing ever gets implemented  //are we running on battery or plugged into the computer?
//ADC_MODE(ADC_VCC);                                // think this is need to be able to use ESP.getVcc() later.. ??? hmm.. problems
const int _mainLoopDelay = 0;                     // just in case eg. 8 ~= 120 fps (1000/120=8.333)
boolean _firstTimeSetupDone = false;              // starts false
bool runonce = true;                              // flag for sending states when first mesh conection
    
boolean DEBUG_GEN = false;                        // realtime serial debugging output - general
boolean DEBUG_SENSORS = false;                    // realtime serial debugging output - sensors
boolean DEBUG_INTERRUPT = false;                  // realtime serial debugging output - interrupts
boolean DEBUG_COMMS = false;                      // realtime serial debugging output - comms

boolean DATA_LOGGING = true;                      // turn data logging on or off eg. rps/mps, dist travelled, etc. ( Note: tracking led mode ticks up _ledsMovePos - NOT this )

uint8_t _testMode = 0;                            // used as an override to test all the modes
byte _curTestMode = 0;                            // the current mode being tested in isolation - sent from device

//a quick calibration could normally be done at the start, unless it gets turned off at the end of a full calibration. ???
uint8_t _doQuickCalibration = 0;                  // set to true to run quick calibration. it will reset itself to false when finished.
//3-axis accelerometer  calibration (these will be moved and integrated later when have communications)
uint8_t _doFullCalibration = 0;                   // set to true to run full calibration. it will reset itself to false when finished.

//to do this justice i would really have to implement a way to swap between (or even enter info) the different led arrays aswell.. starts getting needlessly complicated. 
byte _curBoardProfile = 1;                        // the current board profile - will be set from device - not fully implemented yet  (0="Blank"(set for default testing), 1="Dervish Sama", 2="Drop-down")

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

/*----------------------------arduino pins----------------------------*/
//MPU6050 on I2C. Power with 5v, data connect to board with either 3.3v or 5v.
//I2C SDA = A4 (Pro Mini) or  4(D2) (D1 Mini)
//I2C SCL = A5 (Pro Mini) or  5(D1) (D1 Mini) 

const byte _wheelSensorPin = 0;                   // (0 = D3) wheel sensor input (!!interrupt pin!!!) Using built-in pullup
const byte _ledDOutPin0 = 14;                     // all lights (14 = D5)
const byte _buttonTotal = 1;                      // total buttons in use
const byte _buttonPin[_buttonTotal] = { 2 };      // (2 = D4) array of user input buttons

/*----------------------------sensors - wheels----------------------------*/
//latching bipolar hall effect sensor mounted on chassis, with 8 magents mounted on wheel
MT_BoardWheel w;                                  // declaration

/*----------------------------sensors - MPU6050 6-axis----------------------------*/
/* MPU6050:             X=Right/Left, Y=Forward/Backward, Z=Up/Down
 * orientation (byte):  0=flat, 1=upside-down, 2=up, 3=down, 4=left-side, 5=right-side
 * direction (byte):    -1=stationary, 0=forward, 1=back, 2=up, 3=down, 4=left, 5=right
 */
const unsigned long _mpu6050ReadInterval = 20;    // read loop interval in milliseconds
const unsigned long _orientationInterval = 100;   // main orientation read loop interval in milliseconds
byte _orientationTestSideMidpoint = 0;            // side LED strip midpoint, calculated in startup
MT_BoardOrientation o;                            // declaration - sets defaults

/*----------------------------comms----------------------------*/
//#define HOSTNAME "LlC_"                           //
boolean _wifiActive = true;                       // TEMP true - change back to false when buttons implemented                      

/*----------------------------buttons----------------------------*/
const unsigned long _buttonDebounceTime = 5;      // 5 milli-seconds debounce time
Bounce *_button = new Bounce[_buttonTotal];       // pointer to new array of N buttons
const unsigned long _loopButtonsInterval = 250;   // read loop interval in milliseconds. press button, count '1 (mabye 2)',  then let go.
//unsigned long _loopButtonsPrevMillis = 0;         // previous time for reference
boolean _buttonToggled[_buttonTotal] = { false }; // array of button toggle states

/*----------------------------LED----------------------------*/
//mabye struct should contain int16_t instead of byte. eg. fill_gradient asks for it..
typedef struct {
  byte first;
  byte last;
  byte total;                                     // byte ok as haven't got more than 256 LEDs in a segment
} LED_SEGMENT;
const byte _ledNum = 64;                          // 18 LED strip each side and 2 each end = 40 LEDs (2280mA max)
const byte _segmentTotal = 4;                     // 2 sides, 2 ends
//LED_SEGMENT ledSegment[_segmentTotal] = {         /* drop-board - 1 loop */
//  { 0, 1, 2 },      //rear brake lights
//  { 2, 28, 24 },    //left side
//  { 31, 51, 24 },   //right side - gets reversed when creating CRGBset
//  { 29, 30, 2 },    //front head lights
//};
LED_SEGMENT ledSegment[_segmentTotal] = {         /* Loaded board - 1 loop */
  { 62, 63, 2 },    //rear brake lights
  { 0, 29, 30 },    //left side
  { 32, 61, 30 },   //right side - gets reversed when creating CRGBset
  { 30, 31, 2 },    //front head lights
};

const uint16_t _1totalDiv = (ledSegment[1].total / 4); //used in 'mode/void breathRiseFall'
const uint16_t _2totalDiv = (ledSegment[2].total / 4); //used in 'mode/void breathRiseFall'

CRGBArray<_ledNum> _leds;                         // master array - CRGBArray means can do multiple '_leds(0, 2).fadeToBlackBy(40);' as well as single '_leds[0].fadeToBlackBy(40);'

CRGBSet _ledsRear( _leds(ledSegment[0].first, ledSegment[0].last) );  //first, last
CRGBSet _ledsLeft( _leds(ledSegment[1].first, ledSegment[1].last) );
CRGBSet _ledsRight( _leds(ledSegment[2].last, ledSegment[2].first) ); //reversed - last, first - this will let us access _ledsRight the correct way
CRGBSet _ledsFront( _leds(ledSegment[3].first, ledSegment[3].last) );

/*memory\void setDefaultSettings*/byte _ledGlobalBrightnessCur = 255;               //current global brightness
/*memory\void setDefaultSettings*///byte _headLightsBrightness = 255;                 // use function to set - use '_headLightsColHSV.val' instead
/*memory\void setDefaultSettings*///byte _rearLightsBrightness = 255;                 // use function to set - etc.
/*memory\void setDefaultSettings*/byte _trackLightsFadeAmount = 16;          //64   // division of 256 eg. fadeToBlackBy( _leds, _ledNum, _trackLightsFadeAmount);
volatile byte _ledMovePos = 0;                    // center point for tracking LEDs to wheels

/*memory\void setDefaultSettings*/CHSV _headLightsColHSV( 0, 0, 255);               // fixed - white @ 255
/*memory\void setDefaultSettings*/CHSV _rearLightsColHSV( 0, 255, 255);             // fixed - red @ 255

CRGB solidColor = CRGB::White;                    // TEMP
CRGB solidColor2 = CRGB::Red;                     // TEMP
uint8_t gHue = 0;                                 // rotating "base color" used by many of the patterns
const unsigned long _emergencyFlashInterval = 1000; // 15000=15 sec. 30000=30 sec. 150000=2.5 mins.
boolean _emergencyFlashFlip = false;

/*----------------------------modes----------------------------*/
/* In standby the board 'breathes' (glows gently) at 12 times a minute (average breathing rate of sleeping adult).
 * Main lights on/off is controlled using the blank sub-mode.
 */
typedef struct {
  uint8_t sleep;    //doesn't use Enabled
  uint8_t breathe;  //doesn't use Active
  uint8_t head;
  uint8_t rear;
  uint8_t brake;    //give the brake lights a slight brightness boost when decelerating
  uint8_t indicate;
} MODES_ENABLED_ACTIVE;

MODES_ENABLED_ACTIVE mE = { 1, 1, 1, 1, 1, 0 };   // Enabled
MODES_ENABLED_ACTIVE mA = { 0, 0, 1, 1, 0, 0 };   // Active

bool _isBreathingSynced = false;                  // breath sync local or global

//const byte _mainLightsSubModeTotal = 4;             //never gonna have more than 256 lighting modes..   -- MOVED - see bottom of subMode.h --
/*memory\void setDefaultSettings*/uint8_t _mainLightsSubMode = 3;  //byte //sub-mode for main lights loop: 0=none/blank, 1= , 2= , 3= is always track (basic) cos of sensors loopOrientation()
#include "subMode.h"

/*----------------------------Mesh----------------------------*/
painlessMesh  mesh;                               // initialise
const uint32_t id = DEVICE_ID_BRIDGE1;

void receivedCallback(uint32_t from, String &msg ) {
  if (DEBUG_GEN) { Serial.printf("longboardLight1_Mesh: Received from %u msg=%s\n", from, msg.c_str()); }
  receiveMessage(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  if (runonce == true) {
    publishStatusAll(false);
    runonce = false;
  }
  if (DEBUG_GEN) { Serial.printf("longboardLight1_Mesh: New Connection, nodeId = %u\n", nodeId); }
}

void changedConnectionCallback() {
  if (DEBUG_GEN) { Serial.printf("longboardLight1_Mesh: Changed connections %s\n",mesh.subConnectionJson().c_str()); }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  if (DEBUG_GEN) { Serial.printf("longboardLight1_Mesh: Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset); }
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  if (DEBUG_GEN) { Serial.printf("longboardLight1_Mesh: Delay to node %u is %d us\n", from, delay); }
}

/*----------------------------util - comms - webserver send----------------------------*/

/*----------------------------util - comms - web sockets----------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------MAIN--------------------------------------------------------*/
void setup() {
//clearAllSettings();   // TEMP
  
  Serial.begin(115200);
  
  Serial.println();
  Serial.print( F("Welcome to Gardening For Nomes part 4!") ); Serial.println();
  Serial.print( F("Your host for todays proceedings is.. ") ); Serial.println(_progName);
  Serial.print( F("Program Version: ") ); Serial.println(_progVers);
  Serial.print( F("Heap: ") ); Serial.println(system_get_free_heap_size());
  Serial.print( F("Boot Vers: ") ); Serial.println(system_get_boot_version());
  Serial.print( F("CPU: ") ); Serial.println(system_get_cpu_freq());
  Serial.print( F("SDK: ") ); Serial.println(system_get_sdk_version());
  Serial.print( F("Chip ID: ") ); Serial.println(system_get_chip_id());
  Serial.print( F("Flash ID: ") ); Serial.println(spi_flash_get_id());
  Serial.print( F("Flash Size: ") ); Serial.println(ESP.getFlashChipRealSize());
  Serial.print( F("Vcc: ") ); Serial.println(ESP.getVcc());
  Serial.println();
  
  EEPROM.begin(512);                              // tell it we are using an EEPROM area of 512 bytes
  loadAllSettings();                              // load any saved settings eg. save state when turn board power off. - not fully implemented yet !!!
    flashLED(1);
  setupInterrupts();                              // set any interrupts..
  delay(3000);                                    // ..after setting interrupts, give the power, LED strip, etc. a couple of secs to stabilise
    flashLED(2);
  setupMesh();
    flashLED(3);
  setupLEDs();                                    // setup LEDs first and then use as setup indicator lights
  //_ledsFront = _headLightsColHSV;   //TEMP
  //_ledsRear  = _rearLightsColHSV;   //TEMP
    flashLED(4);
  setupSensors();                                 // setup all sensor inputs (note: sensors on wheels use interrupt pins)
    flashLED(5);
  setupUserInputs();                              // setup any user inputs - buttons etc. well, just buttons for now
    flashLED(6);
  _orientationTestSideMidpoint = ledSegment[1].total / 2; //change it later, easier for 2 segments
  
  //everything done? ok then..
  Serial.print( F("Setup done") );
  Serial.println();
  String s = String(mesh.getNodeId());
  Serial.print("Device Node ID is ");
  Serial.println(s);
  
  delay(400);
  FastLED.clear();
}

void loop() {
  mesh.update();

  /*  if(_firstTimeSetupDone == false) {
    if (DEBUG_GEN) { } 
    //
    _firstTimeSetupDone = true;
  }
*/
  //cut everything out of the loop and do calibrations - put board flat and press button to start these loops
  //check for full calibration bt first
  if (_doFullCalibration == 1) { 
    fullCalibration(); 
    _doQuickCalibration = 0; //just in case - if we have just done a full one, we don't need to do a quick one.
  }
  else if (_doQuickCalibration == 1) { quickCalibration(); }   //..if not, try for a quick one - this has issues!
  else {
    //run the loop normally
    loopUserInputs();   // a board with more interrupts for the buttons would cut out this whole loop!
    loopSensors();
    loopModes();
  }

  FastLED.show();
  FastLED.delay(_mainLoopDelay);

  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}
