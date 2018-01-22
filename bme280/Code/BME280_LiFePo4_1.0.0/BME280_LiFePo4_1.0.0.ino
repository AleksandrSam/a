// For using Deep Sleep mode connect RST -> GPIO16 (D0)
// Connecting of BME280 for ESP8266 using I2C interface:
// SCL -> D1 (GPIO5)
// SDA -> D2 (GPIO4)
// A0 - via voltage divider on resistors connect to plus output on the battery
// For measuring the voltage below 4V use voltage divider on resistors: R1=22kOm, R2=100kOm
// How to calculate voltage divider on resistors http://www.joyta.ru/7328-delitel-napryazheniya-na-rezistorax-raschet-onlajn/

#include <ESP8266WiFi.h>
#include "SparkFunBME280.h"
#include "Wire.h"

const int deepSleepInMinutes = 1;
const int WiFiConnectAttemptsLimit = 5;
const float voltageScale = 225.5;  // R 100 kOm; 3,3V upper voltage of A0 is 1024 after ADC

// Wi-Fi Settings
const char* ssid = "TP-LINK";
const char* password = "76533457";
WiFiClient client;

// ThingSpeak Settings
const int channelID = 234393;
String writeAPIKey = "HMJW0UPXVPCULI91";
const char* server = "api.thingspeak.com";

//Global sensor object
BME280 mySensor;


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  int WiFiConnectAttempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    ++WiFiConnectAttempts;

    if (WiFiConnectAttempts == WiFiConnectAttemptsLimit) {
      ESP.deepSleep((deepSleepInMinutes - 1) * 60 * 1000000 + 59 * 1000000 - WiFiConnectAttempts * 1000000, WAKE_RFCAL);
    }      
  }

  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x76;
  //runMode can be:
  //  0, Sleep mode
  //  1 or 2, Forced mode
  //  3, Normal mode
  mySensor.settings.runMode = 1;
  
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
  
  delay(5);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.    
  mySensor.begin();  // Calling .begin() causes the settings to be loaded

  float t = mySensor.readTempC();
  float h = mySensor.readFloatHumidity();
  float p = mySensor.readFloatPressure() * 0.00750063755419211;  // translation from Pascal to mmRh
  float voltage = analogRead(0) / voltageScale;

  // Construct API request body
  String body = "field1=";
  body += String(t);
  body += "&field2=";
  body += String(h);
  body += "&field3=";
  body += String(p);
  body += "&field4=";
  body += String(voltage);

  if (client.connect(server, 80)) {   
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(body.length());
    client.print("\n\n");
    client.print(body);
    client.print("\n\n");
/*
    Serial.print("Temperature: ");
    Serial.println(t, 2);
    Serial.print("Humidity: ");
    Serial.println(h, 2);
*/
  }
  client.stop();
  ESP.deepSleep((deepSleepInMinutes - 1) * 60 * 1000000 + 59 * 1000000, WAKE_RFCAL);
// 0, WAKE_RF_DEFAULT - RF_CAL or not after deep-sleep wake up, depends on init data byte 108
// 1, WAKE_RFCAL - RF_CAL after deep-sleep wake up, there will be large current
// 2, WAKE_NO_RFCAL - no RF_CAL after deep-sleep wake up, there will only be small current
// 4, WAKE_RF_DISABLED - disable RF after deep-sleep wake up, just like modem sleep, there will be the smallest current  
}

void loop() {
}

