/*
    'Calibration' (for 'longboardLight1' by Thurstan). 
    Standalone calibration program for LED longboard lights with motion tracking.
    https://github.com/mattThurstan/
*/

//running quick calibration from this program is a bit pointless really as it only updates running vars, but hey ho, done it now..
const byte _calibrationType = 1;                  //0=quick calibration, 1=full calibration

/*----------------------------libraries----------------------------*/
#include <MT_BlinkStatusLED.h>                    //my basic status LED blink library
#include <MT_BoardOrientation.h>                  //my mpu6050 sensor related items library

/*----------------------------system----------------------------*/
const String _progName = "Calibration (for longboardLight1)";
const String _progVers = "0.1.0"; 
//const int _mainLoopDelay = 0;                     //just in case
boolean _calibrationComplete = false;             //flip when calibration has finished

/*----------------------------arduino pins----------------------------*/
MT_BlinkStatusLED statusLED(13);                  //setup status LED on pin 13

/*----------------------------sensors - MPU6050 6-axis----------------------------*/
MT_BoardOrientation o;

/*----------------------------MAIN----------------------------*/
void setup() {
  statusLED.Blink1();
  
  Serial.begin(9600);
  Serial.println();
  Serial.print(_progName);
  Serial.print(" ");
  Serial.print(_progVers);
  Serial.println();
  Serial.print("..");
  Serial.println();

  o.Init();                                       //initialise MPU6050 thru library

  Serial.print(F("Current.."));
  Serial.println();
  
  Serial.print(F("MPU6050 Accel Offset XYZ = "));
  Serial.print(o.GetMPU6050AccelOffsetX());
  Serial.print(F(", "));
  Serial.print(o.GetMPU6050AccelOffsetY());
  Serial.print(F(", "));
  Serial.print(o.GetMPU6050AccelOffsetZ());
  Serial.println();
  
  Serial.print(F("MPU6050 Gyro Offset XYZ = "));
  Serial.print(o.GetMPU6050GyroOffsetX());
  Serial.print(F(", "));
  Serial.print(o.GetMPU6050GyroOffsetY());
  Serial.print(F(", "));
  Serial.print(o.GetMPU6050GyroOffsetZ());
  Serial.println();

  Serial.print(F("Setup done."));
  Serial.println();
  statusLED.Blink3();
}

void loop() {

  if (_calibrationComplete == false) {
    if (_calibrationType == 0) { quickCalibration(); } 
    else if (_calibrationType == 1) { fullCalibration(); }
  }
  //delay(_mainLoopDelay);
}

/*----------------------------calibration----------------------------*/
void quickCalibration() {
  Serial.print(F("Quick calibration started"));
  Serial.println();
  delay(3000);      //..it's all ok, we really do want to halt the thread!
  o.QuickCalibration();
  //send final vars by way of serial - erm, no vars
  _calibrationComplete = true;
  Serial.print(F("Quick calibration done"));
  Serial.println();
}

void fullCalibration() {
  Serial.print(F("Full calibration started"));
  Serial.println();
  delay(9000);
  o.FullCalibration();
  
  //send final vars by way of serial
  Serial.print(F("Results.."));
  Serial.println();
  
  Serial.print(F("MPU6050 Accel Offset XYZ = "));
  Serial.print(o.GetMPU6050AccelOffsetX());
  Serial.print(F(", "));
  Serial.print(o.GetMPU6050AccelOffsetY());
  Serial.print(F(", "));
  Serial.print(o.GetMPU6050AccelOffsetZ());
  Serial.println();
  
  Serial.print(F("MPU6050 Gyro Offset XYZ = "));
  Serial.print(o.GetMPU6050GyroOffsetX());
  Serial.print(F(", "));
  Serial.print(o.GetMPU6050GyroOffsetY());
  Serial.print(F(", "));
  Serial.print(o.GetMPU6050GyroOffsetZ());
  Serial.println();
  
  _calibrationComplete = true;
  Serial.print(F("Full calibration complete."));
  Serial.println();
}





