#include "lib/dht.hpp"
#include "lib/pwm.hpp"
#include "lib/thread.hpp"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <iomanip>
#include <iostream>

#define MOTOR_IN1_PIN 13
#define MOTOR_IN2_PIN 14
#define MOTOR_ENA_PIN 15
#define DHT_SDA 16
#define DHT_SCL 17
#define BUZZER_PIN 22
#define TIME_TILL_BEEP_MS 20000

enum Status
{
    good,
    bad_temp,
    bad_hum,
    bad_both
};

// global variable for 2 cores to communicate regarding room status
Status cur_status = good;

void buzzer_delegate()
{
    // configure pwm and set needed tracking variables
    config_pwm(BUZZER_PIN, 150, 0);
    Status last_status = good;
    bool counting_up = true;
    bool to_reset = true;
    uint16_t cur_duty = 0;
    uint16_t cur_freq = 50;
    absolute_time_t time_to_buzz = make_timeout_time_ms(TIME_TILL_BEEP_MS);

    while (true)
    {
        // capture status for this loop to avoid threading bugs
        Status now_status = cur_status;

        if (now_status != last_status)
        {
            // reset variables, we have changed modes, only reset timer in newly in a bad state
            counting_up = true;
            to_reset = true;
            cur_duty = 0;
            cur_freq = 50;
            if (last_status == good)
            {
                time_to_buzz = make_timeout_time_ms(TIME_TILL_BEEP_MS);
            }
        }

        // we only make noise after 2 minutes of being in a bad state
        if (now_status == bad_temp && absolute_time_diff_us(get_absolute_time(), time_to_buzz) < 0)
        {
            // reset pwm frequency on mode change
            if (to_reset)
            {
                set_freq_pwm(BUZZER_PIN, 150);
                to_reset = false;
            }

            // change pwm duty to make noise
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
            // reset pwm duty on mode change
            if (to_reset)
            {
                set_duty_pwm(BUZZER_PIN, 1000);
                to_reset = false;
            }

            // change pwm freq to make noise
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
            if (to_reset)
            {
                set_freq_pwm(BUZZER_PIN, 150);
                set_duty_pwm(BUZZER_PIN, 40000);
                to_reset = false;
            }
        }
        else
        {
            // all good, no noise
            set_duty_pwm(BUZZER_PIN, 0);
        }

        last_status = now_status;
    }
}

int main()
{
    // init printing to terminal and set
    stdio_init_all();
    std::cout << std::fixed << std::setprecision(1);

    // init serial for the dht and pwm for the motor
    init_dht(DHT_SDA, DHT_SCL);
    config_pwm(MOTOR_ENA_PIN, 500, 0);

    // set motor polarity (doesn't really matter what)
    gpio_init(MOTOR_IN1_PIN);
    gpio_init(MOTOR_IN2_PIN);
    gpio_set_dir(MOTOR_IN1_PIN, GPIO_OUT);
    gpio_set_dir(MOTOR_IN2_PIN, GPIO_OUT);
    gpio_put(MOTOR_IN1_PIN, 0);
    gpio_put(MOTOR_IN2_PIN, 1);

    // handle buzzer on other core as will need to constatntly change it
    // and don't want to interupt our polling of the sensor.
    Thread buzzer_thread(buzzer_delegate);

    while (true)
    {
        // reading the dht retruns an optional, so we must make sure its valid
        if (auto data = read_data_dht())
        {
            // check if temp and hum are in acceptable ranges for plant growing
            bool data_bad_temp = (*data).temp < 18 || (*data).temp > 24;
            bool data_bad_hum = (*data).hum < 75 || (*data).hum > 85;

            if (data_bad_temp && data_bad_hum)
            {
                // make motor spin fast and update status to show both temp and hum out of range
                set_duty_pwm(MOTOR_ENA_PIN, 60000);
                cur_status = bad_both;
            }
            else if (data_bad_temp)
            {
                // make motor spin moderate and update status to show temp out of range
                set_duty_pwm(MOTOR_ENA_PIN, 40000);
                cur_status = bad_temp;
            }
            else if (data_bad_hum)
            {
                // make motor spin moderate and update status to show hum out of range
                set_duty_pwm(MOTOR_ENA_PIN, 40000);
                cur_status = bad_hum;
            }
            else
            {
                // all good
                set_duty_pwm(MOTOR_ENA_PIN, 0);
                cur_status = good;
            }

            std::cout << "AM2320 \t Temp: " << data->temp << " C \t Humidity: " << data->hum << "%" << std::endl;
        }
        else
        {
            // dht returned a null optional, error occured
            std::cout << "Error while reading from AM2320" << std::endl;
        }

        sleep_ms(500);
    }

    return 0;
}