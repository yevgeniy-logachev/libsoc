/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1106 drivers

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


#ifndef __LIBSOC_SSD1106__
#define __LIBSOC_SSD1106__

#include <stdint.h>

#include "rgbDriver.hh"
#include "libsoc/gpio.hh"
#include "libsoc_i2c.h"

namespace libSOC {

  class SH1106: public RGB::driver {

  public:
    typedef enum {WIDTH = 128, HEIGHT = 64} size_t;

    SH1106(i2c* i2cDev, gpio* rst, unsigned char height = 32);

    void begin();
  
    void command(uint8_t c);
    void data(uint8_t c);
  
    /** Reset the display */
    virtual void reset(void);

    /** Clear the display */
    virtual void clear(void);

    /** Refresh the display */
    virtual void refresh(void);

    /** Return the width of the display, in pixels */
    virtual uint16_t getWidth(void);
  
    /** Return the height of the display, in pixels */
    virtual uint16_t getHeight(void);

    /** Set a color pixel */
    virtual void drawPixel(int16_t x, int16_t y, RGB::rgb_t color);

    /* Get the color of a pixel */
    virtual RGB::rgb_t getPixel(int16_t x, int16_t y);

  private:
    unsigned char m_height;
    i2c*  m_i2c;
    gpio* m_rst;
    int m_fd;
  };

}

#endif
