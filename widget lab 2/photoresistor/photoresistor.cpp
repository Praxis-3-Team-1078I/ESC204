#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <iostream>
#include <limits>

#define PHOTORESISTOR_PIN 26
#define PHOTORESISTOR_ADC 0
#define ADC_REF 3.3

float adc_to_voltage(uint16_t adc_value)
{
    return ADC_REF * static_cast<float>(adc_value) / static_cast<float>(std::numeric_limits<uint16_t>::max());
}

int main()
{
    stdio_init_all();
    adc_init();

    adc_gpio_init(PHOTORESISTOR_PIN);
    adc_select_input(PHOTORESISTOR_ADC);

    while (true)
    {
        std::cout << adc_to_voltage(adc_read()) << std::endl;
        sleep_ms(500);
    }

    return 0;
}