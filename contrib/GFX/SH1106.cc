/*********************************************************************
This is a library for our Monochrome OLEDs based on SH1106 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
Subsequent modifications by J. Bergeron <janick@bergeron.com>
  - Refactored driver into a Strategy Pattern
  - Port to BoneLib
  - Port to libsoc

BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "SH1106.hh"

using namespace libSOC;
using namespace RGB;


#define SH1106_SETCONTRAST         0x81
#define SH1106_DISPLAYALLON_RESUME 0xA4
#define SH1106_DISPLAYALLON        0xA5
#define SH1106_NORMALDISPLAY       0xA6
#define SH1106_INVERTDISPLAY       0xA7
#define SH1106_DISPLAYOFF          0xAE
#define SH1106_DISPLAYON           0xAF

#define SH1106_SETDISPLAYOFFSET    0xD3
#define SH1106_SETCOMPINS          0xDA

#define SH1106_SETVCOMDETECT       0xDB

#define SH1106_SETDISPLAYCLOCKDIV  0xD5
#define SH1106_SETPRECHARGE        0xD9

#define SH1106_SETMULTIPLEX        0xA8

#define SH1106_SETSTARTLINE        0x40
#define SH1106_SETLOWCOLUMN        0x00
#define SH1106_SETHIGHCOLUMN       0x10
#define SH1106_SETPAGEADDR         0xB0

#define SH1106_COMSCANINC          0xC0
#define SH1106_COMSCANDEC          0xC8

#define SH1106_SEGREMAP            0xA0

#define SH1106_SETPUMPVOUT         0x30

#define _BV(x) (1<<(x))

// the memory buffer for the LCD, in horizontal addressing order
static uint8_t buffer[SH1106::WIDTH * SH1106::HEIGHT / 8] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF,
0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8,
0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01,
0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF,
0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF,
0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF,
0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F,
0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03,
0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03,
0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F,
0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0x7C, 0x7D, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00, 0x30, 0x30, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F,
0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0,
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00,
0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0E,
0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFC,
0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0x06,
0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF8,
0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76, 0xE6, 0xCE, 0xCC, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x03,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0C,
0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E, 0x0C, 0x18, 0x0C, 0x0F,
0x07, 0x01, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00,
0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x07,
0x07, 0x0C, 0x0C, 0x18, 0x1C, 0x0C, 0x06, 0x06, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// reduces how much is refreshed, which speeds it up!
// originally derived from Steve Evans/JCW's mod but cleaned up and
// optimized
#define enablePartialUpdate
#ifdef enablePartialUpdate
static uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif

static void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax) {
#ifdef enablePartialUpdate
  if (xmin < xUpdateMin) xUpdateMin = xmin;
  if (xmax > xUpdateMax) xUpdateMax = xmax;
  if (ymin < yUpdateMin) yUpdateMin = ymin;
  if (ymax > yUpdateMax) yUpdateMax = ymax;
#endif
}

SH1106::SH1106(i2c *i2cDev, gpio* rst, unsigned char height)
  : m_i2c(i2cDev), m_rst(rst), m_height(height)
{
  if (height != 32 && height != 64) {
    fprintf(stderr, "ERROR: Height of SH1106 OLED must be 32 or 64\n");
    exit(1);
  }

  if (0 && m_i2c == NULL) {
    fprintf(stderr, "ERROR: Bad I2C interface/device specified for SH1106::i2c.\n");
    exit(1);
  }

  // set pin directions
  if (m_rst != NULL) m_rst->makeOutput();

  if ((m_fd = open("/dev/i2c-1" , O_RDWR)) < 0) {
    fprintf(stderr, "ERROR: Unable to open I2C device.\n");
    exit(1);
  }

  if (ioctl(m_fd, I2C_SLAVE, 0x3C) < 0) {
    fprintf(stderr, "ERROR: Unable to set I2C slave address.\n");
    exit(1);
  }
}


/** Reset the display */
void
SH1106::reset(void)
{
  if (m_rst == NULL) return;
  // toggle RST low to reset
  m_rst->setValue(0);
  m_rst->setValue(1);
}

/** Clear the display */
void
SH1106::clear(void)
{
  memset(buffer, 0x00, getWidth()*m_height/8);
  updateBoundingBox(0, 0, getWidth()-1, m_height-1);
}

/** Refresh the display */
void
SH1106::refresh(void) {

#ifdef enablePartialUpdate

  unsigned int minRow = yUpdateMin/8;
  unsigned int maxRow = yUpdateMax/8;

  unsigned int minCol = xUpdateMin;
  unsigned int maxCol = xUpdateMax;

  for(int row = minRow; row <= maxRow; row++) {
    command(SH1106_SETPAGEADDR   | row);
    command(SH1106_SETLOWCOLUMN  | ((minCol+2) & 0x0F)) ;//set lower column address
    command(SH1106_SETHIGHCOLUMN | (((minCol+2) >> 4) & 0x0F)) ;//set higher column address
    
    for(int col = minCol; col <= maxCol; col += 16) {
      i2c_smbus_write_i2c_block_data(m_fd, 0x40, 16, buffer + (getWidth()*row)+col);
    }
  }

  xUpdateMin = getWidth() - 1;
  xUpdateMax = 0;
  yUpdateMin = getHeight() - 1;
  yUpdateMax = 0;

#else

  uint8_t *p = buffer;
  for (uint8_t k=0; k<8; k++) {

    command(SH1106_SETPAGEADDR   | k);
    command(SH1106_SETLOWCOLUMN  | 0x02) ;//set lower column address
    command(SH1106_SETHIGHCOLUMN | 0x00) ;//set higher column address

    for (int i = 0; i < 8; i++) {
      i2c_smbus_write_i2c_block_data(m_fd, 0x40, 16, p);
      p += 16;
    }
  }

#endif
}

/** Return the width of the display, in pixels */
uint16_t
SH1106::getWidth(void)
{
  return WIDTH;
}

/** Return the height of the display, in pixels */
uint16_t
SH1106::getHeight(void)
{
  return m_height;
}

/** Set a color pixel */
void
SH1106::drawPixel(int16_t x, int16_t y, rgb_t color)
{
  if ((x < 0) || (x >= getWidth()) || (y < 0) || (y >= getHeight()))
    return;

  uint8_t shade = (color.red + color.green + color.blue) / 3;

  // x is which column
  if (shade < 128)
    buffer[x + (y/8)*getWidth()] |= _BV(y%8);  
  else
    buffer[x + (y/8)*getWidth()] &= ~_BV(y%8); 

  updateBoundingBox(x,y,x,y);
}

/* Get the color of a pixel */
rgb_t
SH1106::getPixel(int16_t x, int16_t y)
{
  if ((x < 0) || (x >= getWidth()) || (y < 0) || (y >= getHeight()))
    return RGB::black;

  if ((buffer[x + (y/8)*getWidth()] >> (7-(y%8))) & 0x1) return RGB::black;
  return RGB::white;
}

void
SH1106::begin(uint8_t *splash)
{
  reset();

  command(SH1106_DISPLAYOFF);                    // 0xAE
  command(SH1106_SETMULTIPLEX);                  // 0xA8
  if (getHeight() == 32) {
    command(0x1F);
  } else {
    command(0x3F);
  }

  command(SH1106_SETSTARTLINE  | 0x00);
  command(SH1106_SETLOWCOLUMN  | 0x02);
  command(SH1106_SETHIGHCOLUMN | 0x00);
  command(SH1106_SETPAGEADDR   | 0x00);

  command(SH1106_SEGREMAP | 0x01);               // SEG0 is column 127
  command(SH1106_NORMALDISPLAY);

  command(0xad);    /*set charge pump enable*/
  command(0x8b);    /*external VCC   */
  command(SH1106_SETPUMPVOUT);    /*0X30---0X33  set VPP   9V liangdu!!!!*/

  command(SH1106_COMSCANDEC);                    // Scan from COM[N-1] to 0
  command(SH1106_SETDISPLAYOFFSET);              // 0xD3
  command(0x00);                                  // no offset
  command(SH1106_SETDISPLAYCLOCKDIV);            // 0xD5
  command(0x80);                                  // the suggested ratio 0x80
  command(SH1106_SETPRECHARGE);                  // 0xd9
  command(0x1F);
  command(SH1106_SETCOMPINS);                    // 0xDA
  command(0x12);
  command(SH1106_SETVCOMDETECT);                 // 0xDB
  command(0x40);

  command(SH1106_SETCONTRAST);                   // 0x81
  command(0x80);

  command(SH1106_DISPLAYALLON_RESUME);           // 0xA4
  command(SH1106_DISPLAYON);//--turn on oled panel

  // Use the AdaFruit splash screen by default
  if (splash != NULL) {
    memcpy(buffer, splash, sizeof(buffer));
  }
  
  // Refresh the screen (will display the splash screen)
  updateBoundingBox(0, 0, getWidth()-1, m_height-1);
  refresh();
}


void
SH1106::command(uint8_t c)
{
  i2c_smbus_write_byte_data(m_fd, 0x00, c);
}

void
SH1106::data(uint8_t c)
{
  i2c_smbus_write_byte_data(m_fd, 0x40, c);
}

bool
SH1106::saveScreen(const char* fname)
{
  FILE *fp = fopen(fname, "w");
  if (fp == NULL) {
    fprintf(stderr, "ERROR: Cannot save screen to file \"%s\": ", fname);
    perror(0);
    return false;
  }

  fprintf(fp, "static uint8_t buffer[SH1106::WIDTH * SH1106::HEIGHT / 8] = {\n");
  for (int i = 0; i < sizeof(buffer); i++) {
    if (i % 16 == 0) fprintf(fp, "   ");
    fprintf(fp, "0x%02x, ", buffer[i]);
    if (i % 16 == 15) fprintf(fp, "\n");
  }
  fprintf(fp, "};\n");
  fclose(fp);

  return true;
}


#ifdef TEST

#include <libsoc/debug.hh>


int
main(int argc, const char* argv[])
{
  libSOC::debug::set_level(0);
  
  //#define I2C
#ifdef I2C
  libSOC::SH1106 drv(NULL, // I2C
		     NULL, // Rst GPIO
		     64);
#else
  libSOC::SH1106 drv(libSOC::gpio::get("CSID4"),     // CLK
		      libSOC::gpio::get("CSID6"),  // DIN
		      libSOC::gpio::get("CSID1"),     // D/C
		      libSOC::gpio::get("CSID2"),    // CS
		      libSOC::gpio::get("CSID0"),    // RST
		      64);
#endif

  int X = drv.getWidth();
  int Y = drv.getHeight();

  int max = (X > Y) ? X : Y;

  float x, y;
  float dx = (X * 1.0) / max;
  float dy = (Y * 1.0) / max;


  printf("Starting test...\n");

  drv.begin();
  printf("Note: The Adafruit splash screen should be displayed on the screen...\n");
  sleep(2);

  drv.refresh();
  printf("Note: The screen should have been cleared...\n");
  sleep(2);

  // Draw a 'X' across the display
  for (x = 0, y = 0; x < X; x += dx, y += dy) {
    drv.drawPixel((int)x, (int)y, libSOC::RGB::black);
    drv.drawPixel((int)(drv.getWidth() - x), (int)y, libSOC::RGB::black);
  }
  drv.refresh();
  printf("Note: There should be a large 'X' across the screen...\n");
  sleep(2);


  // Flash a medium square in the middle
  libSOC::RGB::rgb_t color;

  X = drv.getWidth() / 2 - 10;
  Y = drv.getHeight() / 2 - 10;
  for (int i = 0; i < 11; i++) {
    color = (i%2) ? libSOC::RGB::white : libSOC::RGB::black;
    for (x = 0; x < 20; x++) {
      for (y = 0; y < 20; y++) {
	drv.drawPixel(X+x, Y+y, color);
      }
    }
    drv.refresh();
  }

  // Flash a smaller square in the middle
  X = drv.getWidth() / 2 - 4;
  Y = drv.getHeight() / 2 - 4;
  for (int i = 0; i < 10; i++) {
    color = (i%2) ? libSOC::RGB::white : libSOC::RGB::black;
    for (x = 0; x < 8; x++) {
      for (y = 0; y < 8; y++) {
	drv.drawPixel(X+x, Y+y, color);
      }
    }
    drv.refresh();
  }
  sleep(1);

  drv.reset();

  printf("Done!\n");
  return 0;
}
#endif
