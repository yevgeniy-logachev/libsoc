//
// Copyright (c) 2013-2016 Janick Bergeron
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

#ifndef __LIBSOC_SPI__
#define __LIBSOC_SPI__

#include <stdint.h>

namespace libSOC {

  class spi {
    
  public:
    /**
     * Get the API class singleton for the device on the specified numbered SPI interface and CS
     * If additional CS lines are required, the ID or name of a GPIO (active low) pin can also be selected
     */
    static spi* get(uint32_t spiNum, uint32_t csNum);
    static spi* get(uint32_t spiNum, uint32_t csNum, int gpioID);
    static spi* get(uint32_t spiNum, uint32_t csNum, const char* gpioNameID);
    
    /**
     * Write character stream to device as one stream
     * (i.e. do not de-assert CS between characters)
     * Returns FALSE on failure.
     */
    bool send(unsigned char n, const char* wrbuf);
    
    /** Read the specified number of characters from the device as one stream
     *  into the specified character buffer.
     *  Returns FALSE on failure.
     */
    bool receive(unsigned char n, char* rdbuf);
    
  private:
    void *m_imp;
  };
  
}

#endif
