#include <stdint.h>
#include <SeeedTouchScreen.h>
#include <TFTv2.h>
#include <SPI.h>

#include "tftgfx.h"
#include "demos.h"

// screen is 240x320

TouchScreen ts = TouchScreen(XP, YP, XM, YM); //init TouchScreen port pins
bool landscape = true;

enum drawmode_t {
  DM_RAW,
  
  DM_POINT,
  DM_TRI,
  
  DM_RECT,
  DM_FILLRECT,
  DM_ELLIPSE,
  DM_FILLELLIPSE,
  
  DM_LINE,
  DM_TEXT,
  DM_PLASMA,
  DM_CUBE,
} drawmode;

uint8_t draw_mode = DM_CUBE;

void setup()
{
    Tft.TFTinit();
    Serial.begin(115200);
    Tft.fillRectangle(0, 0, 280, 320, CYAN);
    Tft.drawString("markh", 0, 0, 8, WHITE);
    Tft.drawString("markhamilton.info", 20, 280, 2, WHITE);
}

void updateSerial()
{
}

inline void updateTouch()
{
  Point p = ts.getPoint();
  if (p.z > __PRESURE) {
    Serial.write((uint8_t)map(p.x, TS_MINX, TS_MAXX, 0, 255));        
    Serial.write((uint8_t)map(p.y, TS_MINY, TS_MAXY, 0, 255));
  }
}

void loop()
{
  
  updateSerial();
  
  switch(draw_mode) {
    case DM_CUBE:
      drawCube();
      break;
    case DM_PLASMA:
//      drawPlasma();
      break;
  }

  updateTouch();
}
