#ifndef I_HAL_DELAY
#define I_HAL_DELAY

#include "stdint.h"

class IHalDelay
{
  public:
    virtual void DelayMs(uint32_t ms) = 0;
};

#endif