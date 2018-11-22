unsigned long timeFromStart;
unsigned int displayUpdateTime = 1000;
unsigned long displayTime = 0;

// Modify the line below to match your display and wiring:
// UTFT myGLCD ( ILI9341_S5P, 15, 4, 4 );
//UTFT myGLCD ( ST7735, D7, D5, D8, D2, D2 );
UTFT myGLCD ( ST7735, D7, D5, D0, D6, D6 );

// Declare which fonts we will be using
extern uint8_t BigFont[];

// 0 - gpsFix
// 1 - speed in km/hours
// 2 - odometr sm
// 3 - iTOW
void displayInit(){
    myGLCD.InitLCD (  );
    myGLCD.setFont ( BigFont );
}

void tryToDisplay() {
  timeFromStart = millis();
  if (timeFromStart > displayTime+displayUpdateTime) {
    //Serial.println(timeFromStart-displayTime);
    displayTime = timeFromStart;

    if (data[0]!=data_old[0]){
      data_old[0] = data[0];
      myGLCD.print(String(data[0]), 100, 5);
    }
    if (data[1]!=data_old[1]){
      data_old[1] = data[1];
      myGLCD.print(String(data[1]*0.036), 5, 5);
    }
    if (data[2]!=data_old[2]){
      data_old[2] = data[2]; 
      myGLCD.print(String(data[2]/100000.00), 5, 80);
    }
  }
}


void displayStartScreen() {
   myGLCD.setColor ( 0, 0, 0 );
   myGLCD.fillRect ( 0, 0, 128, 128 );

   myGLCD.setColor ( 255, 255, 255 );  
}



