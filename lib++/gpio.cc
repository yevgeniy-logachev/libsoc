//
// Copyright (c) 2016 Janick Bergeron
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


#include "gpio.hh"


#include "libsoc_gpio.h"
#include "libsoc_board.h"

#include <map>


static std::map<unsigned int, libSOC::gpio*>  g_singletons;
static board_config                          *g_board      = NULL;


libSOC::gpio*
libSOC::gpio::get(unsigned int pin)
{
  libSOC::gpio *p = g_singletons[pin];

  if (p == NULL) {
    void *imp = libsoc_gpio_request(pin, LS_SHARED);
    if (imp == NULL) return NULL;

    p = new libSOC::gpio();
    p->m_imp = imp;
    
    g_singletons[pin] = p;
  }

  return p;
}


libSOC::gpio*
libSOC::gpio::get(const char* name)
{
  if (g_board == NULL) {
    g_board = libsoc_board_init();
  }

  int pin = libsoc_board_gpio_id(g_board, name);
  if (pin < 0) {
    fprintf(stderr, "ERROR: GPIO pin \"%s\" unknown.\n", name);
    return NULL;
  }

  return get((unsigned int) pin);
}

        
bool
libSOC::gpio::makeOutput()
{
  return (libsoc_gpio_set_direction(m_imp, OUTPUT) == EXIT_SUCCESS);
}


bool
libSOC::gpio::makeInput()
{
  return (libsoc_gpio_set_direction(m_imp, INPUT) == EXIT_SUCCESS);
}


bool
libSOC::gpio::isInput()
{
  return (libsoc_gpio_get_direction(m_imp) == INPUT);
}


bool
libSOC::gpio::setValue(bool val)
{
  return (libsoc_gpio_set_level(m_imp, (val) ? HIGH : LOW) == EXIT_SUCCESS);
}


bool
libSOC::gpio::getValue()
{
  return (libsoc_gpio_get_level(m_imp) ==  HIGH);
}
    
    
