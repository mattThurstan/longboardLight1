/*----------------------------util----------------------------*/

void setupSerial() {


    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    //Serial.begin(38400);                    //for doing MPU6050 stuff - don't really need it this fast cos not controlling it over serial
  
  #ifdef DEBUG
    Serial.begin(9600);                     //usb
    _inputString.reserve(200);              // reserve 200 bytes for the inputString:
    Serial.println();
    Serial.print(_progName);
    Serial.print(" ");
    Serial.print(_progVers);
    Serial.println();
    Serial.print("..");
    Serial.println();
  #endif
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
#ifdef DEBUG
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();      // get the new byte:
    _inputString += inChar;                 // add it to the inputString:
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      _stringComplete = true;
    }
  }
}
#endif

