/*----------------------------sensors----------------------------*/

void setupSensors() {
  setupWheel();                      //setup wheel tracking
  //setupGryo();
  //setupAcceleration();
  setupMPU6050();
}

/* sensor input loop
 * called from main loop
 */
void loopSensors() {
  loopWheel();                       //loop wheel tracking
  //loopAcceleration();
  //loopGryo();
  //readMPU6050();
  readMPU6050filtered();
}

/*----------------------------wheel sensors----------------------------*/

void setupWheel() {
  //diameter of wheel
  //circumference = diameter x pi
  //
}

void loopWheel() {
  //speed = RPM * circumference
  //RPM = speed / circumference
  //speed = distance / time
  //distance = speed x time
  //time = distance / speed
  //pos = map(pos, 0, ?, 0, 18);
  //pos = constrain(pos, 0, 18);
  //pos = constrain(map(pos, 0, ?, 0, 18), 0, 18);
  //how many revolutions does it take to move 1 meter?
  //revs per meter = 1000 / circumference
  //eg. 70mm wheel radius
  //circumference = 2 x 3.142 (pi) x 70 = 439.88
  //revs per meter = 1000 / 439.88 = 2.733(472765)
  //revs per meter = 1000 / 440 = 2.272(727272)
  //pos = map(pos, 0, 4.54, 0, 18);
  //
}

/*----------------------------gyro sensors----------------------------*/

void setupGryo() {
  //
}

void loopGryo() {
  //
}

/*----------------------------acceleration sensors----------------------------*/ 

void setupAcceleration() {
  //setupAdxl335();
}

void loopAcceleration() {
  //adxl335calibrationRead();
  //readADXL335();
  //readADXL335b();
}

//

void setupAdxl335() {
  //calibrate with average of 100 samples?
  //calibrateADXL335();
  //_adxl335AccelCur = _acceleration; //TEMP
  orientationCalibrationCalculation();
  _adxl335AccelSamplePrevMillis = millis();               //store the current time
}

/*
 * Orientation Calibration Calculation
 * Function to (re)calculate trigger values for switching orientation.
 * This should be run every time new orientation calibration readings are taken/saved.
 */
void orientationCalibrationCalculation() {
  
    int xmi = 1023;  //min
    int xmiPrev = 1023;
    int ymi = 1023;
    int ymiPrev = 1023;
    int zmi = 1023;
    int zmiPrev = 1023;
    
    int xm = 0;   //max
    int xmPrev = 0;
    int ym = 0;
    int ymPrev = 0;
    int zm = 0;
    int zmPrev = 0;
    
    for (int i = 0; i < 6; i++) {
      int ii;
      if (i == 5) {
        ii = 0;
      } else {
        ii = i + 1;
      }
      xmi = min( _orientationCalibrationXYZ[i][0], _orientationCalibrationXYZ[ii][0] );
      if (xmiPrev < xmi) {
        //if old less than new..
        xmi = xmiPrev;  //replace new
      }
      xmiPrev = xmi;  //whatever the outcome store for next loop
      
      xm = max( _orientationCalibrationXYZ[i][0], _orientationCalibrationXYZ[ii][0] );
      if (xmPrev > xm) {
        //if old greater than new..
        xm = xmPrev;  //replace new
      }
      xmPrev = xm;  //store for next loop
      
      ymi = min( _orientationCalibrationXYZ[i][1], _orientationCalibrationXYZ[ii][1] );
      if (ymiPrev < ymi) { ymi = ymiPrev; }
      ymiPrev = ymi;
      ym = max( _orientationCalibrationXYZ[i][1], _orientationCalibrationXYZ[ii][1] );
      if (ymPrev > ym) { ym = ymPrev; }
      ymPrev = ym;
      
      zmi = min( _orientationCalibrationXYZ[i][2], _orientationCalibrationXYZ[ii][2] );
      if (ymiPrev < ymi) { ymi = ymiPrev; }
      ymiPrev = ymi;
      zm = max( _orientationCalibrationXYZ[i][2], _orientationCalibrationXYZ[ii][2] );
      if (zmPrev > zm) { zm = zmPrev; }
      zmPrev = zm;
    }
    
    int xt; //x temp
    int yt;
    int zt;
    //x = (max - min) / 4 distance from stored value to trigger orientation
    xt = (xm-xmi)/_orientationXYZdistTriggerDivide;
    yt = (ym-ymi)/4;
    zt = (zm-zmi)/4;

    _orientationCalibrationXYZminMaxDist[0][0] = xmi;   //x min
    _orientationCalibrationXYZminMaxDist[0][1] = xm;    //x max
    _orientationCalibrationXYZminMaxDist[0][2] = xt;    //x threshold distance to change 
    _orientationCalibrationXYZminMaxDist[1][0] = ymi;   //y min
    _orientationCalibrationXYZminMaxDist[1][1] = ym;    //y max
    _orientationCalibrationXYZminMaxDist[1][2] = yt;    //y threshold distance to change 
    _orientationCalibrationXYZminMaxDist[2][0] = zmi;   //z min
    _orientationCalibrationXYZminMaxDist[2][1] = zm;    //z max
    _orientationCalibrationXYZminMaxDist[2][2] = zt;    //z threshold distance to change 

    #ifdef DEBUG_ADXL335
    Serial.print("ADXL335 orientation calibration saves min/max: ");
    Serial.print(xmi);
    Serial.print(", ");
    Serial.print(xm);
    Serial.print(", ");
    Serial.print(ymi);
    Serial.print(", ");
    Serial.print(ym);
    Serial.print(", ");
    Serial.print(zmi);
    Serial.print(", ");
    Serial.print(zm);
    Serial.println();
    Serial.print("Calculated distance from stored value to change at: ");
    Serial.print(xt);
    Serial.print(", ");
    Serial.print(yt);
    Serial.print(", ");
    Serial.print(zt);
    Serial.println();
    #endif
}

void adxl335calibrationRead() {
//  unsigned long XSum = 0;
//  unsigned long YSum = 0;
//  unsigned long ZSum = 0;
 
//  for (int i=0; i<10000; i++) {
//    XSum += analogRead(_adxl335xyzPin[0]);
//    YSum += analogRead(_adxl335xyzPin[1]);
//    ZSum += analogRead(_adxl335xyzPin[2]);
//  }
//  #ifdef DEBUG
//    Serial.print("ADXL335 XYZ calibration averages (1000): ");
//    Serial.print(XSum/10000);
//    Serial.print(", ");
//    Serial.print(YSum/10000);
//    Serial.print(", ");
//    Serial.print(ZSum/10000);
//    Serial.println();
//  #endif

    //auto-setting values, pushes min/max higher/lower if needed
  _adxl335AccelCur.x = analogRead(_adxl335xyzPin[0]);
  _adxl335AccelCur.y = analogRead(_adxl335xyzPin[1]);
  _adxl335AccelCur.z = analogRead(_adxl335xyzPin[2]);

  _adxl335AccelCur.xMax = max(_adxl335AccelCur.xMax, _adxl335AccelCur.x); //picks the highest of either value
  _adxl335AccelCur.xMin = min(_adxl335AccelCur.xMin, _adxl335AccelCur.x); //picks the lowest of either value
  _adxl335AccelCur.yMax = max(_adxl335AccelCur.yMax, _adxl335AccelCur.y); //picks the highest of either value
  _adxl335AccelCur.yMin = min(_adxl335AccelCur.yMin, _adxl335AccelCur.y); //picks the lowest of either value
  _adxl335AccelCur.zMax = max(_adxl335AccelCur.zMax, _adxl335AccelCur.z); //picks the highest of either value
  _adxl335AccelCur.zMin = min(_adxl335AccelCur.zMin, _adxl335AccelCur.z); //picks the lowest of either value
  
//  if(_adxl335AccelCur.x > _adxl335AccelCur.xMax) {
//    _adxl335AccelCur.xMax = _adxl335AccelCur.x;
//  } else if(_adxl335AccelCur.x < _adxl335AccelCur.xMin) {
//    _adxl335AccelCur.xMin = _adxl335AccelCur.x;
//  }
//  if(_adxl335AccelCur.y > _adxl335AccelCur.yMax) {
//    _adxl335AccelCur.yMax = _adxl335AccelCur.y;
//  } else if(_adxl335AccelCur.y < _adxl335AccelCur.yMin) {
//    _adxl335AccelCur.yMin = _adxl335AccelCur.y;
//  }
//  if(_adxl335AccelCur.z > _adxl335AccelCur.zMax) {
//    _adxl335AccelCur.zMax = _adxl335AccelCur.z;
//  } else if(_adxl335AccelCur.z < _adxl335AccelCur.zMin) {
//    _adxl335AccelCur.zMin = _adxl335AccelCur.z;
//  }

  Serial.print("ADXL335 XYZ raw: ");
  Serial.print(_adxl335AccelCur.x);
  Serial.print(", ");
  Serial.print(_adxl335AccelCur.y);
  Serial.print(", ");
  Serial.print(_adxl335AccelCur.z);
  Serial.print(" Min/max:  X ");  
  Serial.print(_adxl335AccelCur.xMin);
  Serial.print(", ");
  Serial.print(_adxl335AccelCur.xMax);
  Serial.print(" - Y ");
  Serial.print(_adxl335AccelCur.yMin);
  Serial.print(", ");
  Serial.print(_adxl335AccelCur.yMax);
  Serial.print(" - Z ");
  Serial.print(_adxl335AccelCur.zMin);
  Serial.print(", ");
  Serial.print(_adxl335AccelCur.zMax);
  Serial.println();

}

void readADXL335() {
//  int rX = analogRead(_adxl335xyzPin[0]);
//  int rY = analogRead(_adxl335xyzPin[1]);
//  int rZ = analogRead(_adxl335xyzPin[2]);
//  
//  _acceleration.x = rX;
//  _acceleration.y = rY;
//  _acceleration.z = rZ;

  /* idea is to take small sample every so often, then base calculations on that
   * only using ADXL355 for basic orientation and mabye stop/start
   * balance time against number of samples
   */
  long adxl355AccelCurMillis = millis();   //get current time
  if((long) (adxl355AccelCurMillis - _adxl335AccelSamplePrevMillis) >= _adxl335AccelSampleInterval) {
  
    unsigned long XSum2 = 0;  //temp vars for calculations
    unsigned long YSum2 = 0;
    unsigned long ZSum2 = 0;
    
    for (int i=0; i<_adxl335AccelSampleTotal; i++) {
      XSum2 += analogRead(_adxl335xyzPin[0]);
      YSum2 += analogRead(_adxl335xyzPin[1]);
      ZSum2 += analogRead(_adxl335xyzPin[2]);
    }
    _adxl335AccelCur.x = XSum2/_adxl335AccelSampleTotal;  //100
    _adxl335AccelCur.y = YSum2/_adxl335AccelSampleTotal;
    _adxl335AccelCur.z = ZSum2/_adxl335AccelSampleTotal;

  #ifdef DEBUG_ADXL335
    Serial.print("ADXL335 XYZ calibration averages: ");
    Serial.print(_adxl335AccelCur.x);
    Serial.print(", ");
    Serial.print(_adxl335AccelCur.y);
    Serial.print(", ");
    Serial.print(_adxl335AccelCur.z);
    Serial.println();
  #endif
    //_adxl335AccelPrev = _adxl335AccelCur;
  
    int Xmap = map(_adxl335AccelCur.x, _adxl335AccelCur.xMin, _adxl335AccelCur.xMax, _adxl335AccelMapMin, _adxl335AccelMapMax);  //-10, 10   _adxl335AccelMapMin, _adxl335AccelMapMax
    int Ymap = map(_adxl335AccelCur.y, _adxl335AccelCur.yMin, _adxl335AccelCur.yMax, _adxl335AccelMapMin, _adxl335AccelMapMax);
    int Zmap = map(_adxl335AccelCur.z, _adxl335AccelCur.zMin, _adxl335AccelCur.zMax, _adxl335AccelMapMin, _adxl335AccelMapMax);
  
    //_orientation: 0=flat, 1=upside-down, 2=up, 3=down, 4=left-side, 5=right-side
    
    if(Xmap <= _adxl335AccelOrientationThreshold && Xmap >= -_adxl335AccelOrientationThreshold && Ymap <= _adxl335AccelOrientationThreshold && Ymap >= -_adxl335AccelOrientationThreshold && Zmap >= -_adxl335AccelOrientationThreshold) {
      _orientation = 0;
      //Serial.print("flat - ");
    } else if(Xmap <= _adxl335AccelOrientationThreshold && Xmap >= -_adxl335AccelOrientationThreshold && Ymap <= _adxl335AccelOrientationThreshold && Ymap >= -_adxl335AccelOrientationThreshold && Zmap < -_adxl335AccelOrientationThreshold) {
      _orientation = 1;
      //Serial.print("upside-down - ");
    } else if(Xmap < _adxl335AccelOrientationThreshold && Xmap > -_adxl335AccelOrientationThreshold && Ymap > _adxl335AccelOrientationThreshold) {
      _orientation = 2;
      //Serial.print("up - ");
    } else if(Xmap < _adxl335AccelOrientationThreshold && Xmap > -_adxl335AccelOrientationThreshold && Ymap < -_adxl335AccelOrientationThreshold) {
      _orientation = 3;
      //Serial.print("down - ");
    } else if(Xmap > _adxl335AccelOrientationThreshold && Ymap < _adxl335AccelOrientationThreshold && Ymap > -_adxl335AccelOrientationThreshold) {
      _orientation = 4;
      //Serial.print("left side - ");
    } else if(Xmap < -_adxl335AccelOrientationThreshold && Ymap < _adxl335AccelOrientationThreshold && Ymap > -_adxl335AccelOrientationThreshold) {
      _orientation = 5;
      //Serial.print("right side - ");
    }
  
    //these are kept seperate. used for FX overlays
    if(Xmap >= _adxl335AccelIndicatorThreshold && Xmap <= (_adxl335AccelOrientationThreshold+1) && Ymap <= _adxl335AccelOrientationThreshold && Ymap >= -_adxl335AccelOrientationThreshold) {
      _indicatorLeftActive = true;
      //Serial.print("leaning left - ");
    } else { _indicatorLeftActive = false; } 
    if(Xmap <= -_adxl335AccelIndicatorThreshold && Xmap >= -(_adxl335AccelOrientationThreshold+1) && Ymap <= _adxl335AccelOrientationThreshold && Ymap >= -_adxl335AccelOrientationThreshold) {
      _indicatorRightActive = true;
      //Serial.print("leaning right - ");
    } else { _indicatorRightActive = false; }

  #ifdef DEBUG_ADXL335
    Serial.print("orientation ");
    Serial.print(_orientation);
    if(_indicatorRightActive == true) {
      Serial.print(", right indicator!, ");
    } else if(_indicatorLeftActive == true) {
      Serial.print(", left indicator!, ");
    }
    //Serial.println();

    Serial.print(" - ");
    Serial.print(Xmap);
    Serial.print(", ");
    Serial.print(Ymap);
    Serial.print(", ");
    Serial.print(Zmap);
    
    Serial.println();
  #endif

    //delay(1000);  //TEMP
    _adxl335AccelSamplePrevMillis = millis();               //store the current time
  } //END timed sample loop


  //auto-setting values, pushes min/max higher/lower if needed
//  if(_accelerationCur.x > _accelerationCur.xMax) {
//    _accelerationCur.xMax = _accelerationCur.x;
//  } else if(_accelerationCur.x < _accelerationCur.xMin) {
//    _accelerationCur.xMin = _accelerationCur.x;
//  }
//  if(_accelerationCur.y > _accelerationCur.yMax) {
//    _accelerationCur.yMax = _accelerationCur.y;
//  } else if(_accelerationCur.y < _accelerationCur.yMin) {
//    _accelerationCur.yMin = _accelerationCur.y;
//  }
//  if(_accelerationCur.z > _accelerationCur.zMax) {
//    _accelerationCur.zMax = _accelerationCur.z;
//  } else if(_accelerationCur.z < _accelerationCur.zMin) {
//    _accelerationCur.zMin = _accelerationCur.z;
//  }

////convert read values to degrees -90 to 90 - Needed for atan2
//  int xDeg90 = map(rX, _acceleration.xMin, _acceleration.xMax, -90, 90);
//  int yDeg90 = map(rY, _acceleration.yMin, _acceleration.yMax, -90, 90);
//  int zDeg90 = map(rZ, _acceleration.zMin, _acceleration.zMax, -90, 90);
////Caculate 360deg values like so: atan2(-yAng, -zAng)
////atan2 outputs the value of -π to π (radians)
////We are then converting the radians to degrees
//  _xDeg360 = RAD_TO_DEG * (atan2(-yDeg90, -zDeg90) + PI);
//  _yDeg360 = RAD_TO_DEG * (atan2(-xDeg90, -zDeg90) + PI);
//  _zDeg360 = RAD_TO_DEG * (atan2(-yDeg90, -xDeg90) + PI);

//  #ifdef DEBUG
//    Serial.print(rX);
//    Serial.print(", ");
//    Serial.print(rY);
//    Serial.print(", ");
//    Serial.print(rZ);
//    Serial.print(" - ");
//    Serial.print(xDeg90);
//    Serial.print(", ");
//    Serial.print(yDeg90);
//    Serial.print(", ");
//    Serial.print(zDeg90);
//    Serial.print(" - ");
//    Serial.print(_xDeg360);
//    Serial.print(", ");
//    Serial.print(_yDeg360);
//    Serial.print(", ");
//    Serial.print(_zDeg360);
//    Serial.println();
//  #endif

} //END readADXL335

boolean _nxo[6]; //nearest orientation matrix
boolean _nyo[6];
boolean _nzo[6];
    
void readADXL335b() {
  
  long adxl355AccelCurMillis = millis();   //get current time
  
  if((long) (adxl355AccelCurMillis - _adxl335AccelSamplePrevMillis) >= _adxl335AccelSampleInterval) {
  
    unsigned long XSum2 = 0;  //temp vars for calculations
    unsigned long YSum2 = 0;
    unsigned long ZSum2 = 0;
    
    for (int i=0; i<_adxl335AccelSampleTotal; i++) {
      XSum2 += analogRead(_adxl335xyzPin[0]);
      YSum2 += analogRead(_adxl335xyzPin[1]);
      ZSum2 += analogRead(_adxl335xyzPin[2]);
    }
    _adxl335AccelCur.x = XSum2/_adxl335AccelSampleTotal;  //100
    _adxl335AccelCur.y = YSum2/_adxl335AccelSampleTotal;
    _adxl335AccelCur.z = ZSum2/_adxl335AccelSampleTotal;

    #ifdef DEBUG_ADXL335
    Serial.print("ADXL335 XYZ averages: ");
    Serial.print(_adxl335AccelCur.x);
    Serial.print(", ");
    Serial.print(_adxl335AccelCur.y);
    Serial.print(", ");
    Serial.print(_adxl335AccelCur.z);
    Serial.println();
    #endif
  
    //_orientationCalibrationXYZ[orientation0-5][XYZ]
    //_orientationCalibrationXYZminMaxDist[XYZ][min,max,dist]

    //int nx = nearestOrientationIndex(0, _adxl335AccelCur.x, _orientationCalibrationXYZ[][], 6);
//    boolean nxo[6]; //nearest orientation matrix
//    boolean nyo[6];
//    boolean nzo[6];

    for (int i = 0; i < 6; i++) {
      if (_adxl335AccelCur.x < (_orientationCalibrationXYZ[i][0] + _orientationCalibrationXYZminMaxDist[0][2]) && 
          _adxl335AccelCur.x > (_orientationCalibrationXYZ[i][0] - _orientationCalibrationXYZminMaxDist[0][2]) ) {
        _nxo[i] = true;
      } else {
        _nxo[i] = false;
      }
      if (_adxl335AccelCur.y < (_orientationCalibrationXYZ[i][1] + _orientationCalibrationXYZminMaxDist[1][2]) && 
          _adxl335AccelCur.y > (_orientationCalibrationXYZ[i][1] - _orientationCalibrationXYZminMaxDist[1][2]) ) {
        _nyo[i] = true;
      } else {
        _nyo[i] = false;
      }
      if (_adxl335AccelCur.z < (_orientationCalibrationXYZ[i][2] + _orientationCalibrationXYZminMaxDist[2][2]) && 
          _adxl335AccelCur.z > (_orientationCalibrationXYZ[i][2] - _orientationCalibrationXYZminMaxDist[2][2]) ) {
        _nzo[i] = true;
      } else {
        _nzo[i] = false;
      }
    }

    for (int u = 0; u < 6; u++) {
      if (_nxo[u] == true && _nyo[u] == true && _nzo[u] == true) {
        _orientation = u;
      } 
    }
  
    #ifdef DEBUG_ADXL335
    Serial.print("Orientation: ");
    Serial.print(_orientation);
//    if(_indicatorRightActive == true) {
//      Serial.print(", right indicator!, ");
//    } else if(_indicatorLeftActive == true) {
//      Serial.print(", left indicator!, ");
//    }
    Serial.println();
    #endif

    _adxl335AccelSamplePrevMillis = millis();               //store the current time
  } //END timed sample loop
} //END readADXL335b

//int nearestOrientationIndex(int xyz, int x, int myArray[][], int total)
//{
//  int index = 0; // by default near first element
//
//  int distance = abs(myArray[xyz][index] - x);
//  for (int i = 1; i < total; i++)
//  {
//    int d = abs(myArray[xyz][i] - x);
//    if (d < distance)
//    {
//      index = i;
//      distance = d;
//    }
//  }
//  return index;
//}

/*----------------------------sensor bits----------------------------*/

//void  getOrientation() {
//  //
//}

void setOrientation(int o) {
  //overide function for testing (if needed)
  _orientation = o;
}

