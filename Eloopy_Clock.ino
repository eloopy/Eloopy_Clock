
/*  ELOOPY CLOCK v0.6 - 2023  */


#include <Stepper.h>
#include <WiFi.h>
#include "time.h"
#include "ESPAsyncWebSrv.h"
#include "SPIFFS.h"
#include <DNSServer.h>


const int stepsPerRevolution = 1024;  // change this to fit the number of steps per revolution

using namespace std;

// ULN2003 Motor Driver Pins
#define HIN1 21
#define HIN2 19
#define HIN3 18
#define HIN4 5

#define MIN1 13
#define MIN2 12
#define MIN3 14
#define MIN4 27

#define sensorH 26
#define sensorM 25


int pole1[] ={0,0,0,0, 0,1,1,1, 0};//pole1, 8 step values
int pole2[] ={0,0,0,1, 1,1,0,0, 0};//pole2, 8 step values
int pole3[] ={0,1,1,1, 0,0,0,0, 0};//pole3, 8 step values
int pole4[] ={1,1,0,0, 0,0,0,1, 0};//pole4, 8 step values

int stp = 0 ;
int Hkeep = 0;
int Mkeep = 0;
int Refresh = 0;

int timeDataHour;
int timeDataMinutes;
int timeDataSeconds;
int timeAdj;
int Wifi_TimeOut = 0;
bool swtch = true;
bool hold = false;
int period = 0;
unsigned long time_now = 0;

String ssid;
String password;
String AdminPass;
const char* ap_ssid = "Eloopy_Clock";
String City;

const char *AdminPassfilename = "/pass";
const char *WiFifilename = "/wifi";
const char *Cityfilename = "/city";

const char* ntpServer = "216.239.35.0"; // Google's NTP server

String nets[20];


// initialize the stepper library
Stepper Hmotor(stepsPerRevolution, HIN1, HIN3, HIN2, HIN4);
Stepper Mmotor(stepsPerRevolution, MIN1, MIN3, MIN2, MIN4);


AsyncWebServer server(80);
DNSServer dnsServer; 


void setup() {
  
  Hmotor.setSpeed(30);
  Mmotor.setSpeed(30);

  pinMode(sensorH,INPUT);
  pinMode(sensorM,INPUT);
  
  Serial.begin(115200);
  delay(10);

  Serial.println("");
  Serial.println("*****************");
  Serial.println("Eloopy Clock v0.2");
  Serial.println("*****************");
  Serial.println("");


  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting flie system.");
    return;
  }

  Serial.println("File system ok.");
  

  if (SPIFFS.exists("/wifi") == 1) {
    
    loadAdminPass();
    loadCity();
    loadWIFI();

    WiFi.mode(WIFI_MODE_APSTA);
  
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.print(" ");
  
    WiFi.begin(ssid, password);
    WiFi.softAP(ap_ssid, AdminPass);
  
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print("*");
      
      Wifi_TimeOut++;

      if(Wifi_TimeOut == 120){
          Serial.println();
          Serial.println("Can't connect to WiFi");
      
          SPIFFS.remove("/wifi");

          ESP.restart();
        
      }
    }
    
    Serial.println();
    Serial.println("WiFi connected.");
    Serial.print(" Listening on: ");
    Serial.print(WiFi.softAPIP());
    Serial.println();
    

    Serial.println("Setting Zero.");

    setZeroH();
    delay(500);
    setZeroM();
  
    Serial.print("Getting time"); //Get time from NTP server

  
    while(getTimeData() == 0){
      delay(1);
  
    }
  
    Serial.println("Setting time"); // Motors Setting time 

    setTimeMotors();
    
  }else{
    
    Serial.println("No WiFi");
    
    loadAdminPass();
    
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(ap_ssid, AdminPass);
    Serial.println(WiFi.softAPIP());

    hold = true;
    
    }

  
  webServerSetUp();
  server.begin();
  dnsServer.start(53, "*", WiFi.softAPIP());
  

  

}





void loop() {
  
  dnsServer.processNextRequest();
  
  while (!hold){      
        dnsServer.processNextRequest();
        
        if(millis() - time_now > period){
          time_now = millis();
          period = 60000;


          Mmotor.step(34);
          StopM(); 
          
          Mkeep++;
    
          Serial.print("Hkeep: ");
          Serial.println(Hkeep);
          Serial.print("Mkeep: ");
          Serial.println(Mkeep);
          Serial.println("------------");
          Serial.println();
    
    
    
          if (Mkeep == 60){
            //delay(60000);
            Mkeep = 0;
            setZeroM();
            
            if (Refresh == 5){
              
              setZeroH();
              Refresh = 0;
              
              
              while(getTimeData() == 0){
               delay(1);
      
              }
              
              setTimeMotors();
              
              return;
               
              
            }

            Hmotor.step(-171);
            StopH();
            Refresh++ ;
            Hkeep++ ;
            
            Serial.print("Refresh: ");
            Serial.println(Refresh);
            Serial.print("Hkeep: ");
            Serial.println(Hkeep);
            Serial.print("Mkeep: ");
            Serial.println(Mkeep);
            Serial.println("------------");
            Serial.println();

          
            time_now = millis();
            
          }
        }
      
    }

}
