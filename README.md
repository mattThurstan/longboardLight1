# longboardLight1
LED longboard light setup, by Thurstan <br> 
https://hackaday.io/project/26919-longboardlight1

Repository is an Arduino project

### Concept
Lighting system for a longboard. Using sensors to control brake lights, indicator lights and ground tracking effects.

### Equipment
#### Hardware
- CodeCell C6 
  * ESP32-C6-MINI-1-H8 (ARDUINO_ESP32C6_DEV)
  * 3.3v/5v
  * 8MB Flash + 512KB SRAM
  * WiFi 6 / BLE 5 / Zigbee
  * BQ24232 battery management
  * VCNL4040 light sensor / proximity
  * BN0085 9-axis IMU
  * Pins:
  ** GPIO23 - Wheel tracking sensor interrupt
  ** GPIO22 - [ LED1 (Right) ]
  ** GPIO21 - LED0 (Left OR both)
  ** GPIO03 - 
  ** GPIO02 - 
  ** GPIO01 - BT0
- WS2812B addressable LED strips
  * pixel order = GRB
  * approx. 2A
- Wheel tracking sensor
  * hall effect sensor and magnets
- Power switch (toggle OR digital momentary, with optional control from ESP32)
- Push buttons
- 7.4V LiPo Battery
- >6V to 5V Convertor
  
#### Software
- Arduino
- FastLED	https://github.com/FastLED/FastLED
- I2CdevLib https://www.i2cdevlib.com https://github.com/jrowberg/i2cdevlib 
- CodeCell 	https://github.com/microbotsio/CodeCell

### Licence
- Written by MTS Standish (Thurstan|mattKsp) 2026
- Released under GNU GPLv3 (see LICENCE file)

This software is provided on an "AS-IS-BASIS"

#### Contains code based on:
- I2CdevLib by Jeff Rowberg, Copyright (c) 2011
 * https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_raw/MPU6050_raw.ino
- Arduino MPU6050 auto-calibration script by Luis Ródenas <luisrodenaslorda@gmail.com>
 * http://42bots.com/tutorials/arduino-script-for-mpu-6050-auto-calibration/ 
 * (also) Based on the I2Cdev library and previous work by Jeff Rowberg <jeff@rowberg.net>
- Fields array selection method and Web interface example from ESP8266 + FastLED webserver by Jason Coon, Copyright (C) 2015-2016, GNU GPL-3.0
 * https://github.com/jasoncoon/esp8266-fastled-webserver

### Reference
https://github.com/FastLED/FastLED/wiki/Pixel-reference <br> 
https://www.arduino.cc/en/Main/ArduinoBoardProMini <br> 
https://www.arduino.cc/en/Guide/Troubleshooting#upload <br> 
https://github.com/FastLED/FastLED/wiki/Pixel-reference <br> 
http://forum.arduino.cc/index.php?topic=140494.0 <br> 
http://bildr.org/2011/04/sensing-orientation-with-the-adxl335-arduino/ <br> 
http://42bots.com/tutorials/arduino-script-for-mpu-6050-auto-calibration/ <br> 
https://www.i2cdevlib.com/devices/mpu6050#source <br> 
https://www.i2cdevlib.com/docs/html/class_m_p_u6050.html <br> 
http://www.geekmomprojects.com/gyroscopes-and-accelerometers-on-a-chip/ <br> 
http://engineering.stackexchange.com/questions/3348/calculating-pitch-yaw-and-roll-from-mag-acc-and-gyro-data <br> 
http://www.nxp.com/assets/documents/data/en/application-notes/AN3461.pdf <br> 
http://theboredengineers.com/2012/09/the-quadcopter-get-its-orientation-from-sensors/ <br> 
https://microbots.io/products/codecell-c6?srsltid=AfmBOopTWXuwjsbL_Bt63ITD7-n81sR28a9Ag49fXUb-_uSjcXBQ-8W9 <br> 
https://www.mouser.co.uk/datasheet/3/5991/1/BNO080_085-Datasheet.pdf <br> 
