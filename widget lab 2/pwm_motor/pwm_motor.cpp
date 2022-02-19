#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <iostream>
#include <limits>

#define MOTOR_IN1_PIN 14
#define MOTOR_IN2_PIN 15
#define MOTOR_ENA_PIN 16
#define SYS_CLK_HZ 125000000.0f
#define COUNTS_PER_CYCLE (static_cast<uint>(std::numeric_limits<uint16_t>::max()) + 1)
#define PWM_FREQ_HZ 500
#define DUTY_STEP 5000
#define TIME_LIMIT_MS 10000

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    gpio_init(MOTOR_IN1_PIN);
    gpio_set_dir(MOTOR_IN1_PIN, GPIO_OUT);
    gpio_init(MOTOR_IN2_PIN);
    gpio_set_dir(MOTOR_IN2_PIN, GPIO_OUT);

    gpio_set_function(MOTOR_ENA_PIN, GPIO_FUNC_PWM);
    pwm_config config = pwm_get_default_config();
    static_assert(SYS_CLK_HZ / (PWM_FREQ_HZ * COUNTS_PER_CYCLE) >= 1, "Invalid PWM frequency!");
    pwm_config_set_clkdiv(&config, SYS_CLK_HZ / (PWM_FREQ_HZ * COUNTS_PER_CYCLE));
    pwm_init(pwm_gpio_to_slice_num(MOTOR_ENA_PIN), &config, true);
    pwm_set_gpio_level(MOTOR_ENA_PIN, 0);

    uint16_t duty = 60000;

    for (absolute_time_t timeout = make_timeout_time_ms(TIME_LIMIT_MS);
         absolute_time_diff_us(get_absolute_time(), timeout) > 0; duty -= DUTY_STEP)
    {
        gpio_put(MOTOR_IN1_PIN, false);
        gpio_put(MOTOR_IN2_PIN, true);
        pwm_set_gpio_level(MOTOR_ENA_PIN, duty - 10000);
        std::cout << "Motor is rotating CW at " << (100.0f * (duty - 10000) / std::numeric_limits<uint16_t>::max())
                  << "% duty cycle" << std::endl;
        sleep_ms(2000);

        gpio_put(MOTOR_IN1_PIN, true);
        gpio_put(MOTOR_IN2_PIN, false);
        pwm_set_gpio_level(MOTOR_ENA_PIN, duty);
        std::cout << "Motor is rotating CCW at " << (100.0f * duty / std::numeric_limits<uint16_t>::max())
                  << "% duty cycle" << std::endl;
        sleep_ms(2000);
    }

    pwm_set_gpio_level(MOTOR_ENA_PIN, 0);

    while (true)
    {
        tight_loop_contents();
    }

    return 0;
}