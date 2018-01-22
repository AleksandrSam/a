#include <ESP8266WiFi.h>

// Wi-Fi Settings
const char* ssid = "TP-LINK"; // your wireless network name (SSID)
const char* password = "76533457"; // your Wi-Fi network password
WiFiClient client;

// ThingSpeak Settings
const int channelID = 233904;
String writeAPIKey = "FLL8GM6P3BYGWSPT"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";
const int postingInterval = 60 * 1000; // post data every 1 minute

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  if (client.connect(server, 80)) {
    // Construct API request body
    long rssi = WiFi.RSSI();
    Serial.print("RSSI:");
    Serial.println(rssi);
  
    String body = "field1=";
    body += String(rssi);

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
  }
  client.stop();

  // wait and then post again
  delay(postingInterval);
}

