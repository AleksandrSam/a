void logBeginThingSpeak(){
  ThingSpeak.begin(wclient);
}

void logThingSpeak(float t, float h, float p){
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, p);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

