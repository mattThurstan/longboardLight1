/*----------------------------sensors----------------------------*/

void setupSensors() {
  //setting up wheel pin using '_wheelSensorPin' because hard to handle interrupts from inside a library.
  wheels.Init(0, _wheelSensorPin[0], 0.0345, 8); //currently only using 1 sensor/wheel. 
  //wheels.SetupWheel(1, _wheelSensorPin[1], 0.0345, 8);  
  //setupMPU6050();
  orient.Init(); //need to pass values if any available from memory
  //orient.InitWithVars(a[3], b[3]);
}

/* Sensor input loop (called from Main loop) */
void loopSensors() {
  readMPU6050filtered();
  orientation();
  loopWheel();
}

/*----------------------------wheel sensors----------------------------*/

/* Wheel tracking */
void loopWheel() {
  #ifdef DATA_LOGGING 
    EVERY_N_MILLISECONDS(1000) {                     //FastLED based non-blocking delay to update/display the sequence.
      detachInterrupt(0);     //..no interrupts whilst we process !!!
      wheels.DoUpdate(_directionCur); // !!! need to swap '_directionCur' for a direct line to the mpu6050 library var !!!
      attachInterrupt(digitalPinToInterrupt(_wheelSensorPin[0]), wheelInterrupt0, CHANGE);  //re-attach the interrupt !!!
    } //END timed-loop
  #endif  //END if DATA_LOGGING
} //END loopWheel

