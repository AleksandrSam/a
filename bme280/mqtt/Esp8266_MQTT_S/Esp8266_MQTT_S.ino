#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "BMP280.h"
#include "Wire.h"

BMP280 bmp;

const char *ssid =  "RozumniiEnot";  // Имя вайфай точки доступа
const char *pass =  "Lula_pilula"; // Пароль от точки доступа

const char *mqtt_server = "m20.cloudmqtt.com"; // Имя сервера MQTT
const int mqtt_port = 16369; // Порт для подключения к серверу MQTT
const char *mqtt_user = "ywkvibaz"; // Логин от сервер
const char *mqtt_pass = "fBUFT8DLMaHH"; // Пароль от сервера

int tm=300;
float temp=0;

WiFiClient wclient;      
PubSubClient client(wclient, mqtt_server, mqtt_port);

void setup() {
    Serial.begin(115200);
    delay(10);
    
    if(!bmp.begin(4,5)){
        Serial.println("BMP init failed!");
        while(1);
    }else Serial.println("BMP init success!");
    bmp.setOversampling(4);

    myservo.attach(0);
    sensors.begin();
    
    Serial.println();
    Serial.println();
    pinMode(GREEN_LED, OUTPUT);
    pinMode(Blue_LED, OUTPUT);
}

void loop() {
    // подключаемся к wi-fi
    if (WiFi.status() != WL_CONNECTED) {
        Serial.print("Connecting to ");
        Serial.print(ssid);
        Serial.println("...");
        WiFi.begin(ssid, pass);
        
        if (WiFi.waitForConnectResult() != WL_CONNECTED)
            return;
        Serial.println("WiFi connected");
    }
    
    // подключаемся к MQTT серверу
    if (WiFi.status() == WL_CONNECTED) {
        if (!client.connected()) {
            Serial.println("Connecting to MQTT server");
            if (client.connect(MQTT::Connect("arduinoClient2")
                                 .set_auth(mqtt_user, mqtt_pass))) {
                Serial.println("Connected to MQTT server");
                client.set_callback(callback);
                client.subscribe("test/Gled");                  // подписывааемся по топик с данными для светодиода
                client.subscribe("test/Bled");                  // подписывааемся по топик с данными для светодиода
                client.subscribe("test/Servo");                 // подписывааемся по топик с данными для Сервопривода
            } else {
                Serial.println("Could not connect to MQTT server");   
            }
        }
        
        if (client.connected()){
            client.loop();
            TempSend();
        }
    }
} // конец основного цикла


// Функция отправки показаний с термодатчика
void TempSend(){
    if (tm==0)
    {
        sensors.requestTemperatures();   // от датчика получаем значение температуры
        float temp = sensors.getTempCByIndex(0);
        client.publish("test/temp", String(temp)); // отправляем в топик для термодатчика значение температуры
        //Serial.println(temp);
        Serial.print("T = \t");Serial.print(temp,2); Serial.println(" C\t");

        double T,Pressure;
        char result = bmp.startMeasurment();
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

