#define Relay D4

void relaySetup()
{ 
  pinMode(Relay, OUTPUT);     
}

void relayLoop() {
  digitalWrite(Relay, LOW);   // реле включено
  digitalWrite(Relay, HIGH);  // реле выключено        
}

void relayContiolTemperature(float t){
  if (t < tempKotelOn) {
     digitalWrite(Relay, LOW);   // реле включено
     displayFire(true);
  } else {
     digitalWrite(Relay, HIGH);  // реле выключено
     displayFire(false);
  }
}
