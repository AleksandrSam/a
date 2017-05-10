/* Подключаем библиотеку для работы с LCD */
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPSPlus tinyGps;
SoftwareSerial GPS(7, 8);


/* Создаём объект LCD-дисплея, используя конструктор класса LiquidCrystal
* с 6ю аргументами. Библиотека по количеству аргументов сама определит,
* что нужно использовать 4-битный интерфейс.
* Указываем, к каким пинам Arduino подключены выводы дисплея:
*   RS, E, DB4, DB5, DB6, DB7
*/
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
 /* Инициализируем дисплей: 2 строки по 16 символов */
 lcd.begin(16, 2);

 GPS.begin(9600);
 Serial.begin(19200);
}

void loop()
{
 /* Устанавливаем курсор в 1 столбец 2й строки. Нумерация идёт с нуля,
  * первым аргументом идёт номер столбца.
  */
 lcd.setCursor(0, 1);
 /* Выводим на дисплей число секунд, прошедших с момента старта Arduino */
 lcd.print(millis() / 1000);

  if (GPS.available())
    {

        tinyGps.encode(GPS.read());
        if (tinyGps.altitude.isUpdated())
        Serial.println(tinyGps.altitude.meters());

        Serial.print("LAT=");Serial.println(tinyGps.location.lat(), 6);
        Serial.print("LONG="); Serial.println(tinyGps.location.lng(), 6);
        Serial.print("ALT=");  Serial.println(tinyGps.altitude.meters());
        Serial.print("Speed="); Serial.println(tinyGps.speed.mps());
        Serial.println(tinyGps.time.value()); // Raw time in HHMMSSCC format (u32)
        Serial.println(tinyGps.time.hour()); // Hour (0-23) (u8)
        Serial.println(tinyGps.time.minute()); // Minute (0-59) (u8)
        Serial.println(tinyGps.time.second()); // Second (0-59) (u8)
        Serial.println(tinyGps.time.centisecond()); // 100ths of a second (0-99) (u8)
       
  
      // while(GPS.available())
      //  {
        //    buffer[count++]=GPS.read();
              
          //  if(count == 64)
           //break;
        //}
        //String str1 = ((char*)buffer); //String myString = String((char*)byteArray);
        //Serial.print(str1);



//        int i;
  //      for (i = 0; i < 63; i = i + 1) {
    //      Serial.println(myPins[i]);
      //  }

        //lcd.print(str1);
        //Serial.write(buffer,count);
        //Serial.write(buffer);  lcd.print(GPS.read());
        
       // clrBuff();
        //count = 0;
        delay(500);

    }
   
}

void clrBuff()
{
    for (int i=0; i<count;i++)
    {
        buffer[i]=NULL;
    }
}
