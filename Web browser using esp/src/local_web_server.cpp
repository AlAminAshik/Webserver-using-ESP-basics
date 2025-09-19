//does not need a common wifi network
//browser can connect to the esp32 access point directly
//This code uses webSockets as it gives real-time, low-latency updates without polling or refreshing

#include <Arduino.h>
#include <WiFi.h>
// #include <WebServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Start ESP32 in Access Point mode
const char* apSSID = "ESP32";
const char* apPassword = "12345678";  // at least 8 characters

AsyncWebServer server(80);   // Create a web server on port 80 192.168.4.1:80
AsyncWebSocket ws("/ws"); // Create a WebSocket object

const int sensorPin = 36; // Analog pin for potentiometer
int rawData = 0;
static unsigned long previousMillis = 0;

// // HTML page that refreshes itself every second to show the potentiometer value
// String htmlPage() {
//   String page = "<!DOCTYPE html><html><head>";
//   page += "<meta http-equiv='refresh' content='1'>"; // auto-refresh every 1s
//   page += "<title>ESP32 Potentiometer</title></head><body>";
//   page += "<h2>ESP32 Live Potentiometer Value</h2>";
//   page += "<p>Value: " + String(rawData) + "</p>";
//   page += "</body></html>";
//   return page;
// }

// HTML page with WebSocket client
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Potentiometer Values</title>
  <script>
    var socket;
    function init() {
      socket = new WebSocket("ws://" + window.location.host + "/ws");
      socket.onmessage = function(event) {
        document.getElementById("pot").innerHTML = event.data + " ohms";
      };
    }
    window.onload = init;
  </script>
</head>
<body>
  <h2>ESP32 Live Potentiometer (WebSocket)</h2>
  <p>Potentiometer: <span id="pot">--</span></p>
</body>
</html>
)rawliteral";


//Serial print is a client (web browser is connected)
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client,
               AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Client %u connected\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Client %u disconnected\n", client->id());
  }
}
unsigned long lastSend = 0;


void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.softAP(apSSID, apPassword);

  Serial.println("Access Point started");
  Serial.print("SSID: ");
  Serial.println(apSSID);
  Serial.print("Password: ");
  Serial.println(apPassword);

  // Print local IP (default 192.168.4.1)
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.softAPIP());

  // WebSocket setup
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Serve HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Start server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  rawData = analogRead(sensorPin);
  unsigned long currentMillis = millis();
  //read and print the sensor value every second
    if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;
        Serial.println(rawData);
    }

    unsigned long now = millis();
    if (now - lastSend > 1000) {  // send every 1s
        ws.textAll(String(rawData, 10)); //notify all clients (decimal (base 10) format)
        lastSend = now;
    }
}
