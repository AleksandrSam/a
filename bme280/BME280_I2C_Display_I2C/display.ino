#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
#define OLED_RESET 2
Adafruit_SSD1306 display(OLED_RESET);


boolean _fireWork = false;

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 
 };
  
 
void displayBegin(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  display.clearDisplay();
}

void displayTemperature(float t, float h){
 display.clearDisplay();

 display.setTextColor(WHITE);
 display.setCursor(0,0);
 
 display.setTextSize(3);
 display.print(t, 1);  
 display.setTextSize(2);  
 display.println(" C");
 
 display.println("");

 display.setTextSize(3);
 display.print(h, 1);
 display.setTextSize(2);
 display.println(" %");
    
 display.display();

 if (_fireWork){
  displayFire(true);
 }
}

void displayFire(boolean fireWork) {
  if (fireWork){
    display.drawBitmap(110, 0,  logo16_glcd_bmp, 16, 16, 1);
    display.display();
  }
  _fireWork = fireWork;
}

