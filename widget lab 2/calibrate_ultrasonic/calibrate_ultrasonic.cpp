#include "pico/stdlib.h"
#include "pico/time.h"
#include <iostream>
#include <optional>

#define TRIGGER_PIN 2
#define ECHO_PIN 3
#define TIMEOUT_US 100000

std::optional<float> get_distance()
{
    gpio_put(TRIGGER_PIN, true);
    sleep_us(10);
    gpio_put(TRIGGER_PIN, false);

    absolute_time_t startTime = get_absolute_time();
    while (!gpio_get(ECHO_PIN))
    {
        if (absolute_time_diff_us(startTime, get_absolute_time()) > TIMEOUT_US)
        {
            return std::nullopt;
        }
    }

    startTime = get_absolute_time();
    while (gpio_get(ECHO_PIN))
    {
        if (absolute_time_diff_us(startTime, get_absolute_time()) > TIMEOUT_US)
        {
            return std::nullopt;
        }
    }

    int64_t pulseLen = absolute_time_diff_us(startTime, get_absolute_time());

    return (pulseLen >= 0) ? std::optional<float>(static_cast<float>(pulseLen) * 0.017) : std::nullopt;
}

int main()
{
    stdio_init_all();

    gpio_init(TRIGGER_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    while (true)
    {
        auto dist = get_distance();

        if (dist)
        {
            std::cout << *dist << " cm" << std::endl;
        }
        else
        {
            std::cout << "Retrying!";
        }

        sleep_ms(2000);
    }

    return 0;
}