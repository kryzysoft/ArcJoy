#ifndef NRF_DELAY_H
#define NRF_DELAY_H

#include "IHal/IHalDelay.h"

class NrfDelay: public IHalDelay
{
  public:
    void DelayMs(uint32_t ms);
};

#endif