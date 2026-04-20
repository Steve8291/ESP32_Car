#include <Arduino.h>
#include <ESP32Servo.h>

static const int servoPin = 14;

Servo servo1;

void setup() {

  Serial.begin(115200);
  servo1.attach(servoPin);
}

void loop() {
  servo1.write(117);
  delay(500);
  servo1.write(133);
  delay(500);
  servo1.write(150);
  delay(500);
}

//L 117
//C 133
//R 150