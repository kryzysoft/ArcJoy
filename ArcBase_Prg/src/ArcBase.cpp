#include "ArcBase.h"
#include "ArcBase_c.h"

ArcBase::ArcBase(ArcBaseHardwareConfig *hwConfig)
{
  m_pHwConfig = hwConfig;
}

//static JoyState joyState[2] = {{false,false,false,false,false},{false,false,false,false,false}};
//
#define NONE  0
#define LEFT  1
#define RIGHT 2
#define UP    4
#define DOWN  8


void ArcBase::Run()
{
  arcBase_init();
  uint8_t base_addr_0[4] = {'A', 'r', 'c', '1'};
  uint8_t base_addr_1[4] = {'J', 'o', 'y', '2'};
  uint8_t addr_prefix[8] = {'O', 'T', 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };

  m_pHwConfig->esbPrx->SetupAddress0(base_addr_0);
  m_pHwConfig->esbPrx->SetupAddress1(base_addr_1);
  m_pHwConfig->esbPrx->SetupAddressPrefixes(addr_prefix,8);

  m_pHwConfig->esbPrx->On();

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
      if((data[1] & LEFT) != 0) x = -1;
      else if((data[1] & RIGHT) != 0) x = 1;
      else x = 0;
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


