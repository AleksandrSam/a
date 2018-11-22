uint8_t relay_pin = D1;

void setup()
{
  pinMode(relay_pin, OUTPUT); 
  digitalWrite(relay_pin, LOW);
  
}

void loop()
{
  delay(400);
  digitalWrite(relay_pin, HIGH);
  delay(400);
  digitalWrite(relay_pin, LOW);
}
