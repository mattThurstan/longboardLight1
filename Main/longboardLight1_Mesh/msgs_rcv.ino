/*----------------------------messages - receiving----------------------------*/

/*----------------------------internal mesh messages - get passed to/from MQTT broker by bridge----------------------------*/
void receiveMessage(uint32_t from, String msg)
{
  uint8_t firstMsgIndex = msg.indexOf(':');
  String targetSub = msg.substring(0, firstMsgIndex);
  String msgSub = msg.substring(firstMsgIndex+1);

/*
  //solidColor
  //solidColor2
  //sleep
  //breathe
  //testMode // turn test modes on/off
  //curTestMode // set surrent test mode by number
  //curTestModeName // set surrent test mode by name
  //subMode
  //subModeName
  //globalBrightness
  quickCalibration
  fullCalibration
  resetDefaults
  //wifiOff
  //head
  //headBrightness
  //rear
  //rearBrightness
  //brake
  //indicate
*/
  
  if (targetSub == "lights/rgb/set") {
  
    // get the position of the first and second commas
    uint8_t firstIndex = msgSub.indexOf(',');
    uint8_t lastIndex = msgSub.lastIndexOf(',');
    
    uint8_t rgb_red = msgSub.substring(0, firstIndex).toInt();
    if (rgb_red < 0 || rgb_red > 255) { return; } 
    //else { tempRGB.r = rgb_red; }
    
    uint8_t rgb_green = msgSub.substring(firstIndex + 1, lastIndex).toInt();
    if (rgb_green < 0 || rgb_green > 255) { return; } 
    
    uint8_t rgb_blue = msgSub.substring(lastIndex + 1).toInt();
    if (rgb_blue < 0 || rgb_blue > 255) { return; } 

    setSolidColor(rgb_red, rgb_green, rgb_blue);
  }
  else if (targetSub == "lights/rgb2/set") {
  
    // get the position of the first and second commas
    uint8_t firstIndex = msgSub.indexOf(',');
    uint8_t lastIndex = msgSub.lastIndexOf(',');
    
    uint8_t rgb_red = msgSub.substring(0, firstIndex).toInt();
    if (rgb_red < 0 || rgb_red > 255) { return; } 
    
    uint8_t rgb_green = msgSub.substring(firstIndex + 1, lastIndex).toInt();
    if (rgb_green < 0 || rgb_green > 255) { return; }
    
    uint8_t rgb_blue = msgSub.substring(lastIndex + 1).toInt();
    if (rgb_blue < 0 || rgb_blue > 255) { return; }

    setSolidColor2(rgb_red, rgb_green, rgb_blue);
  }
  else if (targetSub == "sleep/set") {
    if      (msgSub == LIGHTS_ON)   { setSleep(1); }
    else if (msgSub == LIGHTS_OFF)  { setSleep(0); }
  }  
  else if (targetSub == "lights/testmode/switch") {
    if      (msgSub == LIGHTS_ON)   { setTestMode(1); }
    else if (msgSub == LIGHTS_OFF)  { setTestMode(0); }
  } 
  else if (targetSub == "lights/brightness/set") {
    uint8_t brightness = msgSub.toInt();
    if (brightness < 0 || brightness > 255) { /* do nothing... */ return; } 
    else { setGlobalBrightness(brightness); }
  }
  else if (targetSub == "debug/wifioff") {
    //if (msgSub == LIGHTS_ON) {  }
    //else if (msgSub == LIGHTS_OFF) {  }
  } 
  else if (targetSub == "lights/head/set") {
    if      (msgSub == LIGHTS_ON)   { setHead(1); }
    else if (msgSub == LIGHTS_OFF)  { setHead(0); }
  } 
  else if (targetSub == "lights/head/brightness/set") {
    uint8_t brightness = msgSub.toInt();
    if (brightness < 0 || brightness > 255) { /* do nothing... */ return; } 
    else { setHeadBrightness(brightness); }
  } 
  else if (targetSub == "lights/rear/set") {
    if      (msgSub == LIGHTS_ON)   { setRear(1); }
    else if (msgSub == LIGHTS_OFF)  { setRear(0); }
  } 
  else if (targetSub == "lights/rear/brightness/set") {
    uint8_t brightness = msgSub.toInt();
    if (brightness < 0 || brightness > 255) { /* do nothing... */ return; } 
    else { setRearBrightness(brightness); }
  } 
  else if (targetSub == "lights/brake/set") {
    if      (msgSub == LIGHTS_ON)   { setBrake(1); }
    else if (msgSub == LIGHTS_OFF)  { setBrake(0); }
  }
  else if (targetSub == "lights/indicate/set") {
    if      (msgSub == LIGHTS_ON)   { setIndicate(1); }
    else if (msgSub == LIGHTS_OFF)  { setIndicate(0); }
  }
/*----------------------------messages - receiving - modes-----------------*/
  else if (targetSub == "lights/submode") {
    if (msgSub == "Blank") { 
      setMainLightsSubMode(0);
      //setMainLightsSubModeByName(msgSub);
    } else if (msgSub == "Glow") { 
      setMainLightsSubMode(1);
    } else if (msgSub == "Gradient") {
      setMainLightsSubMode(2);
    } else if (msgSub == "Track") {
      setMainLightsSubMode(3);
    } else if (msgSub == "mattKsp Test") {
      setMainLightsSubMode(4);
    } else if (msgSub == "Solid Colour") { 
      setMainLightsSubMode(5);
    } else if (msgSub == "Gradient w Colours") { 
      setMainLightsSubMode(6);
    } else if (msgSub == "Track w Colour") {
      setMainLightsSubMode(7);
    } else if (msgSub == "Rainbow") {
      setMainLightsSubMode(8);
    } else if (msgSub == "Confetti") {
      setMainLightsSubMode(9);
    } else if (msgSub == "Sinelon") {
      setMainLightsSubMode(10);
    } else if (msgSub == "BPM") {
      setMainLightsSubMode(11);
    } else if (msgSub == "Juggle") {
      setMainLightsSubMode(12);
    } else if (msgSub == "Rainbow w Colour") {
      setMainLightsSubMode(13);
    } else if (msgSub == "Rainbow glitter w Colour") {
      setMainLightsSubMode(14);
    } else if (msgSub == "Confetti w Colour") {
      setMainLightsSubMode(15);
    } else if (msgSub == "Sinelon w Colour") {
      setMainLightsSubMode(16);
    } else if (msgSub == "Cylon") {
      setMainLightsSubMode(17);
    } else if (msgSub == "Cylon w Colour") {
      setMainLightsSubMode(18);
    } else { }
    
    //publishSubMode(true);  // too much bounce-back on the network
  }
  else if (targetSub == "lights/testmode") {
    if (msgSub == "Blank") { 
      setCurTestMode(0);
    } else if (msgSub == "SubModes") { 
      setCurTestMode(1);
    } else if (msgSub == "Tracked lights") { 
      setCurTestMode(2);
    } else if (msgSub == "Emergency flash") { 
      setCurTestMode(3);
    } else if (msgSub == "Breathing") { 
      setCurTestMode(4);
    } else if (msgSub == "Left side lights") { 
      setCurTestMode(5);
    } else if (msgSub == "Right side lights") { 
      setCurTestMode(6);
    } else if (msgSub == "Head lights") { 
      setCurTestMode(7);
    } else if (msgSub == "Rear lights") { 
      setCurTestMode(8);
    } else if (msgSub == "Left indicator flash") { 
      setCurTestMode(9);
    } else if (msgSub == "Right indicator flash") { 
      setCurTestMode(10);
    } else if (msgSub == "Orientation") { 
      setCurTestMode(11);
    } else { }
    
    //publishTestMode(true);  // too much bounce-back on the network
  }
/*----------------------------messages - receiving - sunrise/sunset---------*/
  else if (targetSub == "sunrise") {
    // trigger only (global synced)
    if      (msgSub == LIGHTS_ON)   { /* start sunrise */ }
    else if (msgSub == LIGHTS_OFF)  { /* stop sunrise and revert to previous setting */ }
  }
  else if (targetSub == "lights/sunrise") {
    // trigger only (local)
    // note: the single mesh msg of 'sunrise' is used for synced global sunrise
    if      (msgSub == LIGHTS_ON)   { /* start sunrise */ }
    else if (msgSub == LIGHTS_OFF)  { /* stop sunrise and revert to previous setting */ }
    //else if (msgSub == "receive a time for sunrise to happen at") { /* set sunrise time */ }
  }
  else if (targetSub == "sunset") {
    // trigger only (global synced)
    if      (msgSub == LIGHTS_ON)   { /* start sunset */ }
    else if (msgSub == LIGHTS_OFF)  { /* stop sunset and revert to previous setting */ }
  }
  else if (targetSub == "lights/sunset") {
    // trigger only (local)
    // note: the single mesh msg of 'sunset' is used for synced global sunset
    if      (msgSub == LIGHTS_ON)   { /* start sunset */ }
    else if (msgSub == LIGHTS_OFF)  { /* stop sunset and revert to previous setting */ }
    //else if (msgSub == "receive a time for sunset to happen at") { /* set sunset time */ }
  }
/*----------------------------messages - receiving - breathing--------------*/
/* Breath : (noun) Refers to a full cycle of breathing. It can also refer to the air that is inhaled or exhaled */
  else if (targetSub == "breath") {
    // trigger only (global synced)
    // note: the single mesh msg of 'breath' is used for synced global breathing
    if (msgSub == LIGHTS_ON) {
      _isBreathingSynced = true;                    // set sync to global
      setBreathe(1);
    }
    else if (msgSub == LIGHTS_OFF) {
      _isBreathingSynced = false;                   // set sync to local
      setBreathe(0);
    }
  }
  else if (targetSub == "lights/breath") {
    // trigger only (local)
    // note: the single mesh msg of 'breath' is used for synced global breathing
    if (msgSub == LIGHTS_ON) {
      _isBreathingSynced = false;                   // set sync to local
      setBreathe(1);
    }
    else if (msgSub == LIGHTS_OFF) {
      _isBreathingSynced = false;                   // set sync to local
      setBreathe(0);
    }
  }
  else if (targetSub == "lights/breath/xyz/set") {
    // msg will contain xyz coords for origin position within the house
    //
    //publishLightsBreathXyzSet(false);
  }
  else if (targetSub == "lights/breath/xyz/mode") {
    // set positional mode
    if (msgSub == "Independent") { }
    else if (msgSub == "Global") { }
    //publishLightsBreathXyzMode(false);
  }
/*----------------------------messages - receiving - debug------------------*/
  else if(targetSub == "debug/general/set") {
    if (msgSub == LIGHTS_ON) { DEBUG_GEN = true; } 
    else if (msgSub == LIGHTS_OFF) { DEBUG_GEN = false; }
    publishDebugGeneralState(false);
  }
  else if (targetSub == "debug/overlay/set") {
    if      (msgSub == LIGHTS_ON)   { DEBUG_OVERLAY = true; }
    else if (msgSub == LIGHTS_OFF)  { DEBUG_OVERLAY = false; }
    publishDebugOverlayState(false);
  }
  else if (targetSub == "debug/meshsync/set") {
    if      (msgSub == LIGHTS_ON)   { DEBUG_MESHSYNC = true; }
    else if (msgSub == LIGHTS_OFF)  { DEBUG_MESHSYNC = false; }
    publishDebugMeshsyncState(false);
  }
  else if(targetSub == "debug/comms/set") {
    if      (msgSub == LIGHTS_ON)   { DEBUG_COMMS = true; } 
    else if (msgSub == LIGHTS_OFF)  { DEBUG_COMMS = false; }
    publishDebugCommsState(false);
  }
  else if(targetSub == "status/request") {
    if      (msgSub == LIGHTS_ON)   { publishStatusAll(false); }
  }
  
  if (DEBUG_COMMS) { Serial.print(targetSub); Serial.print(" : "); Serial.println(msgSub); }
}
