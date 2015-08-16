/*
     Infrared controlled Light 
*/
#include <IRremote.h>           
int RECV_PIN = 11;                
int ledPin = 10;                
boolean ledState = LOW;        
IRrecv irrecv(RECV_PIN);      
decode_results results;        
 
void setup(){
  Serial.begin(9600);         
  irrecv.enableIRIn();         
  pinMode(ledPin,OUTPUT);     
}
 
void loop() {
  if (irrecv.decode(&results)) {  
      Serial.println(results.value, HEX);
     
      if(results.value == 0xFD00FF){
               ledState = !ledState;            
               digitalWrite(ledPin,ledState); 
           }     
   irrecv.resume();  
   }
   
}
