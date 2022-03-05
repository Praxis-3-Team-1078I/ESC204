#include "dht.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <optional>

// addresses and commands from datasheet
#define AM2320_ADDR 0x5C
#define AM2320_CMD_READREG 0x03
#define START_ADDR 0x0

// checksum algo from dht datasheet
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
    // use an 8 byte buffer for sending and receiving data
    uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    // write to the dht to wake it up
    i2c_write_blocking(i2c_default, AM2320_ADDR, buffer, 1, false);
    sleep_ms(10);

    // send request to read all data from the dht (4 bytes of temp and hum data)
    buffer[0] = AM2320_CMD_READREG;
    buffer[1] = START_ADDR;
    buffer[2] = 4;
    i2c_write_blocking(i2c_default, AM2320_ADDR, buffer, 3, false);
    sleep_ms(2);

    // dht sends us back 8 bytes, read them
    i2c_read_blocking(i2c_default, AM2320_ADDR, buffer, 8, false);

    // if echoed command was not to read 4 bytes of data registers, error
    if (buffer[0] != AM2320_CMD_READREG || buffer[1] != 4)
    {
        return std::nullopt;
    }

    // calulate checksum and compare vs recived checksum (2 last bytes)
    // if no match, error
    uint16_t the_crc = (buffer[7] << 8) | buffer[6];
    uint16_t calc_crc = crc16(buffer, 6);
    if (the_crc != calc_crc)
    {
        return std::nullopt;
    }

    // extract the temp and hum data from recived data
    // temp: high is byte 4, low is byte 5
    // hum: high is byte 2, low is byte 3
    uint16_t tempData = (buffer[4] << 8) | buffer[5];
    uint16_t humData = (buffer[2] << 8) | buffer[3];

    // create struct with temp and hum data by dividing recived values by 10,
    // clearing high of temp as it only communicates sign data
    dht_data data = {static_cast<float>(tempData & 0x7FFF) / 10, static_cast<float>(humData) / 10};

    // if first bit of temp is set, it is negative
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

    // init the i2c controller in the pico and enable and set up
    // the data and clock pins to drive dht
    i2c_init(i2c_default, 50000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(scl);
}