#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <iostream>

#define DHT_SDA 16
#define DHT_SCL 17
#define HUM_HIGH 0x0
#define HUM_LOW 0x1
#define TEMP_HIGH 0x2
#define TEMP_LOW 0x3

float readHum()
{
    uint8_t hum_tot;
    i2c_read_blocking(i2c_default, HUM_HIGH, &hum_tot, 16, true);

    return static_cast<float>(hum_tot) / 10.0;
}

float readTemp()
{
    uint8_t temp_high;
    uint8_t temp_low;
    i2c_read_blocking(i2c_default, TEMP_HIGH, &temp_high, 8, true);
    i2c_read_blocking(i2c_default, TEMP_LOW, &temp_low, 8, true);

    if (temp_high & 0x80)
    {
        temp_high = (temp_high & 0x7F);
        uint16_t temp_tot = (temp_high << 8) | temp_low;
        return -1.0 * static_cast<float>(temp_tot) / 10.0;
    }

    uint16_t temp_tot = (temp_high << 8) | temp_low;
    return static_cast<float>(temp_tot) / 10.0;
}

int main()
{
    stdio_init_all();

    i2c_init(i2c_default, 100000);
    gpio_set_function(DHT_SDA, GPIO_FUNC_I2C);
    gpio_set_function(DHT_SCL, GPIO_FUNC_I2C);

    while (true)
    {
        float curHum = readHum();
        float curTemp = readTemp();

        std::cout << "AM2320 \t Temp: " << curTemp << " C \t Humidity: " << curHum << "%" << std::endl;
    }

    return 0;
}