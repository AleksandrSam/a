/*
For using Deep Sleep mode RST should be connected to GPIO16 (D0 for ESP-12E)
*/
#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTTYPE DHT11
const int DHTPin = 2;  // D4 for ESP-12E
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char humidityTemp[7];

// Wi-Fi Settings
const char* ssid = "TP-LINK"; // your wireless network name (SSID)
const char* password = "76533457"; // your Wi-Fi network password
WiFiClient client;

// ThingSpeak Settings
const int channelID = 234393;
String writeAPIKey = "HMJW0UPXVPCULI91"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  dht.begin();

  if (client.connect(server, 80)) {
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      strcpy(celsiusTemp, "Failed");
      strcpy(humidityTemp, "Failed");
    }
    else {
      // Construct API request body
      String body = "field1=";
      body += String(t);
      body += "&field2=";
      body += String(h);

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

      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");
    }
  }
  client.stop();
  ESP.deepSleep(1 * 60 * 1000000); // 1 Minute
}

void loop() {
}

