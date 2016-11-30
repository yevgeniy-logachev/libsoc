//
// Copyright (c) 2012-2016 Janick Bergeron
// All Rights Reserved
//
//   Licensed under the Apache License, Version 2.0 (the
//   "License"); you may not use this file except in
//   compliance with the License.  You may obtain a copy of
//   the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in
//   writing, software distributed under the License is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//   CONDITIONS OF ANY KIND, either express or implied.  See
//   the License for the specific language governing
//   permissions and limitations under the License.
//


#ifndef __LIBSOC_RGB_DRIVER__
#define __LIBSOC_RGB_DRIVER__

#include <stdint.h>

namespace libSOC {

  namespace RGB {

    typedef struct rgb_s {
      uint8_t red;
      uint8_t green;
      uint8_t blue;
    } rgb_t;
    
  }

  bool operator==(RGB::rgb_t l, RGB::rgb_t r);
  bool operator!=(RGB::rgb_t l, RGB::rgb_t r);


  namespace RGB {

    const rgb_t white = {255, 255, 255};
    const rgb_t black = {  0,   0,   0};
    const rgb_t red   = {255,   0,   0};
    const rgb_t green = {  0, 255,   0};
    const rgb_t blue  = {  0,   0, 255};
    
    
    /** Base class for an abstract RGB display driver */
    class driver {
    public:
      
      /** Reset the display */
      virtual void reset(void) = 0;
      
      /** Clear the display */
      virtual void clear(void) = 0;
      
      /** Refresh the display */
      virtual void refresh(void) = 0;
      
      /** Return the width of the display, in pixels */
      virtual uint16_t getWidth(void) = 0;
      
      /** Return the height of the display, in pixels */
      virtual uint16_t getHeight(void) = 0;
      
      /** Set a color pixel */
      virtual void drawPixel(int16_t x, int16_t y, rgb_t color) = 0;
      
      /* Get the color of a pixel */
      virtual rgb_t getPixel(int16_t x, int16_t y) = 0;
    };
    
  }
  
}

#endif
