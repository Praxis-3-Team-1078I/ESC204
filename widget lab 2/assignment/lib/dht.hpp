#ifndef DHT_HPP
#define DHT_HPP

#include "pico/stdlib.h"
#include <optional>

struct dht_data
{
    float temp;
    float hum;
};

void init_dht(uint sda, uint scl);
std::optional<dht_data> read_data_dht();

#endif