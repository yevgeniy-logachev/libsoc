//
// Copyright (c) 2016-2019 Janick Bergeron
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

#include "linux/gpiod.h"
#include "libsoc_board.h"

#include <map>

#include <errno.h>
#include <stdio.h>
#include <string.h>


static gpiod_chip*                                 g_chip = NULL;
static std::map<unsigned int, libSOC::gpio*>  g_singletons;
static board_config                               *g_board      = NULL;


libSOC::gpio*
libSOC::gpio::get(unsigned int pin)
{
  if (g_chip == NULL) {
    const char* path = "/dev/gpiochip0";
    g_chip = gpiod_chip_open(path);
    if (g_chip == NULL) {
      fprintf(stderr, "ERROR: Unable to open GPIO chip \"%s\": %s\n", path, strerror(errno));
      return NULL;
    }
  }

  libSOC::gpio *p = g_singletons[pin];

  if (p == NULL) {
    struct gpiod_line *imp = gpiod_chip_get_line(g_chip, pin);
    if (imp == NULL) {
      fprintf(stderr, "ERROR: Unable to get GPIO pin %d: %s\n", pin, strerror(errno));
      return NULL;
    }

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
  struct gpiod_line_request_config cfg = {"libSOC++",
					  GPIOD_LINE_REQUEST_DIRECTION_OUTPUT,
					  0};

  return gpiod_line_request((struct gpiod_line*) m_imp, &cfg, 0) == 0;
}


bool
libSOC::gpio::makeInput(inputMode_t mode)
{
  struct gpiod_line_request_config cfg = {"libSOC++",
					  GPIOD_LINE_REQUEST_DIRECTION_INPUT,
					  (mode == PULL_DN) ? GPIOD_LINE_REQUEST_FLAG_OPEN_DRAIN : (mode == PULL_UP) ? GPIOD_LINE_REQUEST_FLAG_OPEN_SOURCE : 0};

  return gpiod_line_request((struct gpiod_line*) m_imp, &cfg, 0) == 0;
}


bool
libSOC::gpio::isInput()
{
  return gpiod_line_direction((struct gpiod_line*) m_imp) == GPIOD_LINE_REQUEST_DIRECTION_INPUT;
}


bool
libSOC::gpio::setValue(bool val)
{
  return gpiod_line_set_value((struct gpiod_line*) m_imp, val) == 0;
}


bool
libSOC::gpio::getValue()
{
  return gpiod_line_get_value((struct gpiod_line*) m_imp);
}
    
    
