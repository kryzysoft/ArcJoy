#ifndef I_HAL_PERIODIC_EVENT_H
#define I_HAL_PERIODIC_EVENT_H

#include "stdint.h"

class IPeriodicEventHandler{
  public:
    virtual void PeriodicEventHandler() = 0;
};

class IHalPeriodicEvent
{
  public:
    virtual void SetPeriodMs(uint32_t ms) = 0;
    virtual void SetPeriodicEventHandler(IPeriodicEventHandler *periodicEventHandler) = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
};

#endif
