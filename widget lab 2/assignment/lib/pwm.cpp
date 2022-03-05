#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pwm.hpp"
#include <limits>

// define pico clock frequency to be able to calculate desired pwm clock divider
#define SYS_CLK_HZ 125000000.0f
// we want to count from 0 to max 16 bit uint, need for clock divider calculations
#define COUNTS_PER_CYCLE (static_cast<uint>(std::numeric_limits<uint16_t>::max()) + 1)

void config_pwm(uint pin, float freq, uint16_t duty)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    pwm_config config = pwm_get_default_config();
    // figure out what we need to divide system clock by to get desired pwm frequency
    pwm_config_set_clkdiv(&config, SYS_CLK_HZ / (freq * COUNTS_PER_CYCLE));
    // start pwm
    pwm_init(pwm_gpio_to_slice_num(pin), &config, true);
    // set our duty to desired level
    pwm_set_gpio_level(pin, duty);
}

// set the duty of the pwm (0, max 16 bit uint)
void set_duty_pwm(uint pin, uint16_t duty)
{
    pwm_set_gpio_level(pin, duty);
}

// calculate and set clock divider for desired frequency
void set_freq_pwm(uint pin, float freq)
{
    pwm_set_clkdiv(pwm_gpio_to_slice_num(pin), SYS_CLK_HZ / (freq * COUNTS_PER_CYCLE));
}