#include "lib/dht.hpp"
#include "lib/pwm.hpp"
#include "lib/thread.hpp"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <iomanip>
#include <iostream>

#define MOTOR_PIN 15
#define DHT_SDA 16
#define DHT_SCL 17
#define BUZZER_PIN 18
#define TIME_TILL_BEEP_MS 120000

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
    absolute_time_t time_to_buzz = make_timeout_time_ms(TIME_TILL_BEEP_MS);

    while (true)
    {
        // capture status for this loop to avoid threading bugs
        Status now_status = cur_status;

        if (now_status != last_status)
        {
            // reset variables, we have changed modes
            counting_up = true;
            cur_duty = 0;
            cur_freq = 50;
            time_to_buzz = make_timeout_time_ms(TIME_TILL_BEEP_MS);
        }

        if (now_status == bad_temp && absolute_time_diff_us(get_absolute_time(), time_to_buzz) < 0)
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
        else if (now_status == bad_hum && absolute_time_diff_us(get_absolute_time(), time_to_buzz) < 0)
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
        else if (now_status == bad_both && absolute_time_diff_us(get_absolute_time(), time_to_buzz) < 0)
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
    config_pwm(MOTOR_PIN, 500, 0);

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
                set_duty_pwm(MOTOR_PIN, 60000);
                cur_status = bad_both;
            }
            else if (data_bad_temp)
            {
                set_duty_pwm(MOTOR_PIN, 60000);
                cur_status = bad_temp;
            }
            else if (data_bad_hum)
            {
                set_duty_pwm(MOTOR_PIN, 60000);
                cur_status = bad_hum;
            }
            else
            {
                set_duty_pwm(MOTOR_PIN, 0);
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