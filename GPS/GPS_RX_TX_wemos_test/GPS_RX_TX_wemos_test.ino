#include <SoftwareSerial.h>
SoftwareSerial GPS(D1, D4);
unsigned char buffer[64]; // buffer
int count=0;

void setup()
{
    GPS.begin(9600);
    Serial.begin(19200);
}

void loop()
{
    if (GPS.available())
    {
        while(GPS.available())
        {
            buffer[count++]=GPS.read();
            if(count == 64)
            break;
        }
        Serial.write(buffer,count);
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
