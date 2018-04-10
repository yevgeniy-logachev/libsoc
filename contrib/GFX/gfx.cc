/***********************************
This is a our graphics core library, for all our displays. 
We'll be adapting all the
existing libaries to use this core to make updating, support 
and upgrading easier!

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
Subsequent modifications by J. Bergeron <janick@bergeron.com>
  - Refactored driver into a Strategy Pattern
  - Port to BoneLib
  - Port to libsoc

BSD license, check license.txt for more information
All text above must be included in any redistribution
****************************************/


#include "gfx.hh"

using namespace libSOC;
using namespace RGB;

#include "glcdfont.c"

#define swap(a, b) { int16_t t = a; a = b; b = t; }
#define abs(x) (((x)>0)?(x):(-(x)))
#define _BV(x) (1<<(7-(x)))


gfx::gfx(driver &drv) :
  m_drv(drv),
  m_auto_refresh(1),
  m_cursor_x(0), m_cursor_y(0),
  m_textcolor(RGB::black), m_textbgcolor(RGB::black),
  m_textsize(1), m_wrap(true)
{
}


uint16_t
gfx::getWidth(void)
{
  return m_drv.getWidth();
}
 

uint16_t
gfx::getHeight(void)
{ 
  return m_drv.getHeight();
}


void
gfx::refreshScreen()
{
  m_drv.refresh();
}


bool
gfx::setAutoRefresh(bool is_on)
{
  bool prev = m_auto_refresh;
  m_auto_refresh = is_on;
  return prev;
}


void
gfx::clearScreen(void)
{
  m_drv.clear();
  m_refresh();
  m_cursor_x = 0;
  m_cursor_y = 0;
}


void
gfx::fillScreen(rgb_t color)
{
  fillRect(0, 0, getWidth(), getHeight(), color);
  m_refresh();
}


// bresenham's algorithm - thx wikpedia
void
gfx::drawLine(uint16_t x0, uint16_t y0, 
	      uint16_t x1, uint16_t y1, 
	      rgb_t color)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      m_drv.drawPixel(y0, x0, color);
    } else {
      m_drv.drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }

  m_refresh();
}


void
gfx::drawFastVLine(uint16_t x, uint16_t y, 
		   uint16_t h, rgb_t color)
{
  for (uint16_t i = 0; i < h; i++) {
    m_drv.drawPixel(x, y+i, color);
  }

  m_refresh();
}


void
gfx::drawFastHLine(uint16_t x, uint16_t y, 
		   uint16_t w, rgb_t color)
{
  for (uint16_t i = 0; i < w; i++) {
    m_drv.drawPixel(x+i, y, color);
  }

  m_refresh();
}


void
gfx::drawRect(uint16_t x, uint16_t y, 
	      uint16_t w, uint16_t h, 
	      rgb_t color)
{
  bool refrsh = setAutoRefresh(0);

  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);

  setAutoRefresh(refrsh);
  m_refresh();
}


void
gfx::fillRect(uint16_t x, uint16_t y,
	      uint16_t w, uint16_t h, 
	      rgb_t color)
{
  bool refrsh = setAutoRefresh(0);

  for (uint16_t i = 0; i < w; i++) {
    drawFastVLine(x+i, y, h, color); 
  }

  setAutoRefresh(refrsh);
  m_refresh();
}


void
gfx::drawRoundRect(uint16_t x, uint16_t y,
		   uint16_t w, uint16_t h,
		   uint16_t r, rgb_t color)
{
  bool refrsh = setAutoRefresh(0);

  drawFastHLine(x+r  , y    , w-2*r, color); // Top
  drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  drawFastVLine(  x    , y+r  , h-2*r, color); // Left
  drawFastVLine(  x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r    , y+r    , r, TopLeft,  color);
  drawCircleHelper(x+w-r-1, y+r    , r, TopRight, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, BotLeft,  color);
  drawCircleHelper(x+r    , y+h-r-1, r, BotRight, color);

  setAutoRefresh(refrsh);
  m_refresh();
}


void
gfx::fillRoundRect(uint16_t x, uint16_t y,
		   uint16_t w, uint16_t h,
		   uint16_t r, rgb_t color)
{
  bool refrsh = setAutoRefresh(0);

  // smarter version
  fillRect(x,       y+r, r,     h-2*r, color); // Left
  fillRect(x+r,     y,   w-2*r, h,     color); // Middle
  fillRect(x+w-r-1, y+r, r,     h-2*r, color); // Right

  // draw four corners
  fillCircleHelper(x+r    , y+r    , r, TopLeft,  color);
  fillCircleHelper(x+w-r-1, y+r    , r, TopRight, color);
  fillCircleHelper(x+w-r-1, y+h-r-1, r, BotLeft,  color);
  fillCircleHelper(x+r    , y+h-r-1, r, BotRight, color);

  setAutoRefresh(refrsh);
  m_refresh();
}


void
gfx::drawCircle(uint16_t x, uint16_t y,
		uint16_t r, rgb_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t dx = 0;
  int16_t dy = r;

  m_drv.drawPixel(x,   y+r, color);
  m_drv.drawPixel(x,   y-r, color);
  m_drv.drawPixel(x+r, y,   color);
  m_drv.drawPixel(x-r, y,   color);

  while (dx < dy) {
    if (f >= 0) {
      dy--;
      ddF_y += 2;
      f += ddF_y;
    }
    dx++;
    ddF_x += 2;
    f += ddF_x;
    m_drv.drawPixel(x + dx, y + dy, color);
    m_drv.drawPixel(x - dx, y + dy, color);
    m_drv.drawPixel(x + dx, y - dy, color);
    m_drv.drawPixel(x - dx, y - dy, color);
    m_drv.drawPixel(x + dy, y + dx, color);
    m_drv.drawPixel(x - dy, y + dx, color);
    m_drv.drawPixel(x + dy, y - dx, color);
    m_drv.drawPixel(x - dy, y - dx, color);
  }

  m_refresh();
}


void
gfx::fillCircle(uint16_t x, uint16_t y,
		uint16_t r, rgb_t color)
{
  bool refrsh = setAutoRefresh(0);

  drawFastVLine(x, y-r, 2*r+1, color);
  fillCircleHelper(x, y, r, TopLeft + TopRight + BotLeft + BotRight, color);

  setAutoRefresh(refrsh);
  m_refresh();
}


void
gfx::drawTriangle(uint16_t x0, uint16_t y0,
		  uint16_t x1, uint16_t y1, 
		  uint16_t x2, uint16_t y2,
		  rgb_t color)
{
  bool refrsh = setAutoRefresh(0);

  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);

  setAutoRefresh(refrsh);
  m_refresh();
}


void
gfx::fillTriangle (uint16_t x0, uint16_t y0,
		   uint16_t x1, uint16_t y1, 
		   uint16_t x2, uint16_t y2,
		   rgb_t color)
{
  int16_t a, b, y, last;

  bool refrsh = setAutoRefresh(0);

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }

  setAutoRefresh(refrsh);
  m_refresh();
}


void
gfx::drawBitmap(uint16_t x, uint16_t y, 
		const uint8_t *bitmap,
		uint16_t w, uint16_t h,
		rgb_t color)
{
  for (int16_t j=0; j<h; j++) {
    for (int16_t i=0; i<w; i++ ) {
      if (bitmap[i + (j/8)*w] & _BV(j%8)) {
	m_drv.drawPixel(x+i, y+j, color);
      }
    }
  }

  m_refresh();
}


void
gfx::setCursor(uint16_t x, uint16_t y)
{
  m_cursor_x = x;
  m_cursor_y = y;
}


void
gfx::moveCursor(uint16_t x, uint16_t y)
{
  m_cursor_x += x;
  m_cursor_y += y;
}


uint16_t
gfx::getCursorX(void)
{
  return m_cursor_x;
}


uint16_t
gfx::getCursorY(void)
{
  return m_cursor_y;
}


void
gfx::setTextSize(uint8_t s)
{
  m_textsize = (s > 0) ? s : 1;
}


void
gfx::setTextColor(rgb_t c)
{
  m_textcolor = c;
  m_textbgcolor = c;
}


void 
gfx::setTextColor(rgb_t c, rgb_t b)
{
  m_textcolor = c;
  m_textbgcolor = b; 
}


void
gfx::setTextWrap(bool w)
{
  m_wrap = w;
}


void
gfx::write(char c)
{
  if (c == '\n') {
    m_cursor_y += m_textsize*8;
    m_cursor_x = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(c, m_cursor_x, m_cursor_y, m_textcolor, m_textbgcolor, m_textsize);
    m_cursor_x += m_textsize*6;
    if (m_wrap && (m_cursor_x > (getWidth() - m_textsize*6))) {
      m_cursor_y += m_textsize*8;
      m_cursor_x = 0;
    }
  }
}


void
gfx::write(const char* c)
{
  bool refrsh = setAutoRefresh(0);

  while (*c != '\0') write(*c++);

  setAutoRefresh(refrsh);
  m_refresh();
}


void
gfx::m_refresh(void)
{
  if (m_auto_refresh) m_drv.refresh();
}


void
gfx::drawCircleHelper(uint16_t x, uint16_t y,
		      uint16_t r, uint8_t corners,
		      rgb_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t dx     = 0;
  int16_t dy     = r;

  while (dx < dy) {
    if (f >= 0) {
      dy--;
      ddF_y += 2;
      f     += ddF_y;
    }
    dx++;
    ddF_x += 2;
    f     += ddF_x;
    if (corners & BotLeft) {
      m_drv.drawPixel(x + dx, y + dy, color);
      m_drv.drawPixel(x + dy, y + dx, color);
    } 
    if (corners & TopRight) {
      m_drv.drawPixel(x + dx, y - dy, color);
      m_drv.drawPixel(x + dy, y - dx, color);
    }
    if (corners & BotRight) {
      m_drv.drawPixel(x - dy, y + dx, color);
      m_drv.drawPixel(x - dx, y + dy, color);
    }
    if (corners & TopLeft) {
      m_drv.drawPixel(x - dy, y - dx, color);
      m_drv.drawPixel(x - dx, y - dy, color);
    }
  }
}


void
gfx::fillCircleHelper(uint16_t x, uint16_t y,
		      uint16_t r, uint8_t corners,
		      rgb_t color) {

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t dx     = 0;
  int16_t dy     = r;

  while (dx < dy) {
    if (f >= 0) {
      dy--;
      ddF_y += 2;
      f     += ddF_y;
    }
    dx++;
    ddF_x += 2;
    f     += ddF_x;
    if (corners & BotLeft) {
      drawFastVLine(x + dx, y, dy, color);
      drawFastVLine(x + dy, y, dx, color);
    } 
    if (corners & TopRight) {
      drawFastVLine(x + dx, y-dy, dy, color);
      drawFastVLine(x + dy, y-dx, dx, color);
    }
    if (corners & BotRight) {
      drawFastVLine(x - dy, y, dx, color);
      drawFastVLine(x - dx, y, dy, color);
    }
    if (corners & TopLeft) {
      drawFastVLine(x - dy, y-dx, dx, color);
      drawFastVLine(x - dx, y-dy, dy, color);
    }
  }
}


void
gfx::drawChar(uint8_t c,
	      uint16_t x, uint16_t y,
	      rgb_t color, rgb_t bg,
	      uint8_t size)
{
  if((x >= getWidth())        || // Clip right
     (y >= getHeight())       || // Clip bottom
     ((x + 5 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = font[(c*5)+i];

    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          m_drv.drawPixel(x+i, y+j, color);
        else {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          m_drv.drawPixel(x+i, y+j, bg);
        else {  // big size
          fillRect(x+i*size, y+j*size, size, size, bg);
        } 	
      }
      line >>= 1;
    }
  }
}



#ifdef TEST

#include <stdio.h>
#include <unistd.h>

#include "SH1106.hh"

int
main(int argc, const char* argv[])
{
  libSOC::SH1106 drv(NULL, // I2C
		      NULL, // Rst
                      64);

  printf("Starting Test...\n");

  drv.begin();

  libSOC::gfx fx(drv);

  int X = fx.getWidth();
  int Y = fx.getHeight();

  int max = (X > Y) ? X : Y;

  fx.refreshScreen();
  sleep(1);
  fx.clearScreen();

  // Draw a 'X' across the display
  fx.drawLine(0,0, X,Y, libSOC::RGB::black);
  fx.drawLine(X,0, 0,Y, libSOC::RGB::black);
  usleep(500000);
  fx.clearScreen();

  fx.fillScreen(libSOC::RGB::black);
  usleep(500000);
  fx.fillScreen(libSOC::RGB::white);
  usleep(500000);

  fx.drawCircle(X/2, Y/2, 24, libSOC::RGB::black);
  fx.drawCircle(X/2, Y/2, 18, libSOC::RGB::black);
  fx.drawCircle(X/2, Y/2, 12, libSOC::RGB::black);
  fx.drawCircle(X/2, Y/2,  6, libSOC::RGB::black);
  usleep(500000);

  fx.clearScreen();
  fx.fillCircle(X/2, Y/2, 24, libSOC::RGB::black);
  fx.fillCircle(X/2, Y/2, 18, libSOC::RGB::white);
  fx.fillCircle(X/2, Y/2, 12, libSOC::RGB::black);
  fx.fillCircle(X/2, Y/2,  6, libSOC::RGB::white);
  usleep(500000);

  fx.setAutoRefresh(0);
  fx.clearScreen();
  fx.drawRoundRect( 0,0,  X,   Y,    6, libSOC::RGB::black);
  fx.drawRoundRect( 6,6,  X-12,Y-12, 6, libSOC::RGB::black);
  fx.drawRoundRect(12,12, X-24,Y-24, 6, libSOC::RGB::black);
  fx.refreshScreen();
  usleep(500000);

  fx.fillRoundRect( 0,0,  X,   Y,    6, libSOC::RGB::black);
  fx.fillRoundRect( 6,6,  X-12,Y-12, 6, libSOC::RGB::white);
  fx.fillRoundRect(12,12, X-24,Y-24, 6, libSOC::RGB::black);
  fx.refreshScreen();
  usleep(500000);

  fx.setAutoRefresh(1);
  fx.clearScreen();
  fx.drawTriangle( 42,2,  63,44, 21,44, libSOC::RGB::black);
  usleep(500000);

  fx.fillTriangle( 42,2,  63,44, 21,44, libSOC::RGB::black);
  usleep(500000);

  fx.clearScreen();
  fx.setCursor(0, 0);
  fx.setTextColor(libSOC::RGB::black, libSOC::RGB::white);
  fx.setTextSize(1);
  fx.setTextWrap(1);
  fx.write('A');
  fx.write('B');
  fx.write('C');
  fx.setAutoRefresh(0);
  fx.write('D');
  fx.write('E');
  fx.write('F');
  fx.write('G');
  fx.setAutoRefresh(1);
  fx.write('\n');
  fx.setTextColor(libSOC::RGB::white, libSOC::RGB::black);
  fx.setTextSize(2);
  fx.setTextWrap(1);
  fx.write("ABCDEFG");
  usleep(500000);

  fx.clearScreen();
  fx.setCursor(0, 0);
  fx.setTextSize(1);
  fx.setTextColor(libSOC::RGB::white);
  fx.fillScreen(libSOC::RGB::black);
  fx.write("Mary had a\nlittle lamb,\nand he went wherever\nshe went.");
  sleep(2);

  fx.clearScreen();
  drv.reset();

  printf("Done!\n");
  return 0;
}

#endif
