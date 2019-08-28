#include "SwitchController.h"

#define DEBOUNCE_TIME_MS 5

SwitchController::SwitchController(IHalGpioInput *switches[], uint32_t switchesCount):
  m_switchesCount(switchesCount),
  m_hasChanged(false),
  m_debouncer(
    {DEBOUNCE_TIME_MS,DEBOUNCE_TIME_MS,DEBOUNCE_TIME_MS,DEBOUNCE_TIME_MS,DEBOUNCE_TIME_MS
    ,DEBOUNCE_TIME_MS,DEBOUNCE_TIME_MS,DEBOUNCE_TIME_MS,DEBOUNCE_TIME_MS,DEBOUNCE_TIME_MS})
{
  for(uint8_t i=0; i<switchesCount; i++)
  {
    m_switches[i] = switches[i];
  }
}

void SwitchController::Tick(uint32_t timeMs)
{
  for(uint8_t i=0; i<m_switchesCount; i++)
  {
    m_debouncer[i].SetState(m_switches[i]->IsUp(), timeMs);
    if(m_debouncer[i].HasChanged())
    {
      m_hasChanged = true;
    }
  }
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
    if(m_debouncer[i].IsDown())
    {
      retVal|=1;
    }
  }
  return retVal;
}
