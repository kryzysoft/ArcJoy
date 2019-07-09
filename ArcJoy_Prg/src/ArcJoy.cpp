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
static void ledInit(void);
static void dipSwitchInit(void);
static void dipSwitchDisable(void);
static uint8_t dipSwitchReadState(void);
static void joyInit(void);
static void joyDisable(void);
static uint8_t joyReadState(void);
static void joyButtonsInit(void);
static void joyButtonsDisable(void);
static uint8_t joyButtonsReadState(void);
static void radioInit(void);
static void radioDisable(void);
static void radioSendState(uint8_t joyButtons, uint8_t joystick);
static void nrfEsbEventHandler(nrf_esb_evt_t const * p_event);
static void systemOff(void);
static void timerHeartbeatEventHandler(nrf_timer_event_t event_type, void* p_context);
static void timerInit(void);
static void lfclkConfig(void);
static void rtc_config(void);
static void rtc_handler(nrf_drv_rtc_int_type_t int_type);
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

#define MAX_RADIO_FAILS 5

static nrf_esb_payload_t tx_payload = NRF_ESB_CREATE_PAYLOAD(0, 0x01, 0x00);
static nrf_esb_payload_t rx_payload;
static const nrf_drv_timer_t heartbeatTimer = NRF_DRV_TIMER_INSTANCE(0);

const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */
#define COMPARE_COUNTERTIME  (4UL)  

static volatile bool radioSuccess = false;
static volatile bool radioDone = false;

static volatile bool sendHeartbeat = false;
static volatile bool sendJoyState = false;

static void joyEvent(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);



ArcJoy::ArcJoy(IHalGpio *redLed, IHalGpio *blueLed, IHalGpio *dipSwitch1, IHalGpio *dipSwitch2, IHalGpio *dipSwitch3, IHalGpio *dipSwitch4, IHalGpio *dipSwitch5, IHalGpio *dipSwitch6)
{
  m_pRedLed = redLed;
  m_pBlueLed = blueLed;
  m_pDipSwitch1 = dipSwitch1;
  m_pDipSwitch2 = dipSwitch2;
  m_pDipSwitch3 = dipSwitch3;
  m_pDipSwitch4 = dipSwitch4;
  m_pDipSwitch5 = dipSwitch5;
  m_pDipSwitch5 = dipSwitch6;
}

void ArcJoy::Run()
{
  uint8_t joyState = NONE;
  uint8_t dipSwitchState = 0;
  uint8_t joyButtonsState = 0;
  uint32_t fails = 0;

//  ledInit();
//  dipSwitchInit();
  joyInit();
  joyButtonsInit();
  lfclkConfig();
  rtc_config();
  radioInit();

  bool led = false;

  while(true)
  {
    while(sendHeartbeat || sendJoyState)
    {
      if(sendHeartbeat)
      {
        sendHeartbeat = false;
        clockStart();
        radioSuccess = false;
        radioInit();
        joyState = joyReadState();
        joyButtonsState = joyButtonsReadState();
        radioDone = false;
        radioSendState(joyButtonsState,joyState);;
        while(!radioDone)
        {

        }
        radioDisable();
        clockStop();
        if(radioSuccess)
        {
          fails = 0;
          m_pBlueLed->Up();
        }
        else
        {
          fails++;
          m_pRedLed->Up();
        }
        nrf_delay_ms(20);
        m_pBlueLed->Down();
        m_pRedLed->Down();
      }
      if(sendJoyState)
      {
        sendJoyState = false;
        clockStart();
        radioSuccess = false;
        radioInit();
        joyState = joyReadState();
        joyButtonsState = joyButtonsReadState();
        radioDone = false;
        radioSendState(joyButtonsState,joyState);;
        while(!radioDone)
        {

        }
        radioDisable();
        clockStop();
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
      systemOff();
    }
  }
}




static void goToSleep(void)
{
  __SEV();
  __WFE();
  __WFE();
}

static void clockStart(void)
{
    // Start HFCLK and wait for it to start.
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

static void clockStop(void)
{
    // Start HFCLK and wait for it to start.
    NRF_CLOCK->TASKS_HFCLKSTOP = 1;
}


uint8_t ArcJoy::dipSwitchReadState()
{
  uint8_t retVal = 0;

  if(m_pDipSwitch1->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  if(m_pDipSwitch2->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  if(m_pDipSwitch3->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  if(m_pDipSwitch4->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  if(m_pDipSwitch5->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  if(m_pDipSwitch6->IsDown())
  {
    retVal |= 1;
    retVal = retVal << 1;
  }
  
  return retVal;
}

static void joyInit(void)
{
    nrf_gpio_cfg_input(JOY_LEFT, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_RIGHT, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_UP, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_DOWN, NRF_GPIO_PIN_NOPULL);
    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrfx_gpiote_init();
    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    nrfx_gpiote_in_init(JOY_LEFT,&config,joyEvent);
    nrfx_gpiote_in_init(JOY_RIGHT,&config,joyEvent);
    nrfx_gpiote_in_init(JOY_UP,&config,joyEvent);
    nrfx_gpiote_in_init(JOY_DOWN,&config,joyEvent);

   nrfx_gpiote_in_event_enable(JOY_LEFT,true);
   nrfx_gpiote_in_event_enable(JOY_RIGHT,true);
   nrfx_gpiote_in_event_enable(JOY_UP,true);
   nrfx_gpiote_in_event_enable(JOY_DOWN,true);


    nrf_delay_ms(1);
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

static uint8_t joyReadState(void)
{
  uint8_t joyState = 0x0F;

  if(nrf_gpio_pin_read(JOY_LEFT)) joyState &= (~LEFT);
  if(nrf_gpio_pin_read(JOY_RIGHT)) joyState &= (~RIGHT);
  if(nrf_gpio_pin_read(JOY_UP)) joyState &= (~UP);
  if(nrf_gpio_pin_read(JOY_DOWN)) joyState &= (~DOWN);

  return joyState;
} 

static void joyEvent(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  sendJoyState = true;
}

static void joyButtonsInit(void)
{
    nrf_gpio_cfg_input(JOY_BUTTON_1, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_BUTTON_2, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_BUTTON_3, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_BUTTON_4, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_BUTTON_5, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_BUTTON_6, NRF_GPIO_PIN_NOPULL);

    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    nrfx_gpiote_in_init(JOY_BUTTON_1,&config,joyEvent);
    nrfx_gpiote_in_init(JOY_BUTTON_2,&config,joyEvent);
    nrfx_gpiote_in_init(JOY_BUTTON_3,&config,joyEvent);
    nrfx_gpiote_in_init(JOY_BUTTON_4,&config,joyEvent);
    nrfx_gpiote_in_init(JOY_BUTTON_5,&config,joyEvent);
    nrfx_gpiote_in_init(JOY_BUTTON_6,&config,joyEvent);

   nrfx_gpiote_in_event_enable(JOY_BUTTON_1,true);
   nrfx_gpiote_in_event_enable(JOY_BUTTON_2,true);
   nrfx_gpiote_in_event_enable(JOY_BUTTON_3,true);
   nrfx_gpiote_in_event_enable(JOY_BUTTON_4,true);
   nrfx_gpiote_in_event_enable(JOY_BUTTON_5,true);
   nrfx_gpiote_in_event_enable(JOY_BUTTON_6,true);

    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);
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

static uint8_t joyButtonsReadState(void)
{
  uint8_t retVal = 0;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_1);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_2);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_3);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_4);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_5);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_6);

  retVal ^= 0x3F;
  
  return retVal;
} 

static void radioInit(void)
{
    uint32_t err_code;
    uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t base_addr_1[4] = {0xC2, 0xC2, 0xC2, 0xC2};
    uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 };

    nrf_esb_config_t nrf_esb_config         = NRF_ESB_DEFAULT_CONFIG;
    nrf_esb_config.retransmit_count         = 6;
    nrf_esb_config.selective_auto_ack       = false;
    nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
    nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_2MBPS;
    nrf_esb_config.event_handler            = nrfEsbEventHandler;
    nrf_esb_config.mode                     = NRF_ESB_MODE_PTX;

    nrf_esb_init(&nrf_esb_config);
    nrf_esb_set_base_address_0(base_addr_0);
    nrf_esb_set_base_address_1(base_addr_1);
    nrf_esb_set_prefixes(addr_prefix, 8);

    tx_payload.length  = 2;
    tx_payload.pipe    = 0;
    tx_payload.data[0] = 0x00;
}

static void radioDisable(void)
{
  nrf_esb_disable();
}

static void radioSendState(uint8_t joyButtons, uint8_t joystick)
{
    tx_payload.length  = 3;
    tx_payload.pipe    = 0;
    tx_payload.data[0] = FRAME_JOYSTATE;
    tx_payload.data[1] = joystick;
    tx_payload.data[2] = joyButtons;
    tx_payload.noack = false;

    nrf_esb_write_payload(&tx_payload);
}

static void nrfEsbEventHandler(nrf_esb_evt_t const * p_event)
{
    radioDone = true;
    switch (p_event->evt_id)
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
            radioSuccess = true;
            break;
        case NRF_ESB_EVENT_TX_FAILED:
            nrf_esb_flush_tx();
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            while (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS);
            break;
    }
}


static void systemOff( void )
{
    NRF_POWER->SYSTEMOFF = 0x1;
    (void) NRF_POWER->SYSTEMOFF;
    while (true);
}

static void lfclkConfig(void)
{
    nrf_drv_clock_init();
    nrf_drv_clock_lfclk_request(NULL);
}

static void rtc_config(void)
{
    uint32_t err_code;

    //Initialize RTC instance
    nrfx_rtc_config_t rtcConfig = NRFX_RTC_DEFAULT_CONFIG;
  //  nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
    rtcConfig.prescaler = 4095;
    
    nrfx_rtc_init(&rtc, &rtcConfig, rtc_handler);

//    nrf_drv_rtc_init(&rtc, &config, rtc_handler);
    
    //Enable tick event & interrupt
//    nrf_drv_rtc_tick_enable(&rtc,true);


    //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds

  //  err_code = nrf_drv_rtc_cc_set(&rtc,0,COMPARE_COUNTERTIME * 24,true);
  nrfx_rtc_cc_set(&rtc,0,COMPARE_COUNTERTIME*8, true);

    //Power on RTC instance
    nrfx_rtc_enable(&rtc);
//    nrf_drv_rtc_enable(&rtc);
}

/** @brief: Function for handling the RTC0 interrupts.
 * Triggered on TICK and COMPARE0 match.
 */

volatile bool ledState = false;
static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
      sendHeartbeat = true;
      nrfx_rtc_counter_clear(&rtc);
      nrfx_rtc_cc_set(&rtc,0,COMPARE_COUNTERTIME*8, true);
    }
}