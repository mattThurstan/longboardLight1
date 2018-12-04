/*----------------------------comms - Jason Coon Field (pt1)----------------------------*/
typedef String (*FieldSetter)(String);
typedef String (*FieldGetter)();

const String NumberFieldType = "Number";
const String BooleanFieldType = "Boolean";
const String ButtonFieldType = "Button";      //used for a on-way single press eg. put board to sleep and turn off WIFI, does not require feedback.
const String SelectFieldType = "Select";
const String ColorFieldType = "Color";
const String SectionFieldType = "Section";

typedef struct Field {
  String name;
  String label;
  String type;
  uint8_t min;
  uint8_t max;
  FieldGetter getValue;
  FieldGetter getOptions;
  FieldSetter setValue;
};

typedef Field FieldList[];

Field getField(String name, FieldList fields, uint8_t count) {
  for (uint8_t i = 0; i < count; i++) {
    Field field = fields[i];
    if (field.name == name) {
      return field;
    }
  }
  return Field();
}

String getFieldValue(String name, FieldList fields, uint8_t count) {
  Field field = getField(name, fields, count);
  if (field.getValue) {
    return field.getValue();
  }
  return String();
}

String setFieldValue(String name, String value, FieldList fields, uint8_t count) {
  Field field = getField(name, fields, count);
  if (field.setValue) {
    return field.setValue(value);
  }
  return String();
}

/*----------------------------comms - Jason Coon Field (pt1) - setup package requests for web----------------------------*/
String getMainFieldsJson(FieldList fields, uint8_t count) {
  String json = "[";

  for (uint8_t i = 0; i < count; i++) {
    Field field = fields[i];

    if (field.name == "sleep" || field.name == "head" || field.name == "rear" || field.name == "brake" || field.name == "indicate" || field.name == "headBrightness" || field.name == "rearBrightness" || field.name == "quickCalibration" || field.name == "fullCalibration" || field.name == "boardProfile" || field.name == "curBoardProfile") { 
      //quick hack (sorry) cos i dont want these on the main page
    } else {
      json += "{\"name\":\"" + field.name + "\",\"label\":\"" + field.label + "\",\"type\":\"" + field.type + "\"";
  
      if(field.getValue) {
        if (field.type == ColorFieldType || field.type == "String") {
          json += ",\"value\":\"" + field.getValue() + "\"";
        }
        else {
          json += ",\"value\":" + field.getValue();
        }
      }
  
      if (field.type == NumberFieldType) {
        json += ",\"min\":" + String(field.min);
        json += ",\"max\":" + String(field.max);
      }
  
      if (field.getOptions) {
        json += ",\"options\":[";
        json += field.getOptions();
        json += "]";
      }
  
      json += "}";
  
      if (i < count - 10) {    // 1 + 11  how many do we need to miss off the end of the list? - hack
        json += ",";
      }
        
    }
  }

  json += "]";

  if (DEBUG_COMMS) { 
  Serial.print(F("/main HTTP_GET returns: "));
  Serial.println(json);
  }

  return json;
}

String getSafetyFieldsJson() {
  String json = "[";
  
//typedef struct {
//  uint8_t sleep;    //doesn't use Enabled
//  uint8_t breathe;  //doesn't use Active
//  uint8_t head;
//  uint8_t rear;
//  uint8_t brake;    //give the brake lights a slight brightness boost when decelerating
//  uint8_t indicate;
//} MODES_ENABLED_ACTIVE;

  json += "{\"name\":\"head\",\"label\":\"Head Lights\",\"type\":\"Section\"},";
  json += "{\"name\":\"head\",\"label\":\"Head Lights\",\"type\":\"Boolean\",\"value\":" + String(mE.head) + "},";
  //json += "{\"name\":\"headBrightness\",\"label\":\"Brightness\",\"type\":\"Number\",\"value\":" + String(_headLightsBrightness) + "},";
  json += "{\"name\":\"headBrightness\",\"label\":\"Brightness\",\"type\":\"Number\",\"value\":" + String(_headLightsColHSV.val) + "},";
  
  json += "{\"name\":\"rear\",\"label\":\"Rear Lights\",\"type\":\"Section\"},";
  json += "{\"name\":\"rear\",\"label\":\"Rear Lights\",\"type\":\"Boolean\",\"value\":" + String(mE.rear) + "},";
  //json += "{\"name\":\"rearBrightness\",\"label\":\"Brightness\",\"type\":\"Number\",\"value\":" + String(_rearLightsBrightness) + "},";
  json += "{\"name\":\"rearBrightness\",\"label\":\"Brightness\",\"type\":\"Number\",\"value\":" + String(_rearLightsColHSV.val) + "},";
  
  json += "{\"name\":\"other\",\"label\":\"Other Lights\",\"type\":\"Section\"},";
  json += "{\"name\":\"brake\",\"label\":\"Brake Lights\",\"type\":\"Boolean\",\"value\":" + String(mE.brake) + "},";
  json += "{\"name\":\"indicate\",\"label\":\"Indicator Lights\",\"type\":\"Boolean\",\"value\":" + String(mE.indicate) + "}";
  
  json += "]";

  if (DEBUG_COMMS) { 
  Serial.print(F("/safety HTTP_GET returns: "));
  Serial.println(json);
  }

  return json;
}

String getCalibrationFieldsJson()
{

  String json = "[";

  json += "{\"name\":\"quickCalibration\",\"label\":\"Quick Calibration\",\"type\":\"Section\"},";
  json += "{\"name\":\"quickCalibration\",\"label\":\"Quick Calibration\",\"type\":\"Boolean\",\"value\":" + String(_doQuickCalibration) + "},";
  json += "{\"name\":\"fullCalibration\",\"label\":\"Full Calibration\",\"type\":\"Section\"},";
  json += "{\"name\":\"fullCalibration\",\"label\":\"Full Calibration\",\"type\":\"Boolean\",\"value\":" + String(_doFullCalibration) + "}";
  
  json += "]";

  if (DEBUG_COMMS) { 
  Serial.print(F("/calibration HTTP_GET returns: "));
  Serial.println(json);
  }

  return json;
}

String getStatsFieldsJson() 
{
  String smaxMps = String(w.GetMaxMps());
  String sdistTraveledForward = String(w.GetDistTraveledForward());
  String sdistTraveledBackward = String(w.GetDistTraveledBackward());
  String vcc = String(ESP.getVcc());
  
  String json = "[";

  json += "{\"name\":\"wheel\",\"label\":\"Movement stats\",\"type\":\"Section\"},";
  json += "{\"name\":\"maxMps\",\"label\":\"Max speed reached (Meters Per Sec)\",\"type\":\"Stat\",\"text\":\"" + smaxMps + "\"},";
  json += "{\"name\":\"distTraveledForward\",\"label\":\"Distance Travelled Forward\",\"type\":\"Stat\",\"text\":\"" + sdistTraveledForward + "\"},";
  json += "{\"name\":\"distTraveledBackward\",\"label\":\"Distance Travelled Backward\",\"type\":\"Stat\",\"text\":\"" + sdistTraveledBackward + "\"},";

  json += "{\"name\":\"program\",\"label\":\"System stats\",\"type\":\"Section\"},";
  json += "{\"name\":\"progName\",\"label\":\"Program Name\",\"type\":\"Stat\",\"text\":\"" + _progName + "\"},";
  json += "{\"name\":\"progVers\",\"label\":\"Program Version\",\"type\":\"Stat\",\"text\":\"" + _progVers + "\"},";
  json += "{\"name\":\"batteryPowered\",\"label\":\"Running on battery\",\"type\":\"Stat\",\"text\":\"" + String(_batteryPowered) + "\"},";

  //voltage           //ESP.getVcc  ???
  //battery level
  //curSessionUptime
  //totalHrsAwake
  //totalHrsAsleep
  //board Profile
  
  json += "{\"name\":\"extras\",\"label\":\"Extras\",\"type\":\"Section\"},";
  json += "{\"name\":\"wifiOff\",\"label\":\"Turn the WIFI off\",\"type\":\"Button\",\"value\":" + String(0) + "},";
  json += "{\"name\":\"sleep\",\"label\":\"Put the board to sleep (turns WIFI off)\",\"type\":\"Button\",\"value\":" + String(0) + "},";
  //json += "{\"name\":\"sleep\",\"label\":\"..only press when it all goes wrong!\",\"type\":\"Section\"},";
  json += "{\"name\":\"resetDefaults\",\"label\":\"Reset to default settings\",\"type\":\"Button\",\"value\":" + String(0) + "}";
  
  json += "]";

  if (DEBUG_COMMS) { 
  Serial.print(F("/stats HTTP_GET returns: "));
  Serial.println(json);
  }
  
  return json;
}


