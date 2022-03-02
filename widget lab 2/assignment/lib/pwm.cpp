#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pwm.hpp"
#include <limits>

#define SYS_CLK_HZ 125000000.0f
#define COUNTS_PER_CYCLE (static_cast<uint>(std::numeric_limits<uint16_t>::max()) + 1)

void config_pwm(uint pin, float freq, uint16_t duty)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, SYS_CLK_HZ / (freq * COUNTS_PER_CYCLE));
    pwm_init(pwm_gpio_to_slice_num(pin), &config, true);
    pwm_set_gpio_level(pin, duty);
}

void set_duty_pwm(uint pin, uint16_t duty)
{
    pwm_set_gpio_level(pin, duty);
}

void set_freq_pwm(uint pin, float freq)
{
    pwm_set_clkdiv(pwm_gpio_to_slice_num(pin), SYS_CLK_HZ / (freq * COUNTS_PER_CYCLE));
}