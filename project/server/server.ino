#include <aWOT.h>

#include "StaticFiles.h"
#include "arduino_secrets.h"
#include <A4988.h>
#include <Adafruit_AS726x.h>
#include <Arduino_JSON.h>
#include <Servo.h>
#include <WiFiNINA.h>

WiFiServer server(80);
Application app;

// servo stuff
#define SERVO_PIN 3
Servo servo;

// stepper stuff
// using a 200-step motor
#define MOTOR_STEPS 200
#define RPM 60
// configure the pins connected
#define DIR 8
#define STEP 9
#define MS1 10
#define MS2 11
#define MS3 12
A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3);

// color sensor stuff
#define SENSOR_MAX 5000
Adafruit_AS726x ams;
float calibratedValues[AS726x_NUM_CHANNELS];

// timing for our measurements
#define TIME_BETWEEN_TESTS 3600000
long last_test_time;

enum WaterState
{
    Safe,
    Contaminated,
    Unknown
};
WaterState curState = Unknown;

void takeMeasurement()
{
}

void getWaterState(Request &req, Response &res)
{
    JSONVar data;
    switch (curState)
    {
    case Safe:
        data["waterState"] = "safe";
        break;
    case Contaminated:
        data["waterState"] = "contaminated";
        break;
    default:
        data["waterState"] = "unknown";
    }

    res.set("Connection", "close");
    res.set("Transfer-Encoding", "chunked");
    res.set("Content-Type", "application/json");
    res.beginHeaders();
    res.endHeaders();

    res.println(JSON.stringify(data));
}

void setup()
{
    Serial.begin(115200);

    while (!Serial)
        ;

    servo.attach(SERVO_PIN);
    stepper.begin(RPM, 1);

    // begin and make sure we can talk to the color sensor
    if (!ams.begin())
    {
        Serial.println("Could not connect to color sensor! Please check your wiring.");
        while (1)
            ;
    }
    ams.setConversionType(MODE_2);
    ams.drvOn();

    WiFi.beginEnterprise(SECRET_SSID, SECRET_USER, SECRET_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.print("http://");
    Serial.println(WiFi.localIP());

    app.get("/api", &getWaterState);
    app.use(staticFiles());

    server.begin();

    // take a measurement before we start
    takeMeasurement();
    last_test_time = millis();
}

void loop()
{
    WiFiClient client = server.available();

    if (client.connected())
    {
        app.process(&client);
    }

    if (millis() - last_test_time > TIME_BETWEEN_TESTS)
    {
        takeMeasurement();
        last_test_time = millis();
    }
}