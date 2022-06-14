#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char *ssid = "FTTH-1-2.4G-483580_EXT";
const char *password = "QyFxdyrD";

//---------------------------------------------------------------
// Our HTML webpage contents in program memory
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<head>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
</head>
<html>
<body>
<script>
                function set_delay(value) {
            $.ajax({
                url: '/set',
                type: "POST",
                data: { value: value, },
                success: function (data) {
                    document.getElementById("delay-value").innerHTML = value;
                }
            });
        }
            </script>
<center>
<input type="range" id="delay" min="0" max="5000" oninput="set_delay(this.value)" onchange="set_delay(this.value)" step="10">
<p id="delay-value">500</p>
<center> 
</body>
</html>
)=====";
//---------------------------------------------------------------
// On board LED Connected to GPIO2
#define LED 2

// SSID and Password of your WiFi router

// Declare a global object variable from the ESP8266WebServer class.
unsigned int led_delay = 500;
unsigned long previousMillis = 0;
int ledState = LOW;
ESP8266WebServer server(80); // Server on port 80

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot()
{
  Serial.println("You called root page");
  String s = MAIN_page;             // Read HTML contents
  server.send(200, "text/html", s); // Send web page
}

//==============================================================
//                  SETUP
//==============================================================
void setup(void)
{
  Serial.begin(115200);

  WiFi.begin(ssid, password); // Connect to your WiFi router
  Serial.println("");

  // Onboard LED port Direction output
  pinMode(LED, OUTPUT);
  // Power on LED state off
  digitalWrite(LED, HIGH);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // IP address assigned to your ESP

  server.on("/", handleRoot); // Which routine to handle at root location. This is display page
  server.on("/set", []()
            {
    String value = server.arg("value");
    led_delay = value.toInt();
    server.send(200, "text/plain", value); });

  server.begin(); // Start server
  Serial.println("HTTP server started");
}
//==============================================================
//                     LOOP
//==============================================================
void blink(int currentMillis)
{
  if (currentMillis - previousMillis >= led_delay)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
    {
      ledState = HIGH;
    }
    else
    {
      ledState = LOW;
    }
    digitalWrite(LED, ledState); // set the LED with the ledState of LED
  }
}

void loop(void)
{
  unsigned long currentMillis = millis();
  blink(currentMillis);
  server.handleClient(); // Handle client requests
}