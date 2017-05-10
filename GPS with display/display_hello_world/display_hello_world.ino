/* Подключаем библиотеку для работы с LCD */
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "TinyGPS++.h"

TinyGPSPlus tinyGps;
SoftwareSerial GPS(7, 8);
unsigned char buffer[64]; // buffer
int count=0;

/* Создаём объект LCD-дисплея, используя конструктор класса LiquidCrystal
* с 6ю аргументами. Библиотека по количеству аргументов сама определит,
* что нужно использовать 4-битный интерфейс.
* Указываем, к каким пинам Arduino подключены выводы дисплея:
*   RS, E, DB4, DB5, DB6, DB7
*/
LiquidCrystal lcd(9,6, 5, 10, 3, 2);

void setup()
{
 /* Инициализируем дисплей: 2 строки по 16 символов */
 lcd.begin(16, 2);
 /* Выводим на дисплей традиционную фразу (: */
 lcd.print("Lula Pilula");

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

       
  
       while(GPS.available())
        {
            buffer[count++]=GPS.read();
              
            if(count == 64)
           break;
        }
        //String str1 = ((char*)buffer); //String myString = String((char*)byteArray);
//        Serial.print(str1);



//        int i;
  //      for (i = 0; i < 63; i = i + 1) {
    //      Serial.println(myPins[i]);
      //  }

        //lcd.print(str1);
        Serial.write(buffer,count);
        //Serial.write(buffer);  lcd.print(GPS.read());
        
        clrBuff();
        count = 0;
        //delay(1000);

    }
   
}

void clrBuff()
{
    for (int i=0; i<count;i++)
    {
        buffer[i]=NULL;
    }
}
