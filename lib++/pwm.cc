//
// Copyright (c) 2017 Janick Bergeron
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


#include "pwm.hh"
#include "libsoc_pwm.h"


#include <stdlib.h>
#include <stdio.h>
#include <map>


static std::map<unsigned int, libSOC::pwm*>  g_singletons;


libSOC::pwm*
libSOC::pwm::get(unsigned int chip, unsigned int num, bool invert)
{
  chip &= 0xFFFF;
  num  &= 0xFFFF;

  unsigned int key = (chip << 16) + num;
  libSOC::pwm *p = g_singletons[key];

  if (p == NULL) {
    void *imp = libsoc_pwm_request(chip, num, LS_SHARED);
    if (imp == NULL) return NULL;

    p = new libSOC::pwm();
    p->mImp    = imp;
    p->mInvert = invert;
    p->mPeriod = 0;

    int rc = EXIT_SUCCESS;
    /* Not supported in Raspberry Pi?
    if (p->mInvert) rc = libsoc_pwm_set_polarity((::pwm*) p->mImp, INVERSED);
    else rc = libsoc_pwm_set_polarity((::pwm*) p->mImp, NORMAL);
    */
    if (rc == EXIT_FAILURE) {
      libsoc_pwm_free((::pwm*) p->mImp);
      return NULL;
    }
    
    g_singletons[key] = p;
  }

  if (p->mInvert != invert) {
    fprintf(stderr, "ERROR: PWM%d.%d already configured with %s polarity.\n", chip, num,
	    (p->mInvert) ? "INVERTED" : "NORMAL");
    return NULL;
  }

  return p;
}


bool
libSOC::pwm::enable()
{
  if (mPeriod == 0) return false;

  if (libsoc_pwm_set_enabled((::pwm*) mImp, ENABLED) == EXIT_FAILURE) return false;

  return true;
}


bool
libSOC::pwm::disable()
{
  if (libsoc_pwm_set_enabled((::pwm*) mImp, DISABLED) == EXIT_FAILURE) return false;

  return true;
}


bool
libSOC::pwm::setPulse(unsigned int period, unsigned int duty)
{
  if (duty > period) duty = period;

  fprintf(stderr, "T=%d, DC=%d\n", period, duty);
  if (period > mPeriod) {
    // Slowing down... set period first as duty cycle is safe
    if (libsoc_pwm_set_period((::pwm*) mImp, period) == EXIT_FAILURE) return false;
    if (libsoc_pwm_set_duty_cycle((::pwm*) mImp, duty) == EXIT_FAILURE) return false;
  } else {
    // Speeding up... set duty cycle first to keep it safe
    if (libsoc_pwm_set_duty_cycle((::pwm*) mImp, duty) == EXIT_FAILURE) return false;
    if (libsoc_pwm_set_period((::pwm*) mImp, period) == EXIT_FAILURE) return false;
  }

  mPeriod = period;

  return true;
}
