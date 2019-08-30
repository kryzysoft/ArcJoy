#include "ArcBase.h"
#include "RttDebugLog.h"

#define NONE  0
#define LEFT  1
#define RIGHT 2
#define UP    4
#define DOWN  8

#define HEARTBEAT_PERIOD 1

ArcBase::ArcBase(ArcBaseHardwareConfig *hwConfig)
{
  m_pHwConfig = hwConfig;
}

void ArcBase::Run()
{
  DebugInfo("Application init");
  uint8_t base_addr_0[4] = {'A', 'r', 'c', '1'};
  uint8_t base_addr_1[4] = {'J', 'o', 'y', '2'};
  uint8_t addr_prefix[8] = {'O', 'T', 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };

  DebugInfo("RTC setup");
  m_pHwConfig->rtcClock->SetupAlarmHandler(this);
  m_pHwConfig->rtcClock->SetupAlarmInSeconds(HEARTBEAT_PERIOD);

  DebugInfo("ESB setup");
  m_pHwConfig->esbPrx->SetupAddress0(base_addr_0);
  m_pHwConfig->esbPrx->SetupAddress1(base_addr_1);
  m_pHwConfig->esbPrx->SetupAddressPrefixes(addr_prefix,8);
  m_pHwConfig->esbPrx->SetRfChannel(27);
  m_pHwConfig->esbPrx->On();

  DebugInfo("LED Init");
  m_pHwConfig->ledJoy1->Up();
  m_pHwConfig->ledJoy2->Up();
  m_pHwConfig->ledNoJoys->Down();

  m_joy1TimeOut = 0;
  m_joy2TimeOut = 0;

  DebugInfo("Application started");
  while(true)
  {
    uint8_t data[256];
    uint8_t dataLength;
    uint8_t pipe;

    while(m_pHwConfig->esbPrx->GetNextFrame(&pipe,data,&dataLength))
    {
      uint8_t joyIndex = pipe;
      uint8_t x, y, buttons;
      
      if(joyIndex>1) joyIndex = 1;

      if(joyIndex == 0)
      {
        DebugInfo("Joy 1 moved");
        if(m_joy1TimeOut == 0)
        {
          DebugInfo("Joy 1 connected");
        }
        m_joy1TimeOut = 5;
        m_pHwConfig->ledJoy1->Down();
        m_pHwConfig->ledNoJoys->Up();
      }
      if(joyIndex == 1)
      {
        DebugInfo("Joy 2 moved");
        if(m_joy2TimeOut == 0)
        {
          DebugInfo("Joy 1 connected");
        }
        m_joy2TimeOut = 5;
        m_pHwConfig->ledJoy2->Down();
        m_pHwConfig->ledNoJoys->Up();
      }

      if((data[1] & LEFT) != 0) 
      {
        x = -1;
        DebugInfo("Joy 1 x = -1");
      }
      else if((data[1] & RIGHT) != 0)
      {
        x = 1;
        DebugInfo("Joy 1 x = 1");
      }
      else
      {
        x = 0;
        DebugInfo("Joy 1 x = 0");
      }
      if((data[1] & UP) != 0) y = -1;
      else if((data[1] & DOWN) != 0) y = 1;
      else y = 0;
      buttons = data[2];

      m_pHwConfig->joysticks->SetJoyPosition(joyIndex,x,y);
      m_pHwConfig->joysticks->SetJoyButtons(joyIndex,buttons);
    }
    m_pHwConfig->joysticks->ProcessJoyEvents();
  }
}

void ArcBase::RtcAlarmHandler()
{
  if(m_joy1TimeOut>0) m_joy1TimeOut--;
  if(m_joy2TimeOut>0) m_joy2TimeOut--;

  if(m_joy1TimeOut == 0)
  {
    DebugWarn("Lost connection with joy 1");
    m_pHwConfig->ledJoy1->Up();
  }
  if(m_joy2TimeOut == 0)
  {
    DebugWarn("Lost connection with joy 2");
    m_pHwConfig->ledJoy2->Up();
  }
  if((m_joy1TimeOut == 0)&&(m_joy2TimeOut == 0))
  {
    m_pHwConfig->ledNoJoys->Down();
  }
  m_pHwConfig->rtcClock->SetupAlarmInSeconds(HEARTBEAT_PERIOD);
}
