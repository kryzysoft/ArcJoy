#include "NrfRtc.h"
#include "nrf.h"
#include "nrf_esb.h"
#include "nrf_delay.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "DefFix.h"


const nrf_drv_rtc_t NrfRtc::rtc = NRF_DRV_RTC_INSTANCE(0);
IRtcAlarmHandler *NrfRtc::rtcAlarmHandler = NULL;

NrfRtc::NrfRtc()
{
  nrf_drv_clock_init();
  nrf_drv_clock_lfclk_request(NULL);
  
  nrfx_rtc_config_t rtcConfig = NRFX_RTC_DEFAULT_CONFIG;
  rtcConfig.prescaler = 4095;
    
  nrfx_rtc_init(&rtc, &rtcConfig, rtcAlarm);
  nrfx_rtc_enable(&rtc);
}

void NrfRtc::SetupAlarmInSeconds(uint32_t seconds)
{
  nrfx_rtc_counter_clear(&rtc);
  nrfx_rtc_cc_set(&rtc,0,seconds*8, true);
}

void NrfRtc::SetAlarmHandler(IRtcAlarmHandler *rtcAlarmHandler)
{
  NrfRtc::rtcAlarmHandler = rtcAlarmHandler;
}

void NrfRtc::rtcAlarm(nrf_drv_rtc_int_type_t int_type)
{
  NrfRtc::rtcAlarmHandler->RtcAlarmHandler();
}
