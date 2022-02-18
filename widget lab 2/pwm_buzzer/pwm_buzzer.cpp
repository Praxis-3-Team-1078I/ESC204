#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <limits>
#include <string>

#define BUZZER_PIN 16
#define SYS_CLK_HZ 125000000.0f
#define COUNTS_PER_CYCLE (static_cast<uint>(std::numeric_limits<uint16_t>::max()) + 1)
#define PWM_FREQ_HZ 150

#define DUTY
// #define FREQ

int main()
{
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    pwm_config config = pwm_get_default_config();
    static_assert(SYS_CLK_HZ / (PWM_FREQ_HZ * COUNTS_PER_CYCLE) >= 1, "Invalid PWM frequency!");
    pwm_config_set_clkdiv(&config, SYS_CLK_HZ / (PWM_FREQ_HZ * COUNTS_PER_CYCLE));
    pwm_init(pwm_gpio_to_slice_num(BUZZER_PIN), &config, true);
    pwm_set_gpio_level(BUZZER_PIN, 1000);

    while (true)
    {
#if defined(DUTY)
        {
            for (int i = 0; i < 40000; i += 1000)
            {
                pwm_set_gpio_level(BUZZER_PIN, static_cast<uint16_t>(i));
                sleep_ms(100);
                pwm_set_gpio_level(BUZZER_PIN, 0);
                sleep_ms(10);
            }

            for (int i = 40000; i > 0; i -= 1000)
            {
                pwm_set_gpio_level(BUZZER_PIN, static_cast<uint16_t>(i));
                sleep_ms(100);
                pwm_set_gpio_level(BUZZER_PIN, 0);
                sleep_ms(10);
            }
        }
#elif defined(FREQ)
        {
            for (int i = 50; i < 700; i += 50)
            {
                pwm_set_clkdiv(pwm_gpio_to_slice_num(BUZZER_PIN), SYS_CLK_HZ / (i * COUNTS_PER_CYCLE));
                sleep_ms(100);
            }
            for (int i = 700; i > 0; i -= 50)
            {
                pwm_set_clkdiv(pwm_gpio_to_slice_num(BUZZER_PIN), SYS_CLK_HZ / (i * COUNTS_PER_CYCLE));
                sleep_ms(100);
            }
        }
#endif
    }

    return 0;
}