#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <limits>

#define LED_PIN 16
#define SYS_CLK_HZ 125000000.0f
#define COUNTS_PER_CYCLE (static_cast<uint>(std::numeric_limits<uint16_t>::max()) + 1)
#define PWM_FREQ_HZ 1000

int main()
{
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
    pwm_config config = pwm_get_default_config();
    static_assert(SYS_CLK_HZ / (PWM_FREQ_HZ * COUNTS_PER_CYCLE) >= 1, "Invalid PWM frequency!");
    pwm_config_set_clkdiv(&config, SYS_CLK_HZ / (PWM_FREQ_HZ * COUNTS_PER_CYCLE));
    pwm_init(pwm_gpio_to_slice_num(LED_PIN), &config, true);

    while (true)
    {
        for (int i = 0; i < std::numeric_limits<uint16_t>::max(); i += 50)
        {
            pwm_set_gpio_level(LED_PIN, static_cast<uint16_t>(i));
            sleep_ms(1);
        }

        for (int i = std::numeric_limits<uint16_t>::max(); i > 0; i -= 50)
        {
            pwm_set_gpio_level(LED_PIN, static_cast<uint16_t>(i));
            sleep_ms(1);
        }
    }

    return 0;
}