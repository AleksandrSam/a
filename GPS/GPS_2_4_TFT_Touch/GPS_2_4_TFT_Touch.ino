#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h> 
#include <UTFT.h>
#include <SPI.h> // not necessary if Arduino IDE version >=1.6.6
#include <SoftwareSerial.h>

/* Set these to your desired credentials. */
const char *ssid = "_Speedometr";
const char *password = "1234";
int milisInterval = 2000;
int count = 0;
ESP8266WebServer server(80);

SoftwareSerial GPS(D2, D1);

float prevSpeed = 0;
unsigned long prevTime = 0;
unsigned short prevGpsFix = 0;


float data[4] = {0, 0, 0, 0 };
float data_old[4] = {1, 1, 1, 1 };

enum _ubxMsgType {
  MT_NONE,
  MT_NAV_POSLLH,
  MT_NAV_STATUS,
  MT_NAV_VELNED
};

const char UBLOX_INIT[] PROGMEM = {
  // Disable NMEA
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x24, // GxGGA off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2B, // GxGLL off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x32, // GxGSA off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39, // GxGSV off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x40, // GxRMC off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x47, // GxVTG off

  // Disable UBX
//  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0xDC, //NAV-PVT off
//  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0xB9, //NAV-POSLLH off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xC0, //NAV-STATUS off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x29, //NAV-VELNED off
  0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x55,
  //0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x96,0x00,0x00,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xC9,0x0C, // 38400 serial begin

  // Enable UBX
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x18,0xE1, //NAV-PVT on
  // 0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0xBE, //NAV-POSLLH on
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0xC5, //NAV-STATUS on
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x12, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x23, 0x2E, //NAV-VELNED on

  // Rate
 //  0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12 //(10Hz)
  0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A //(5Hz)
 //  0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39 //(1Hz)
  // 0xB5,0x62,0x06,0x08,0x06,0x00,0x3C,0x00,0x01,0x00,0x01,0x00,0x52,0x22 //16.67
};

const unsigned char UBX_HEADER[]        = { 0xB5, 0x62 };
const unsigned char NAV_POSLLH_HEADER[] = { 0x01, 0x02 };
const unsigned char NAV_STATUS_HEADER[] = { 0x01, 0x03 };
const unsigned char NAV_VELNED_HEADER[] = { 0x01, 0x012 };

struct NAV_POSLLH {
  unsigned char cls;
  unsigned char id;
  unsigned short len;
  unsigned long iTOW;
  long lon;
  long lat;
  long height;
  long hMSL;
  unsigned long hAcc;
  unsigned long vAcc;
};

struct NAV_STATUS {
  unsigned char cls;
  unsigned char id;
  unsigned short len;
  unsigned long iTOW;
  unsigned char gpsFix;
  char flags;
  char fixStat;
  char flags2;
  unsigned long ttff;
  unsigned long msss;
};

struct NAV_VELNED {
  unsigned char cls;
  unsigned char id;
  unsigned short len;
  unsigned long iTOW;
  long velN;
  long velE;
  long velD;
  unsigned long speed;
  unsigned long gSpeed;
  long heading;
  unsigned long sAcc;
  unsigned long cAcc;
};

union UBXMessage {
  NAV_POSLLH navPosllh;
  NAV_STATUS navStatus;
  NAV_VELNED navVelved;
};

UBXMessage ubxMessage;

void setup (  ) {
  // Just get some random numbers
  // randomSeed ( analogRead ( 0 ) );
  //
  WiFi.disconnect();

  displayInit();
  delay(100);
  
  Serial.begin(9600);
  GPS.begin(9600);
  WiFi.softAP(ssid, password);
  delay(100);

  for (int i = 0; i < sizeof(UBLOX_INIT); i++) {
    GPS.write( pgm_read_byte(UBLOX_INIT + i) );
    delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
  }

  // Serial.setDebugOutput(false);
  delay(500);
  IPAddress myIP = WiFi.softAPIP();
  server.on("/", handleRootWiFi);
  server.on("/data", getDataWiFi);
  server.begin();

  
  displayStartScreen();
}


void loop (  ) {
  server.handleClient();
  int msgType = processGPS();
  if ( msgType == MT_NAV_POSLLH ) {
    //    Serial.print("iTOW:");      Serial.print(ubxMessage.navPosllh.iTOW);
    //    Serial.print(" lat/lon: "); Serial.print(ubxMessage.navPosllh.lat/10000000.0f); Serial.print(","); Serial.print(ubxMessage.navPosllh.lon/10000000.0f);
    //    Serial.print(" hAcc: ");    Serial.print(ubxMessage.navPosllh.hAcc/1000.0f);
    //    Serial.println();
    //    myGLCD.setColor ( 0, 255, 255 );
    //    myGLCD.print ( String(ubxMessage.navPosllh.hAcc/1000.0f), 5, 15 );
    //    myGLCD.print ( "1", 5, 15 );
  }
  else if ( msgType == MT_NAV_STATUS ) {
    Serial.print("gpsFix:");  Serial.print(ubxMessage.navStatus.gpsFix);
    Serial.println();

    data[0] = ubxMessage.navStatus.gpsFix;
    if (data[0] == 0){
      data[1] = 0;
      data[3] = 0;
    }

  }
  else if ( msgType == MT_NAV_VELNED ) {

    // Serial.print(" speed: ");    Serial.println(ubxMessage.navVelved.speed*0.036);


    data[1] = ubxMessage.navVelved.speed;
    data[3] = ubxMessage.navVelved.iTOW;

    Serial.println(data[1]);
    unsigned int deltaTime;
    float distance = 0;
    if (data[0] > 1) {

      if (prevGpsFix == 0) {
          deltaTime = 0;
          data[1] = 0;
          prevSpeed = 0;
      }
      
      deltaTime = data[3] - prevTime;
      if (deltaTime>1000) {
        deltaTime = 0;
        data[1] = 0;
        prevSpeed = 0;
      }

      //Serial.print(deltaTime); Serial.print(";"); Serial.print(data[3]); Serial.print(";"); Serial.println(prevTime);
      
      if ( data[1] > 20) {
        
//        Serial.print(speed); Serial.print(";"); Serial.print(prevSpeed); Serial.print(";"); Serial.println(deltaTime);
//        Serial.println(data[2]);
//        if ((data[1] - prevSpeed) > 200) {
//          deltaTime = 0;
//          data[1] = 0;
//          prevSpeed = 0;
//        }
        distance = ((3 *  data[1] - prevSpeed) * deltaTime) / 2000;
        if (distance < 0 ) {
          distance = 0;
        }
        data[2] += distance;
        prevSpeed = data[1]; //cm/s
      }
      prevTime = data[3]; //ms
    }
    prevGpsFix = data[0];
  }

  tryToDisplay();

}

//-------GPS method

// The last two bytes of the message is a checksum value, used to confirm that the received payload is valid.
// The procedure used to calculate this is given as pseudo-code in the uBlox manual.
void calcChecksum(unsigned char* CK, int msgSize) {
  memset(CK, 0, 2);
  for (int i = 0; i < msgSize; i++) {
    CK[0] += ((unsigned char*)(&ubxMessage))[i];
    CK[1] += CK[0];
  }
}


// Compares the first two bytes of the ubxMessage struct with a specific message header.
// Returns true if the two bytes match.
boolean compareMsgHeader(const unsigned char* msgHeader) {
  unsigned char* ptr = (unsigned char*)(&ubxMessage);
  return ptr[0] == msgHeader[0] && ptr[1] == msgHeader[1];
}


// Reads in bytes from the GPS module and checks to see if a valid message has been constructed.
// Returns the type of the message found if successful, or MT_NONE if no message was found.
// After a successful return the contents of the ubxMessage union will be valid, for the
// message type that was found. Note that further calls to this function can invalidate the
// message content, so you must use the obtained values before calling this function again.
int processGPS() {
  static int fpos = 0;
  static unsigned char checksum[2];

  static byte currentMsgType = MT_NONE;
  static int payloadSize = sizeof(UBXMessage);

  while ( GPS.available() ) {

    byte c = GPS.read();
    Serial.write(c);

    if ( fpos < 2 ) {
      // For the first two bytes we are simply looking for a match with the UBX header bytes (0xB5,0x62)
      if ( c == UBX_HEADER[fpos] )
        fpos++;
      else
        fpos = 0; // Reset to beginning state.
    }
    else {
      // If we come here then fpos >= 2, which means we have found a match with the UBX_HEADER
      // and we are now reading in the bytes that make up the payload.

      // Place the incoming byte into the ubxMessage struct. The position is fpos-2 because
      // the struct does not include the initial two-byte header (UBX_HEADER).
      if ( (fpos - 2) < payloadSize )
        ((unsigned char*)(&ubxMessage))[fpos - 2] = c;

      fpos++;

      if ( fpos == 4 ) {
        // We have just received the second byte of the message type header,
        // so now we can check to see what kind of message it is.
        if ( compareMsgHeader(NAV_POSLLH_HEADER) ) {
          currentMsgType = MT_NAV_POSLLH;
          payloadSize = sizeof(NAV_POSLLH);
        }
        else if ( compareMsgHeader(NAV_STATUS_HEADER) ) {
          currentMsgType = MT_NAV_STATUS;
          payloadSize = sizeof(NAV_STATUS);
        }
        else if ( compareMsgHeader(NAV_VELNED_HEADER) ) {
          currentMsgType = MT_NAV_VELNED;
          payloadSize = sizeof(NAV_VELNED);
        }
        else {
          // unknown message type, bail
          fpos = 0;
          continue;
        }
      }

      if ( fpos == (payloadSize + 2) ) {
        // All payload bytes have now been received, so we can calculate the
        // expected checksum value to compare with the next two incoming bytes.
        calcChecksum(checksum, payloadSize);
      }
      else if ( fpos == (payloadSize + 3) ) {
        // First byte after the payload, ie. first byte of the checksum.
        // Does it match the first byte of the checksum we calculated?
        if ( c != checksum[0] ) {
          // Checksum doesn't match, reset to beginning state and try again.
          fpos = 0;
        }
      }
      else if ( fpos == (payloadSize + 4) ) {
        // Second byte after the payload, ie. second byte of the checksum.
        // Does it match the second byte of the checksum we calculated?
        fpos = 0; // We will reset the state regardless of whether the checksum matches.
        if ( c == checksum[1] ) {
          // Checksum matches, we have a valid message.
          return currentMsgType;
        }
      }
      else if ( fpos > (payloadSize + 4) ) {
        // We have now read more bytes than both the expected payload and checksum
        // together, so something went wrong. Reset to beginning state and try again.
        fpos = 0;
      }
    }
  }
  return MT_NONE;
}

//-  Wi-Fi  ----------------------------------


//---------------------------------


