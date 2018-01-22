#include "SparkFunBME280.h"
#include "Wire.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Connecting of BME280 for ESP8266 using I2C interface
// SCL -> D1 (GPIO5)
// SDA -> D2 (GPIO4)

//Global sensor object
BME280 mySensor;
#define OLED_RESET 2
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

void setup()
{
	mySensor.settings.commInterface = I2C_MODE;
	mySensor.settings.I2CAddress = 0x76;
	//renMode can be:
	//  0, Sleep mode
	//  1 or 2, Forced mode
	//  3, Normal mode
	mySensor.settings.runMode = 3;
  
	//tStandby can be:
	//  0, 0.5ms
	//  1, 62.5ms
	//  2, 125ms
	//  3, 250ms
	//  4, 500ms
	//  5, 1000ms
	//  6, 10ms
	//  7, 20ms
	mySensor.settings.tStandby = 0;
	
	//filter can be off or number of FIR coefficients to use:
	//  0, filter off
	//  1, coefficients = 2
	//  2, coefficients = 4
	//  3, coefficients = 8
	//  4, coefficients = 16
	mySensor.settings.filter = 0;
	
	//tempOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	mySensor.settings.tempOverSample = 1;

	//pressOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
    mySensor.settings.pressOverSample = 1;
	
	//humidOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	mySensor.settings.humidOverSample = 1;
	
	Serial.begin(115200);
	delay(5);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.    
	Serial.println(mySensor.begin(), HEX); //Calling .begin() causes the settings to be loaded

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  display.clearDisplay();

 
  
}

void loop()
{
	Serial.print("Temperature: ");
	Serial.print(mySensor.readTempC(), 2);
	Serial.println(" degrees C");

	Serial.print("%RH: ");
	Serial.print(mySensor.readFloatHumidity(), 2);
	Serial.println(" %");

  Serial.print("Pressure: ");
  Serial.print(mySensor.readFloatPressure() * 0.00750063755419211, 2);
  Serial.println(" mm Hg");

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  
  display.setCursor(0,0);
  display.print(mySensor.readTempC(), 2);
    
  display.println(" C");
  display.print(mySensor.readFloatHumidity(), 2);
  display.println(" %");
  
  display.print(mySensor.readFloatPressure() * 0.00750063755419211, 2);
  display.setTextSize(1);
  display.println(" mm Hg");
    
  display.display();
 
  
	Serial.println();	
	delay(5000);
}
