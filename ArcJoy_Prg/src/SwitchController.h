#ifndef SWITCH_CONTROLLER_H
#define SWITCH_CONTROLLER_H

#include "IHal/IHalGpioInput.h"
#include "IHal/IHalDelay.h"

class SwitchController
{
  private:
    static const int32_t MAX_SWITCHES_COUNT = 10;
    static const int32_t DEBOUNCE_TIME_MS = 5;
    uint32_t m_switchesCount;
    IHalDelay *m_delay;
    IHalGpioInput *m_switches[MAX_SWITCHES_COUNT];
    bool m_switchStates[MAX_SWITCHES_COUNT];
  public:
    SwitchController(IHalGpioInput *switches[MAX_SWITCHES_COUNT], uint32_t switchesCount, IHalDelay *delay);
    void Tick();
    bool HasChanged();
    uint8_t GetStateAsByte(uint8_t firstSwitch, uint8_t lastSwitch);
};

#endif
