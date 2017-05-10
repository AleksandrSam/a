#include <SPI.h>

#include <TinyGPS.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <SD.h>


/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

TinyGPS gps;
SoftwareSerial ss(7, 8); //7-blue, 8-white
LiquidCrystal lcd(9, 6, 5, 10, 3, 2);
File sensorDataFile;
char* SENSOR_DATA_FILE_NAME = "1.txt";  // Название файла
unsigned char buffer[64]; // buffer
int count=0;

void setup()
{
  Serial.begin(19200);
  ss.begin(9600);
  lcd.begin(16, 2);


  Serial.print("Simple TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  Serial.println();

   // Инициализируем СД карту
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  char msg[] = "";
  
  // Открываем (или создаем, если его нет) файл для записи данных с датчика
  sensorDataFile = SD.open(SENSOR_DATA_FILE_NAME, FILE_WRITE);
  
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
     //  Serial.write(c); // uncomment this line if you want to see the GPS data flowing
       
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
 lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("C=");lcd.print(chars); 
    lcd.setCursor(0, 1);
    lcd.print("S=");lcd.print(sentences);lcd.print(" E=");lcd.print(failed);
    
  if (newData)
  {
    if (sensorDataFile)
        {
         
          float flat, flon;
          unsigned long age;

          Serial.write("Catched");
          lcd.clear();
          
          gps.f_get_position(&flat, &flon, &age);
          lcd.setCursor(0, 0);
          lcd.print (flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
          lcd.setCursor(0, 1);
          //lcd.print (flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);  
          lcd.print (gps.f_speed_kmph());
          
          sensorDataFile.println(gps.f_speed_kmph());
        }
        else
        {
          Serial.println("File error, \t");
        }
  }else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("C=");lcd.print(chars); 
    lcd.setCursor(0, 1);
    lcd.print("S=");lcd.print(sentences);lcd.print(" E=");lcd.print(failed);
  }
//
//  while(ss.available())
//        {
//           buffer[count++]=ss.read();
//              
//            if(count == 64)
//            break;
//        }
//        Serial.write(buffer, count);
//        //Serial.print("\n");
//        
//        clrBuff();
//        count = 0;
    
  
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  
  if (chars == 0) {
    Serial.println("** No characters received from GPS: check wiring **");
    lcd.setCursor(0, 0);
    lcd.print("No characters"); 
    lcd.setCursor(0, 1);
    lcd.print("received from GPS");
  }
  sensorDataFile.close(); //close writing to the file
  delay(2000);
}


void clrBuff()
{
    for (int i=0; i<count;i++)
    {
        buffer[i]=NULL;
    }
}
