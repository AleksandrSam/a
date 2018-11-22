 
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>

int address = 0;

String sec_act="";
String sec_pause="";

/* Set these to your desired credentials. */
const char *ssid = "_Speedometr";
const char *password = "1234";

int ledState = LOW;     

long previousMillis = 0;
long previousMillis_sec = 0;

int status_pause=1;
int status_act=1;

ESP8266WebServer server(80);






//////////////////////////////////////////////// OUTPUT TO CLIENT
void handleRoot() {
   server.send(200, "text/html", "<h1>You are connected</h1>");
   Serial.println("---");
//  int new_arg0=server.arg(0).toInt(); 
//  int new_arg1=server.arg(1).toInt(); 
//  
//  //isDigit(inChar)){};
//  
//  String txt_add="";
//  
//  if (server.args() > 0){ // if first call
//  
//  if (new_arg0 > 0 || server.arg(0) == "0") 
//  {E_write(server.arg(0),100); //write to eprom arg0 from address 100
//  sec_act=server.arg(0);
//  }else{txt_add=txt_add+"<font color=red><hr>? Время работы: "+server.arg(0)+ " ?";};
//  
//  if (new_arg1 > 0 || server.arg(1) == "0") {
//      E_write(server.arg(1),200); //write to eprom arg0 from address 200
//      sec_pause=server.arg(1);
//  }else{txt_add=txt_add+"<font color=red><hr>? Время паузы: "+server.arg(1)+ " ? ";}; 
//  
//                          };
//  
//  if (txt_add == ""){txt_add="<font color=green>Установлено.</font>";};
//  
//  String  txt="<meta name='viewport' content='width=240'><meta charset='utf-8'/>";
//  txt=txt+"Цикл : Работа & Пауза<hr>"+sec_act+" & "+sec_pause+" [сек]<hr>";
//  txt=txt+"<form action='http://192.168.4.1' method='get'>";
//  txt=txt+"<input placeholder='работа (сек)'  name='fname'><br><br>";
//  txt=txt+"<input  placeholder='пауза (сек)' name='lname'><br><br>";
//  txt=txt+"<button type='submit' >Установить</button></form>";
//  
//  
//  txt=txt+txt_add;
//  Serial.println(txt);
//  
//  
//  server.send(200,"text/html", txt);

};


 

/////////////////////////////////////                       S E T U P 
void setup() {
  
  Serial.begin(115200);
  delay(1000);
 
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  
  Serial.println("Real act (sec):"+sec_act);

} // END SETUP

//////////////////////////////////////////////////// L O O P
void loop() { 
  server.handleClient();



};


