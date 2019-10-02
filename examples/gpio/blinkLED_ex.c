/*
 * Copyright (c) 2012-2015 Janick Bergeron
 * All Rights Reserved
 * 
 *   Licensed under the Apache License, Version 2.0 (the
 *   "License"); you may not use this file except in
 *   compliance with the License.  You may obtain a copy of
 *   the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in
 *   writing, software distributed under the License is
 *   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *   CONDITIONS OF ANY KIND, either express or implied.  See
 *   the License for the specific language governing
 *   permissions and limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "libsoc_board.h"
#include "libsoc_gpio.h"
#include "libsoc_debug.h"


void usage(const char* cmd)
{
  fprintf(stderr, "Usage: %s [options] -n name | -i id\n", cmd);
  fprintf(stderr, "\n");
  fprintf(stderr, "    Blink a LED driven by the GPIO specified by the name or numeric ID.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Options: -h         print this help message\n");
  fprintf(stderr, "\n");
}

int
main(int argc, char *argv[])
{
  board_config *board = libsoc_board_init();
  if (board == NULL) {
    fprintf(stderr, "ERROR: Cannot initialize board pin database.\n");
    exit(-1);
  }
  
  gpio *gpioLED = NULL;

  signed char optchar;
  
  while ((optchar = getopt(argc, argv, "dhi:n:")) >= 0) {
    switch (optchar) {

    case 'd':
      libsoc_set_debug(1);
      break;

    case 'i':
      if (gpioLED != NULL) {
	fprintf(stderr, "ERROR: Must specify only one of -n or -i.\n");
	usage(argv[0]);
	return -1;
      }
      gpioLED = libsoc_gpio_request(atoi(optarg), LS_SHARED);
      if (gpioLED == NULL) {
	fprintf(stderr, "ERROR: Unable to get pin #%d.\n", atoi(optarg));
	return -1;
      }
      break;
      
    case 'n':
      if (gpioLED != NULL) {
	fprintf(stderr, "ERROR: Must specify only one of -n or -i.\n");
	usage(argv[0]);
	return -1;
      }
      {
	int pin = libsoc_board_gpio_id(board, optarg);
	if (pin < 0) {
	  fprintf(stderr, "ERROR: Invalid GPIO pin name \"%s\"..\n", optarg);
	  return -1;
	}
	gpioLED = libsoc_gpio_request(pin, LS_SHARED);
	if (gpioLED == NULL) {
	  fprintf(stderr, "ERROR: Unable to get pin \"%s\" (aka #%d).\n", optarg, pin);
	  return -1;
	}
      }
      break;
      
    case '?':
    default:
      fprintf(stderr, "ERROR: Unknown options '%c' (%d).\n", optchar, optchar);

    case 'h':
      usage(argv[0]);
      exit(1);
    }
  }
  if (gpioLED == NULL) {
    fprintf(stderr, "ERROR: Must specify one of -n or -i.\n");
    usage(argv[0]);
    return -1;
  }
  
  // Set direction to OUTPUT, and initialize to LOW
  libsoc_gpio_set_direction(gpioLED, OUTPUT);
  libsoc_gpio_set_level(gpioLED, LOW);

  // Blink the LED at 1Hz for 5 secs
  int i;
  for (i = 0; i < 5; i++) {
    usleep(500000);
    libsoc_gpio_set_level(gpioLED, HIGH);
    usleep(500000);
    libsoc_gpio_set_level(gpioLED, LOW);
  }
  
  libsoc_gpio_free(gpioLED);
  libsoc_board_free(board);
  
  return 0;
}
