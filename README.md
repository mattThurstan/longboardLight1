# longboardLight1
LED longboard light setup, by Thurstan

Repository is an Arduino project

### Concept
Lighting system for a longboard. Using sensors to control brake lights, indicator lights and ground tracking effects.

### Equipment
#### Hardware
- Arduino Mini Pro board
  * 5v
  * ATmega328
  * 16 MHz clock speed
  * 1K EEPROM
- WS2812B addressable LED strips
  * pixel order = GRB
- Movement sensors
  * ~~ADXL335~~
  * MPU6050
- Wheel tracking sensor
- 7.4V LiPo Battery
- 7.4V-5V Convertor
  
#### Software
- Arduino
- FastLED library  https://github.com/FastLED/FastLED
- I2CdevLib  https://www.i2cdevlib.com  https://github.com/jrowberg/i2cdevlib
- VVVV

### Licence
- Written by MTS Standish (Thurstan|mattKsp) 2016
- Released under GNU GPLv3 (see LICENCE file)

This software is provided on an "AS-IS-BASIS"

#### Contains code based on:
- I2CdevLib by Jeff Rowberg Copyright (c) 2011
 * https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_raw/MPU6050_raw.ino
- Arduino MPU6050 auto-calibration script by Luis Ródenas  <luisrodenaslorda@gmail.com>
 * http://42bots.com/tutorials/arduino-script-for-mpu-6050-auto-calibration/ 
 * (also) Based on the I2Cdev library and previous work by Jeff Rowberg <jeff@rowberg.net>

### Reference
https://github.com/FastLED/FastLED/wiki/Pixel-reference <br>
