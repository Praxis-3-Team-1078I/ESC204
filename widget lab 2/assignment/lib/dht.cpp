#include "dht.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <optional>

#define AM2320_ADDR 0x5C
#define AM2320_CMD_READREG 0x03
#define START_ADDR 0x0

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

std::optional<dht_data> read_data_dht()
{
    uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    i2c_write_blocking(i2c_default, AM2320_ADDR, buffer, 1, false);
    sleep_ms(10);

    buffer[0] = AM2320_CMD_READREG;
    buffer[1] = START_ADDR;
    buffer[2] = 4;
    i2c_write_blocking(i2c_default, AM2320_ADDR, buffer, 3, false);
    sleep_ms(2);

    i2c_read_blocking(i2c_default, AM2320_ADDR, buffer, 8, false);

    if (buffer[0] != 0x03 || buffer[1] != 0x04)
    {
        return std::nullopt;
    }

    uint16_t the_crc = (buffer[7] << 8) | buffer[6];
    uint16_t calc_crc = crc16(buffer, 6);
    if (the_crc != calc_crc)
    {
        return std::nullopt;
    }

    uint16_t tempData = (buffer[4] << 8) | buffer[5];
    uint16_t humData = (buffer[2] << 8) | buffer[3];

    dht_data data = {static_cast<float>(tempData & 0x7FFF) / 10, static_cast<float>(humData) / 10};

    if (tempData & 0x8000)
    {
        data.temp *= -1;
    }

    return data;
}

void init_dht(uint sda, uint scl)
{
    // use only valid sda0 and scl0 pins
    assert((sda % 4 == 0));
    assert(((scl - 1) % 4 == 0));

    i2c_init(i2c_default, 50000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(scl);
}