#include <math.h>
#include <stdio.h>

#define PI 3.1415926
#define ZENITH -.83
#define SUNRISE 1
#define SUNSET 2
#define NEVERSETS -999
#define NEVERRISES -998

float calculateSuntime(int year,int month,int day,float lat, float lng,int localOffset, int daylightSavings, int request) {
	/*
	   localOffset will be <0 for western hemisphere and >0 for eastern hemisphere
	   daylightSavings should be 1 if it is in effect during the summer otherwise it should be 0
	 */
	//1. first calculate the day of the year
	float N1 = floor(275 * month / 9);
	float N2 = floor((month + 9) / 12);
	float N3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
	float N = N1 - (N2 * N3) + day - 30;
	float t, H;
	float UT;

	//2. convert the longitude to hour value and calculate an approximate time
	float lngHour = lng / 15.0;      
	if (request == SUNRISE)
	{
		printf("Requested sunrise\n");
		t = N + ((6 - lngHour) / 24);  
	}
	else
	{
		printf("Requested sunset\n");
		t = N + ((18 - lngHour) / 24);  
	}
	//3. calculate the Sun's mean anomaly   
	float M = (0.9856 * t) - 3.289;

	//4. calculate the Sun's true longitude
	float L = fmod(M + (1.916 * sin((PI/180)*M)) + (0.020 * sin(2 *(PI/180) * M)) + 282.634,360.0);

	//5a. calculate the Sun's right ascension      
	float RA = fmod(180/PI*atan(0.91764 * tan((PI/180)*L)),360.0);

	//5b. right ascension value needs to be in the same quadrant as L   
	float Lquadrant  = floor( L/90) * 90;
	float RAquadrant = floor(RA/90) * 90;
	RA = RA + (Lquadrant - RAquadrant);

	//5c. right ascension value needs to be converted into hours   
	RA = RA / 15;

	//6. calculate the Sun's declination
	float sinDec = 0.39782 * sin((PI/180)*L);
	float cosDec = cos(asin(sinDec));

	//7a. calculate the Sun's local hour angle
	float cosH = (sin((PI/180)*ZENITH) - (sinDec * sin((PI/180)*lat))) / (cosDec * cos((PI/180)*lat));

	if (cosH > 1)
	{
		printf("norise with cosH=%f\n", cosH);
		return NEVERRISES;
	}
	else if (cosH < -1)
	{
		printf("noset with cosH=%f\n", cosH);
		return NEVERSETS;
	}

	/*   
		 if (cosH >  1) 
		 the sun never rises on this location (on the specified date)
		 if (cosH < -1)
		 the sun never sets on this location (on the specified date)
	 */

	//7b. finish calculating H and convert into hours

	if (request == SUNRISE)
	{
		H = 360 - (180/PI)*acos(cosH);   //   if if rising time is desired:
	}
	else if (request == SUNSET)
	{
		H = (180/PI)*acos(cosH);
//		H = acos(cosH);
	}

	H = H / 15;


	//8. calculate local mean time of rising/setting      
	float T = H + RA - (0.06571 * t) - 6.622;


	//9. adjust back to UTC
	UT = fmod(T - lngHour,24.0);

	//10. convert UT value to local time zone of latitude/longitude
	return UT + localOffset + daylightSavings;

}

struct LED {
	float latitude;
	float longitude;
	float sunriseTime;
	float sunsetTime;
	int localOffset;
};

int main(void)
{
	int WIDTH=32;
	int HEIGHT=16;
	int localOffset=0;
	float thisLat, thisLong, thisSunsetTime, thisSunriseTime;;
	LED map[HEIGHT][WIDTH];
	
	for (int h=0;h<HEIGHT;h++)
	{
		thisLat = ((90-(h*(5.625*2))));
		for (int w=0;w<WIDTH;w++)
		{
			thisLong = ((-180+(w*11.25)));
			map[h][w].latitude = thisLat;
			map[h][w].longitude = thisLong;

			if (w < 15.5)
			{
				localOffset = float(-12) + ((float(w)/float(15.0)) * float(12.0));
			}
			else
			{
				localOffset = float(0) + floor(((float(w-15)/float(15)) * float(12)));
			}

			thisSunriseTime = calculateSuntime(2015,12,21,thisLat,thisLong,localOffset,0,SUNRISE);
			thisSunsetTime = calculateSuntime(2015,12,21,thisLat,thisLong,localOffset,0,SUNSET);
			
			if (thisSunriseTime != NEVERRISES)
			{
				map[h][w].sunriseTime = fmod(24 + thisSunriseTime, 24.0);
			}
			else
			{
				map[h][w].sunriseTime = NEVERRISES;
				 
			}

			if (thisSunsetTime != NEVERSETS)
			{
				map[h][w].sunsetTime = fmod(24 + calculateSuntime(2015,12,21,thisLat,thisLong,localOffset,0,SUNSET), 24.0);
			}
			else
			{
				map[h][w].sunsetTime = NEVERSETS;
			}

			map[h][w].localOffset = localOffset;


			printf("For h=%d and w=%d, lat=%f and long=%f, computed sunrise=%f and sunset=%f with localOffset %d\n", h, w, thisLat, thisLong, map[h][w].sunriseTime, map[h][w].sunsetTime, localOffset);
		}

	}

	float currentTimeGMT = 01.50;
	float currentLocalTime;

	for (int h=0;h<HEIGHT;h++)
	{
//		printf("On row %d\n", h);
		for (int w=0;w<WIDTH;w++)
		{

			if (map[h][w].sunriseTime == NEVERRISES)
			{
				printf("0 ");
				continue;
			}
			else if (map[h][w].sunsetTime == NEVERSETS)
			{
				printf("1 ");
				continue;
			}

			currentLocalTime = currentTimeGMT + map[h][w].localOffset;
			if (currentLocalTime < 0)
			{
				currentLocalTime+=24;
			}
			else if (currentLocalTime > 24)
			{
				currentLocalTime-=24;
			}
			

//			printf("For h=%d and w=%d, lat=%f and long=%f, computed sunrise=%f and sunset=%f with localOffset %d and localtime %f\n", h, w, map[h][w].latitude, map[h][w].longitude, map[h][w].sunriseTime, map[h][w].sunsetTime, map[h][w].localOffset, currentLocalTime);
//			printf("%f", currentLocalTime);
			if ((currentLocalTime >= map[h][w].sunriseTime) && (currentLocalTime <= map[h][w].sunsetTime))
			{
				printf("1 ");
			}
			else
			{
				printf("0 ");
			}

		}
		printf("\n");

	}



	
	return 0;
}

