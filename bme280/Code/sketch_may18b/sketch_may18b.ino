//For using Deep Sleep mode RST should be connected to GPIO16 (D0)

void setup() {
Serial.begin(115200);
delay(1);
Serial.println("__Preved9");
ESP.deepSleep(1 * 15 * 1000000);
}

void loop() {
}
