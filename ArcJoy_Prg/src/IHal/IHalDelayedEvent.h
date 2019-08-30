#ifndef I_HAL_DELAYED_EVENT_H
#define I_HAL_DELAYED_EVENT_H

#include "stdint.h"

class IDelayedEventHandler{
  public:
    virtual void DelayedEventHandler() = 0;
};

class IHalDelayedEvent
{
  public:
    virtual void ScheduleEvent(IDelayedEventHandler *delayedEventHandler, uint32_t delayMs) = 0;
    virtual ~IHalDelayedEvent(){};
};

#endif
