
//----------------------------------------------------------------------------------------------------
//  Project Name : Solar Powered WiFi Weather Station V2.0
//  Authors: Keith Hungerford and Debasish Dutta
//  Website : www.opengreenenergy.com
//  This code is derived from the example code of farmerkeith_BMP280.h library  
//  Download the library from https://github.com/farmerkeith/BMP280-library
//  Main microcontroller (ESP8266) and BME280 both sleep between measurements
//  BME280 is used in single shot mode ("forced mode")
//  Measurement read command is delayed,
//  By repeatedly checking the "measuring" bit of status register (0xF3) until ready

//  Last updated on 12/02/2019

////  Features :  //////////////////////////////////////////////////////////////////////////////////////////////////////
                                                                                                                         
// 1. Connect to Wi-Fi, and upload the data to either Blynk App  or Thingspeak

// 2. Monitoring Weather parameters like Temperature, Pressure, Humidity and Altitude.

// 3. Extra Ports to add more Weather Sensors like UV Index, Light and Rain Guage etc.

// 4. Remote Battery Status Monitoring

// 5. Using Sleep mode to reduce the energy consumed                                        

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// configuration constants
const bool bme280Debug = 0; // controls serial printing
// set to 1 to enable printing of BME280 or BMP280 transactions

// configure period between reports
const long measurementInterval = 300000;  // 5mn measurement interval in ms

// configuration control constant for use of either Blynk or Thingspeak
//const String App = "BLYNK";         //  alternative is line below
const String App = "Thingspeak"; //  alternative is line above

#include <Wire.h>
#include <farmerkeith_BMP280.h> // library, download from https://github.com/farmerkeith/BMP280-library
#include <ESP8266WiFi.h>        // do we need this? Not when using Blynk
#include <ArduinoOTA.h>

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <BlynkSimpleEsp8266.h>


bme280 bme0 (0, bme280Debug) ; // creates object bme0 of type bme280, base address

// unsigned long eventCounter = 0; // to count measurement events

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "f95037bcf01142a7b5618fb2fe66a48a"; // copy it from the mail received from Blynk
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Bbobo"; // WiFi Router ssid
char pass[] = "Obomai232Obomai232"; // WiFi Router password
// Thingspeak Write API
const char* server = "api.thingspeak.com";
const char* api_key = "6OE6DSG9XRY6LDW2"; // API write key 

#include "PTHsleep.h" // tab file


void setup() {
  int count=0;
  pinMode(D0,OUTPUT);
  ESP.wdtDisable(); // enable Hardware watchdog by disabling software watchdog
  Serial.begin(74880);

  Serial.println("\nStart of solarWiFiWeatherStationESP_12th Feb2019");

  // **************Choose Application**********************************
  
  if (App == "BLYNK")  // for posting datas to Blynk App
  { 
    Blynk.begin(auth, ssid, pass);
  } 
  else if (App == "Thingspeak")  // for posting datas to Thingspeak website
  {
    Serial.println("\nbefore wifi begin");
      WiFi.begin(ssid, pass);
      Serial.println("\nafter wifi begin");
     while ((WiFi.status() != WL_CONNECTED) && (count++ < 15)) 
     {
            delay(500);
            Serial.print(".");
     }
     if (count <= 15) { 
      Serial.println("");
      Serial.println("WiFi connected"); 
     } 
  } 
  else 
  {
    WiFi.begin(ssid, pass);
    Serial.print(App);
    Serial.println(" is not a valid application");
  }
  
 // bme0.updateF4Control16xSleep(); // use instead of saveCounter if counter is not required

  bme0.updateF4Control(7,7,1);  // forced mode
//*******************************************************************
  unsigned long baseEventTime = millis();

  //  eventCounter = recoverCounter();         // comment out if counter is not required
  byte temperatureSamples = pow(2, osrs_t - 1);
  byte pressureSamples = pow(2, osrs_p - 1);
  byte humiditySamples = pow(2, osrs_h - 1);
  Serial.print ("Temperature samples=");
  Serial.print (temperatureSamples);
  Serial.print (" Pressure samples=");
  Serial.print (pressureSamples);
  Serial.print (" Humidity samples=");
  Serial.println (humiditySamples);

  // Wire.begin(); // initialise I2C protocol - not needed here since it is in bmp library
  bme0.begin(osrs_t, osrs_p, 1, 0, 0, 0, osrs_h);
  // parameters are (osrs_t, osrs_p, mode, t_sb, filter, spi3W_en, osrs_h)
  // see BME280 data sheet for definitions
  // this is single shot mode with no filtering
  bme0.updateF4Control(7,7,1);  // forced mode

  measurementEvent();
  //  eventCounter ++;
  //  saveCounter(eventCounter);         // this also puts bme0 to sleep
 // bme0.updateF4Control16xSleep(); // use instead of saveCounter if counter is not required
 
  goToSleep();
  
} // end of void setup()


void loop() {
} // end of void loop()
