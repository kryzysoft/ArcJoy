#include "nRF_Hal/NrfTimerDelayedEvent.h"

const nrfx_timer_t TIMER_INSTANCE = NRFX_TIMER_INSTANCE(1);
IDelayedEventHandler *NrfTimerDelayedEvent::m_pDelayedEventHandler = NULL;

NrfTimerDelayedEvent::NrfTimerDelayedEvent()
{
  nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
  nrfx_timer_init(&TIMER_INSTANCE, &timer_cfg, &TimerDelayedEvent);
}

void NrfTimerDelayedEvent::ScheduleEvent(IDelayedEventHandler *delayedEventHandler, uint32_t delayMs)
{
  NrfTimerDelayedEvent::m_pDelayedEventHandler = delayedEventHandler;
  nrfx_timer_clear(&TIMER_INSTANCE);
  uint32_t ticks = nrfx_timer_ms_to_ticks(&TIMER_INSTANCE, delayMs);
  nrfx_timer_compare(&TIMER_INSTANCE, NRF_TIMER_CC_CHANNEL0, ticks, true);

  nrfx_timer_enable(&TIMER_INSTANCE);
}

void NrfTimerDelayedEvent::TimerDelayedEvent(nrf_timer_event_t event_type, void *p_context)
{
  if(event_type == NRF_TIMER_EVENT_COMPARE0)
  {
    nrfx_timer_disable(&TIMER_INSTANCE);
    NrfTimerDelayedEvent::m_pDelayedEventHandler->DelayedEventHandler();
  }
}
