#ifndef I_HAL_RTC_H
#define I_HAL_RTC_H

#include "stdint.h"

class IHalRtcAlarmHandler
{
  public:
    virtual void RtcAlarmHandler() = 0;
};

class IHalRtc
{
  public:
    virtual void SetupAlarmInSeconds(uint32_t seconds) = 0;
    virtual void SetupAlarmHandler(IHalRtcAlarmHandler *rtcAlarmHandler) = 0;
};

#endif