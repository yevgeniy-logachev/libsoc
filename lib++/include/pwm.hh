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


#ifndef _LIBSOC_PWM_HH_
#define _LIBSOC_PWM_HH_


namespace libSOC {


  class pwm
  {
  public:
    /**
     * Get the PWM object singleton for the specified controller
     * \param chip number
     * \param num PWM number on that chip
     * \param  invert Invert the polarity if true
     */
    static pwm* get(unsigned int chip, unsigned int num, bool invert = false);


    /**
     * \brief Enable the PWM
     * \return TRUE on success
     */
    bool enable();


    /**
     * \brief Disable the PWM
     * \return TRUE on success
     */
    bool disable();


    /**
     * \brief Set the period
     * \param period in ns
     * \param dutyCycle in ns
     * \return true if is input
     */
    bool setPulse(unsigned int period, unsigned int duty);


  private:
    void          *mImp;
    bool           mInvert;
    unsigned int   mPeriod;
  };

};
#endif
