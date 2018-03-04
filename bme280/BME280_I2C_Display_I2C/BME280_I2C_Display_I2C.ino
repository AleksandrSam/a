#include "SparkFunBME280.h"
#include <SPI.h>

// Connecting of BME280 for ESP8266 using I2C interface
// SCL -> D1 (GPIO5)
// SDA -> D2 (GPIO4)

//Global sensor object
BME280 bme280;

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define tempKotelOn 30
int kotolTimeOnCount = 12;

void setup()
{
  Serial.begin(115200);
  
  WiFiConnect();

	bme280.settings.commInterface = I2C_MODE;
	bme280.settings.I2CAddress = 0x76;
	//renMode can be:
	//  0, Sleep mode
	//  1 or 2, Forced mode
	//  3, Normal mode
	bme280.settings.runMode = 3;
  
	//tStandby can be:
	//  0, 0.5ms
	//  1, 62.5ms
	//  2, 125ms
	//  3, 250ms
	//  4, 500ms
	//  5, 1000ms
	//  6, 10ms
	//  7, 20ms
	bme280.settings.tStandby = 0;
	
	//filter can be off or number of FIR coefficients to use:
	//  0, filter off
	//  1, coefficients = 2
	//  2, coefficients = 4
	//  3, coefficients = 8
	//  4, coefficients = 16
	bme280.settings.filter = 0;
	
	//tempOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	bme280.settings.tempOverSample = 1;

	//pressOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  bme280.settings.pressOverSample = 1;
	
	//humidOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	bme280.settings.humidOverSample = 1;
	
	delay(5);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.    
	Serial.println(bme280.begin(), HEX); //Calling .begin() causes the settings to be loaded

  displayBegin();

  logBeginThingSpeak();
  
}

void loop()
{  
  //GET data from BME280
  
  float t = bme280.readTempC();
  float p = bme280.readFloatPressure() * 0.00750063755419211;
  float h = bme280.readFloatHumidity();
  
	Serial.print("Temperature: ");
	Serial.print(t, 2);
	Serial.println(" degrees C");

	Serial.print("%RH: ");
	Serial.print(h, 2);
	Serial.println(" %");

  Serial.print("Pressure: ");
  Serial.print(p, 2);
  Serial.println(" mm Hg");

  displayTemperature(t,h);
  if (kotolTimeOnCount>11) {
    relayContiolTemperature(t);
    kotolTimeOnCount = 0;
  }  
  kotolTimeOnCount++;
  
  logThingSpeak(t,h,p);

	Serial.println();	
  
	delay(5000);
}
