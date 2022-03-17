#include "arduino_secrets.h"
#include <WiFiNINA.h>

WiFiServer server(80);

enum Dir
{
    none,
    N,
    E,
    S,
    W
};
Dir dir = none;

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    Serial.println("Starting web server");

    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE)
    {
        Serial.println("Communication with WiFi module failed!");

        while (true)
            ;
    }

    int status = WiFi.beginEnterprise(SECRET_SSID, SECRET_USER, SECRET_PASS);
    if (status != WL_CONNECTED)
    {
        Serial.println("Creating access point failed");
        while (true)
            ;
    }

    server.begin();
    printWiFiStatus();
}

void loop()
{
    WiFiClient client = server.available();

    if (client)
    {
        Serial.println("New client");
        String currentLine = "";
        while (client.connected())
        {
            delayMicroseconds(10);
            if (client.available())
            {
                char c = client.read();
                Serial.write(c);
                if (c == '\n')
                {
                    // got 2 \n in a row
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.print("<meta http-equiv=\"refresh\" content=\"1\" />");

                        client.print("<h1>Microcontroller Control Portal</h1>");
                        client.print("<h2>Control Directions:</h2>");
                        client.print("<button id=\"N\" class=");
                        client.print((dir == N) ? "selected" : "none");
                        client.print(" type=\"button\" onclick=\"set_dir(this.id)\">North</button>");
                        client.print("<button id=\"E\" class=");
                        client.print((dir == E) ? "selected" : "none");
                        client.print(" type=\"button\" onclick=\"set_dir(this.id)\">East</button>");
                        client.print("<button id=\"S\" class=");
                        client.print((dir == S) ? "selected" : "none");
                        client.print(" type=\"button\" onclick=\"set_dir(this.id)\">South</button>");
                        client.print("<button id=\"W\" class=");
                        client.print((dir == W) ? "selected" : "none");
                        client.print(" type=\"button\" onclick=\"set_dir(this.id)\">West</button>");

                        client.print("<h2>Sensor Data:</h2>");

                        client.print("<style> .selected { background-color: LightGreen; } </style>");

                        client.print("<script>");
                        String lit_id;
                        switch (dir)
                        {
                        case N:
                            lit_id = "N";
                            break;
                        case E:
                            lit_id = "E";
                            break;
                        case S:
                            lit_id = "S";
                            break;
                        case W:
                            lit_id = "W";
                            break;
                        case none:
                            lit_id = "None";
                        }
                        client.print("var lit_id = \"");
                        client.print(lit_id);
                        client.print("\";");
                        client.print("function set_dir(clicked_id) {");
                        client.print(
                            "window.location.replace(\"/\" + ((lit_id === clicked_id) ? \"\" : clicked_id)); }");
                        client.print("</script>");

                        // HTTP response ends with another blank line:
                        client.println();
                        break;
                    }
                    else
                    {
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {
                    currentLine += c;
                }

                if (currentLine.endsWith("GET /N"))
                {
                    dir = N;
                }
                else if (currentLine.endsWith("GET /E"))
                {
                    dir = E;
                }
                else if (currentLine.endsWith("GET /S"))
                {
                    dir = S;
                }
                else if (currentLine.endsWith("GET /W"))
                {
                    dir = W;
                }
                else if (currentLine.endsWith("GET /"))
                {
                    dir = none;
                }
            }
        }
        client.stop();
        Serial.println("Client disconnected");
    }
}

void printWiFiStatus()
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print where to go in a browser:
    Serial.print("To see this page in action, open a browser to http://");
    Serial.println(ip);
}