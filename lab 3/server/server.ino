#include "StaticFiles.h"
#include "arduino_secrets.h"
#include <Arduino_JSON.h>
#include <Arduino_LSM6DSOX.h>
#include <WiFiNINA.h>
#include <aWOT.h>

#define M1ENA 2
#define M1IN1 3
#define M1IN2 4
#define M2IN3 5
#define M2IN4 6
#define M2ENA 9

WiFiServer server(80);
Application app;

String curDir = "none";
double curSpd = 100.0;

void handleStateChange(String dir, double spd)
{
    if (dir == "N")
    {
        digitalWrite(M1IN1, LOW);
        digitalWrite(M1IN2, HIGH);
        digitalWrite(M2IN3, LOW);
        digitalWrite(M2IN4, HIGH);
        analogWrite(M1ENA, 255 * spd);
        analogWrite(M2ENA, 255 * spd);
    }
    else if (dir == "E")
    {
        digitalWrite(M1IN1, LOW);
        digitalWrite(M1IN2, HIGH);
        digitalWrite(M2IN3, HIGH);
        digitalWrite(M2IN4, LOW);
        analogWrite(M1ENA, 255 * spd);
        analogWrite(M2ENA, 255 * spd);
    }
    else if (dir == "S")
    {
        digitalWrite(M1IN1, HIGH);
        digitalWrite(M1IN2, LOW);
        digitalWrite(M2IN3, HIGH);
        digitalWrite(M2IN4, LOW);
        analogWrite(M1ENA, 255 * spd);
        analogWrite(M2ENA, 255 * spd);
    }
    else if (dir == "W")
    {
        digitalWrite(M1IN1, HIGH);
        digitalWrite(M1IN2, LOW);
        digitalWrite(M2IN3, LOW);
        digitalWrite(M2IN4, HIGH);
        analogWrite(M1ENA, 255 * spd);
        analogWrite(M2ENA, 255 * spd);
    }
    else
    {
        analogWrite(M1ENA, 0);
        analogWrite(M2ENA, 0);
    }
}

void getState(Request &req, Response &res)
{
    JSONVar data;
    data["curDir"] = curDir;
    data["curSpd"] = curSpd;

    res.set("Connection", "close");
    res.set("Transfer-Encoding", "chunked");
    res.set("Content-Type", "application/json");
    res.beginHeaders();
    res.endHeaders();

    res.println(JSON.stringify(data));
}

void setState(Request &req, Response &res)
{
    JSONVar data = JSON.parse(req.readString());

    if (JSON.typeof(data) == "undefined")
    {
        res.sendStatus(400);
        return;
    }

    if (data.hasOwnProperty("curDir"))
    {
        String tmp = (const char *)data["curDir"];
        if (tmp == "none" || tmp == "N" || tmp == "E" || tmp == "S" || tmp == "W")
        {
            curDir = tmp;
        }
    }

    if (data.hasOwnProperty("curSpd"))
    {
        double tmp = (double)data["curSpd"];
        if (tmp >= 0 && tmp <= 100)
        {
            curSpd = tmp;
        }
    }

    handleStateChange(curDir, curSpd);

    return getState(req, res);
}

void getData(Request &req, Response &res)
{
    JSONVar data;

    JSONVar gyroData;
    float gx, gy, gz;

    if (IMU.gyroscopeAvailable())
    {
        IMU.readGyroscope(gx, gy, gz);
    }
    gyroData["x"] = gx;
    gyroData["y"] = gy;
    gyroData["z"] = gz;

    JSONVar accelData;
    float ax = 0, ay = 0, az = 0;

    if (IMU.accelerationAvailable())
    {
        IMU.readAcceleration(ax, ay, az);
    }
    accelData["x"] = ax;
    accelData["y"] = ay;
    accelData["z"] = az;

    data["accel"] = accelData;
    data["gyro"] = gyroData;

    res.set("Connection", "close");
    res.set("Transfer-Encoding", "chunked");
    res.set("Content-Type", "application/json");
    res.beginHeaders();
    res.endHeaders();

    res.println(JSON.stringify(data));
}

void setup()
{
    pinMode(M1ENA, OUTPUT);
    pinMode(M1IN1, OUTPUT);
    pinMode(M1IN2, OUTPUT);
    pinMode(M2IN3, OUTPUT);
    pinMode(M2IN4, OUTPUT);
    pinMode(M2ENA, OUTPUT);

    Serial.begin(115200);

    while (!Serial)
        ;

    if (!IMU.begin())
    {
        Serial.println("Failed to initialize IMU!");
        while (1)
            ;
    }

    WiFi.beginEnterprise(SECRET_SSID, SECRET_USER, SECRET_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.print("http://");
    Serial.println(WiFi.localIP());

    app.get("/api", &getState);
    app.put("/api", &setState);
    app.get("/data", &getData);
    app.use(staticFiles());

    server.begin();
}

void loop()
{
    WiFiClient client = server.available();

    if (client.connected())
    {
        app.process(&client);
    }
}