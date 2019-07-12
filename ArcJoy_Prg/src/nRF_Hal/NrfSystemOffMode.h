#ifndef NRF_SYSTEM_OFF_MODE_H
#define NRF_SYSTEM_OFF_MODE_H

#include "IHal/IHalLowPowerMode.h"

class NrfSystemOffMode: public IHalLowPowerMode
{
  public:
    void Enter();
};

#endif
