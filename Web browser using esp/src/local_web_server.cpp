#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);   // Create a web server on port 80 192.168.4.1:80

const int sensorPin = 36; // Analog pin for potentiometer
int rawData = 0;
static unsigned long previousMillis = 0;

// HTML page
String htmlPage() {
  String page = "<!DOCTYPE html><html><head>";
  page += "<meta http-equiv='refresh' content='1'>"; // auto-refresh every 1s
  page += "<title>ESP32 Potentiometer</title></head><body>";
  page += "<h2>ESP32 Live Potentiometer Value</h2>";
  page += "<p>Value: " + String(rawData) + "</p>";
  page += "</body></html>";
  return page;
}

void handleRoot() {
  server.send(200, "text/html", htmlPage());    // Send web page
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  //pinMode(sensorPin, INPUT);

  // Start ESP32 in Access Point mode
  const char* apSSID = "ESP32";
  const char* apPassword = "12345678";  // at least 8 characters

  WiFi.softAP(apSSID, apPassword);

  Serial.println("Access Point started");
  Serial.print("SSID: ");
  Serial.println(apSSID);
  Serial.print("Password: ");
  Serial.println(apPassword);

  // Print local IP (default 192.168.4.1)
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Define server routes
  server.on("/", handleRoot);

  //fix: handle any unknown requests
    server.onNotFound([]() {
        //either this
        //server.send(404, "text/plain", "404: Not Found"); 
        //OR this
        handleRoot();  // always serve main page
    });

  // Start server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
  rawData = analogRead(sensorPin);
  unsigned long currentMillis = millis();
  //read and print the sensor value every second
    if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;
        Serial.println(rawData);
    }
}
