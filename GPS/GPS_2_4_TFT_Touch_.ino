#include <UTFT.h>
#include <SPI.h> // not necessary if Arduino IDE version >=1.6.6
#include <XPT2046.h>
#include <SoftwareSerial.h>

SoftwareSerial GPS(D1, D4);
char buffer[64]; // buffer
int count=0;

// Declare which fonts we will be using
extern uint8_t SmallFont[];

// Modify the line below to match your display and wiring:
UTFT myGLCD ( ILI9341_S5P, 15, 4, 4 );


void setup (  ) {
  
 
    // Just get some random numbers
   // randomSeed ( analogRead ( 0 ) );

    // Setup the LCD
    myGLCD.InitLCD (  );
    myGLCD.setFont ( SmallFont );

   Serial.begin(19200);
    GPS.begin(9600);
    Serial.setDebugOutput(false);
   //myGLCD.clrScr (  );
   // myGLCD.setColor ( 255, 0, 0 );
   // myGLCD.fillRect ( 0, 0, 320, 240 );
    
   myGLCD.setColor ( 0, 255, 255 );
  myGLCD.print ( String("Si11n"), 5, 15 );

   
}

static uint16_t prev_x = 0xffff, prev_y = 0xffff;

void loop (  ) {
  if (GPS.available())
    {
        while(GPS.available())
        {
            buffer[count++]=GPS.read();
            if(count == 64)
            break;
        }
            
        myGLCD.setColor ( 0, 255, 255 );
        myGLCD.print ( String(buffer), 5, 15 );


        //for (i=0;i<count;i++) {
         //buffer[i]
        //}
        
        Serial.write(buffer, count);
        clrBuff();
        count = 0;
    }
}

void clrBuff()
{
    for (int i=0; i<count;i++)
    {
        buffer[i]=NULL;
    }
}
