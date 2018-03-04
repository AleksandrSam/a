#include "Wire.h"
#include <ESP8266WiFi.h>

const char* ssid = "rokuMITMproxy"; // "RozumniiEnot"; "rokuMITMproxy";  
const char* password = "qwerty123"; // "qwerty123"; "Lula_pilula";

void WiFiConnect() {
  WiFi.begin(ssid, password);
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
}

