#include "pico/stdlib.h"
#include "pico/time.h"
#include <iostream>

#define BUTTON_PIN 16
#define TRIGGER_PIN 2
#define ECHO_PIN 3

bool trigger_on_button_pressed()
{
    static bool prev_button_value = false;

    bool cur_button_value = gpio_get(BUTTON_PIN);
    bool button_pressed = false;

    if (!cur_button_value && prev_button_value)
    {
        gpio_put(TRIGGER_PIN, true);
        button_pressed = true;
        std::cout << "Trigger" << std::endl;
        gpio_put(TRIGGER_PIN, false);
    }

    prev_button_value = cur_button_value;
    return button_pressed;
}

uint capture_echo(uint64_t timeout_us)
{
    uint points_captured = 0;
    absolute_time_t timeout_time = make_timeout_time_us(timeout_us);

    while (!gpio_get(ECHO_PIN) && absolute_time_diff_us(get_absolute_time(), timeout_time) > 0)
    {
        tight_loop_contents();
    }

    if (absolute_time_diff_us(get_absolute_time(), timeout_time) <= 0)
    {
        std::cout << "Timeout!" << std::endl;
    }
    else
    {
        while (gpio_get(ECHO_PIN))
        {
            points_captured++;
        }
    }

    return points_captured;
}

void plot_data(uint data)
{
    std::cout << data << std::endl;
}

int main()
{
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_init(TRIGGER_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_pull_up(ECHO_PIN);

    while (true)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(BUTTON_PIN));

        if (trigger_on_button_pressed())
        {
            uint data = capture_echo(5000);
            if (data > 0)
            {
                plot_data(data);
            }
        }
    }

    return 0;
}