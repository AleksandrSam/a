void WiFiConnect() {
  WiFi.begin(ssid, password);
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int count = 0;
  
  while (WiFi.status() != WL_CONNECTED && count<10) {
    delay(500);
    Serial.print(".");
    count++;
    Serial.println(count);
  }

  if (count==10){
    Serial.println("");
    Serial.println("WiFi isn't connected");
    WiFi.disconnect();
  }else { 
    Serial.println("");
    Serial.println("WiFi connected");
  }
  
  
}

