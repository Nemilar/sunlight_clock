/* vim: set filetype=cpp: */

#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68


typedef struct 
{
	byte second;
	byte minute;
	byte hour;
	byte dayOfWeek;
	byte dayOfMonth;
	byte month;
	byte year;
} datetime;
datetime t;



byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

/* Modified from http://tronixstuff.com/2014/12/01/tutorial-using-ds1307-and-ds3231-real-time-clock-modules-with-arduino/ */
void readDS3231time()
{
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(0); // set DS3231 register pointer to 00h
	Wire.endTransmission();
        Serial.println("Begin request.");
	Wire.requestFrom(DS3231_I2C_ADDRESS, 7);

	// request seven bytes of data from DS3231 starting from register 00h
	   t.second = bcdToDec(Wire.read() & 0x7f);
	   t.minute = bcdToDec(Wire.read());
	   t.hour = bcdToDec(Wire.read() & 0x3f);
	   t.dayOfWeek = bcdToDec(Wire.read());
	   t.dayOfMonth = bcdToDec(Wire.read());
	   t.month = bcdToDec(Wire.read());
	   t.year = bcdToDec(Wire.read());
}
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void setup()
{
	Wire.begin();
	Serial.begin(9600);
	Serial.println("Booting...");
	delay(5000);
	Serial.println("Setting time...");
	setDS3231time(0,36,21,6,25,15);
	Serial.println("Time has been set.");
}



void loop()
{
	Serial.println("Read time.");
	readDS3231time();
	Serial.println(t.year);
	Serial.println("Sleep.");
	delay(1000);
}


