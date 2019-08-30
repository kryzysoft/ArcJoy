#ifndef NRF_TIMER_DELAYED_EVENT_H
#define NRF_TIMER_DELAYED_EVENT_H

#include "IHal/IHalDelayedEvent.h"
#include "nrfx_timer.h"

class NrfTimerDelayedEvent: public IHalDelayedEvent
{
  private:
    static void TimerDelayedEvent(nrf_timer_event_t event_type, void *p_context);
    static IDelayedEventHandler *m_pDelayedEventHandler;
  public:
    NrfTimerDelayedEvent();
    virtual void ScheduleEvent(IDelayedEventHandler *delayedEventHandler, uint32_t delayMs);
};

#endif
