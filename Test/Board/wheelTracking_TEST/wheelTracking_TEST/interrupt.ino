/*----------------------------interrupt----------------------------*/
void setupInterrupts() {
  pinMode(_wheelSensorPin[0], INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(_wheelSensorPin[0]), wheelInterrupt0, CHANGE);  //on digital pin 2
}

void wheelInterrupt0() {
  
//    if ( PIND & ( 1<<PD2 ) ) {
//      Serial.print("HIGH");
//      Serial.println();
//    } else {
//      Serial.print("LOW");
//      Serial.println();
//    }
  
  _wheelCounter++;
  
  //not sure if arduino pro mini 328 5V 16MHz is fast enough to do this ???
  if (_directionCur == 0) { _ledMovePos++; /*forward*/ }
  else if (_directionCur == 1) { _ledMovePos--; /*backward*/ }

  //_ledMovePos++;
}


