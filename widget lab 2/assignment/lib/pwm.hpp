#ifndef PWM_HPP
#define PWM_HPP

#include "pico/stdlib.h"

void config_pwm(uint pin, float freq, uint16_t duty);
void set_duty_pwm(uint pin, uint16_t duty);
void set_freq_pwm(uint pin, float freq);

#endif