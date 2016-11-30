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

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "spi.hh"

#include "gpio.hh"

#include "libsoc_spi.h"
#include <map>

static std::map<uint64_t, spi*> m_singletons;

static unsigned int mapIndex(uint32_t spiNum, uint32_t csNum)
{
  return ((uint64_t) csNum) << 32 + spiNum;
}


//
// Internal implementation structure
//
typedef struct spi_s {
  spi          *imp;
  libSOC::gpio *cs;
} spi_t;


libSOC::spi*
libSOC::spi::get(uint32_t spiNum, uint32_t csNum)
{
  ::spi* s = m_singletons[mapIndex(spiNum, csNum)];

  if (s == NULL) {
    s = libsoc_spi_init(spiNum, csNum);
    if (s == NULL) return NULL;

    libsoc_spi_set_bits_per_word(s, BITS_8);
    // libsoc_spi_set_speed(s, 1000);
    libsoc_spi_set_mode(s, MODE_1);

    m_singletons[mapIndex(spiNum, csNum)] = s;
  }

  libSOC::spi *obj = new libSOC::spi();

  spi_t *imp = new spi_t;
  imp->imp = s;
  imp->cs  = NULL;

  obj->m_imp = imp;

  return obj;
}


libSOC::spi*
libSOC::spi::get(uint32_t spiNum, uint32_t csNum, int gpioID)
{
  libSOC::spi *obj = get(spiNum, csNum, -1);
  if (obj == NULL) return NULL;

  spi_t *imp = (spi_t*) obj->m_imp;
  if (gpioID >= 0) {
    imp->cs = libSOC::gpio::get(gpioID);
    if (imp->cs == NULL) {
      delete imp;
      delete obj;
      return NULL;
    }
  }

  return obj;
}


libSOC::spi*
libSOC::spi::get(uint32_t spiNum, uint32_t csNum, const char *gpioName)
{
  libSOC::spi *obj = get(spiNum, csNum, -1);
  if (obj == NULL) return NULL;

  spi_t *imp = (spi_t*) obj->m_imp;
  if (gpioName != NULL) {
    imp->cs = libSOC::gpio::get(gpioName);
    if (imp->cs == NULL) {
      delete imp;
      delete obj;
      return NULL;
    }
  }

  return obj;
}


bool
libSOC::spi::send(unsigned char n, const char* wrbuf)
{
  spi_t *imp = (spi_t*) m_imp;
  
  if (imp->cs != NULL) imp->cs->setValue(0);

  bool rc = (libsoc_spi_write(imp->imp, (uint8_t*) wrbuf, n) == EXIT_SUCCESS);

  if (imp->cs != NULL) imp->cs->setValue(1);

  return rc;
}


bool
libSOC::spi::receive(unsigned char n, char* rdbuf)
{
  spi_t *imp = (spi_t*) m_imp;
  
  if (imp->cs != NULL) imp->cs->setValue(0);

  bool rc = (libsoc_spi_read(((spi_t*) m_imp)->imp, (uint8_t*) rdbuf, n) == EXIT_SUCCESS);

  if (imp->cs != NULL) imp->cs->setValue(1);

  return rc;
}
