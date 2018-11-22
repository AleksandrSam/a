
#include <UTFT.h>
#include <SPI.h> // not necessary if Arduino IDE version >=1.6.6
#include <XPT2046.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];

// Modify the line below to match your display and wiring:
UTFT myGLCD ( ILI9341_S5P, 15, 5, 4 );
XPT2046 touch(/*cs=*/ 16, /*irq=*/ 0);

void setup (  ) {

    // Just get some random numbers
    randomSeed ( analogRead ( 0 ) );

    // Setup the LCD
    myGLCD.InitLCD (  );
    myGLCD.setFont ( SmallFont );

    touch.setRotation(touch.ROT270);

    touch.begin(240, 320);  // Must be done before setting rotation
    touch.setCalibration(1846, 1779, 304, 272);

}

static uint16_t prev_x = 0xffff, prev_y = 0xffff;

void loop (  ) {
   
  if (touch.isTouching()) {
    uint16_t x, y;
    touch.getPosition(x, y);
    if (prev_x == 0xffff) {
      myGLCD.drawPixel(x, y);
    } else {
      myGLCD.drawLine(prev_x, prev_y, x, y);
    }
    prev_x = x;
    prev_y = y;
  } else {
    prev_x = prev_y = 0xffff;
  }

  delay(20);
}
