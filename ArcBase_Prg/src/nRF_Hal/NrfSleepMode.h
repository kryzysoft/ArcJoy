#ifndef NRF_SLEEP_MODE_H
#define NRF_SLEEP_MODE_H

#include "IHal/IHalLowPowerMode.h"

class NrfSleepMode: public IHalLowPowerMode
{
  public:
    void Enter();
};

#endif
