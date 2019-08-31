#include "nRF_Hal/NrfTimerPeriodicEvent.h"

const nrfx_timer_t TIMER_INSTANCE_0 = NRFX_TIMER_INSTANCE(0);
IPeriodicEventHandler *NrfTimerPeriodicEvent::m_pPeriodicEventHandler = NULL;

NrfTimerPeriodicEvent::NrfTimerPeriodicEvent()
{
  nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
  nrfx_timer_init(&TIMER_INSTANCE_0, &timer_cfg, &TimerPeriodicEvent);
}

void NrfTimerPeriodicEvent::SetPeriodMs(uint32_t ms)
{
  uint32_t ticks = nrfx_timer_ms_to_ticks(&TIMER_INSTANCE_0, ms);
  nrfx_timer_compare(&TIMER_INSTANCE_0, NRF_TIMER_CC_CHANNEL0, ticks, true);
}

void NrfTimerPeriodicEvent::SetPeriodicEventHandler(IPeriodicEventHandler *periodicEventHandler)
{
  NrfTimerPeriodicEvent::m_pPeriodicEventHandler = periodicEventHandler;
}

void NrfTimerPeriodicEvent::Start()
{
  nrfx_timer_enable(&TIMER_INSTANCE_0);
}

void NrfTimerPeriodicEvent::Stop()
{
  nrfx_timer_disable(&TIMER_INSTANCE_0);
}

void NrfTimerPeriodicEvent::TimerPeriodicEvent(nrf_timer_event_t event_type, void *p_context)
{
  if(event_type == NRF_TIMER_EVENT_COMPARE0)
  {
    nrfx_timer_clear(&TIMER_INSTANCE_0);
    NrfTimerPeriodicEvent::m_pPeriodicEventHandler->PeriodicEventHandler();
  }
}
