
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
