#ifndef NRF_GPIO_WAKE_UP_H
#define NRF_GPIO_WAKE_UP_H

#include "IHal/IHalGpioWakeUp.h"
#include "stdint.h"

class NrfGpioWakeUp: public IHalGpioWakeUp
{
  private:
    uint32_t m_pinNumber;
  public:
    NrfGpioWakeUp(uint32_t pinNumber);
    void EnableWakeUp();
};

#endif