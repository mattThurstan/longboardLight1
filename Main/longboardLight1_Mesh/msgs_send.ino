/*----------------------------messages - sending----------------------------*/
void meshSendSingle(String msg, bool save) {
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { shouldSaveSettings = true; }
}
void publishMeshMsgSingleState(String msg, boolean state, bool save) {
  msg += ":"; //..just so we are all sure what is going on here !?
  if (state == false) {  msg += "OFF"; }
  else if (state == true) { msg += "ON"; }
  meshSendSingle(msg, save);
}
void publishMeshMsgSingleString(String msg1, String msg2, bool save) {
  msg1 += ":"; //..just so we are all sure what is going on here !?
  msg1 += msg2;
  meshSendSingle(msg1, save);
}
void publishMeshMsgSingleColor(String msg, uint8_t r, uint8_t g, uint8_t b, bool save) {
  msg1 += ":"; //..just so we are all sure what is going on here !?
  msg += String(r);
  msg += ",";
  msg += String(g);
  msg += ",";
  msg += String(b);
  meshSendSingle(msg, save);
}

void publishSolidColor(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishSolidColor "); }
  publishMeshMsgSingleColor("lights/rgb/status", solidColor.r, solidColor.g, solidColor.b, save)
}

void publishSolidColor2(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishSolidColor2 "); }
  publishMeshMsgSingleColor("lights/rgb2/status", solidColor2.r, solidColor2.g, solidColor2.b, save)
}
void publishSleep(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishSleep "); }
  boolean state = false;
  if (mE.sleep == 0) { state = false; } 
  else if (mE.sleep == 1) { state = true; }
  publishMeshMsgSingleState("sleep/status", state, save);
}

void publishBrightness(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishBrightness "); }
  publishMeshMsgSingleString("lights/brightness/status", ((String)_ledGlobalBrightnessCur), save);
}

void publishLightsHead(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishLightsHead "); }
  boolean state = false;
  if (mE.head == 0) { state = false; } 
  else if (mE.head == 1) { state = true; }
  publishMeshMsgSingleState("lights/head/status", state, save);
}

void publishHeadBrightness(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishHeadBrightness "); } 
  publishMeshMsgSingleString("lights/head/brightness/status", ((String)_headLightsColHSV.val), save);
}

void publishLightsRear(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishLightsRear "); }
  boolean state = false;
  if (mE.rear == 0) { state = false; } 
  else if (mE.rear == 1) { state = true; }
  publishMeshMsgSingleState("lights/rear/status", state, save);
}

void publishRearBrightness(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishRearBrightness "); } 
  publishMeshMsgSingleString("lights/rear/brightness/status", ((String)_rearLightsColHSV.val, save);
}

void publishLightsBrake(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishLightsBrake "); }
  boolean state = false;
  if (mE.brake == 0) { state = false; } 
  else if (mE.brake == 1) { state = true; }
  publishMeshMsgSingleState("lights/brake/status", state, save);
}

void publishLightsIndicate(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishLightsIndicate "); }
  boolean state = false;
  if (mE.indicate == 0) { state = false; } 
  else if (mE.indicate == 1) { state = true; }
  publishMeshMsgSingleState("lights/indicate/status", state, save);
}

/*----------------------------messages - sending - modes--------------------*/
void publishMainLightsSubMode(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishMainLightsSubMode "); }
  publishMeshMsgSingleString("lights/submode", modeName[_modeCur], save);
}

void publishLightsTestMode(bool save) {
  // onOff
  if (DEBUG_COMMS) { Serial.print("publishLightsTestMode "); }
  boolean state = false;
  if (_testMode == 0) { state = false; } 
  else if (_testMode == 1) { state = true; }
  publishMeshMsgSingleState("lights/testmode/status", state, save);
}

void publishLightsCurTestMode(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishLightsCurTestMode "); }
  publishMeshMsgSingleString("lights/testmode", _curTestModesListName[_curTestMode], save);
}

/*----------------------------messages - sending - sunrise/sunset-----------*/
/*----------------------------messages - sending - breathing----------------*/
void publishBreath(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishBreath "); }
  boolean state = false;
  if (mE.breathe == 0) { state = false; } 
  else if (mE.breathe == 1) { state = true; }
  publishMeshMsgSingleState("breath", state, save);
}

void publishLightsBreath(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishLightsBreath "); }
  boolean state = false;
  if (mE.breathe == 0) { state = false; } 
  else if (mE.breathe == 1) { state = true; }
  publishMeshMsgSingleState("lights/breath", state, save);
}

/*----------------------------messages - sending - debug--------------------*/
void publishDebugGeneralState(bool save) {
  if (DEBUG_COMMS) { Serial.print("publishDebugGeneralState "); }
  publishMeshMsgSingleState("debug/general/status", DEBUG_GEN, save);
//  String msg = "debug/general/status";
//  msg += ":"; //..just so we are all sure what is going on here !?
//  if (DEBUG_GEN == false) {
//    msg += "OFF";
//  } else if (DEBUG_GEN == true) {
//    msg += "ON";
//  }
//  mesh.sendSingle(id, msg);
//  if (DEBUG_COMMS) { Serial.println(msg); }
//  if (save == true) { shouldSaveSettings = true; }
}

void publishDebugOverlayState(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishDebugOverlayState "); }
  publishMeshMsgSingleState("debug/overlay/status", DEBUG_OVERLAY, save);
//  String msg = "debug/overlay/status";
//  msg += ":"; //..just so we are all sure what is going on here !?
//  if (DEBUG_OVERLAY == false) {
//    msg += "OFF";
//  } else if (DEBUG_OVERLAY == true) {
//    msg += "ON";
//  }
//  mesh.sendSingle(id, msg);
//  if (DEBUG_COMMS) { Serial.println(msg); }
//  if (save == true) { shouldSaveSettings = true; }
}

void publishDebugMeshsyncState(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishDebugMeshsyncState "); }
  publishMeshMsgSingleState("debug/meshsync/status", DEBUG_MESHSYNC, save);
//  String msg = "debug/meshsync/status";
//  msg += ":"; //..just so we are all sure what is going on here !?
//  if (DEBUG_MESHSYNC == false) {
//    msg += "OFF";
//  } else if (DEBUG_MESHSYNC == true) {
//    msg += "ON";
//  }
//  mesh.sendSingle(id, msg);
//  if (DEBUG_COMMS) { Serial.println(msg); }
//  if (save == true) { shouldSaveSettings = true; }
}

void publishDebugCommsState(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishDebugCommsMeshsyncState "); }
  publishMeshMsgSingleState("debug/comms/status", DEBUG_COMMS, save);
//  String msg = "debug/comms/status";
//  msg += ":"; //..just so we are all sure what is going on here !?
//  if (DEBUG_COMMS == false) {
//    msg += "OFF";
//  } else if (DEBUG_COMMS == true) {
//    msg += "ON";
//  }
//  mesh.sendSingle(id, msg);
//  if (DEBUG_COMMS) { Serial.println(msg); }
//  if (save == true) { shouldSaveSettings = true; }
}

void publishStatusAll(bool save) {
  if (DEBUG_COMMS) { Serial.println("publishStatusAll "); }
  publishSolidColor(false);
  publishSolidColor2(false);
  publishSleep(false);
  publishBrightness(false);
  publishLightsHead(false);
  publishHeadBrightness(false);
  publishLightsRear(false);
  publishRearBrightness(false);
  publishLightsBrake(false);
  publishLightsIndicate(false);
  publishMainLightsSubMode(false);
  publishLightsTestMode(false);
  publishLightsCurTestMode(false);
  //publishBreath(false);
  //publishLightsBreath(false);
  publishDebugGeneralState(false);
  publishDebugOverlayState(false);
  publishDebugMeshsyncState(false);
  publishDebugCommsState(false);
}
