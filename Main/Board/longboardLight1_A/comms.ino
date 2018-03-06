/*----------------------------comms----------------------------*/
void startComms() {
    
  initializeWiFisoftAP();

  httpUpdateServer.setup(&webServer);

  webServerGetPostSetup();
  
  webServer.serveStatic("/", SPIFFS, "/", "max-age=86400");

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
//  if (!MDNS.begin("LlC")) {    //HOSTNAME //"esp8266"
//    Serial.println("Error setting up MDNS responder!");
//    while(1) { 
//      delay(1000);
//    }
//  }
//  Serial.println("mDNS responder started");

  webServer.begin();
  if (DEBUG_COMMS) { Serial.println("HTTP web server started"); }

  webSocketsServer.begin();
  webSocketsServer.onEvent(webSocketEvent);
  if (DEBUG_COMMS) { Serial.println("Web socket server started"); }

  /* Setup the DNS server redirecting all the domains to the apIP */  
//  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
//  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // Add service to MDNS-SD
//  MDNS.addService("http", "tcp", 80);
  if (DEBUG_COMMS) { Serial.println("Comms started / WIFI on"); }
}

void stopComms() {
//  dnsServer.stop();
  webSocketsServer.disconnect();
  webServer.close();  //???
  webServer.stop();   //???
  //httpUpdateServer ???
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  if (DEBUG_COMMS) { Serial.println("Comms stopped / WIFI off"); }
}

void initializeWiFisoftAP() 
{
  WiFi.mode(WIFI_AP);

/*
  String AP_NameString = String(HOSTNAME);              // 'LlC-'
  AP_NameString += String(ESP.getChipId(), HEX);

 * Everyone else will get the above,
 * but as this is my board..
 */
  String AP_NameString = "LlC_Thurstan";
 
  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i = 0; i < AP_NameString.length(); i++) {
    AP_NameChar[i] = AP_NameString.charAt(i);
  }

  if (DEBUG_COMMS) { Serial.println("Starting AP"); }
  
  //if WiFi password in use
  if (WiFiAPPSK != NULL) {     
    Serial.println(WiFi.softAP(AP_NameChar, WiFiAPPSK) ? "ready" : "failed");
  } else {
    Serial.println(WiFi.softAP(AP_NameChar) ? "ready" : "failed");
  }

  //restrict number of devices connected in AP mode to 1 (default is 4)
//  struct softap_config conf;
//  wifi_softap_get_config(&conf);
//  conf.max_connection = 1; 
//  ETS_UART_INTR_DISABLE();
//  ret = wifi_softap_set_config(&conf);
//  ETS_UART_INTR_ENABLE();

  if (DEBUG_COMMS) { 
  Serial.println("Connect to Wi-Fi access point: ");
  Serial.println(AP_NameChar);

  delay(500); // Without delay I've seen the IP address blank
  Serial.println("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  }
}

void handleRoot() { /* ??? */ }
void handleNotFound() { /* ??? */ }

//all the web get and post setup package requests you will ever need
void webServerGetPostSetup() 
{
  //webServer.on("/", handleRoot);
  webServer.on("/generate_204", handleRoot);    //Android captive portal.
  webServer.on("/fwlink", handleRoot);             //Microsoft captive portal.
  webServer.onNotFound ( handleNotFound );
  
  webServer.on("/main", HTTP_GET, []() {
    String json = getMainFieldsJson(fields, fieldCount);
    webServer.send(200, "text/json", json);
    if (DEBUG_COMMS) { Serial.println("..all HTTP_GET"); }
  });
  
  webServer.on("/safety", HTTP_GET, []() {
    String json = getSafetyFieldsJson();
    webServer.send(200, "text/json", json);
    if (DEBUG_COMMS) { Serial.println("..safety HTTP_GET"); }
  });
  
  webServer.on("/calibration", HTTP_GET, []() {
    String json = getCalibrationFieldsJson();
    webServer.send(200, "text/json", json);
    if (DEBUG_COMMS) { Serial.println("..calibration HTTP_GET"); }
  });
  
  webServer.on("/stats", HTTP_GET, []() {
    String json = getStatsFieldsJson();
    webServer.send(200, "text/json", json);
    if (DEBUG_COMMS) { Serial.println("..stats HTTP_GET"); }
  });

  
//  webServer.on("/fieldValue", HTTP_GET, []() {
//    String name = webServer.arg("name");
//    String value = getFieldValue(name, fields, fieldCount);
//    webServer.send(200, "text/json", value);
//    Serial.println("..field value HTTP_GET ");
//  });
//
//  webServer.on("/fieldValue", HTTP_POST, []() {
//    String name = webServer.arg("name");
//    String value = webServer.arg("value");
//    String newValue = setFieldValue(name, value, fields, fieldCount);
//    webServer.send(200, "text/json", newValue);
//    Serial.print("..field value HTTP_POST "); Serial.print(name); Serial.print(", "); Serial.println(value);
//  });

//  webServer.on("/sleep", HTTP_POST, []() {
//    String value = webServer.arg("value");
//    //uint8_t iv = value.toInt();
//    //bool bv =  Convert.ToBoolean(value);
//    setSleep(value.toInt());
//    sendInt(mA.sleep);
//    Serial.print("..sleep HTTP_POST " ); Serial.println(value);
//  });

//  webServer.on("/breathe", HTTP_POST, []() {
//    String value = webServer.arg("value");
//    //uint8_t iv = value.toInt();
//    //bool bv =  Convert.ToBoolean(value);
//    setBreathe(value.toInt());
//    sendInt(mE.breathe);
//    Serial.print("..sleep HTTP_POST " ); Serial.println(value);
//  });

//  webServer.on("/solidColor", HTTP_POST, []() {
//    String r = webServer.arg("r");
//    String g = webServer.arg("g");
//    String b = webServer.arg("b");
//    setSolidColor(r.toInt(), g.toInt(), b.toInt());
//    sendString(String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
//    Serial.print("..solidColor HTTP_POST "); Serial.println(String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
//  });
//
//  webServer.on("/solidColor2", HTTP_POST, []() {
//    String r = webServer.arg("r");
//    String g = webServer.arg("g");
//    String b = webServer.arg("b");
//    setSolidColor2(r.toInt(), g.toInt(), b.toInt());
//    sendString(String(solidColor2.r) + "," + String(solidColor2.g) + "," + String(solidColor2.b));
//    Serial.print("..solidColor2 HTTP_POST "); Serial.println(String(solidColor2.r) + "," + String(solidColor2.g) + "," + String(solidColor2.b));
//  });
//
//  //turn test modes on/off
//  webServer.on("/testModes", HTTP_POST, []() {
//    String value = webServer.arg("value");
//    setTestModes(value.toInt());
//    sendInt(_testMode);
//    Serial.print("..testModes HTTP_POST " ); Serial.println(value);
//  });
//
//  //set current test mode by number
//  webServer.on("/curTestModes", HTTP_POST, []() {
//    String value = webServer.arg("value");
//    setCurTestMode(value.toInt());
//    sendInt(_curTestMode);
//    Serial.print("..curTestModes HTTP_POST " ); Serial.println(value);
//  });
//
//  //set current test mode by name
//  webServer.on("/curTestModesName", HTTP_POST, []() {
//    String value = webServer.arg("value");
//    setCurTestModeByName(value);
//    sendInt(_curTestMode);
//    Serial.print("..curTestModesName HTTP_POST " ); Serial.println(value);
//  });
//
//  webServer.on("/subMode", HTTP_POST, []() {
//    String value = webServer.arg("value");
//    setMainLightsSubMode(value.toInt());
//    sendInt(_mainLightsSubMode);
//    Serial.print("..pattern HTTP_POST "); Serial.println(value);
//  });
//
//  webServer.on("/subModeName", HTTP_POST, []() {
//    String value = webServer.arg("value");
//    setMainLightsSubModeName(value);
//    sendInt(_mainLightsSubMode);
//    Serial.print("..patternName HTTP_POST "); Serial.println(value);
//  });
//
//  webServer.on("/brightness", HTTP_POST, []() {
//    String value = webServer.arg("value");
//    setGlobalBrightness(value.toInt());
//    sendInt(_ledGlobalBrightnessCur);
//    Serial.print("..brightness HTTP_POST "); Serial.println(value);
//  });
//  
}

