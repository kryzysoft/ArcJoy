#include "SwitchController.h"

SwitchController::SwitchController(IHalGpioInput *switches[MAX_SWITCHES_COUNT], uint32_t switchesCount, IHalDelay *delay):
  m_switchesCount(switchesCount),
  m_delay(delay)
{
  for(uint8_t i=0; i<switchesCount; i++)
  {
    m_switches[i] = switches[i];
    m_switchStates[i] = m_switches[i]->IsUp();
  }
}

bool SwitchController::HasChanged()
{
  bool hasChanged = false;
  m_delay->DelayMs(DEBOUNCE_TIME_MS);
  for(uint8_t i=0; i<m_switchesCount; i++)
  {
    bool switchState = m_switches[i]->IsUp();
    if(switchState != m_switchStates[i])
    {
      m_switchStates[i] = switchState;
      hasChanged = true;
    }
  }    
  return hasChanged;
}

uint8_t SwitchController::GetStateAsByte(uint8_t firstSwitch, uint8_t lastSwitch)
{
  uint8_t retVal = 0;
  for(int8_t i=lastSwitch; i>=firstSwitch; i--)
  {
    retVal<<=1;
    if(!m_switchStates[i])
    {
      retVal|=1;
    }
  }
  return retVal;
}
