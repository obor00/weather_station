// tab file for functions used by powerSaverPressureAndTemperatureAndHumidity
// BME280 configuration settings

void measurementEvent() {

		//*********** Measures Pressure, Temperature, Humidity, Voltage and calculates Altitude
		// then reports all of these to Blynk or Thingspeak

    float temperature = bme0.readTemperature();
    float pressure = bme0.readPressure();
    float altitude = bme0.readAltitude(1013.25);
    float humidity = bme0.readHumidity();
    
	  Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(" *C");

    Serial.print("Humidity = ");
    Serial.print(humidity);
    Serial.println(" %");
    
    Serial.print(F("Pressure = "));
    Serial.print(pressure);
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(altitude); /* Adjusted to local forecast! */
    Serial.println(" m");

		//******Battery Voltage Monitoring*********************************************

		// Voltage divider R1 = 220k+100k+220k =540k and R2=100k
		float calib_factor = 4.2; // 5.28;  change this value to calibrate the battery voltage
		unsigned long raw = analogRead(A0);
		float volt= raw * calib_factor/1024;

		Serial.print( " m. \nVoltage = ");
		Serial.print(volt, 2); // print with 2 decimal places
		Serial.println (" V");

		//*******************************************************************************
		// code block for uploading data to Thingspeak website

		if (App == "Thingspeak") {
				// Send data to ThingSpeak
				WiFiClient client;
				if (client.connect(server,80)) {
						Serial.println("Connect to ThingSpeak - OK");

						String postStr = "";
						postStr+="GET /update?api_key=";
						postStr+=api_key;
						postStr+="&field1=";
						postStr+=String(pressure);
						postStr+="&field2=";
						postStr+=String(temperature);
						postStr+="&field3=";
						postStr+=String(humidity);
						postStr+="&field4=";
						postStr+=String(altitude);
						postStr+="&field5=";
						postStr+=String(volt);
						postStr+=" HTTP/1.1\r\nHost: a.c.d\r\nConnection: close\r\n\r\n";
						postStr+="";
						client.print(postStr);

						//*******************************************************************************

				}
				while(client.available()){
						String line = client.readStringUntil('\r');
						Serial.print(line);
				}
		}

		//  Serial.print (" Counter= ");
		//  Serial.println (eventCounter);

} // end of void measurementEvent()


void goToSleep() {
		// calculate required sleep time and go to sleep
		// long time=millis();
		long sleepTime = measurementInterval * 1000;

		Serial.print ("Going to sleep now for ");
		Serial.print(sleepTime/1e6);
		Serial.println (" seconds.");

		ESP.deepSleep(sleepTime );


} // end of void goToSleep()
