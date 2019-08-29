#include "SwitchController.h"

#define DEBOUNCE_TIME_MS 5

SwitchController::SwitchController(IHalGpioInput *switches[MAX_SWITCHES_COUNT], uint32_t switchesCount, IHalDelay *delay):
  m_switchesCount(switchesCount),
  m_hasChanged(false),
  m_delay(delay)
{
  for(uint8_t i=0; i<switchesCount; i++)
  {
    m_switches[i] = switches[i];
    m_switchStates[i] = m_switches[i]->IsUp();
  }
}

void SwitchController::Tick()
{
  bool change = false;
//  for(uint8_t i=0; i<m_switchesCount; i++)
//  {
//    if(m_switchStates[i] != m_switches[i]->IsUp())
//    {
//      change = true;
//      break;
//    }
//  }
//  if(change)
//  {
    m_delay->DelayMs(DEBOUNCE_TIME_MS);
    for(uint8_t i=0; i<m_switchesCount; i++)
    {
      if(m_switches[i]->IsUp() != m_switchStates[i])
      {
        m_switchStates[i] = m_switches[i]->IsUp();
        m_hasChanged = true;
      }
    }    
//  }
}

bool SwitchController::HasChanged()
{
  bool retVal = m_hasChanged;
  m_hasChanged = false;
  return retVal;
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
