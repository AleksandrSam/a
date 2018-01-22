const int LED=5;

void setup()
{
Serial.begin(115200);
pinMode(LED, OUTPUT);

digitalWrite(LED,HIGH);
delay(3000);
digitalWrite(LED,LOW);
Serial.println("Sleep for 60 seconds");
ESP.deepSleep(60 * 1000000); // Sleep for 60 seconds
}

void loop()
{
}  

