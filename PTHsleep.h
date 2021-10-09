// tab file for functions used by powerSaverPressureAndTemperatureAndHumidity
// BME280 configuration settings
const byte osrs_t = 2; // setting for temperature oversampling
// No. of samples = 2 ^ (osrs_t-1) or 0 for osrs_t==0
const byte osrs_p = 5; // setting for pressure oversampling
// No. of samples = 2 ^ (osrs_p-1) or 0 for osrs_p==0
const byte osrs_h = 5; // setting for humidity oversampling
// No. of samples = 2 ^ (osrs_h-1) or 0 for osrs_h==0

void measurementEvent() {

		//*********** Measures Pressure, Temperature, Humidity, Voltage and calculates Altitude
		// then reports all of these to Blynk or Thingspeak

		while (bme0.readRegister(0xF3) >> 3); // loop until F3bit 3 ==0, measurement is ready
		double temperature, pressure;
		double humidity = bme0.readHumidity (temperature, pressure); // measure pressure, temperature and humidity

		float temp = bme0.getTemperature();
		long  temperature0raw = 0;
		long      pressure0raw = bme0.readRawPressure (temperature0raw);
		double t_fine;
		temperature = bme0.calcTemperature (temperature0raw, t_fine);
		pressure = bme0.calcPressure (pressure0raw, t_fine);

		float altitude = bme0.calcAltitude (pressure);

		Serial.print("Atm press = ");
		Serial.print(pressure, 2); // print with 2 decimal places
		Serial.print(" hPa. Temperature = ");

		Serial.print(temperature, 2); // print with 2 decimal places
		Serial.print( " deg C. Humidity = ");
		Serial.print(humidity, 2); // print with 2 decimal places
		Serial.print( " %RH. Altitude = ");
		Serial.print(altitude, 2); // print with 2 decimal places

		//******Battery Voltage Monitoring*********************************************

		// Voltage divider R1 = 220k+100k+220k =540k and R2=100k
		float calib_factor = 4.2; // 5.28;  change this value to calibrate the battery voltage
		unsigned long raw = analogRead(A0);
		float volt= raw * calib_factor/1024;

		Serial.print( " m. \nVoltage = ");
		Serial.print(volt, 2); // print with 2 decimal places
		Serial.println (" V");

		//*******************************************************************************
		// code block for uploading data to BLYNK App

		if (App == "BLYNK") { // choose application
				Blynk.virtualWrite(0, pressure);    // virtual pin 0
				Blynk.virtualWrite(1, temperature); // virtual pin 1
				Blynk.virtualWrite(2, humidity);    // virtual pin 2
				Blynk.virtualWrite(3, altitude);    // virtual pin 3
				Blynk.virtualWrite(4, volt);        // virtual pin 4

		}
		//*******************************************************************************
		// code block for uploading data to Thingspeak website

		else if (App == "Thingspeak") {
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


// ----------recover Event counter----------------------------
int recoverCounter() {
		// read value of counter back from bmp0
		byte bme0F4value = bme0.readF4Sleep(); // 0 to 63
		byte bme0F5value = bme0.readF5Sleep(); // 0 to 63
		return bme0F5value * 64 + bme0F4value; // 0 to 4095
		if (bme280Debug) {
				Serial.print ("bme0F4,F5values from bme0 Registers 0xF4,F5 =");
				Serial.print (bme0F4value);
				Serial.print (" ");
				Serial.println (bme0F5value);
		} // end of if (bme280Debug)
} // end of void recoverCounter()


void saveCounter(int counter) {
		// write value of counter into bme0
		// bme0.updateF4ControlSleep(counter & 0x3F); // store eventCounter
		// bme0.updateF5ConfigSleep((counter / 64) & 0x3F); // store eventCounter
		// this also puts bme0 to sleep
		if (bme280Debug) {
				Serial.print ("saved counter to bme0F4,F5 as ");
				Serial.print (counter & 0x3F);
				Serial.print (", ");
				Serial.println ((counter / 64) & 0x3F);
		} // end of if (bme280Debug)
} // end of void saveCounter()


void goToSleep() {
		// calculate required sleep time and go to sleep
		// long time=millis();
		long sleepTime = measurementInterval * 1000;

		Serial.print ("Going to sleep now for ");
		Serial.print(sleepTime/1e6);
		Serial.println (" seconds.");

		ESP.deepSleep(sleepTime );


} // end of void goToSleep()
