#include "ArcJoy.h"
#include "stdint.h"


void JOYAPP_Run();

static void joyInit(void);
static uint8_t joyReadState(void);
static void joyButtonsInit(void);
static uint8_t joyButtonsReadState(void);

#define NONE  0
#define LEFT  1
#define RIGHT 2
#define UP    4
#define DOWN  8

#define MAX_RADIO_FAILS 5

#define HEARTBEAT_PERIOD 1

bool ArcJoy::sendHeartbeat = false;
bool ArcJoy::sendJoyState = false;

ArcJoy::ArcJoy(ArcJoyHardwareConfig *hwConfig)
{
  m_pHwConfig = hwConfig;
}

void ArcJoy::Run()
{
  uint8_t joyState = NONE;
  uint8_t dipSwitchState = 0;
  uint8_t joyButtonsState = 0;
  uint32_t fails = 0;

  uint8_t base_addr_0[4] = {'A', 'r', 'c', '1'};
  uint8_t base_addr_1[4] = {'J', 'o', 'y', '2'};
  uint8_t addr_prefix[8] = {'O', 'T', 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };

  joyInit();
  joyButtonsInit();

  
  m_pHwConfig->rtcClock->SetupAlarmHandler(this);
  m_pHwConfig->rtcClock->SetupAlarmInSeconds(HEARTBEAT_PERIOD);

  m_pHwConfig->esbPtx->SetupAddress0(base_addr_0);
  m_pHwConfig->esbPtx->SetupAddress1(base_addr_1);
  m_pHwConfig->esbPtx->SetupAddressPrefixes(addr_prefix,8);

  bool led = false;

  while(true)
  {
    m_pHwConfig->dipSwitch1->Enable();
    if(m_pHwConfig->dipSwitch1->IsUp())
    {
      m_joyNumber = 0;
    }
    else
    {
      m_joyNumber = 1;
    }
    m_pHwConfig->dipSwitch1->Disable();

    while(sendHeartbeat || sendJoyState)
    {
      if(sendHeartbeat)
      {
        sendHeartbeat = false;
        m_pHwConfig->esbPtx->On();
        joyState = joyReadState();
        joyButtonsState = joyButtonsReadState();
        radioSendState(joyButtonsState,joyState);
        while(m_pHwConfig->esbPtx->IsRadioBusy())
        {
        }
        m_pHwConfig->esbPtx->Off();
        if(m_pHwConfig->esbPtx->SendSucceeded())
        {
          fails = 0;
          m_pHwConfig->blueLed->Up();
        }
        else
        {
          fails++;
          m_pHwConfig->redLed->Up();
        }
        m_pHwConfig->delay->DelayMs(50);
        m_pHwConfig->redLed->Down();
        m_pHwConfig->blueLed->Down();
      }
      if(sendJoyState)
      {
        sendJoyState = false;
        m_pHwConfig->esbPtx->On();
        joyState = joyReadState();
        joyButtonsState = joyButtonsReadState();
        radioSendState(joyButtonsState,joyState);
        while(m_pHwConfig->esbPtx->IsRadioBusy())
        {

        }
        m_pHwConfig->esbPtx->Off();
      }
    }
    if(fails < MAX_RADIO_FAILS)
    {
      m_pHwConfig->sleepMode->Enter();
    }
    else
    {
      joyGpioDisable();
      m_pHwConfig->offMode->Enter();
    }
  }
}


void ArcJoy::joyInit(void)
{
  m_pHwConfig->joyLeft->SetupHandler(this);
  m_pHwConfig->joyRight->SetupHandler(this);
  m_pHwConfig->joyUp->SetupHandler(this);
  m_pHwConfig->joyDown->SetupHandler(this);
}

uint8_t ArcJoy::dipSwitchReadState()
{
  uint8_t retVal = 0;

  if(m_pHwConfig->dipSwitch6->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  if(m_pHwConfig->dipSwitch5->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  if(m_pHwConfig->dipSwitch4->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  if(m_pHwConfig->dipSwitch3->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  if(m_pHwConfig->dipSwitch2->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  if(m_pHwConfig->dipSwitch1->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  
  return retVal;
}

uint8_t ArcJoy::joyReadState(void)
{
  uint8_t joyState = 0;
  
  if(m_pHwConfig->joyLeft->IsDown()) joyState |= LEFT;
  if(m_pHwConfig->joyRight->IsDown()) joyState |= RIGHT;
  if(m_pHwConfig->joyUp->IsDown()) joyState |= UP;
  if(m_pHwConfig->joyDown->IsDown()) joyState |= DOWN;

  return joyState;
} 

void ArcJoy::GpioHandler()
{
  ArcJoy::sendJoyState = true;
}

void ArcJoy::joyButtonsInit(void)
{
  m_pHwConfig->joyButton1->SetupHandler(this);
  m_pHwConfig->joyButton2->SetupHandler(this);
  m_pHwConfig->joyButton3->SetupHandler(this);
  m_pHwConfig->joyButton4->SetupHandler(this);
  m_pHwConfig->joyButton5->SetupHandler(this);
  m_pHwConfig->joyButton6->SetupHandler(this);
}

void ArcJoy::joyGpioDisable(void)
{
  m_pHwConfig->joyButton2->Disable();
  m_pHwConfig->joyButton3->Disable();
  m_pHwConfig->joyButton4->Disable();
  m_pHwConfig->joyButton5->Disable();
  m_pHwConfig->joyButton6->Disable();

  m_pHwConfig->joyLeft->Disable();
  m_pHwConfig->joyRight->Disable();
  m_pHwConfig->joyUp->Disable();
  m_pHwConfig->joyDown->Disable();
}

uint8_t ArcJoy::joyButtonsReadState(void)
{
  uint8_t retVal = 0;

  if(m_pHwConfig->joyButton1->IsDown()) retVal |= 1;
  retVal = retVal << 1;
  if(m_pHwConfig->joyButton2->IsDown()) retVal |= 1;
  retVal = retVal << 1;
  if(m_pHwConfig->joyButton3->IsDown()) retVal |= 1;
  retVal = retVal << 1;
  if(m_pHwConfig->joyButton4->IsDown()) retVal |= 1;
  retVal = retVal << 1;
  if(m_pHwConfig->joyButton5->IsDown()) retVal |= 1;
  retVal = retVal << 1;
  if(m_pHwConfig->joyButton6->IsDown()) retVal |= 1;
  return retVal;
} 

void ArcJoy::radioSendState(uint8_t joyButtons, uint8_t joystick)
{
  const uint8_t dataLength = 3;
  uint8_t data[dataLength];
  data[0] = m_frameCounter;
  if(m_frameCounter<255)
  {
    m_frameCounter++;
  }
  else
  {
    m_frameCounter = 0;
  }
  data[1] = joystick;
  data[2] = joyButtons;
  m_pHwConfig->esbPtx->SendFrame(m_joyNumber,data,dataLength);
}

void ArcJoy::RtcAlarmHandler()
{
  ArcJoy::sendHeartbeat = true;
  m_pHwConfig->rtcClock->SetupAlarmInSeconds(HEARTBEAT_PERIOD);
}