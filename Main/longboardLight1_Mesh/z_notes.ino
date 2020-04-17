/*----------------------------notes----------------------------*/

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
 * JSON and WebSockets = headache
 * https://github.com/douglascrockford/JSON-js/blob/master/json2.js
 * http://api.jquery.com/jQuery.extend/
 * http://api.jquery.com/jQuery.param/
 * http://api.jquery.com/Types/#PlainObject
 * https://github.com/Coopdis/easyMesh/blob/master/examples/demoToy/data/main.js
 * https://github.com/Coopdis/easyMesh/blob/master/examples/demoToy/demoToy.ino
 * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/JSON/stringify
 * 
 * 
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
 * double   = float 4-byte value
 */

/* Wheels
 *  
 * black marked side of magents are North)
 * my wheels 1 - shark wheels 70mm diameter / 78A (about 69mm due to wear etc.)
 * - 2 * 3.14 (PI) * half diameter (radius) = 219.8
 * - 2 * 3.14 * 34.5 = 216.66 (with 8 magnets this gives 27.08mm spacing between each)
 * LED spacing on strips = 33.3mm (3 LEDs in 100mm, 30 per meter)
 * circumference = 2 * PI * radius
 */

/* Arduino Pro Mini 
 *  
 * Pins:
 * serial - 0,1+
 * interrupts - 2,3
 * PWM - 3,5,6,9,10,11
 * SPI - 10(SS), 11(MOSI), 13(SCK)
 * I2C - A4(SDA), A5(SCL) 
 * 
 * 
 * ArduinoMiniPro A4 -> GY-521 (MPU6050) SDA
 * ArduinoMiniPro A5 -> GY-521 (MPU6050) SCL
 * GY-521 (MPU6050) INT (interrupt) pin optional - needs interrupt pin to check its fancy onboard calculations such as no-motion status
 */

 /*
  * ESP8266
  * 
  * ESP.restart() tells the SDK to reboot
  */

 /* Sleep mode
  *  
 * ..the following example line is wrong, and will then be explained..
 * 
 * 'the user could put the board to sleep, then turn off, then expect the same when turned on later'
 * 
 * 'the user puts the board to sleep' - all good
 * 'then turns off' - still good
 * 'then expect the same when turned on later' - wrong, cos handling the board to turn it on would result in waking it from sleep
 * ..therefore _sleep is 'Active', NOT 'Enabled' and not saved to memory
 */


/* WS2812B addressable 5V LED strips
 *  
 * power/mA per LED = 60
 * total _leds = 40
 * total power/mA = 2400 (full white) ( 75% duty cycle/mA = 1800 )
 * total power per colour/mA = 800  ( 75% duty cycle/mA = 600 )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
 */

/* Calibration
 * 
 * place flat
 * get motion data
 * get offsets
 * nudge offsets towards motion data
 * set offsets
 * repeat x times, for a time period, or until within tolerance
 */

 /*
// 4-segment led array orders
const byte _ledActualOrder[40]       = {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 };
const byte _ledLoopOrder[40]         = {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,  0 };
const byte _ledLeftFullOrder[20]     = {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 39 };
const byte _ledRightFullOrder[20]    = {  0, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38 };
const byte _ledLeftOverlayOrder[18]  = {  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
const byte _ledRightOverlayOrder[18] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37 };

void ledLoopTest() {
  for (byte i = 0; i < 40; i++) {
    _leds[ _ledLoopOrder[i] ] = CRGB::Red;
  }
  for (byte i = 0; i < getLedSideTotal; i++) {
    _leds[ getLed[i] ] = CRGB::Red;
  }  
}

byte getLedSideTotal() {
  byte u;
  if (mE.head == 0 || mE.rear == 0) {
    u = 20;
  } else {
    u = 18;
  }
  return u;
}
byte getLed(i) {
  if (mE.head == 0 || mE.rear == 0) {
    y = _ledLeftFullOrder[i];
  } else {
    y = _ledLeftOverlayOrder[i];
  }
}
*/

/* 
 * WeMos D1 (R2 &) mini, 80 MHz, 115200 baud, 4M, (1M SPIFFS)
 * 
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
 *
 */

/*
 * Home Assistant config 
 * 
 *   light:
 *     - platform: mqtt
 *       name: 'Stairs lights'
 *       state_topic: 'house/stairs/lights/light/status'
 *       command_topic: 'house/stairs/lights/light/switch'
 *       state_value_template: "{{ value_json.state }}"
 *       brightness_state_topic: 'house/stairs/lights/brightness/status'
 *       brightness_command_topic: 'house/stairs/lights/brightness/set'
 *       brightness_value_template: "{{ value_json.brightness }}"
 *       hue_state_topic: 'house/stairs/lights/hue/status'
 *       hue_command_topic: 'house/stairs/lights/hue/set'
 *       rgb_state_topic: 'house/stairs/lights/rgb/status'
 *       rgb_command_topic: 'house/stairs/lights/rgb/set'
 *       rgb_value_template: "{{ value_json.rgb | join(,) }}"
 *       qos: 0
 *       payload_on: "ON"
 *       payload_off: "OFF"
 *       optimistic: false
 */
 
