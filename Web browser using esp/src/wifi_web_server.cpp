// //the esp32 and web browser must be on the same network

// #include <Arduino.h>
// #include <WiFi.h>
// #include <WebServer.h>  // Simple web server

// // Replace with your WiFi credentials
// const char* ssid = "Amin Residence";
// const char* password = "alamiralif86";

// WebServer server(80);   // Create a web server on port 80 192.168.4.1:80

// const int sensorPin = 36; // Analog pin for potentiometer
// int rawData = 0;
// static unsigned long previousMillis = 0;

// // HTML page (auto-refresh with JS)
// String htmlPage() {
//   String page = "<!DOCTYPE html><html><head>";
//   page += "<meta http-equiv='refresh' content='1'>"; // refresh every 1 sec
//   page += "<title>ESP32 Potentiometer</title></head><body>";
//   page += "<h2>ESP32 Live Potentiometer</h2>";
//   page += "<p>Value: " + String(rawData) + "</p>";
//   page += "</body></html>";
//   return page;
// }

// void handleRoot() {
//   server.send(200, "text/html", htmlPage());
// }

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   // Connect to WiFi
//   Serial.println("Connecting to WiFi...");
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.println("WiFi connected!");
//   Serial.print("ESP32 IP Address: ");
//   Serial.println(WiFi.localIP());

//   // Define server route
//   server.on("/", handleRoot);

//     //fix: handle any unknown requests
//     server.onNotFound([]() {
//         //either this
//         //server.send(404, "text/plain", "404: Not Found"); 
//         //OR this
//         handleRoot();  // always serve main page
//     });

//   // Start server
//   server.begin();
//   Serial.println("Web server started");
// }

// void loop() {
//   server.handleClient();
//     rawData = analogRead(sensorPin);
//   unsigned long currentMillis = millis();
//   //read and print the sensor value every second
//     if (currentMillis - previousMillis >= 1000) {
//         previousMillis = currentMillis;
//         Serial.println(rawData);
//     }
// }
