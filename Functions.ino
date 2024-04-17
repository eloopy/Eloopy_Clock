
/*  ELOOPY CLOCK v0.6 - 2023  */

void StopH() {

  digitalWrite(HIN1, pole1[8]);  
  digitalWrite(HIN2, pole1[8]);
  digitalWrite(HIN3, pole1[8]); 
  digitalWrite(HIN4, pole1[8]);


}

void StopM() {

  digitalWrite(MIN1, pole1[8]);  
  digitalWrite(MIN2, pole1[8]);
  digitalWrite(MIN3, pole1[8]); 
  digitalWrite(MIN4, pole1[8]);


}

void setZeroH() {

  stp = 0;
  

  while (digitalRead(sensorH) == HIGH) {

     digitalWrite(HIN1, pole1[stp]);  
     digitalWrite(HIN2, pole2[stp]); 
     digitalWrite(HIN3, pole3[stp]); 
     digitalWrite(HIN4, pole4[stp]);

      stp++;

      if (stp < 0) {
        stp = 7;
      }

      if (stp > 7) {
        stp = 0;
      }

      delay(1);

  }

  StopH();

}


void setZeroM() {

  stp = 7;
  

  while (digitalRead(sensorM) == HIGH) {

     digitalWrite(MIN1, pole1[stp]);  
     digitalWrite(MIN2, pole2[stp]); 
     digitalWrite(MIN3, pole3[stp]); 
     digitalWrite(MIN4, pole4[stp]);

      stp--;

      if (stp < 0) {
        stp = 7;
      }

      if (stp > 7) {
        stp = 0;
      }

      delay(1);

  }

  StopM();


}


int getTimeData(){

  struct tm timeinfo;
  
  if(!getLocalTime(&timeinfo)){
    Serial.print("*");
    return 0;
  }

  Serial.println();
  Serial.println(&timeinfo);
  
  char timeHour[3];
  strftime(timeHour,3, "%I", &timeinfo);
  String tempString = String(timeHour); 
  timeDataHour = tempString.toInt();

  Serial.print(timeDataHour);
  Serial.print(" ");
  
  char timeMinutes[3];
  strftime(timeMinutes,3, "%M", &timeinfo);
  tempString = String(timeMinutes); 
  timeDataMinutes = tempString.toInt();

  Serial.print(timeDataMinutes);
  Serial.print(" ");

  char timeSeconds[3];
  strftime(timeSeconds,3, "%S", &timeinfo);
  tempString = String(timeSeconds); 
  timeDataSeconds = tempString.toInt();

  Serial.print(timeDataSeconds);
  Serial.println();

  timeAdj = ((60 - timeDataSeconds)*1000);
  if (timeAdj <=10000) timeAdj = 1;

  Serial.print("timeAdj: ");
  Serial.print(timeAdj/1000);
  Serial.println(" sec");
  
  return 1;
}




void setTimeMotors(){
    Hmotor.step(-171 * timeDataHour);
    StopH();
    Hkeep = timeDataHour;
  
    Mmotor.step(34 * timeDataMinutes);
    StopM();
    Mkeep = timeDataMinutes;
  
  
    delay(timeAdj);
  
  
  
  
}




void loadWIFI(){

  
  File file = SPIFFS.open(WiFifilename, "r");
 
    if(!file){
        Serial.println("Failed to open file for reading");
    }
 

   vector<String> v;
  while (file.available()) {
    v.push_back(file.readStringUntil('\n'));
  }
  
  file.close();


  ssid = v[0];
  password = v[1];

  ssid.trim();
  password.trim();
  
}

void loadAdminPass(){
  
  
  File file = SPIFFS.open(AdminPassfilename, "r");
 
    if(!file){
        Serial.println("Failed to open file for reading");
    }
 

   vector<String> v;
  while (file.available()) {
    v.push_back(file.readStringUntil('\n'));
  }
  
  file.close();

  AdminPass = v[0];
  
  AdminPass.trim();
  
  
}



void loadCity(){

  // Time zone codes --> https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
  
  File file = SPIFFS.open(Cityfilename, "r");
 
    if(!file){
        Serial.println("Failed to open file for reading");
    }
 

   vector<String> v;
  while (file.available()) {
    v.push_back(file.readStringUntil('\n'));
  }
  
  file.close();

  City = v[0];
  
  City.trim();
  Serial.println(City);

  configTime(0, 0, ntpServer);

  if(City == "Barcelona") {  
      setenv("TZ","CET-1CEST,M3.5.0,M10.5.0/3",1);
      tzset();
  }
  
  if(City == "Buenos Aires"){ 
      setenv("TZ","<-03>3",1);
      tzset();
  }
  
  if(City == "London"){  
      setenv("TZ","GMT0BST,M3.5.0/1,M10.5.0",1);
      tzset();
  }

  if(City == "New York"){  
      setenv("TZ","EST5EDT,M3.2.0,M11.1.0",1);
      tzset();
  }
  
  
}



void writeFile(String data, String fName, const char* Mode){
  

  File file = SPIFFS.open(fName, Mode);

  if(!file){
     Serial.println("There was an error opening the file for writing");
     return;
  }

  if(file.println(data)) {
    Serial.println("File was written OK");
  }else {
    Serial.println("File write failed");
    }

  file.close();
  
  
}



String fillForm(const String& var){

  String addToform = "";
  
  if(var == "SSID"){
    addToform = "value=\"" + ssid + "\"";
    return addToform;
    
  }else if(var == "PASS"){
    addToform = "value=\"" + password + "\"";
    return addToform;
    
  }else if(var == "adminpass"){
    addToform = "value=\"" + AdminPass + "\"";
    return addToform;
    
  }else if(var == "city"){
    addToform = "value=\"" + City + "\"";
    return addToform;
    
    }

  return String();
}




String fillNetworkForm(const String& var){

  String addToform = "";
  
  if(var == "SSID"){
    addToform = "value=\"" + ssid + "\"";
    return addToform;
    
  }else if(var == "PASS"){
    addToform = "value=\"" + password + "\"";
    return addToform;
    
  }else if(var == "NET"){
    listNetworks();
    
    for (int i=0; i<20; i++){
      if (nets[i] != ""){
        addToform = addToform + "<input type=\"radio\"  name=\"nope\" value=\"" + nets[i] + "\" onclick=\"fillSSID(this)\">\n<label for=\"" + nets[i] + "\">" + nets[i] + "</label><br>" + "\n\n";
        }
      
      }
     
    return addToform;
    
  }
    

  return String();
}

void listNetworks() {
  
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }
  for (int thisNet = 0; thisNet <= numSsid; thisNet++) {
    
    nets[thisNet] ="";
    nets[thisNet] = WiFi.SSID(thisNet);
    Serial.println(WiFi.SSID(thisNet));
    
  }
  Serial.println("--------------------------------------");
}
