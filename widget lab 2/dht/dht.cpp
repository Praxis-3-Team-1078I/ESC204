#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <iomanip>
#include <iostream>

#define DHT_SDA 16
#define DHT_SCL 17
#define AM2320_ADDR 0x5C
#define AM2320_CMD_READREG 0x03
#define HUM_HIGH 0x0
#define HUM_LOW 0x1
#define TEMP_HIGH 0x2
#define TEMP_LOW 0x3

uint16_t crc16(uint8_t *buffer, uint8_t nbytes)
{
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < nbytes; i++)
    {
        uint8_t b = buffer[i];
        crc ^= b;
        for (int x = 0; x < 8; x++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

bool readData(float *temp, float *hum)
{
    uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    i2c_write_blocking(i2c_default, AM2320_ADDR, buffer, 1, false);
    sleep_ms(10);

    buffer[0] = AM2320_CMD_READREG;
    buffer[1] = HUM_HIGH;
    buffer[2] = 4;
    i2c_write_blocking(i2c_default, AM2320_ADDR, buffer, 3, false);
    sleep_ms(2);

    i2c_read_blocking(i2c_default, AM2320_ADDR, buffer, 8, false);

    if (buffer[0] != 0x03 || buffer[1] != 0x04)
    {
        return false;
    }

    uint16_t the_crc = (buffer[7] << 8) | buffer[6];
    uint16_t calc_crc = crc16(buffer, 6);
    if (the_crc != calc_crc)
    {
        return false;
    }

    uint16_t tempData = (buffer[4] << 8) | buffer[5];
    uint16_t humData = (buffer[2] << 8) | buffer[3];

    *temp = static_cast<float>(tempData & 0x7FFF) / 10.0;
    *hum = static_cast<float>(humData) / 10.0;

    if (tempData & 0x8000)
    {
        *temp *= -1.0;
    }

    return true;
}

int main()
{
    stdio_init_all();
    std::cout << std::fixed << std::setprecision(1);

    i2c_init(i2c_default, 50000);
    sleep_ms(3);
    gpio_set_function(DHT_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(DHT_SDA);
    gpio_set_function(DHT_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(DHT_SCL);

    float curTemp;
    float curHum;

    while (true)
    {
        if (readData(&curTemp, &curHum))
        {

            std::cout << "AM2320 \t Temp: " << curTemp << " C \t Humidity: " << curHum << "%" << std::endl;
        }
        else
        {
            std::cout << "Error while reading from AM2320" << std::endl;
        }
        sleep_ms(1500);
    }

    return 0;
}