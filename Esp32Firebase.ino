/* Includes ........................................................................*/
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
/* Constant to need ........................................................................*/
// Insert your network credentials
#define WIFI_SSID "network"
#define WIFI_PASSWORD "password"

// Insert Firebase project API Key
#define API_KEY "firebase api key"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "firebase database url" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
/* ..... ........................................................................*/
/* SETUP ........................................................................*/
/*...............................................................................*/
void setup(){
/*CONNEXION WIFI.................................................................*/  
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
/*CONFIGURATION FIREBASE AND CONNEXION....................................................................*/
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
/* ..... ........................................................................*/
/* LOOP ........................................................................*/
/*...............................................................................*/
void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    count++;

    FirebaseJson json ;
    json.add("nom","kadjouh");
    json.add("prenom","nabil");
    json.add("age",count);

    if (Firebase.RTDB.pushJSON(&fbdo, " person" , &json)) 
       {
           Serial.println("PASSED");
           Serial.println("PATH: " + fbdo.dataPath());
           Serial.println("TYPE: " + fbdo.dataType()) ;
      } else 
      {
         Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
      }
    
  }  
  
}
