//This code only serial prints the analog data received using a potentiometer
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
}

void loop() {
    int sensorValue = analogRead(A0); // A0 is GPIO36
    Serial.println(sensorValue);
    delay(1000);
}
