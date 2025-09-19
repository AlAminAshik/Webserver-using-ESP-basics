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

const int ledPin = 2; // GPIO2 is the built-in LED pin

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

// // HTML page with WebSocket client (only show potentiometer value)
// const char index_html[] PROGMEM = R"rawliteral(
// <!DOCTYPE html>
// <html>
// <head>
//   <title>ESP32 Potentiometer Values</title>
//   <script>
//     var socket;
//     function init() {
//       socket = new WebSocket("ws://" + window.location.host + "/ws");
//       socket.onmessage = function(event) {
//         document.getElementById("pot").innerHTML = event.data + " ohms";
//       };
//     }
//     window.onload = init;
//   </script>
// </head>
// <body>
//   <h2>ESP32 Live Potentiometer (WebSocket)</h2>
//   <p>Potentiometer: <span id="pot">--</span></p>
// </body>
// </html>
// )rawliteral";

// HTML + JS (to control led and view pot value from web browser)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Live Potentiometer (WebSocket)</title>
  <script>
    var socket;
    function init() {
      socket = new WebSocket("ws://" + window.location.host + "/ws");
      socket.onmessage = function(event) {
        document.getElementById("pot").innerHTML = event.data + " ohms";
      };
    }
    function ledOn() {
      socket.send("LED_ON");
    }
    function ledOff() {
      socket.send("LED_OFF");
    }
    window.onload = init;
  </script>
</head>
<body>
  <h2>ESP32 Live Potentiometer (WebSocket)</h2>
  <hr>
  <p>Potentiometer: <span id="pot">--</span></p>
  <button onclick="ledOn()">Turn LED ON</button>
  <button onclick="ledOff()">Turn LED OFF</button>
</body>
</html>
)rawliteral";


//on website event
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client,
               AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Client %u connected\n", client->id());   //serial print if a client (web browser) is connected
  } 
  else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Client %u disconnected\n", client->id());    //serial print if a client (web browser) is disconnected
  } 
  else if (type == WS_EVT_DATA) {     //when data is received from client
    //data is not null terminated
    String msg = "";
    for (size_t i = 0; i < len; i++) {
      msg += (char)data[i];
    }
    Serial.println("Received: " + msg); //print received message

    if (msg == "LED_ON") {
      digitalWrite(ledPin, HIGH);       //turn on board led on
    } else if (msg == "LED_OFF") {
      digitalWrite(ledPin, LOW);        //turn off board led off
    }
  }

}

unsigned long lastSend = 0;     //keep track of data being send to browser


void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(ledPin, OUTPUT); // Initialize the built-in LED pin as an output

  WiFi.softAP(apSSID, apPassword);  // Start the access point

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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){  //when root is requested
    request->send_P(200, "text/html", index_html);  //send webpage
  });

  // Start server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  ws.cleanupClients(); //clean up old clients by handling disconnection safely.

  rawData = analogRead(sensorPin);
  unsigned long currentMillis = millis();
  //read and print the sensor value every second
    if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;
        Serial.println(rawData);
    }

    unsigned long now = millis();
    if (now - lastSend > 500) {  // send every 500ms (sending data very fast can cause connection to close)
        //check if clients are connected before sending to avoid reset
        if(ws.count() > 0) ws.textAll(String(rawData, 10)); //notify all clients (decimal (base 10) format)
        lastSend = now;
    }
}
