/*----------------------------interrupt----------------------------*/
//attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);  //   RISING  FALLING  LOW?  //use this one ???
//attachInterrupt(interruptPin, blink, CHANGE);
/*
 * https://www.arduino.cc/en/Reference/PortManipulation
 * PORTD maps to Arduino digital pins 0 to 7
 * 
 * DDRD - The Port D Data Direction Register - read/write
 * PORTD - The Port D Data Register - read/write
 * PIND - The Port D Input Pins Register - read only 
 * 
 * PORTB maps to Arduino digital pins 8 to 13 The two high bits (6 & 7) map to the crystal pins and are not usable
 * 
 * DDRB - The Port B Data Direction Register - read/write
 * PORTB - The Port B Data Register - read/write
 * PINB - The Port B Input Pins Register - read only 
 * 
 * PORTC maps to Arduino analog pins 0 to 5. Pins 6 & 7 are only accessible on the Arduino Mini
 * 
 * DDRC - The Port C Data Direction Register - read/write
 * PORTC - The Port C Data Register - read/write
 * PINC - The Port C Input Pins Register - read only 
 */
    
void setupInterrupts() {
  pinMode(_wheelSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(_wheelSensorPin), wheelInterrupt0, CHANGE);  //on digital pin 2
}

void wheelInterrupt0() {
  //if..
  //get port read index for D
  //bitshift to pin 2
  //check if HIGH
  //(this should be fast)
  //might have to use a faster processor to do stuff like check North/South to tell if we have skipped one etc.
  #if DEBUG_INTERRUPT
    if ( PIND & ( 1<<PD2 ) ) {
      Serial.print("HIGH");
      Serial.println();
    } else {
      Serial.print("LOW");
      Serial.println();
    }
  #endif
  
  #ifdef DATA_LOGGING 
    w.IncrementCounter();  //this is clocking up the wheel counter, which is then used to calculate speed etc.
    //w.DecrementCounter(0);
  #endif

  //not sure if arduino pro mini 328 5V 16MHz is fast enough to do this ??? - seems ok so far..
  if (o.GetDirection() == 0) { _ledMovePos++; /*forward*/ }
  else if (o.GetDirection() == 1) { _ledMovePos--; /*backward*/ }
}


