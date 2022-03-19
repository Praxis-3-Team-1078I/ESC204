#include "StaticFiles.h"
#include "arduino_secrets.h"
#include <Arduino_JSON.h>
#include <WiFiNINA.h>
#include <aWOT.h>

WiFiServer server(80);
Application app;

String curDir = "none";

void getState(Request &req, Response &res)
{
    JSONVar data;
    data["curDir"] = curDir;

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
        curDir = (const char *)data["curDir"];
    }

    return getState(req, res);
}

void setup()
{
    Serial.begin(115200);

    while (!Serial)
        ;

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