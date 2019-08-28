#ifndef SWITCH_CONTROLLER_H
#define SWITCH_CONTROLLER_H

#include "Debouncer.h"

#define MAX_SWITCHES_COUNT 10

class SwitchController
{
  private:
    Debouncer m_debouncer[MAX_SWITCHES_COUNT];
    uint32_t m_switchesCount;
    IHalGPioInput *m_switches;
    bool m_hasChanged;    
  public:
    SwitchController(IHalGpioInput *switches, uint32_t switchesCount);
    void Tick(uint8_t timeMs);
    bool HasChanged();
    uint8_t GetStateAsByte(uint8_t firstSwitch, uint8_t lastSwitch);
}

#endif