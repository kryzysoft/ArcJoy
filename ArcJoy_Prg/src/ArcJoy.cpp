#include "ArcJoy.h"

#include "stdint.h"
#include "nrf.h"
#include "nrf_esb.h"
#include "nrf_delay.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "nrfx_gpiote.h"
#include "DefFix.h"

void JOYAPP_Run();
static void clockStart(void);
static void clockStop(void);

static void joyInit(void);
static void joyDisable(void);
static uint8_t joyReadState(void);
static void joyButtonsInit(void);
static void joyButtonsDisable(void);
static uint8_t joyButtonsReadState(void);

static void systemOff(void);
static void goToSleep(void);

#define NONE  0
#define LEFT  1
#define RIGHT 2
#define UP    4
#define DOWN  8

#define JOY_LEFT     3
#define JOY_RIGHT   22
#define JOY_UP       8
#define JOY_DOWN    26

#define JOY_BUTTON_1 9
#define JOY_BUTTON_2 6
#define JOY_BUTTON_3 7
#define JOY_BUTTON_4 5
#define JOY_BUTTON_5 2
#define JOY_BUTTON_6 4

#define FRAME_HEARTBEAT 0
#define FRAME_JOYSTATE  1

#define MAX_RADIO_FAILS 2

static const nrf_drv_timer_t heartbeatTimer = NRF_DRV_TIMER_INSTANCE(0);

const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */
#define COMPARE_COUNTERTIME (10UL)


static void joyEvent(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

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

  uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7};
  uint8_t base_addr_1[4] = {0xC2, 0xC2, 0xC2, 0xC2};
  uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 };

  joyInit();
  joyButtonsInit();
  
  m_pHwConfig->rtcClock->SetupAlarmHandler(this);
  m_pHwConfig->rtcClock->SetupAlarmInSeconds(COMPARE_COUNTERTIME);

  m_pHwConfig->esbPtx->On();
  m_pHwConfig->esbPtx->SetupAddress0(base_addr_0);
  m_pHwConfig->esbPtx->SetupAddress1(base_addr_1);
  m_pHwConfig->esbPtx->SetupAddressPrefixes(addr_prefix,8);

  bool led = false;

  while(true)
  {
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
        nrf_delay_ms(50);
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
      goToSleep();
    }
    else
    {
      joyDisable();
      joyButtonsDisable();
      nrf_gpio_cfg_sense_input(JOY_BUTTON_1, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
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

static void goToSleep(void)
{
  __SEV();
  __WFE();
  __WFE();
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

static void joyDisable(void)
{
    nrf_gpio_cfg_default(JOY_LEFT);
    nrf_gpio_cfg_default(JOY_RIGHT);
    nrf_gpio_cfg_default(JOY_UP);
    nrf_gpio_cfg_default(JOY_DOWN);
    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);
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

static void joyButtonsDisable(void)
{
    nrf_gpio_cfg_default(JOY_BUTTON_1);
    nrf_gpio_cfg_default(JOY_BUTTON_2);
    nrf_gpio_cfg_default(JOY_BUTTON_3);
    nrf_gpio_cfg_default(JOY_BUTTON_4);
    nrf_gpio_cfg_default(JOY_BUTTON_5);
    nrf_gpio_cfg_default(JOY_BUTTON_6);
    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);
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
  data[0] = FRAME_JOYSTATE;
  data[1] = joystick;
  data[2] = joyButtons;
  m_pHwConfig->esbPtx->SendFrame(0,data,dataLength);
}

void ArcJoy::RtcAlarmHandler()
{
  ArcJoy::sendHeartbeat = true;
  m_pHwConfig->rtcClock->SetupAlarmInSeconds(COMPARE_COUNTERTIME);
}