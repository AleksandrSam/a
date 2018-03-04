#include <ThingSpeak.h>

unsigned long myChannelNumber = 408054;
const char * myWriteAPIKey = "R4D9PGAL7MSNNVXO";

const char* server = "api.thingspeak.com";
WiFiClient client;

void logBeginThingSpeak(){
  ThingSpeak.begin(client);
}

void logThingSpeak(float t, float h, float p){
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
 // ThingSpeak.setField(3, p);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

