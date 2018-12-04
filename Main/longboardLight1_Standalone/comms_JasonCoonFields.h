/*----------------------------comms - Jason Coon Fields (pt2)----------------------------*/
// comms_JasonCoonFields.h  has to be defined AFTER patterns have been setup

/*----------------------------comms - get----------------------------*/
String getSleep() {
  return String(mA.sleep);
}

String getBreathe() {
  return String(mE.breathe);
}

String getHead() {
  return String(mE.head);
}

String getRear() {
  return String(mE.rear);
}

String getBrake() {
  return String(mE.brake);
}

String getIndicate() {
  return String(mE.indicate);
}

String getGlobalBrightness() {
  return String(_ledGlobalBrightnessCur);
}

String getHeadBrightness() {
  //return String(_headLightsBrightness);
  return String(_headLightsColHSV.val);
}

String getRearBrightness() {
  //return String(_rearLightsBrightness);
  return String(_rearLightsColHSV.val);
}

String getTrackLightsFadeAmount() {
  return String(_trackLightsFadeAmount);
}

String getSubMode() {
  return String(_mainLightsSubMode);
}

String getSubModes() {
  String json = "";

  for (uint8_t i = 0; i < _mainLightsSubModeTotal; i++) {
    json += "\"" + patterns[i].name + "\"";
    if (i < _mainLightsSubModeTotal - 1)
      json += ",";
  }

  return json;
}

String getSolidColor() {
  return String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b);
}
String getSolidColor2() {
  return String(solidColor2.r) + "," + String(solidColor2.g) + "," + String(solidColor2.b);
}

//test modes
String getTestMode() {
  return String(_testMode);
}
 
String getCurTestMode() {
  return String(_curTestMode);
}
String _curTestModesListName[] = { "Blank", "Main lights (sub-modes)", "Tracked lights (basic)", "Emergency flash", "Breathing", "Left side lights", "Right side lights", "Head lights", "Rear lights", "Left indicator flash", "Right indicator flash", "Orientation" };
String getCurTestModeList() {
  String json = "";
  
  json += "\"Blank\",";
  json += "\"Main lights (sub-modes)\",";
  json += "\"Tracked lights (basic)\",";
  json += "\"Emergency flash\",";
  json += "\"Breathing\",";
  json += "\"Left side lights\",";
  json += "\"Right side lights\",";
  json += "\"Head lights\",";
  json += "\"Rear lights\",";
  json += "\"Left indicator flash\","; //not implemented yet
  json += "\"Right indicator flash\","; //not implemented yet
  json += "\"Orientation\"";
  
  //json += "{\"name\":\"wheel\",\"label\":\"Wheel\",\"type\":\"Section\"},";
  
  return json;
}

String getQuickCalibration() {
  return String(_doQuickCalibration);
}

String getFullCalibration() {
  return String(_doFullCalibration);
}

//board profiles - not done yet
//String getBoardProfile() {
//  return String(_boardProfile);
//}
 
String getCurBoardProfile() {
  return String(_curBoardProfile);
}
String _curBoardProfilesListName[] = { "Blank", "Dervish Sama", "Drop-down" };
String getCurBoardProfileList() {
  String json = "";
  
  json += "\"Blank\",";
  json += "\"Dervish Sama\",";
  json += "\"Drop-down\",";
  
  return json;
}

/*----------------------------comms - fields list----------------------------*/
FieldList fields = {
  { "main", "Main", SectionFieldType },
  { "globalBrightness", "Global Brightness", NumberFieldType, 1, 255, getGlobalBrightness },
  { "subMode", "Sub-Mode", SelectFieldType, 0, _mainLightsSubModeTotal, getSubMode, getSubModes },
  { "solidColor", "Color 1 or Left side", SectionFieldType },
  { "solidColor", "Color 1", ColorFieldType, 0, 255, getSolidColor },
  { "solidColor2", "Color 2 or Right side", SectionFieldType },
  { "solidColor2", "Color 2", ColorFieldType, 0, 255, getSolidColor2 },
  { "extras", "Extras", SectionFieldType },
  { "trackLightsFadeAmount", "Tracking Dot Trail Length", NumberFieldType, 1, 255, getTrackLightsFadeAmount },
  { "breathe", "Breathing", BooleanFieldType, 0, 1, getBreathe },
//  { "palette", "Palette", SelectFieldType, 0, paletteCount, getPalette, getPalettes },
//  { "speed", "Speed", NumberFieldType, 1, 255, getSpeed },
//  { "autoplay", "Autoplay", SectionFieldType },
//  { "autoplay", "Autoplay", BooleanFieldType, 0, 1, getAutoplay },
//  { "autoplayDuration", "Autoplay Duration", NumberFieldType, 0, 255, getAutoplayDuration },

  //disable test modes when done
  { "test", "Test", SectionFieldType },
  { "testMode", "Test Modes", BooleanFieldType, 0, 1, getTestMode },
  { "curTestMode", "Current Test Mode", SelectFieldType, 0, 11, getCurTestMode, getCurTestModeList },

  //these shouldn't get shown on the main page..
  //{ "sleep", "Sleep", SectionFieldType },
  { "sleep", "Sleep", BooleanFieldType, 0, 1, getSleep },
  { "head", "Head Lights", BooleanFieldType, 0, 1, getHead },
  { "headBrightness", "Brightness", NumberFieldType, 1, 255, getHeadBrightness },
  { "rear", "Rear Lights", BooleanFieldType, 0, 1, getRear },
  { "rearBrightness", "Brightness", NumberFieldType, 1, 255, getRearBrightness },
  { "brake", "Brake Lights", BooleanFieldType, 0, 1, getBrake },
  { "indicate", "Indicator Lights", BooleanFieldType, 0, 1, getIndicate },
  { "quickCalibration", "Quick Calibration", BooleanFieldType, 0, 1, getQuickCalibration },
  { "fullCalibration", "Full Calibration", BooleanFieldType, 0, 1, getFullCalibration },
  //{ "boardProfile", "Board Profiles", BooleanFieldType, 0, 1, getBoardProfile },
  //{ "curBoardProfile", "Current Board Profile", SelectFieldType, 0, 11, getCurBoardProfile, getCurBoardProfileList },
};

uint8_t fieldCount = ARRAY_SIZE(fields);

