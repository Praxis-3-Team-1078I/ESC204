#include "pico/stdlib.h"
#include <iostream>

#define PIR_PIN 1

int main()
{
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_init(PIR_PIN);
    gpio_set_dir(PIR_PIN, GPIO_IN);

    while (true)
    {
        bool cur_pir_value = gpio_get(PIR_PIN);
        gpio_put(PICO_DEFAULT_LED_PIN, cur_pir_value);

        if (cur_pir_value)
        {
            std::cout << 1 << std::endl;
        }
        else
        {
            std::cout << 0.01 << std::endl;
        }
        sleep_ms(200);
    }

    return 0;
}