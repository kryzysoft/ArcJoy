/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "nrf_esb.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sdk_common.h"
#include "nrf.h"
#include "nrf_esb_error_codes.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "nrf_drv_usbd.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_error.h"
#include "nrf_drv_power.h"
#include "boards.h"
#include "ArcBase_c.h"

#include "app_timer.h"
#include "app_usbd.h"
#include "app_usbd_core.h"
#include "app_usbd_hid_generic.h"
#include "app_usbd_hid_mouse.h"
#include "app_usbd_hid_kbd.h"
#include "app_error.h"
#include "bsp.h"

#include "bsp_cli.h"
#include "nrf_cli.h"
#include "nrf_cli_uart.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/**
 * @brief CLI interface over UART
 */
NRF_CLI_UART_DEF(m_cli_uart_transport, 0, 64, 16);
NRF_CLI_DEF(m_cli_uart,
            "uart_cli:~$ ",
            &m_cli_uart_transport.transport,
            '\r',
            4);

/**
 * @brief Enable USB power detection
 */
#ifndef USBD_POWER_DETECTION
#define USBD_POWER_DETECTION true
#endif

/**
 * @brief HID generic class interface number.
 * */
#define HID_GENERIC_INTERFACE  0

/**
 * @brief HID generic class endpoint number.
 * */
#define HID_GENERIC_EPIN       NRF_DRV_USBD_EPIN1

/**
 * @brief Mouse speed (value sent via HID when board button is pressed).
 * */
#define CONFIG_MOUSE_MOVE_SPEED (1)

/**
 * @brief Mouse move repeat time in milliseconds
 */
#define CONFIG_MOUSE_MOVE_TIME_MS (5)


/* GPIO used as LED & buttons in this example */
#define LED_USB_START    (BSP_BOARD_LED_0)
#define LED_HID_REP_IN   (BSP_BOARD_LED_2)

#define BTN_MOUSE_X_POS  0
#define BTN_MOUSE_Y_POS  1
#define BTN_MOUSE_LEFT   2
#define BTN_MOUSE_RIGHT  3

/**
 * @brief Left button mask in buttons report
 */
#define HID_BTN_LEFT_MASK  (1U << 0)

/**
 * @brief Right button mask in buttons report
 */
#define HID_BTN_RIGHT_MASK (1U << 1)

/* HID report layout */
#define HID_BTN_IDX   2 /**< Button bit mask position */
#define HID_X_IDX     0 /**< X offset position */
#define HID_Y_IDX     1 /**< Y offset position */
#define HID_REP_SIZE  4 /**< The size of the report */

/**
 * @brief Number of reports defined in report descriptor.
 */
#define REPORT_IN_QUEUE_SIZE    1

/**
 * @brief Size of maximum output report. HID generic class will reserve
 *        this buffer size + 1 memory space. 
 *
 * Maximum value of this define is 63 bytes. Library automatically adds
 * one byte for report ID. This means that output report size is limited
 * to 64 bytes.
 */
#define REPORT_OUT_MAXSIZE  0

/**
 * @brief HID generic class endpoints count.
 * */

/**
 * @brief List of HID generic class endpoints.
 * */
#define ENDPOINT_LIST()                                      \
(                                                            \
        HID_GENERIC_EPIN                                     \
)

/**
 * @brief Additional key release events
 *
 * This example needs to process release events of used buttons
 */
enum {
    BSP_USER_EVENT_RELEASE_0 = BSP_EVENT_KEY_LAST + 1, /**< Button 0 released */
    BSP_USER_EVENT_RELEASE_1,                          /**< Button 1 released */
    BSP_USER_EVENT_RELEASE_2,                          /**< Button 2 released */
    BSP_USER_EVENT_RELEASE_3,                          /**< Button 3 released */
    BSP_USER_EVENT_RELEASE_4,                          /**< Button 4 released */
    BSP_USER_EVENT_RELEASE_5,                          /**< Button 5 released */
    BSP_USER_EVENT_RELEASE_6,                          /**< Button 6 released */
    BSP_USER_EVENT_RELEASE_7,                          /**< Button 7 released */
};

/**
 * @brief HID generic mouse action types
 */
typedef enum {
    HID_GENERIC_MOUSE_X,
    HID_GENERIC_MOUSE_Y,
    HID_GENERIC_MOUSE_BTN_LEFT,
    HID_GENERIC_MOUSE_BTN_RIGHT,
} hid_generic_mouse_action_t;

/**
 * @brief User event handler.
 * */
static void hid_user1_ev_handler(app_usbd_class_inst_t const * p_inst,
                                app_usbd_hid_user_event_t event);

static void hid_user2_ev_handler(app_usbd_class_inst_t const * p_inst,
                                app_usbd_hid_user_event_t event);

/**
 * @brief Reuse HID mouse report descriptor for HID generic class
 */

#define APP_USBD_HID_JOY_REPORT_DSC_BUTTON1(bcnt) {                \
    0x05, 0x01,                    /* USAGE_PAGE (Generic Desktop) */ \
    0x09, 0x05,                    /* USAGE (Game Pad)*/ \
    0xa1, 0x01,                    /* COLLECTION (Application)*/ \
    0x05, 0x01,                    /*   USAGE_PAGE (Generic Desktop)*/ \
    0xa1, 0x00,                    /*   COLLECTION (Physical)*/ \
    0x85, 0x01,                    /*   COLLECTION (Physical)*/ \
    0x09, 0x30,                    /*   USAGE (X)*/ \
    0x09, 0x31,                    /*   USAGE (Y)*/ \
    0x15, 0x81,                    /*   LOGICAL_MINIMUM (-127)*/ \
    0x25, 0x7f,                    /*   LOGICAL_MAXIMUM (127)*/ \
    0x75, 0x08,                    /*   REPORT_SIZE (8)*/ \
    0x95, 0x02,                    /*   REPORT_COUNT (2)*/ \
    0x81, 0x02,                    /*   INPUT (Data,Var,Abs)*/ \
    0x05, 0x09,                    /*   USAGE_PAGE (Button)*/ \
    0x19, 0x01,                    /*   USAGE_MINIMUM (Button 1)*/ \
    0x29, 0x08,                    /*   USAGE_MAXIMUM (Button 8)*/ \
    0x15, 0x00,                    /*   LOGICAL_MINIMUM (0)*/ \
    0x25, 0x01,                    /*   LOGICAL_MAXIMUM (1)*/ \
    0x75, 0x01,                    /*   REPORT_SIZE (1)*/ \
    0x95, 0x08,                    /*   REPORT_COUNT (16)*/ \
    0x81, 0x02,                    /*   INPUT (Data,Var,Abs)*/ \
    0xc0,                          /* END_COLLECTION*/ \
    0xc0                           /* END_COLLECTION*/ \
}

#define APP_USBD_HID_JOY_REPORT_DSC_BUTTON2(bcnt) {                \
    0x05, 0x01,                    /* USAGE_PAGE (Generic Desktop) */ \
    0x09, 0x05,                    /* USAGE (Game Pad)*/ \
    0xa1, 0x01,                    /* COLLECTION (Application)*/ \
    0x05, 0x01,                    /*   USAGE_PAGE (Generic Desktop)*/ \
    0xa1, 0x00,                    /*   COLLECTION (Physical)*/ \
    0x85, 0x02,                    /*   COLLECTION (Physical)*/ \
    0x09, 0x30,                    /*   USAGE (X)*/ \
    0x09, 0x31,                    /*   USAGE (Y)*/ \
    0x15, 0x81,                    /*   LOGICAL_MINIMUM (-127)*/ \
    0x25, 0x7f,                    /*   LOGICAL_MAXIMUM (127)*/ \
    0x75, 0x08,                    /*   REPORT_SIZE (8)*/ \
    0x95, 0x02,                    /*   REPORT_COUNT (2)*/ \
    0x81, 0x02,                    /*   INPUT (Data,Var,Abs)*/ \
    0x05, 0x09,                    /*   USAGE_PAGE (Button)*/ \
    0x19, 0x01,                    /*   USAGE_MINIMUM (Button 1)*/ \
    0x29, 0x08,                    /*   USAGE_MAXIMUM (Button 8)*/ \
    0x15, 0x00,                    /*   LOGICAL_MINIMUM (0)*/ \
    0x25, 0x01,                    /*   LOGICAL_MAXIMUM (1)*/ \
    0x75, 0x01,                    /*   REPORT_SIZE (1)*/ \
    0x95, 0x08,                    /*   REPORT_COUNT (16)*/ \
    0x81, 0x02,                    /*   INPUT (Data,Var,Abs)*/ \
    0xc0,                          /* END_COLLECTION*/ \
    0xc0                           /* END_COLLECTION*/ \
}


APP_USBD_HID_GENERIC_SUBCLASS_REPORT_DESC(mouse1_desc,APP_USBD_HID_JOY_REPORT_DSC_BUTTON1(bcnt));
APP_USBD_HID_GENERIC_SUBCLASS_REPORT_DESC(mouse2_desc,APP_USBD_HID_JOY_REPORT_DSC_BUTTON2(bcnt));

static const app_usbd_hid_subclass_desc_t * reps1[] = {&mouse1_desc};
static const app_usbd_hid_subclass_desc_t * reps2[] = {&mouse2_desc};

/*lint -save -e26 -e64 -e123 -e505 -e651*/

/**
 * @brief Global HID generic instance
 */
APP_USBD_HID_GENERIC_GLOBAL_DEF(m_app_hid1_generic,
                                HID_GENERIC_INTERFACE,
                                hid_user1_ev_handler,
                                (NRF_DRV_USBD_EPIN1),
                                reps1,
                                REPORT_IN_QUEUE_SIZE,
                                REPORT_OUT_MAXSIZE,
                                APP_USBD_HID_SUBCLASS_NONE,
                                APP_USBD_HID_PROTO_GENERIC);

APP_USBD_HID_GENERIC_GLOBAL_DEF(m_app_hid2_generic,
                                1,
                                hid_user2_ev_handler,
                                (NRF_DRV_USBD_EPIN2),
                                reps2,
                                REPORT_IN_QUEUE_SIZE,
                                REPORT_OUT_MAXSIZE,
                                APP_USBD_HID_SUBCLASS_NONE,
                                APP_USBD_HID_PROTO_GENERIC);
/*lint -restore*/







uint8_t led_nr;

nrf_esb_payload_t rx_payload;

/*lint -save -esym(40, BUTTON_1) -esym(40, BUTTON_2) -esym(40, BUTTON_3) -esym(40, BUTTON_4) -esym(40, LED_1) -esym(40, LED_2) -esym(40, LED_3) -esym(40, LED_4) */

void clocks_start( void )
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}


void gpio_init( void )
{
    bsp_board_init(BSP_INIT_LEDS);
}



/**
 * @brief Mouse state
 *
 * Current mouse status
 */
struct
{
    int16_t acc_x;    /**< Accumulated x state */
    int16_t acc_y;    /**< Accumulated y state */
    uint8_t btn;      /**< Current btn state */
    uint8_t last_btn; /**< Last transfered button state */
}m_mouse_state;

/**
 * @brief Mark the ongoing transmission
 *
 * Marks that the report buffer is busy and cannot be used until transmission finishes
 * or invalidates (by USB reset or suspend event).
 */
static bool m_report_pending;

/**
 * @brief Timer to repeat mouse move
 */
APP_TIMER_DEF(m_mouse_move_timer);

/**
 * @brief Get maximal allowed accumulated value
 *
 * Function gets maximal value from the accumulated input.
 * @sa m_mouse_state::acc_x, m_mouse_state::acc_y
 */
static int8_t hid_acc_for_report_get(int16_t acc)
{
    if(acc > INT8_MAX)
    {
        return INT8_MAX;
    }
    else if(acc < INT8_MIN)
    {
        return INT8_MIN;
    }
    else
    {
        return (int8_t)(acc);
    }
}

/**
 * @brief Internal function that process mouse state
 *
 * This function checks current mouse state and tries to send
 * new report if required.
 * If report sending was successful it clears accumulated positions
 * and mark last button state that was transfered.
 */
static void hid_generic_mouse1_process_state(JoyState joyState)
{
  static uint8_t report[HID_REP_SIZE];

  report[0] = 1;
  report[3] = joyState.fire;

  report[1]   = -joyState.left*127 + joyState.right*127;
  report[2]   =  -joyState.up*127 + joyState.down*127;

  app_usbd_hid_generic_in_report_set(&m_app_hid1_generic,report,sizeof(report));
}

static void hid_generic_mouse2_process_state(JoyState joyState)
{
  static uint8_t report[HID_REP_SIZE];
  report[0] = 2;
  report[3] = joyState.fire;
  report[1]   = -joyState.left*127 + joyState.right*127;
  report[2]   =  -joyState.up*127 + joyState.down*127;
  app_usbd_hid_generic_in_report_set(&m_app_hid2_generic,report,sizeof(report));
}


/**
 * @brief Class specific event handler.
 *
 * @param p_inst    Class instance.
 * @param event     Class specific event.
 * */
static void hid_user1_ev_handler(app_usbd_class_inst_t const * p_inst,
                                app_usbd_hid_user_event_t event)
{
    switch (event)
    {
        case APP_USBD_HID_USER_EVT_OUT_REPORT_READY:
        {
            /* No output report defined for this example.*/
            ASSERT(0);
            break;
        }
        case APP_USBD_HID_USER_EVT_IN_REPORT_DONE:
        {
//            m_report_pending = false;
//            hid_generic_mouse1_process_state();
//            bsp_board_led_invert(LED_HID_REP_IN);
            break;
        }
        case APP_USBD_HID_USER_EVT_SET_BOOT_PROTO:
        {
//            UNUSED_RETURN_VALUE(hid_generic_clear_buffer(p_inst));
//            NRF_LOG_INFO("SET_BOOT_PROTO");
            break;
        }
        case APP_USBD_HID_USER_EVT_SET_REPORT_PROTO:
        {
//            UNUSED_RETURN_VALUE(hid_generic_clear_buffer(p_inst));
//            NRF_LOG_INFO("SET_REPORT_PROTO");
            break;
        }
        default:
            break;
    }
}

static void hid_user2_ev_handler(app_usbd_class_inst_t const * p_inst,
                                app_usbd_hid_user_event_t event)
{
    switch (event)
    {
        case APP_USBD_HID_USER_EVT_OUT_REPORT_READY:
        {
            /* No output report defined for this example.*/
            ASSERT(0);
            break;
        }
        case APP_USBD_HID_USER_EVT_IN_REPORT_DONE:
        {
//            m_report_pending = false;
//            hid_generic_mouse2_process_state();
//            bsp_board_led_invert(LED_HID_REP_IN);
            break;
        }
        case APP_USBD_HID_USER_EVT_SET_BOOT_PROTO:
        {
//            UNUSED_RETURN_VALUE(hid_generic_clear_buffer(p_inst));
//            NRF_LOG_INFO("SET_BOOT_PROTO");
            break;
        }
        case APP_USBD_HID_USER_EVT_SET_REPORT_PROTO:
        {
//            UNUSED_RETURN_VALUE(hid_generic_clear_buffer(p_inst));
//            NRF_LOG_INFO("SET_REPORT_PROTO");
            break;
        }
        default:
            break;
    }
}

/**
 * @brief USBD library specific event handler.
 *
 * @param event     USBD library event.
 * */
static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SOF:
            break;
        case APP_USBD_EVT_DRV_RESET:
            m_report_pending = false;
            break;
        case APP_USBD_EVT_DRV_SUSPEND:
            m_report_pending = false;
            app_usbd_suspend_req(); // Allow the library to put the peripheral into sleep mode
            bsp_board_leds_off();
            break;
        case APP_USBD_EVT_DRV_RESUME:
            m_report_pending = false;
            bsp_board_led_on(LED_USB_START);
            break;
        case APP_USBD_EVT_STARTED:
            m_report_pending = false;
            bsp_board_led_on(LED_USB_START);
            break;
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            bsp_board_leds_off();
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            NRF_LOG_INFO("USB power detected");
            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            NRF_LOG_INFO("USB power removed");
            app_usbd_stop();
            break;
        case APP_USBD_EVT_POWER_READY:
            NRF_LOG_INFO("USB ready");
            app_usbd_start();
            break;
        default:
            break;
    }
}


static void bsp_event_callback(bsp_event_t ev)
{
//    switch ((unsigned int)ev)
//    {
//        case CONCAT_2(BSP_EVENT_KEY_, BTN_MOUSE_X_POS):
//            hid_generic_mouse_action(HID_GENERIC_MOUSE_X, CONFIG_MOUSE_MOVE_SPEED);
//            UNUSED_RETURN_VALUE(app_timer_start(m_mouse_move_timer, APP_TIMER_TICKS(CONFIG_MOUSE_MOVE_TIME_MS), NULL));
//            break;
//
//        case CONCAT_2(BSP_EVENT_KEY_, BTN_MOUSE_Y_POS):
//            hid_generic_mouse_action(HID_GENERIC_MOUSE_Y, CONFIG_MOUSE_MOVE_SPEED);
//            UNUSED_RETURN_VALUE(app_timer_start(m_mouse_move_timer, APP_TIMER_TICKS(CONFIG_MOUSE_MOVE_TIME_MS), NULL));
//            break;
//
//        case CONCAT_2(BSP_EVENT_KEY_, BTN_MOUSE_RIGHT):
//            hid_generic_mouse_action(HID_GENERIC_MOUSE_BTN_RIGHT, 1);
//            break;
//        case CONCAT_2(BSP_USER_EVENT_RELEASE_, BTN_MOUSE_RIGHT):
//            hid_generic_mouse_action(HID_GENERIC_MOUSE_BTN_RIGHT, -1);
//            break;
//
//        case CONCAT_2(BSP_EVENT_KEY_, BTN_MOUSE_LEFT):
//            hid_generic_mouse_action(HID_GENERIC_MOUSE_BTN_LEFT, 1);
//            break;
//        case CONCAT_2(BSP_USER_EVENT_RELEASE_, BTN_MOUSE_LEFT):
//            hid_generic_mouse_action(HID_GENERIC_MOUSE_BTN_LEFT, -1);
//            break;
//
//        default:
//            return; // no implementation needed
//    }
}


/**
 * @brief Auxiliary internal macro
 *
 * Macro used only in @ref init_bsp to simplify the configuration
 */
#define INIT_BSP_ASSIGN_RELEASE_ACTION(btn)                      \
    APP_ERROR_CHECK(                                             \
        bsp_event_to_button_action_assign(                       \
            btn,                                                 \
            BSP_BUTTON_ACTION_RELEASE,                           \
            (bsp_event_t)CONCAT_2(BSP_USER_EVENT_RELEASE_, btn)) \
    )

static void init_bsp(void)
{
    ret_code_t ret;
    ret = bsp_init(BSP_INIT_BUTTONS, bsp_event_callback);
    APP_ERROR_CHECK(ret);

    INIT_BSP_ASSIGN_RELEASE_ACTION(BTN_MOUSE_LEFT );
    INIT_BSP_ASSIGN_RELEASE_ACTION(BTN_MOUSE_RIGHT);

    /* Configure LEDs */
    bsp_board_init(BSP_INIT_LEDS);
}

static void init_cli(void)
{
    ret_code_t ret;
    ret = bsp_cli_init(bsp_event_callback);
    APP_ERROR_CHECK(ret);
    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;
    uart_config.pseltxd = TX_PIN_NUMBER;
    uart_config.pselrxd = RX_PIN_NUMBER;
    uart_config.hwfc    = NRF_UART_HWFC_DISABLED;
    ret = nrf_cli_init(&m_cli_uart, &uart_config, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(ret);
    ret = nrf_cli_start(&m_cli_uart);
    APP_ERROR_CHECK(ret);
}

static ret_code_t idle_handle(app_usbd_class_inst_t const * p_inst, uint8_t report_id)
{
    switch (report_id)
    {
        case 0:
        {
            uint8_t report[] = {0xBE, 0xEF};
            return app_usbd_hid_generic_idle_report_set(
              &m_app_hid1_generic,
              report,
              sizeof(report));
        }
        default:
            return NRF_ERROR_NOT_SUPPORTED;
    }
    
}

void arcBase_init(void)
{
    ret_code_t ret;
    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = usbd_user_ev_handler
    };

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);

    nrf_drv_clock_lfclk_request(NULL);

    while(!nrf_drv_clock_lfclk_is_running())
    {
        /* Just waiting */
    }

    ret = app_timer_init();


    ret = app_usbd_init(&usbd_config);


    NRF_LOG_INFO("USBD HID generic example started.");

    app_usbd_class_inst_t const * class_inst_generic1;
    app_usbd_class_inst_t const * class_inst_generic2;

    class_inst_generic1 = app_usbd_hid_generic_class_inst_get(&m_app_hid1_generic);


    ret = app_usbd_class_append(class_inst_generic1);
    APP_ERROR_CHECK(ret);
    class_inst_generic2 = app_usbd_hid_generic_class_inst_get(&m_app_hid2_generic);

    ret = app_usbd_class_append(class_inst_generic2);
    APP_ERROR_CHECK(ret);


    if (USBD_POWER_DETECTION)
    {
        ret = app_usbd_power_events_enable();
        APP_ERROR_CHECK(ret);
    }
    else
    {
        NRF_LOG_INFO("No USB power detection enabled\r\nStarting USB now");

        app_usbd_enable();
        app_usbd_start();
    }

    app_timer_start(m_mouse_move_timer, APP_TIMER_TICKS(CONFIG_MOUSE_MOVE_TIME_MS), NULL);
    clocks_start();
//    esb_init();
//    nrf_esb_start_rx();
}

void arcBase_whileLoop(JoyState joy1, JoyState joy2)
{
  while (app_usbd_event_queue_process())
  {
      /* Nothing to do */
  }
  hid_generic_mouse1_process_state(joy1);
  hid_generic_mouse2_process_state(joy2);
  nrf_cli_process(&m_cli_uart);

  UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
  /* Sleep CPU only if there was no interrupt since last loop processing */
  __WFE();
}
