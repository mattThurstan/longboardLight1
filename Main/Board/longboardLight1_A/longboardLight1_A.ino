/*
    'longboardLight1_A' by Thurstan. LED longboard lights with motion tracking.

    -- longboardLight1_A MOVED AT V0.305 TO D1 MINI (ESP8266 (ESP12E)) --
    
    Copyright (C) 2018  MTS Standish (mattThurstan)

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

    Implements fields array selection method and web interface by Jason Coon, from his ESP8266 + FastLED webserver (Copyright (C) 2015-2016 GNU GPL-3.0)
    https://github.com/jasoncoon/esp8266-fastled-webserver
*/


extern "C" {
#include "user_interface.h"                       //for ESP8266 library access
}

/*----------------------------libraries----------------------------*/
#include <EEPROM.h>                               //ESP8266 style EEPROM (using 512 bytes) - WeMos D1 (R2 &) mini, 80 MHz, 115200 baud, 4M, (3M SPIFFS)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WebSocketsServer.h>
#include <FS.h>                                   //file server
#include <ArduinoJson.h>
#include <Bounce2.h>                              //buttons with de-bounce
#include <FastLED.h>                              //WS2812B LED strip control and effects
#include <MT_BoardWheel.h>                        //(single version) attempt to move all board wheel related items to a single library
#include <MT_BoardOrientation.h>                  //attempt to move all board mpu6050 sensor related items to a single library

/*----------------------------system----------------------------*/
const String _progName = "longboardLight1_A";
const String _progVers = "0.352";                 //tweaks - working
const uint8_t _batteryPowered = 1; //take away const if power charge sensing ever gets implemented  //are we running on battery or plugged into the computer?
//ADC_MODE(ADC_VCC);                                //think this is need to be able to use ESP.getVcc() later.. ??? hmm.. problems
//const int _mainLoopDelay = 0;                     //just in case  - using FastLED.delay instead..
#define UPDATES_PER_SECOND 0           //120      //main loop FastLED show delay - 1000/120
boolean _firstTimeSetupDone = false;              //starts false
/*
 * Serial baud speeds:
 * 9600   - Arduino Pro Mini ATmega328 5V 16MHz
 * 38400  - for controlling MPU6050 over serial (don't really need it this fast so not doing it this way)
 * 115200 - D1 Mini (ESP8266)
 */
#define SERIAL_SPEED 115200                       //Serial coms baud speed

boolean DEBUG = true;                             //realtime serial debugging output - general
boolean DEBUG_INTERRUPT = false;                   //realtime serial debugging output - interrupts
boolean DEBUG_COMMS = true;                       //realtime serial debugging output - comms
boolean DATA_LOGGING = true;                      //turn data logging on or off eg. rps/mps, dist travelled, etc. ( Note: tracking led mode ticks up _ledsMovePos - NOT this )

uint8_t _testMode = 0;                            //used as an override to test all the modes
byte _curTestMode = 0;                            //the current mode being tested in isolation - sent from device

//a quick calibration could normally be done at the start, unless it gets turned off at the end of a full calibration. ???
uint8_t _doQuickCalibration = 0;                  //set to true to run quick calibration. it will reset itself to false when finished.
//3-axis accelerometer  calibration (these will be moved and integrated later when have communications)
uint8_t _doFullCalibration = 0;                   //set to true to run full calibration. it will reset itself to false when finished.

//const byte _buttonTotal = 2;                      //how many butons are there? - cannot set Bounce using this unfortunately!
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

/*----------------------------arduino pins----------------------------*/
//MPU6050 on I2C. Power with 5v, data connect to board with either 3.3v or 5v.
//I2C SDA = A4 (Pro Mini) or  4(D2) (D1 Mini)
//I2C SCL = A5 (Pro Mini) or  5(D1) (D1 Mini) 
const byte _wheelSensorPin = 0; //0  //D3            //wheel sensor input (!!interrupt pin!!!) Using built-in pullup
const byte _ledDOutPin0 = 14;     //D5            //rear/all lights
//const byte _ledDOutPin1 = 12;     //D6            //left
//const byte _ledDOutPin2 = 13;     //D7            //right and head lights
const byte _buttonTotal = 1; //2                  //total buttons in use
const byte _buttonPin[_buttonTotal] = { 2 }; //2 //D4, { 2, 16 } //array of user input buttons - uses _buttonTotal   ..ran out of internal pullups on ESP8266. next button has to have external pullup. see user input setup
/*
 * Pinout Wemos D1 Mini (ESP-8266)
 * RX  3
 * TX  1
 * A0  Analog input, max 3.2V input  A0
 * D0  16  IO  GPIO16   - no PWM or I2C or interrupt
 * D1  5  IO, SCL GPIO5  I2C
 * D2  4  IO, SDA GPIO4  I2C
 * D3  0  IO, 10k Pull-up GPIO0
 * D4  2  IO, 10k Pull-up, BUILTIN_LED  GPIO2
 * D5  14  IO, SCK GPIO14
 * D6  12  IO, MISO  GPIO12
 * D7  13  IO, MOSI  GPIO13
 * D8  15  IO, 10k Pull-down, SS GPIO15
 */

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

/*----------------------------comms----------------------------*/

const char WiFiAPPSK[] = "";                      // AP mode password
#define HOSTNAME "LlC_"                           //< WIFI Hostname. The initializeWiFi function adds the Chip ID at the end.

ESP8266WebServer webServer(80);
WebSocketsServer webSocketsServer = WebSocketsServer(81);
ESP8266HTTPUpdateServer httpUpdateServer;
const byte DNS_PORT = 53;
//DNSServer dnsServer;

const size_t _bufferSize = 200; //JSON_OBJECTSIZE(4) + 40; //size for 'name' 'value'or'r' 'g' 'b'
boolean _wifiActive = true;                         //TEMP true - change back to false when buttons implemented                      

/*----------------------------buttons----------------------------*/
const unsigned long _buttonDebounceTime = 5;      //5 milli-seconds debounce time
Bounce *_button = new Bounce[_buttonTotal];       //pointer to new array of N buttons
const unsigned long _loopButtonsInterval = 250;   //read loop interval in milliseconds. press button, count '1 (mabye 2)',  then let go.
//unsigned long _loopButtonsPrevMillis = 0;         //previous time for reference
boolean _buttonToggled[_buttonTotal] = { false }; //array of button toggle states

/*----------------------------LED----------------------------*/
//..see up top.. #define UPDATES_PER_SECOND 120   //main loop FastLED show delay - 1000/120
//mabye struct should contain int16_t instead of byte. eg. fill_gradient asks for it..
typedef struct {
  byte first;
  byte last;
  byte total;                                     //byte ok as haven't got more than 256 LEDs in a segment
} LED_SEGMENT;
const byte _ledNum = 64;//52; //40;                    //18 LED strip each side and 2 each end = 40 LEDs (2280mA max)
const byte _segmentTotal = 4;                     //2 sides, 2 ends
//LED_SEGMENT ledSegment[_segmentTotal] = {         /* drop-board - 1 loop */
//  { 0, 1, 2 },      //rear brake lights
//  { 2, 28, 24 },    //left side
//  { 31, 51, 24 },   //right side - gets reversed when creating CRGBset
//  { 29, 30, 2 },    //front head lights
//};
LED_SEGMENT ledSegment[_segmentTotal] = {         /* Loaded board */
  { 62, 63, 2 },    //rear brake lights
  { 0, 29, 30 },    //left side
  { 32, 61, 30 },   //right side - gets reversed when creating CRGBset
  { 30, 31, 2 },    //front head lights
};

const uint16_t _1totalDiv3 = (ledSegment[1].total / 3); //used in 'mode/void breathRiseFall'
const uint16_t _2totalDiv3 = (ledSegment[2].total / 3); //used in 'mode/void breathRiseFall'
//const uint16_t _1totalDiv3 = 9;
//const uint16_t _2totalDiv3 = 9;

CRGBArray<_ledNum> _leds;                         //master array - CRGBArray means can do multiple '_leds(0, 2).fadeToBlackBy(40);' as well as single '_leds[0].fadeToBlackBy(40);'

CRGBSet _ledsRear( _leds(ledSegment[0].first, ledSegment[0].last) );  //first, last
CRGBSet _ledsLeft( _leds(ledSegment[1].first, ledSegment[1].last) );
CRGBSet _ledsRight( _leds(ledSegment[2].last, ledSegment[2].first) ); //reversed - last, first - this will let us access _ledsRight the correct way
CRGBSet _ledsFront( _leds(ledSegment[3].first, ledSegment[3].last) );

/*memory\void setDefaultSettings*/byte _ledGlobalBrightnessCur = 255;               //current global brightness
/*memory\void setDefaultSettings*/byte _headLightsBrightness = 255;                 //use function to set
/*memory\void setDefaultSettings*/byte _rearLightsBrightness = 255;                 //use function to set
/*memory\void setDefaultSettings*/byte _trackLightsFadeAmount = 16;          //64   //division of 256 eg. fadeToBlackBy( _leds, _ledNum, _trackLightsFadeAmount);
volatile byte _ledMovePos = 0;                    //center point for tracking LEDs to wheels

CHSV _headLightsColHSV( 0, 0, 255);               //fixed - white @ 255
CHSV _rearLightsColHSV( 0, 255, 255);             //fixed - red @ 255

CRGB solidColor = CRGB::White;   //TEMP
CRGB solidColor2 = CRGB::Red;  //TEMP
uint8_t gHue = 0;                                 //rotating "base color" used by many of the patterns

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

MODES_ENABLED_ACTIVE mE = { 1, 1, 1, 1, 1, 0 }; // Enabled
MODES_ENABLED_ACTIVE mA = { 0, 0, 1, 1, 0, 0 }; // Active

#include "comms_JasonCoonField.h" //comms - Jason Coon Field array selection init part 1 - BEFORE patterns (subMode.h)

//const byte _mainLightsSubModeTotal = 4;             //never gonna have more than 256 lighting modes..   -- MOVED - see bottom of subMode.h --
/*memory\void setDefaultSettings*/uint8_t _mainLightsSubMode = 3;  //byte //sub-mode for main lights loop: 0=none/blank, 1= , 2= , 3= is always track (basic) cos of sensors loopOrientation()

#include "subMode.h"


/*----------------------------util - comms - webserver send----------------------------*/
//moved everything to webSockets
//void sendInt(uint8_t value) { sendString(String(value)); }
//void sendString(String value) { webServer.send(200, "text/plain", value);}

/*----------------------------util - comms - web sockets----------------------------*/
void broadcastInt(String name, uint8_t value) {
  String json = "{\"name\":\"" + name + "\",\"value\":" + String(value) + "}";
  webSocketsServer.broadcastTXT(json);
  if (DEBUG_COMMS) { Serial.print("WebSocket Broadcast Int "); Serial.println(json); }
}

void broadcastString(String name, String value) {
  String json = "{\"name\":\"" + name + "\",\"value\":\"" + String(value) + "\"}";
  webSocketsServer.broadcastTXT(json);
  if (DEBUG_COMMS) { Serial.print("WebSocket Broadcast String "); Serial.println(json); }
}

#include "comms_JasonCoonFields.h"  //comms - Jason Coon Field array selection init part 2 - AFTER patterns, hmm,.. and broadcastint, but before websocketevent

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
     
  switch (type) {
    case WStype_DISCONNECTED:
      {
        if (DEBUG_COMMS) { Serial.printf("[%u] Disconnected!\n", num); }
      }
      break;

    case WStype_CONNECTED:
      {
        if (DEBUG_COMMS) { 
          IPAddress ip = webSocketsServer.remoteIP(num);
          Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        }
      }
      break;

    case WStype_TEXT:
      {
        if (DEBUG_COMMS) { Serial.printf("[%u] get Text: %s\n", num, payload); }

        StaticJsonBuffer<200> _jsonBuffer;
        JsonObject& parsed = _jsonBuffer.parseObject(payload); //Parse message
        
        if (!parsed.success()) {   //Check for errors in parsing
          if (DEBUG_COMMS) { Serial.println("Parsing failed"); }
          //for (uint8_t i = 0; i <length; i++) {
          //  Serial.print(payload[i]);
          //}
          //Serial.println();
          return;
        }

      //const char * sensorType = parsed["sensor"];           //Get sensor type value
      //const char * sensorIdentifier = parsed["identifier"]; //Get sensor type value
      //const char * timestamp = parsed["timestamp"];         //Get timestamp
      //int value = parsed["value"];                          //Get value of sensor measurement

      //String name = webServer.arg("name");
      //String value = getFieldValue(name, fields, fieldCount);
      //String nn = parsed["name"];
      //const char * nn = parsed["name"];
      //String nn = parsed["name"];
      //String snn = nn;

        String nn = parsed["name"];
        uint8_t vv = parsed["value"];
          
        if (nn == "solidColor" || nn == "solidColor2") {
          uint8_t r = parsed["r"];
          uint8_t g = parsed["g"];
          uint8_t b = parsed["b"];
          if (nn == "solidColor") {
            setSolidColor(r, g, b);
          } else {
            setSolidColor2(r, g, b);
          }
          if (DEBUG_COMMS) { Serial.print("WStype_TEXT received - "); } //Serial.print(nn); 
        } else {
          //String vv = parsed["value"];
          if (nn == "sleep") {
            setSleep(vv);
          } else if  (nn == "breathe") {
            setBreathe(vv);
          } else if  (nn == "testMode") {
            setTestMode(vv);
          } else if  (nn == "curTestMode") {
            setCurTestMode(vv);
          } else if  (nn == "curTestModeName") {
            String nom = parsed["value"];
            setCurTestModeByName(nom);
          } else if  (nn == "subMode") {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
            setMainLightsSubMode(vv);
          } else if  (nn == "subModeName") {
            String nem = parsed["value"];
            setMainLightsSubModeByName(nem);
          } else if  (nn == "globalBrightness") {
            setGlobalBrightness(vv);
          } else if  (nn == "quickCalibration") {
            setQuickCalibration(vv);
          } else if  (nn == "fullCalibration") {
            setFullCalibration(vv);
          } else if  (nn == "resetDefaults") {
            resetDefaults(vv);
          } else if  (nn == "wifiOff") {
            //if (vv == 1) { stopComms(); } //needs a button first
          } else if  (nn == "head") {
            setHead(vv);
          } else if  (nn == "headBrightness") {
            setHeadBrightness(vv);
          } else if  (nn == "rear") {
            setRear(vv);
          } else if  (nn == "rearBrightness") {
            setRearBrightness(vv);
          } else if  (nn == "brake") {
            setBrake(vv);
          } else if  (nn == "indicate") {
            setIndicate(vv);
          }
          if (DEBUG_COMMS) { Serial.print("WStype_TEXT received - "); Serial.print(nn); Serial.print(", "); Serial.println(vv); }
        }

      }
      break;

    case WStype_BIN:
      {
        if (DEBUG_COMMS) { Serial.printf("[%u] get binary length: %u\n", num, length); }
        hexdump(payload, length);
      }
      break;
  }
}


/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------MAIN--------------------------------------------------------*/
void setup() {

  //clearAllSettings();   // TEMP
  
  EEPROM.begin(512);                              //tell it we are using an EEPROM area of 512 bytes
  setupSerial();                                  //..see 'util'
  loadAllSettings();                              //load any saved settings eg. save state when turn board power off. - not fully implemented yet !!!
  setupInterrupts();                              //set any interrupts..
  delay(3000);                                    //..after setting interrupts, give the power, LED strip, etc. a couple of secs to stabilise

  if (DEBUG) { 
    Serial.println();
    Serial.print( F("Welcome to Gardening For Nomes part 4!") ); Serial.println();
    Serial.print( F("Your host for todays proceedings is.. ") ); //Serial.println();
    //Serial.print( F("Program Name: ") ); 
    Serial.println(_progName);
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
  }

  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      if (DEBUG) { Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str()); }
    }
    if (DEBUG) { Serial.printf("\n"); }
  }
  
  if (_wifiActive) { startComms(); }              //WIFI gets turned on by a physical button. WIFI gets turned off via webpage or by the board going to sleep.
  
  setupLEDs();                                    //setup LEDs first and then use as setup indicator lights
  _ledsFront = _headLightsColHSV;   //TEMP
  _ledsRear  = _rearLightsColHSV;   //TEMP
  delay(400);
    //_leds[ledSegment[1].first] = CRGB::Yellow;
    //_leds[ledSegment[2].first] = CRGB::Yellow;
    _ledsLeft[0] = CRGB::Yellow;
    _ledsRight[0] = CRGB::Yellow;
    FastLED.show();
  setupSensors();                                 //setup all sensor inputs (note: sensors on wheels use interrupt pins)
  delay(400);
    _ledsLeft[2] = CRGB::Fuchsia;
    _ledsRight[2] = CRGB::Fuchsia;
    FastLED.show();
  setupUserInputs();                              //setup any user inputs - buttons etc. well, just buttons for now
  delay(400);
    _ledsLeft[4] = CRGB::Green;
    _ledsRight[4] = CRGB::Green;
    FastLED.show();
  _orientationTestSideMidpoint = ledSegment[1].total / 2; //change it later, easier for 2 segments
  //checkStartupButtons();  //check to see if any button has been held down during startup eg. full calibration
  //
  if (DEBUG) { 
  //everything done? ok then..
    Serial.print(F("Setup done"));
    Serial.println();
  }
  //statusLED.Blink1();
  delay(400);
    _ledsLeft[6] = CRGB::MediumTurquoise;
    _ledsRight[6] = CRGB::MediumTurquoise;
    FastLED.show();
  delay(400);
  FastLED.clear();
}

void loop() {
/*  if(_firstTimeSetupDone == false) {
    if (DEBUG) { } 
    //
    _firstTimeSetupDone = true;
  }
*/
  if (_wifiActive) { 
    webSocketsServer.loop();
    webServer.handleClient();
  } 
  
  //cut everything out of the loop and do calibrations - put board flat and press button to start these loops
  //check for full calibration bt first
  if (_doFullCalibration == 1) { 
    fullCalibration(); 
    _doQuickCalibration = 0; //just in case - if we have just done a full one, we don't need to do a quick one.
  }
  else if (_doQuickCalibration == 1) { quickCalibration(); }   //..if not, try for a quick one - this has issues!
  else {
    //run the loop normally
    loopUserInputs();   //if a board with more interrupts for the buttons, that would cut out this whole loop!
    loopSensors();
    loopModes();
  }
  
  FastLED.show();
  //delay(_mainLoopDelay);
  //FastLED.delay(1000 / UPDATES_PER_SECOND);
  FastLED.delay(UPDATES_PER_SECOND);  //currently '0'

  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

