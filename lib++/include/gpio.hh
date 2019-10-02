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


#ifndef _LIBSOC_GPIO_HH_
#define _LIBSOC_GPIO_HH_


namespace libSOC {


  class gpio
  {
  public:
    typedef enum {NO_PULL, PULL_UP, PULL_DN} inputMode_t;

    /**
     * Get the GPIO object singleton for the specified pin
     */
    static gpio* get(unsigned int pin);


    /**
     * Get the GPIO object singleton for the named pin
     */
    static gpio* get(const char* name);

        
    /**
     * \brief set GPIO to output
     * \return TRUE on success
     */
    bool makeOutput();


    /**
     * \brief set GPIO to input, with no pull, pull-up, or pull-down
     * \return TRUE on success
     */
    bool makeInput(inputMode_t mode = NO_PULL);


    /**
     * \brief set GPIO to interrupt with specified callback function
     * \return TRUE on success
     */
      typedef enum {RISING = 1, FALLING = 2, BOTH = 3} edge_t;
      bool makeInterrupt(void (*fct)(void*), void* arg, edge_t edge, inputMode_t mode = NO_PULL);


    /**
     * \brief Check the direction of the GPIO
     * \return true if is input
     */
    bool isInput();


    /**
     * \brief set the GPIO output
     * \param val Set to HIGH if true
     * \return true is succesful
     */
    bool setValue(bool val);


    /**
     * Check the value of the GPIO input
     * \return 1 if HIGH, 0 otherwiseb

     */
    bool getValue();
    
    
  private:
    void *m_imp;
  };

};
#endif
