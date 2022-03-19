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
#define WIFI_SSID "wifi"
#define WIFI_PASSWORD "password"

// Insert Firebase project API Key
#define API_KEY "apikey"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "databaseURL" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

/*..Time const............................................................. */
 const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
/*..printlocal time function................................................ */
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  } else
  {
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  }
}
 
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
  Serial.println("CONNEXION REUSSITE--------------------------");
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

 /*init and get the time................................................................................*/
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
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
void loop()
   {
   //printLocalTime();
   
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 20000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    count++;

    FirebaseJson json ;
    ////////////////////////////////////////////////////////////
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Time Failed");
      
  } else
  {
     
     //.......
     char nomJour[10];
     strftime(nomJour,10, "%A", &timeinfo);
     char jour[3];
     strftime(jour,3, "%d", &timeinfo);
     char mois[3];
     strftime(mois,3, "%m", &timeinfo);
     char annee[6];
     strftime(annee,6, "%Y", &timeinfo);

    char heures[3];
     strftime(heures,3, "%I", &timeinfo);
     
     char minutes[3];
     strftime(minutes,3, "%M", &timeinfo);
     
     char secondes[3];
     strftime(secondes,3, "%S", &timeinfo);

     
    json.add("nom","kadjouh");
    json.add("prenom","nabil");
    json.add("age",count);
    
    json.add("nom du jour",nomJour);
    json.add("jour",jour);
    json.add("mois",mois);
    json.add("annee",annee);

    json.add("heures",heures);
    json.add("minutes",minutes);
    json.add("secondes",secondes);
     
  /*   strcat(nomJour,strcat(" ",strcat(jour,strcat("/",strcat(mois,strcat("/",annee))))));
    json.add("date", nomJour);*/
    
    /*json.add("date",strcat(nomJour,strcat(" ",strcat(jour,strcat("/",strcat(mois,strcat("/",annee)))))));*/
    
  /*  */
    
 /*  json.add("temps",strcat(strcat(strcat(strcat(heures,":"),minutes),":"),secondes));*/
   /* json.add("date",strcat(strcat(strcat(strcat(jour,"/"),mois),"/"),annee));*/
    
  
  
  }

    ////////////////////////////////////////////////////////////
     
    
   
   // json.add("time",printLocalTime());

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
