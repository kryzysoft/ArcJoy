#include "app_usbd.h"
#include "app_usbd_hid_generic.h"
#include "nrf_drv_clock.h"
#include "nRF_Hal/NrfUsbJoysticksRaw.h"

static bool m_report_pending;

static void hid_user1_ev_handler(app_usbd_class_inst_t const * p_inst,
                                app_usbd_hid_user_event_t event);

static void hid_user2_ev_handler(app_usbd_class_inst_t const * p_inst,
                                app_usbd_hid_user_event_t event);

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
#define HID_REP_SIZE  4 /**< The size of the report */

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

#define HID_GENERIC_INTERFACE  0

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
            break;
        case APP_USBD_EVT_DRV_RESUME:
            m_report_pending = false;
            break;
        case APP_USBD_EVT_STARTED:
            m_report_pending = false;
            break;
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            app_usbd_stop();
            break;
        case APP_USBD_EVT_POWER_READY:
            app_usbd_start();
            break;
        default:
            break;
    }
}

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

void JOYSTICKS_Init(void)
{
    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = usbd_user_ev_handler
    };

    nrf_drv_clock_init();

    app_usbd_init(&usbd_config);

    app_usbd_class_inst_t const * class_inst_generic1;
    app_usbd_class_inst_t const * class_inst_generic2;

    class_inst_generic1 = app_usbd_hid_generic_class_inst_get(&m_app_hid1_generic);
    app_usbd_class_append(class_inst_generic1);

    class_inst_generic2 = app_usbd_hid_generic_class_inst_get(&m_app_hid2_generic);
    app_usbd_class_append(class_inst_generic2);


    app_usbd_power_events_enable();
//    app_timer_start(m_mouse_move_timer, APP_TIMER_TICKS(CONFIG_MOUSE_MOVE_TIME_MS), NULL);
}

void JOYSTICKS_SendReports(JoyState joy1, JoyState joy2)
{
  while (app_usbd_event_queue_process())
  {
  }
  hid_generic_mouse1_process_state(joy1);
  hid_generic_mouse2_process_state(joy2);
}