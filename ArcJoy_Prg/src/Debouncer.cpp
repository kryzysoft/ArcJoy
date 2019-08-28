#include "Debouncer.h"

Debouncer::Debouncer(uint32_t debounceTimeout):m_debounceTimeout(debounceTimeout), m_debouncingStartTime(0), m_hasChanged(false), m_state(false), m_debouncingStarted(false)
{
}

void Debouncer::SetState(bool state, uint32_t time)
{
  if(m_debouncingStarted)
  {
    if(time - m_debouncingStartTime >= m_debounceTimeout)
    {
      m_debouncingStarted = false;
      if(state != m_state)
      {
        m_hasChanged = true;
        m_state = state;
      }
    }
  }
  else
  {
    if(m_state != state)
    {
      m_debouncingStarted = true;
      m_debouncingStartTime = time;
    }
  }
}

bool Debouncer::IsUp()
{
  return m_state;
}

bool Debouncer::IsDown()
{
  return !m_state;
}

bool Debouncer::HasChanged()
{
  bool retVal = m_hasChanged;
  m_hasChanged = false;
  return retVal;
}

bool Debouncer::DebouncingOngoing()
{
  return m_debouncingStarted;
}
