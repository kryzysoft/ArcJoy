#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include "stdint.h"


class Debouncer
{
  private:
    const uint32_t m_debounceTimeout;
    uint32_t m_debouncingStartTime;
    bool m_hasChanged;
    bool m_state;
    bool m_debouncingStarted;
  public:
    Debouncer(uint32_t debounceTimeout);
    void SetState(bool state, uint32_t time);
    bool IsUp();
    bool IsDown();
    bool HasChanged();
    bool DebouncingOngoing();
};


#endif
