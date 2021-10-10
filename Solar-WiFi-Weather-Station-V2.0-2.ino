
// type de carte WEMOS D1 R1 !

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
const bool bme280Debug = 1; // controls serial printing
// set to 1 to enable printing of BME280 or BMP280 transactions

// configure period between reports
const long measurementInterval = 900000;  // 5mn measurement interval in ms

const String App = "Thingspeak"; //  alternative is line above

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

Adafruit_BME280 bme0; // I2C

// unsigned long eventCounter = 0; // to count measurement events

char ssid[] = "obo-ap-rpi"; // WiFi Router ssid
char pass[] = "rpiObomai232Obomai232"; // WiFi Router password
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

	if (App == "Thingspeak")  // for posting datas to Thingspeak website
	{
		Serial.print("\nWIFI trying to connect ");
		WiFi.begin(ssid, pass);
		while ((WiFi.status() != WL_CONNECTED) && (count++ < 15))
		{
			delay(500);
			Serial.print(".");
		}
		if (count <= 15) {
			Serial.println("");
			Serial.println("WiFi connected");
		}
		else {
			Serial.println("");
			Serial.println("WiFi **NOT** connected");
		}
	}
	else
	{
		WiFi.begin(ssid, pass);
		Serial.print(App);
		Serial.println(" is not a valid application");
	}

  //if (!bme0.begin(0x76, 0x60)) {
   if (!bme0.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme0.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

	measurementEvent();
	//  eventCounter ++;
	//  saveCounter(eventCounter);         // this also puts bme0 to sleep
	// bme0.updateF4Control16xSleep(); // use instead of saveCounter if counter is not required

	goToSleep();

} // end of void setup()


void loop() {
} // end of void loop()
