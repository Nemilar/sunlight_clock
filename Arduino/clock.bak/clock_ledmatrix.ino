#include <Arduino.h>

#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

void setDaylight(int x, int y)
{
  matrix.drawPixel(x, y, matrix.Color333(7,7,7));
}

void setNighttime(int x, int y)
{
  matrix.drawPixel(x, y, matrix.Color333(0, 0, 0));
}

void matrixBootupTest(int d)
{
  for (int x=0; x<32; x++)
  {
    for (int y=0;y<16;y++)
    {
      matrix.drawPixel(x, y, matrix.Color333(7,7,7));
      delay(d);
    }
  }
}

void initMatrix()
{
  matrix.begin();
}
