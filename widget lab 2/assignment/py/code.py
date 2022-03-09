import adafruit_am2320
import bitbangio
import board
import digitalio
import pwmio
import time


# useful constants
MOTOR_IN1_PIN = board.GP13
MOTOR_IN2_PIN = board.GP14
MOTOR_ENA_PIN = board.GP15
DHT_SDA = board.GP16
DHT_SCL = board.GP17
BUZZER_PIN = board.GP22
TIME_TILL_BEEP_S = 20

# status variables for buzzer handler
cur_status, last_status = "good", "good"
counting_up = True
to_reset = True
cur_duty = 0
cur_freq = 50
time_to_buzz = time.monotonic() + TIME_TILL_BEEP_S
buzzer_wait_time = 0
motor_wait_time = 0
b_to = 0
b_on = True


def buzzer_handler(buzzer):
    global cur_status, last_status, counting_up, to_reset, cur_duty, cur_freq, time_to_buzz, buzzer_wait_time, b_to, b_on
    # buzzer handling code
    if cur_status != last_status:
        # reset all variables
        counting_up = True
        to_reset = True
        cur_duty = 0
        cur_freq = 50
        buzzer_wait_time = 0
        b_to = 0
        b_on = True
        # make timeout time for buzzer to buzz
        if (last_status == "good"):
            time_to_buzz = time.monotonic() + TIME_TILL_BEEP_S

    if cur_status == "bad_temp" and time.monotonic() > time_to_buzz:
        if to_reset:
            buzzer.frequency = 150
            to_reset = False
            # buzzer will buzz in on and off pattern, so make timeout
            b_to = time.monotonic() + 0.100

        # increase then decrease duty cycle of buzzer
        if b_to > time.monotonic() and b_on:
            buzzer.duty_cycle = cur_duty
        elif b_to <= time.monotonic() and b_on:
            b_on = False
            b_to = time.monotonic() + 0.010
        elif b_to > time.monotonic() and not b_on:
            buzzer.duty_cycle = 0
        elif b_to <= time.monotonic() and not b_on:
            b_on = True
            b_to = time.monotonic() + 0.100
            if counting_up:
                cur_duty += 1000
                counting_up = cur_duty < 40000
            else:
                cur_duty -= 1000
                counting_up = not (cur_duty > 0)

    elif cur_status == "bad_hum" and time.monotonic() > time_to_buzz:
        if to_reset:
            buzzer.duty_cycle = 1000
            to_reset = False
            b_to = time.monotonic() + 0.100

        # increase then decrease frequency of buzzer
        buzzer.frequency = cur_freq

        if b_to <= time.monotonic():
            b_to = time.monotonic() + 0.100
            if counting_up:
                cur_freq += 50
                counting_up = cur_freq < 700
            else:
                cur_freq -= 50
                counting_up = not (cur_freq > 50)

    elif cur_status == "bad_both" and time.monotonic() > time_to_buzz:
        # solid beeping noise at loud volume and standard frequency
        if to_reset:
            buzzer.frequency = 150
            buzzer.duty_cycle = 40000
            to_reset = False
    else:
        # all good, no noise
        buzzer.duty_cycle = 0

    last_status = cur_status


# set up dht
i2c = bitbangio.I2C(DHT_SCL, DHT_SDA)
dht = adafruit_am2320.AM2320(i2c)

# set up motor
motor = pwmio.PWMOut(MOTOR_ENA_PIN, frequency=500, duty_cycle=0)
# set polarity
m_in1 = digitalio.DigitalInOut(MOTOR_IN1_PIN)
m_in1.direction = digitalio.Direction.OUTPUT
m_in1.value = False
m_in2 = digitalio.DigitalInOut(MOTOR_IN2_PIN)
m_in2.direction = digitalio.Direction.OUTPUT
m_in2.value = True

# set up buzzer
buzzer = pwmio.PWMOut(BUZZER_PIN, frequency=150, duty_cycle=0)

# timeout to read from sensor
dht_to = time.monotonic() + 0.500

# main loop
while True:
    # read from dht sensor every 500 ms
    if dht_to <= time.monotonic():
        dht_to = time.monotonic() + 0.500
        try:
            temp, hum = dht.temperature, dht.relative_humidity
        except:
            print("Error while reading from AM2320")
            continue
        print(f"AM2320 \t Temp: {temp} C \t Humidity: {hum}%")

    # define acceptable temperature and humidity ranges
    bad_temp = temp < 18 or temp > 24
    bad_hum = hum < 75 or hum > 85

    if bad_temp and bad_hum:
        # make motor spin fast and update status to show both temp and hum out of range
        motor.duty_cycle = 60000
        cur_status = "bad_both"
    elif bad_temp:
        # make motor spin moderate and update status to show temp out of range
        motor.duty_cycle = 40000
        cur_status = "bad_temp"

    elif bad_hum:
        # make motor spin moderate and update status to show hum out of range
        motor.duty_cycle = 40000
        cur_status = "bad_hum"
    else:
        # all good
        motor.duty_cycle = 0
        cur_status = "good"

    # run buzzer update code
    buzzer_handler(buzzer)
