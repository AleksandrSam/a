
#include <TinyGPS.h>
#include <LiquidCrystal_I2C.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DEFINES //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EARTH_RADIUS_METERS  6372795.0f // Earth radius, meters
#define MINIMUM_ACCUMULATE_DISTANCE 13 // Minimum distance length to add to the overall trip
#define SCRREN_COUNT  3 // how many screens are in the system (counting starts at 0)
#define DEBOUNCE_DELAY 25 // how mony time wait for button debounce
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITS AND  VARIABLES //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
LiquidCrystal_I2C lcd(0x27,16,2);

const int button_A = 8; // screen choose button
const int button_B = 9; // dist reset button

int button_A_state = 0;
int button_B_state = 0;
int button_A_prev_state = 1;
int button_B_prev_state = 1;

int analogInput = 0; // voltmeter input
int value = 0; // input value for A2D
float vin = 0.0; // the final voltage value

byte timeZone = 0; // used in hour calculation according to timezone

TinyGPS gps; // GPS device definition

unsigned long time = 0;
const unsigned long time_over = 100; // millisecond
const int draw_screen = 5; // 5 * time_over = 0.5 sec
const int print_serial = 10; // 10 * time_over = 1 sec

int draw_screen_cont = 0;
int print_serial_cont = 0;


int incomingByte; // stores the incoming GPS serial data

float speed_kmh = 0; // speed from GPS sensor
float heading_deg = 0; // heading from GPS sensor
float altitude_m = 0; // altitude from GPS sensor

byte screen = 0; // current screeen to show

float distance = 0; // measured distance
float trip = 0; // measured trip

float flat, flon, prev_flat, prev_flon; // current and previous coordinates
unsigned long age; // age of GPS coordinates, ms
int year; // yesr, from GPS
byte month, day, hour, minute, second, hundredths; // time and date from GPS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void printFloat(double f, int digits = 2); // definition for TOP-DOWN design

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP /////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{ 
  Serial.begin(19200); // init hardware serial

  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
  lcd.setCursor(5, 4);

  showSplash(); // show splash
  time = millis();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP /////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  screen_switch(); // check if screen scroll button was pressed
  void_dist(); // check if distance reset button was pressed
  
  GPS_Update(gps); // precess GPS data
  
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////

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
     }
   delay(3000);
   lcd.clear(); 
   }
   else
   {
     if (screen == 0)  // Latitude longitude speed and heading
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
     if (screen == 1)  // Distance, Average speed and maximum speed
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
     if (screen == 3)  // Batt voltage
     {  
       value = analogRead(analogInput);
       vin = (value * 4.8) / 1024.0;
       lcd.setCursor(0, 0);
       lcd.print("Battery Voltage");
       lcd.setCursor(4, 1);
       lcd.print(vin);
       lcd.print(" volts");
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void showSplash() // show splash at the init
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("-=MindHunteR=-'s");
  lcd.setCursor(0, 1);
  lcd.print("GPS  speedometer");
  delay(3000);
  lcd.clear();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void screen_switch(void) // switch the screen when button is pressed
{
  button_A_prev_state = button_A_state;
  button_A_state = digitalRead(button_A);
  if (button_A_state == LOW) delay(DEBOUNCE_DELAY);
  if ((button_A_state == LOW) && (digitalRead(button_A) == LOW) && (button_A_prev_state))
  {
    screen++;
    lcd.clear();
  } 
  if (screen > SCRREN_COUNT) screen = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
  
void void_dist(void) // reset the trip when button is pressed
{
  button_B_prev_state = button_B_state;
  button_B_state = digitalRead(button_B);
  if (button_B_state == LOW) delay(DEBOUNCE_DELAY*30);
  if ((button_B_state == LOW) && (digitalRead(button_B) == LOW) && (button_B_prev_state))
  {
    trip = 0;
    distance = 0;
    lcd.clear();
  } 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRECESS GPS DATA //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void GPS_Update(TinyGPS &gps)
{
  while( Serial.available( ) ) // when serial data is available
  {
    incomingByte = Serial.read( );
   
    if( gps.encode( incomingByte ) )      // when valid NMEA sentence received
    {
      gps.f_get_position(&flat, &flon, &age); // decode coordinates
      gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
      // decode data, time
        
      heading_deg = gps.f_course(); // decode heading
      speed_kmh = gps.f_speed_kmph(); // decode speed
      altitude_m = gps.f_altitude(); // decode altitude
      
      // If we're moving, calculate and save distance traveled
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
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

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
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
This uses the ‘haversine’ formula to calculate the great-circle distance between two points – that is,
the shortest distance over the earth’s surface – giving an ‘as-the-crow-flies’ distance between the points
(ignoring any hills, of course!).
Haversine
formula:   
a = sin²(Δφ/2) + cos(φ1).cos(φ2).sin²(Δλ/2)
c = 2.atan2(√a, √(1−a))
d = R.c
where   φ is latitude, λ is longitude, R is earth’s radius (mean radius = 6,371km)
    note that angles need to be in radians to pass to trig functions!
JavaScript:   

var R = 6371; // km
var dLat = (lat2-lat1).toRad();
var dLon = (lon2-lon1).toRad();
var lat1 = lat1.toRad();
var lat2 = lat2.toRad();

var a = Math.sin(dLat/2) * Math.sin(dLat/2) +
        Math.sin(dLon/2) * Math.sin(dLon/2) * Math.cos(lat1) * Math.cos(lat2); 
var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a)); 
var d = R * c;

The haversine formula1 ‘remains particularly well-conditioned for numerical computation even at small
distances’ – unlike calculations based on the spherical law of cosines. The ‘versed sine’ is 1-cosθ,
and the ‘half-versed-sine’ (1-cosθ)/2 = sin²(θ/2) as used above. It was published by Roger Sinnott in
Sky & Telescope magazine in 1984 (“Virtues of the Haversine”), though known about for much longer by
navigators. (For the curious, c is the angular distance in radians, and a is the square of half the
chord length between the points). A (surprisingly marginal) performance improvement can be obtained,
of course, by factoring out the terms which get squared.
*/

