//BMP-BME-280
#include "SparkFunBME280.h"
#include <SPI.h>
//MQTT
#include "Wire.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//ThingSpeak
#include <ThingSpeak.h>
//Wi-Fi
#include "Wire.h"
#include <ESP8266WiFi.h>
//Display
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>


// -+- Thing Speak -+-
unsigned long myChannelNumber = 475102;
const char * myWriteAPIKey = "AN3MTQ8P2QRK8EEM";
const char* server = "api.thingspeak.com";
//--------------------


// -+- Wi Fi -+-
const char* ssid = "RozumniiEnot"; // "RozumniiEnot"; "rokuMITMproxy";  
const char* password = "Lula_pilula"; // "qwerty123"; "Lula_pilula";

WiFiClient wclient;      
//---------------------

// -+- MQTT -+-
const char *mqtt_server = "m20.cloudmqtt.com"; // Имя сервера MQTT
const int mqtt_port = 16369; // Порт для подключения к серверу MQTT
const char *mqtt_user = "ywkvibaz"; // Логин от сервер
const char *mqtt_pass = "fBUFT8DLMaHH"; // Пароль от сервера

PubSubClient client(wclient, mqtt_server, mqtt_port);

int tm=300;
float temp=0;
//--------------------

// Connecting of BME280 for ESP8266 using I2C interface
// SCL -> D1 (GPIO5)
// SDA -> D2 (GPIO4)

//Global sensor object
BME280 bme280;

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define tempKotelOn 30
int kotolTimeOnCount = 12;

void setup()
{
  Serial.begin(9600);
  
  WiFiConnect();

	bme280.settings.commInterface = I2C_MODE;
	bme280.settings.I2CAddress = 0x76;
	//renMode can be:
	//  0, Sleep mode
	//  1 or 2, Forced mode
	//  3, Normal mode
	bme280.settings.runMode = 3;
  
	//tStandby can be:
	//  0, 0.5ms
	//  1, 62.5ms
	//  2, 125ms
	//  3, 250ms
	//  4, 500ms
	//  5, 1000ms
	//  6, 10ms
	//  7, 20ms
	bme280.settings.tStandby = 0;
	
	//filter can be off or number of FIR coefficients to use:
	//  0, filter off
	//  1, coefficients = 2
	//  2, coefficients = 4
	//  3, coefficients = 8
	//  4, coefficients = 16
	bme280.settings.filter = 0;
	
	//tempOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	bme280.settings.tempOverSample = 1;

	//pressOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  bme280.settings.pressOverSample = 1;
	
	//humidOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	bme280.settings.humidOverSample = 1;
	
	delay(5);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.    
	Serial.println(bme280.begin(), HEX); //Calling .begin() causes the settings to be loaded

  displayBegin();

  logBeginThingSpeak();
  
}

void loop()
{  
  //GET data from BME280
  
  float t = bme280.readTempC();
  float p = bme280.readFloatPressure() * 0.00750063755419211;
  float h = bme280.readFloatHumidity();
  
	Serial.print("Temperature: ");
	Serial.print(t, 2);
	Serial.println(" degrees C");

	Serial.print("%RH: ");
	Serial.print(h, 2);
	Serial.println(" %");

  Serial.print("Pressure: ");
  Serial.print(p, 2);
  Serial.println(" mm Hg");

  displayTemperature(t, h);
  if (kotolTimeOnCount>11) {
    relayContiolTemperature(t);
    kotolTimeOnCount = 0;
  }  
  kotolTimeOnCount++;
  
  logThingSpeak(t, h, p);

	Serial.println();	

  // подключаемся к MQTT серверу
  if (WiFi.status() == WL_CONNECTED) {
      if (!client.connected()) {
          Serial.println("Connecting to MQTT server");
          if (client.connect(MQTT::Connect("arduinoClient2")
                               .set_auth(mqtt_user, mqtt_pass))) {
              Serial.println("Connected to MQTT server");
              client.set_callback(callback);
             // client.subscribe("test/Gled");                  // подписывааемся по топик с данными для светодиода
             // client.subscribe("test/Bled");                  // подписывааемся по топик с данными для светодиода
             // client.subscribe("test/Servo");                 // подписывааемся по топик с данными для Сервопривода
          } else {
              Serial.println("Could not connect to MQTT server");   
          }
      }
      
      if (client.connected()){
          client.loop();
          TempSend();
      }
  }
  
	delay(5000);
}

// Функция отправки показаний с термодатчика
void TempSend(){
    if (tm==0)
    {
      //  sensors.requestTemperatures();   // от датчика получаем значение температуры
     //   float temp = sensors.getTempCByIndex(0);
     //   client.publish("test/temp", String(temp)); // отправляем в топик для термодатчика значение температуры
        //Serial.println(temp);
    //    Serial.print("T = \t");Serial.print(temp,2); Serial.println(" C\t");

        double T,Pressure;
       /* char result = bmp.startMeasurment();
        if(result != 0){
            delay(result);
            result = bmp.getTemperatureAndPressure(T,Pressure);
            if(result!=0)
            {
                Serial.print("P = \t");Serial.print(Pressure,2); Serial.println(" mBar\t");            
            }
            else {
                Serial.println("Error.");
            }
        }
        else {
            Serial.println("Error.");
        }
        client.publish("test/pressure", String(Pressure));
        */
        tm = 1000;  // пауза меду отправками значений температуры  коло 3 секунд
    }
    tm--; 
    delay(10);  
}

void callback(const MQTT::Publish& pub)     // Функция получения данных от сервера
{
    Serial.print(pub.topic());                // выводим в сериал порт название топика
    Serial.print(" => ");
    Serial.println(pub.payload_string());     // выводим в сериал порт значение полученных данных
    
    String payload = pub.payload_string();
    
    if(String(pub.topic()) == "test/Gled")    //  проверяем из нужного ли нам топика пришли данные 
    {
        int stledG = payload.toInt();         //  преобразуем полученные данные в тип integer
      //  digitalWrite(GREEN_LED, stledG);       //  включаем или выключаем светодиод в зависимоти от полученных значений данных
    }
    
    if(String(pub.topic()) == "test/Bled")    //  проверяем из нужного ли нам топика пришли данные 
    {
        int stledB = payload.toInt();         //  преобразуем полученные данные в тип integer
    //    digitalWrite(Blue_LED, stledB);        //  включаем или выключаем светодиод в зависимоти от полученных значений данных
    }
    
    if(String(pub.topic()) == "test/Servo")   // проверяем из нужного ли нам топика пришли данные 
    {
        int pos = payload.toInt();            //  преобразуем полученные данные в тип integer
       // myservo.write(pos);                   //  поворачиваем сервопривод
    }
}

