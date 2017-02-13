/*----------------------------sensors----------------------------*/

void setupSensors() {
  setupWheel();                      //setup wheel tracking
  setupMPU6050();
}

/* sensor input loop
 * called from main loop
 */
void loopSensors() {
  loopWheel();                       //loop wheel tracking
  readMPU6050filtered();
  orientation();
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

