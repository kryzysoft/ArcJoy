#undef NRF_ESB_CREATE_PAYLOAD

#define NRF_ESB_CREATE_PAYLOAD(_pipe, ...)                                                  \
        {.length = NUM_VA_ARGS(__VA_ARGS__), .pipe = _pipe, .rssi = 0, .noack = 0, .pid=0, .data = {__VA_ARGS__}};         \
        STATIC_ASSERT(NUM_VA_ARGS(__VA_ARGS__) > 0 && NUM_VA_ARGS(__VA_ARGS__) <= 63)

#undef NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE

#define NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(hi_accu) \
    {                                               \
        .sense = NRF_GPIOTE_POLARITY_TOGGLE,        \
        .pull = NRF_GPIO_PIN_NOPULL,                \
        .is_watcher = false,                        \
        .hi_accuracy = hi_accu,                     \
        .skip_gpio_setup = false                    \
    }

#undef NRFX_RTC_DEFAULT_CONFIG


#define NRFX_RTC_DEFAULT_CONFIG                                                     \
{                                                                                   \
    .prescaler          = RTC_FREQ_TO_PRESCALER(NRFX_RTC_DEFAULT_CONFIG_FREQUENCY), \
    .interrupt_priority = NRFX_RTC_DEFAULT_CONFIG_IRQ_PRIORITY,                     \
    .tick_latency       = NRFX_RTC_US_TO_TICKS(NRFX_RTC_MAXIMUM_LATENCY_US,         \
                                               NRFX_RTC_DEFAULT_CONFIG_FREQUENCY),  \
    .reliable           = NRFX_RTC_DEFAULT_CONFIG_RELIABLE,                         \
}

#undef STATIC_ASSERT_SIMPLE
#define STATIC_ASSERT_SIMPLE(EXPR)

#undef STATIC_ASSERT_MSG
#define STATIC_ASSERT_MSG(EXPR, MSG)