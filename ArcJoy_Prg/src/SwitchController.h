#ifndef SWITCH_CONTROLLER_H
#define SWITCH_CONTROLLER_H

#include "Debouncer.h"
#include "IHal/IHalGpioInput.h"

#define MAX_SWITCHES_COUNT 10

class SwitchController
{
  private:
    Debouncer m_debouncer[MAX_SWITCHES_COUNT];
    uint32_t m_switchesCount;
    IHalGpioInput *m_switches[MAX_SWITCHES_COUNT];
    bool m_hasChanged;    
  public:
    SwitchController(IHalGpioInput *switches[MAX_SWITCHES_COUNT], uint32_t switchesCount);
    void Tick(uint32_t timeMs);
    bool HasChanged();
    uint8_t GetStateAsByte(uint8_t firstSwitch, uint8_t lastSwitch);
    bool DebouncingInProgress();
};

#endif
