#include <Arduino.h>
// Arduino Project

int motor1A = 13;
int motor2A = 14;
int enableA = 27;

const int freq = 20000;
const int resolution = 8;


void stop(int rtime) {
    digitalWrite(motor1A, LOW);
    digitalWrite(motor2A, LOW);
    ledcWrite(enableA, 0);
    delay(rtime);
}

void Forward(int spd, int rtime){
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor2A, LOW);
    ledcWrite(enableA, spd);
    delay(rtime);
}
    

void reverse(int spd, int rtime){
    digitalWrite(motor1A, LOW);
    digitalWrite(motor2A, HIGH);
    ledcWrite(enableA, spd);
    delay(rtime);
}

void setup() {
    Serial.begin(115200);
    pinMode(motor1A, OUTPUT);
    pinMode(motor2A, OUTPUT);

    ledcAttach(enableA, freq, resolution);
    ledcWrite(enableA, 0);
}

void loop() {
    Forward(255, 2000);
    stop(100);
    reverse(255, 2000);
    stop(1000);
    Forward(255, 500);
    stop(100);
    reverse(255, 500);
    stop(100);
    Forward(255, 500);
    stop(100);
    reverse(255, 500);
    for (int spd = 2000; spd <= 255; spd += 5) {
        Serial.println(spd);
        Forward(spd, 500);
    }
}