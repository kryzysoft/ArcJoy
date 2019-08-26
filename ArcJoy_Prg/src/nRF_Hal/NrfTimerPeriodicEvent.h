#ifndef NRF_TIMER_PERIODIC_EVENT_H
#define NRF_TIMER_PERIODIC_EVENT_H

#include "IHal/IHalPeriodicEvent.h"
#include "nrfx_timer.h"

class NrfTimerPeriodicEvent: public IHalPeriodicEvent
{
  private:
    static void TimerPeriodicEvent(nrf_timer_event_t event_type, void *p_context);
    static IPeriodicEventHandler *m_pPeriodicEventHandler;
  public:
    NrfTimerPeriodicEvent();
    virtual void SetPeriodMs(uint32_t ms);
    virtual void SetPeriodicEventHandler(IPeriodicEventHandler *periodicEventHandler);
    virtual void Start();
    virtual void Stop();
};

#endif
