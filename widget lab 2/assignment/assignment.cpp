#include "lib/dht.hpp"
#include "lib/pwm.hpp"
#include "lib/thread.hpp"
#include "pico/stdlib.h"
#include <iomanip>
#include <iostream>

#define DHT_SDA 16
#define DHT_SCL 17

#define BUZZER_PIN 18

enum Status
{
    good,
    bad_temp,
    bad_hum,
    bad_both
};

Status cur_status = good;

void buzzer_delegate()
{
    config_pwm(BUZZER_PIN, 150, 0);
    Status last_status = good;
    bool counting_up = true;
    uint16_t cur_duty = 0;
    uint16_t cur_freq = 50;

    while (true)
    {
        // capture status for this loop to avoid threading bugs
        Status now_status = cur_status;

        if (now_status != last_status)
        {
            // reset variables, we have changed modes
            bool counting_up = true;
            uint16_t curDuty = 0;
            uint16_t cur_freq = 50;
        }

        if (now_status == bad_temp)
        {
            if (now_status != last_status)
            {
                set_freq_pwm(BUZZER_PIN, 150);
            }

            // change pwm duty
            set_duty_pwm(BUZZER_PIN, cur_duty);
            sleep_ms(100);
            set_duty_pwm(BUZZER_PIN, 0);
            sleep_ms(10);

            if (counting_up)
            {
                cur_duty += 1000;
                counting_up = cur_duty < 40000;
            }
            else
            {
                cur_duty -= 1000;
                counting_up = !(cur_duty > 0);
            }
        }
        else if (now_status == bad_hum)
        {
            if (now_status != last_status)
            {
                set_duty_pwm(BUZZER_PIN, 1000);
            }

            // change pwm freq
            set_freq_pwm(BUZZER_PIN, cur_freq);
            sleep_ms(100);

            if (counting_up)
            {
                cur_freq += 50;
                counting_up = cur_freq < 700;
            }
            else
            {
                cur_freq -= 50;
                counting_up = !(cur_freq > 50);
            }
        }
        else if (now_status == bad_both)
        {
            // solid beeping noise at loud volume and standard frequency
            if (now_status != last_status)
            {
                set_freq_pwm(BUZZER_PIN, 150);
                set_duty_pwm(BUZZER_PIN, 40000);
            }
        }
        else
        {
            // all good, no noise
            if (now_status != last_status)
            {
                set_duty_pwm(BUZZER_PIN, 0);
            }
        }

        last_status = now_status;
    }
}

int main()
{
    stdio_init_all();
    std::cout << std::fixed << std::setprecision(1);

    init_dht(DHT_SDA, DHT_SCL);

    // handle buzzer on other core as will need to constatntly change it
    // and don't want to interupt our polling of the sensor.
    Thread buzzer_thread(buzzer_delegate);

    while (true)
    {
        if (auto data = read_data_dht())
        {
            bool data_bad_temp = (*data).temp < 20 || (*data).temp > 30;
            bool data_bad_hum = (*data).hum < 40 || (*data).hum > 80;

            if (data_bad_temp && data_bad_hum)
            {
                cur_status = bad_both;
            }
            else if (data_bad_temp)
            {
                cur_status = bad_temp;
            }
            else if (data_bad_hum)
            {
                cur_status = bad_hum;
            }
            else
            {
                cur_status = good;
            }

            std::cout << "AM2320 \t Temp: " << data->temp << " C \t Humidity: " << data->hum << "%" << std::endl;
        }
        else
        {
            std::cout << "Error while reading from AM2320" << std::endl;
        }

        sleep_ms(500);
    }

    return 0;
}