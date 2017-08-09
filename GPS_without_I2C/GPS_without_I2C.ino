#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <LiquidCrystal.h>
//LiquidCrystal lcd(7,6,5,4, 3, 2);
LiquidCrystal lcd(9,8,7,6,5,4);

SoftwareSerial ss(2,3);

TinyGPS gps;

int incomingByte; // stores the incoming GPS serial data

float speed_kmh = 0; // speed from GPS sensor
float heading_deg = 0; // heading from GPS sensor
float altitude_m = 0; // altitude from GPS sensor

byte screen = 0; // current screeen to show

float distance = 0; // measured distance
float trip = 0; // measured trip

unsigned long time = 0;
const unsigned long time_over = 100; // millisecond
const int draw_screen = 5; // 5 * time_over = 0.5 sec
const int print_serial = 10; // 10 * time_over = 1 sec

int draw_screen_cont = 0;
int print_serial_cont = 0;

byte timeZone = 0; // used in hour calculation according to timezone

float flat, flon, prev_flat, prev_flon; // current and previous coordinates
unsigned long age; // age of GPS coordinates, ms
int year; // yesr, from GPS
byte month, day, hour, minute, second, hundredths; // time and date from GPS

#define EARTH_RADIUS_METERS  6372795.0f // Earth radius, meters
#define MINIMUM_ACCUMULATE_DISTANCE 13 // Minimum distance length to add to the overall trip
#define SCRREN_COUNT  3 // how many screens are in the system (counting starts at 0)
#define DEBOUNCE_DELAY 25 // how mony time wait for button debounce

int p = 12;
boolean playSoundState = false;

void printFloat(double f, int digits = 2); // definition for TOP-DOWN design

int count=0;
void setup()
{
    Serial.begin(19200);
    ss.begin(9600);

    lcd.begin(16, 2);

    showSplash(); // show splash
    time = millis();
    playSound();
}

void loop()
{

  GPS_Update();

  if ( millis() > (time + time_over)) // drive the screens, feed serial with data and check the battery
  {
    time = millis();
    draw_screen_cont++;
    if (draw_screen_cont >= draw_screen)
      {
        draw_screen_cont = 0;
        send_data_to_lcd();
      }
    print_serial_cont++;
    if (print_serial_cont >= print_serial)
    {  
      print_serial_cont = 0;
      send_data_to_serial();
    }    
  } 
  
}

void send_data_to_lcd(void) // update LCD witd data
{
 gps.f_get_position(&flat, &flon, &age);
 if ((age > 3000) || (age == TinyGPS::GPS_INVALID_AGE))
   {
     lcd.clear();
     if (age == TinyGPS::GPS_INVALID_AGE)
     {
       lcd.setCursor(4, 0);
       lcd.print("WARNING!");
       lcd.setCursor(0, 1);
       lcd.print("No fix detected");
     }
     else if (age > 3000)
     {
       lcd.setCursor(4, 0);
       lcd.print("WARNING!");
       lcd.setCursor(4, 1);       
       lcd.print("DATA LOSS");
       playSoundState = false;
     }
   delay(3000);
   lcd.clear(); 
   }
   else
   {
     if (screen == 1)  // Latitude longitude speed and heading
     {
       lcd.setCursor(0, 0);
       printLCDFloat(flat, 5);
       lcd.print(" ");
       lcd.setCursor(0, 1);
       printLCDFloat(flon, 5);
       lcd.print(" ");
       lcd.setCursor(12, 1); 
       lcd.print("    ");
       lcd.setCursor(12, 1);
       printLCDFloat(heading_deg, 0);
       lcd.print((char)223);
       
       lcd.setCursor(10, 0);
       if (speed_kmh < 99.5)
         lcd.print(" ");
       if (speed_kmh < 9.5)
         lcd.print(" ");
       printLCDFloat(speed_kmh, 0);
       lcd.print("kmh");
     }
     if (screen == 0)  // Distance, Average speed and maximum speed
     {
       lcd.setCursor(0, 0);
       lcd.print("Speed ");
       printLCDFloat(speed_kmh, 0);
       lcd.print(" km/h   ");
       lcd.setCursor(0, 1);
       lcd.print("Dist ");
       printLCDFloat(trip/1000, 3);
       lcd.print(" km   ");
     }
     if (screen == 2)  // Date and Time
     {
       int timeIZone;
       if (timeZone <= 12)
         timeIZone = (int)timeZone;
       else
         timeIZone = ((int)timeZone - 12) * - 1;
       
       int ihour = (int)hour + timeIZone;
       if (ihour > 23)
       {
         ihour = ihour - 24;
         day = day - 1;
       }
       if (ihour < 0)
       {
         ihour = ihour + 24;
         day = day - 1;
       }
       
       lcd.setCursor(0, 0);
       lcd.print("Date ");
       if (day < 10)
         lcd.print("0");
       printLCDFloat(day, 0);
       lcd.print("/");
       if (month < 10)
         lcd.print("0");
       printLCDFloat(month, 0);
       lcd.print("/");
       printLCDFloat(year, 0);
    
       lcd.setCursor(0, 1);
       lcd.print("Time ");
       if (ihour < 10)
         lcd.print("0");
       printLCDFloat(ihour, 0);
       lcd.print(":");
       if (minute < 10)
         lcd.print("0");
       printLCDFloat(minute, 0);
       lcd.print(":");
       if (second < 10)
         lcd.print("0");
       printLCDFloat(second, 0);
     }
     if (!playSoundState)
     {
       playSound();
       playSoundState = true;
     }
   }

}

void printLCDFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0)
  {
    lcd.print("-");
    number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  char sTemp[10];
  ltoa(int_part, sTemp, 10);
  lcd.print(sTemp);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    lcd.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    ltoa(toPrint, sTemp, 10);
    lcd.print(sTemp);
    remainder -= toPrint; 
  } 
}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0)
  {
    Serial.print('-');
    number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint; 
  } 
}

void send_data_to_serial(void) // prints data over the serial interface
{
  Serial.println("Acquired Data");
  Serial.println("-------------");
  Serial.print("Lat/Long: "); 
  printFloat(flat, 5); 
  Serial.print(" / "); 
  printFloat(flon, 5);
  Serial.print("   Fix age: "); 
  Serial.print(age); 
  Serial.println(" ms.");
  Serial.print("Date: "); 
  Serial.print(static_cast<int>(day)); 
  Serial.print("/"); 
  Serial.print(static_cast<int>(month)); 
  Serial.print("/"); 
  Serial.print(year);
  Serial.print("  Time: "); 
  Serial.print(static_cast<int>(hour)); 
  Serial.print(":"); 
  Serial.print(static_cast<int>(minute)); 
  Serial.print(":"); 
  Serial.print(static_cast<int>(second)); 
  Serial.print("."); 
  Serial.print(static_cast<int>(hundredths));
  Serial.println();
  Serial.print("Alt: "); 
  printFloat(altitude_m); 
  Serial.print(" m");
  Serial.print("  Course: "); 
  printFloat(heading_deg); 
  Serial.print(" deg");
  Serial.print("  Speed: "); 
  printFloat(speed_kmh); 
  Serial.print(" km/h");   
  Serial.println(); 
  Serial.println("-------------");
  Serial.println();
}

void GPS_Update()
{
  if (ss.available() > 0)
  {
    char c = ss.read();
    
    lcd.setCursor(0,1);
    Serial.write(c); // uncomment this line if you want to see the GPS data flowing
   // lcd.print(c);
    
  //  newData = gps.encode(c);
    if (gps.encode(c)) 
    {
      gps.f_get_position(&flat, &flon, &age); // decode coordinates
      gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
      // decode data, time
        
      heading_deg = gps.f_course(); // decode heading
      speed_kmh = gps.f_speed_kmph(); // decode speed
      altitude_m = gps.f_altitude(); // decode altitude
      
      if(speed_kmh > 3.2f )
      {
        if (prev_flat != 0.0f)
        {
          distance = DistanceBetween2Points(flat, flon, prev_flat, prev_flon);
          if(distance >= MINIMUM_ACCUMULATE_DISTANCE) // Accumulate every MINIMUM_ACCUMULATE_DISTANCE
          {
            trip += distance;
            prev_flat = flat;
            prev_flon = flon;
          }
        }
        else
        {
          prev_flat = flat;
          prev_flon = flon;
        }
      }
      
    }

   
    
//    if (speedKm.isUpdated())
//    {
//      lcd.setCursor(0, 0);
//      lcd.print(speedKm.value()); 
//    }
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Great Circle distance calculation - HABVERSINE  ///////////////////////////////////////////////////////
// http://www.movable-type.co.uk/scripts/latlong.html //////////////////////////////////////////////////// 
// Returns the distance between two lat/lon points on this great Earth ///////////////////////////////////
// (Note: Assumes sea level, does not take into account altitude. Close enough.) /////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
float DistanceBetween2Points( float Lat, float Lon, float prev_Lat, float prev_Lon)
{
  float dLat = radians(Lat - prev_Lat);
  float dLon = radians(Lon - prev_Lon);
  float a = sin( dLat / 2.0f ) * sin( dLat / 2.0f ) +
            cos( radians( prev_Lat ) ) * cos( radians( Lat ) ) *
            sin( dLon / 2.0f ) * sin( dLon / 2.0f );
            
  float d = 2.0f * atan2( sqrt( a ), sqrt( 1.0f - a ) );
  return d * EARTH_RADIUS_METERS;
}

byte raccoon[8] = 
{
  B10001,
  B01110,
  B11111,
  B10101,
  B01110,
  B00100,
  B10001,
  B10001,
};

//================= CAR animation ==============
byte carAnim_1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b10000,
  0b10000,
  0b11000,
  0b00000,
  0b00000
};

byte carAnim_2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b11100,
  0b11100,
  0b11110,
  0b10000,
  0b00000
};

byte carAnim_3[8] = {
  0b00000,
  0b10000,
  0b01000,
  0b11111,
  0b11111,
  0b11111,
  0b10100,
  0b01000
};

byte carAnim_4[8] = {
  0b11000,
  0b00100,
  0b00010,
  0b11111,
  0b11111,
  0b11111,
  0b00101,
  0b00010
};

byte carAnim_5[8] = {
  0b11110,
  0b00001,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b00001,
  0b00000
};

byte carAnim_6[8] = {
  0b11111,
  0b10000,
  0b10000,
  0b11111,
  0b11111,
  0b11111,
  0b01000,
  0b10000
};

byte carAnim_7[8] = {
  0b00111,
  0b00100,
  0b00100,
  0b11111,
  0b11111,
  0b11111,
  0b01010,
  0b00100
};

byte carAnim_8[8] = {
  0b00001,
  0b00001,
  0b00001,
  0b01111,
  0b01111,
  0b11111,
  0b00010,
  0b00001
};
//============================================

void showSplash() // show splash at the init
{ 
  lcd.clear();
  
  //lcd.createChar(1, raccoon); 

  lcd.createChar(0, carAnim_1); 
  lcd.createChar(1, carAnim_2); 
  lcd.createChar(2, carAnim_3); 
  lcd.createChar(3, carAnim_4); 
  lcd.createChar(4, carAnim_5); 
  lcd.createChar(5, carAnim_6); 
  lcd.createChar(6, carAnim_7); 
  lcd.createChar(7, carAnim_8); 
  
  lcd.setCursor(0, 0); 
  lcd.print("\1");
//
//  for (int j = 0; j<100; j++)
//  {
//  for (int i = 0; i<8; i++)
//  { 
//    lcd.setCursor(0, 0); 
//    lcd.write(i);
//    if (i>=2) 
//    {
//      
//    }
//    delay(100);
//  }
//  }

  int chainLCD[16] = {};
  lcd.setCursor(0, 0); 
  delay(3000);
  lcd.print(chainLCD[0]);

  for (int i = 0; i<100; i++)
  {
    if (chainLCD[0] == 0)
    {
      // init firs chain
      chainLCD[0]++;
    };

    for (int j=1; j<16; j++)
    {
      if (chainLCD[j-1] == 4)
      {
        chainLCD[j]++;
      };
    };
    for (int j=0; j<16; j++)
    {
      if (chainLCD[j] > 0 && chainLCD[j] < 10)
      {
        lcd.setCursor(j, 1);
        lcd.write(chainLCD[j]-1);
        chainLCD[j]++;
   
        if (chainLCD[j] == 10)
        {    
            lcd.setCursor(j, 1);
            lcd.print(" ");
        };
      };
    };
    delay(100);
  }
//  for (int j = 0; j<3; j++)
//  {
//
//
//  }

  lcd.setCursor(2, 0);
  lcd.print("Rozumnii enot");
  
  lcd.setCursor(0, 1);
  lcd.print("GPS  speedometer");


  
  
  delay(3000);
  lcd.clear();
}

void playSound()
{
  //tone (p, 600);
  //delay(100);
  //noTone(p); 
}

