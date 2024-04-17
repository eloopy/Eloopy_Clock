/*  ELOOPY CLOCK v0.6 - 2023  */


void webServerSetUp(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false);
  });
  

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/wifi.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/wifi.html", String(), false, fillNetworkForm);
  });

  server.on("/adminPass.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/adminPass.html", String(), false, fillForm);
  });

  server.on("/worldTime.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/worldTime.html", String(), false, fillForm);
  });

  

  server.on("/adminpassset", HTTP_GET, [] (AsyncWebServerRequest *request) {
  String formAdminPass;
 
  if (request->hasParam("adminpass")) {
    formAdminPass = request->getParam("adminpass")->value();
  }
  
  formAdminPass.trim();
  writeFile(formAdminPass, AdminPassfilename, "w");
  delay(10);
  
  request->send(200, "text/plain", "Done. Eloopy Clock will restart and connect to your router");

  delay(1000);
  ESP.restart();
  });
  

  server.on("/wifiSet", HTTP_GET, [] (AsyncWebServerRequest *request) {
  String formSSID;
  String formPass;
 
  if (request->hasParam("ssid") && request->hasParam("pass")) {
    formSSID = request->getParam("ssid")->value();
    formPass = request->getParam("pass")->value();
  }

  formSSID.trim();
  formPass.trim();

  writeFile(formSSID, WiFifilename, "a");
  delay(10);
  writeFile(formPass, WiFifilename, "a");
  
  request->send(200, "text/plain", "Done. Eloopy Clock will restart and connect to your router");

  delay(1000);
  ESP.restart();
  });



  server.on("/cityset", HTTP_GET, [] (AsyncWebServerRequest *request) {
  String formCity;
 
  if (request->hasParam("city")) {
    formCity = request->getParam("city")->value();
  }
  
  formCity.trim();
  writeFile(formCity, Cityfilename, "w");
  delay(10);
  
  request->send(200, "text/plain", "Done. Eloopy Clock will restart and connect to your router");

  delay(1000);
  ESP.restart();
  });


  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect("/");
    Serial.print("server.notfound triggered: ");
    Serial.println(request->url());
  });

}
