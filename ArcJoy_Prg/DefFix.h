#undef NRF_ESB_CREATE_PAYLOAD

#define NRF_ESB_CREATE_PAYLOAD(_pipe, ...)                                                  \
        {.length = NUM_VA_ARGS(__VA_ARGS__), .pipe = _pipe, .rssi = 0, .noack - 0, .pid=0, .data = {__VA_ARGS__}};         \
        STATIC_ASSERT(NUM_VA_ARGS(__VA_ARGS__) > 0 && NUM_VA_ARGS(__VA_ARGS__) <= 63)

#undef NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(hi_accu) \

#define NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(hi_accu) \
    {                                               \
        .sense = NRF_GPIOTE_POLARITY_TOGGLE,        \
        .pull = NRF_GPIO_PIN_NOPULL,                \
        .is_watcher = false,                        \
        .hi_accuracy = hi_accu,                     \
        .skip_gpio_setup = 0                        \
    }

