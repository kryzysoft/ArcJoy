#ifndef IHAL_NRF_RTC_H
#define IHAL_NRF_RTC_H

#include "IHal/IHalRtc.h"
#include "nrf_drv_rtc.h"

class NrfRtc: public IHalRtc
{
  private:
    static const nrf_drv_rtc_t rtc;
    static void rtcAlarm(nrf_drv_rtc_int_type_t int_type);
    static IRtcAlarmHandler *rtcAlarmHandler;
  public:
    NrfRtc();
    void SetupAlarmInSeconds(uint32_t seconds);
    void SetAlarmHandler(IRtcAlarmHandler *rtcAlarmHandler);
};

#endif
