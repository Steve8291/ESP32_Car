// https://docs.sunfounder.com/projects/esp32-starter-kit/en/latest/arduino/basic_projects/ar_irremote.html

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ESP32Servo.h>


const uint16_t IR_RECEIVE_PIN = 19;
const int servoPin = 14;
int motor1A = 13;
int motor2A = 14;
int enableA = 27;

const int freq = 20000;
const int resolution = 8;
const int minSpeed = 200; // Minimum speed to ensure the motor starts moving, adjust as needed
const int maxLeft = 117;
const int maxRight = 150;
const int center = 133;
unsigned long repeatInterval = 1000; // Time in milliseconds to consider a repeat command as active.
int turnStep = 1; // Step size for turning the servo
int speedStep = 1; // Step size for adjusting speed
int currentAngle = 133; // Start at center position

// TO DO: Create an enum for KeyCodes to improve readability.


Servo servo1;
IRrecv irrecv(IR_RECEIVE_PIN);

// Create a decode_results object
decode_results results;


void stop() {
    digitalWrite(motor1A, LOW);
    digitalWrite(motor2A, LOW);
    ledcWrite(enableA, 0);
}

// Need to determine the lowest speed value and the increment steps for the motor to achieve a smooth acceleration and deceleration.
void forward(int spd, int rtime){
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

void turnLeft() {
    if (currentAngle > maxLeft) {
        currentAngle -= turnStep;
        servo1.write(currentAngle);
        Serial.print("Turning Left: ");
        Serial.println(currentAngle);
    }
}


void turnRight() {
    // Finish this function to turn right, similar to turnLeft but in the opposite direction
}

void handleRepeat(uint32_t key) {
    switch (key) {
        case 0xFF629D:
            handleCommand(0xFF629D);
            Serial.println("REPEAT: FORWARD");
            break;
        case 0xFFA857:
            Serial.println("REPEAT: REVERSE");
            break;
        case 0xFF22DD:
            Serial.println("REPEAT: TURN LEFT");
            break;
        case 0xFFC23D:
            Serial.println("REPEAT: TURN RIGHT");
            break;
    }
}

// Function to decode the value of the IR signal
void handleCommand(uint64_t result) {
    uint32_t val = (uint32_t)result;
    static uint32_t lastValidKey = 0;
    static unsigned long lastRepeatTime = 0;
    static int speed = 0;

    if (val != 0xFFFFFFFF) {
        lastValidKey = val; // Store the last valid key
    }

    switch(val) {
        case 0xFF629D:
            if (speed == 255) break;
            if (speed < 255) speed += speedStep;
            if (speed > 255) speed = 255; // Cap the speed at 255
            if (speed < minSpeed) speed = minSpeed; // Ensure speed is above minimum threshold

            digitalWrite(motor1A, HIGH);
            digitalWrite(motor2A, LOW);
            ledcWrite(enableA, speed);
            Serial.println("FORWARD");
            break;
        case 0xFFA857:
            reverse(255, 3000);
            Serial.println("REVERSE");
            break;
        case 0xFF22DD: 
            if (currentAngle == maxLeft) break;
            if (currentAngle < maxLeft) currentAngle = maxLeft;
            if (currentAngle > maxLeft) currentAngle -= turnStep;

            servo1.write(currentAngle);
            Serial.println(currentAngle);
            Serial.println("TURN LEFT");
            lastRepeatTime = millis();
            break;
        case 0xFFC23D:
            Serial.println("TURN RIGHT");
            lastRepeatTime = millis();
            break;
        case 0xFF02FD:
            stop();
            Serial.println("STOP");
            break;
        case 0xFFFFFFFF:
            if ((millis() - lastRepeatTime) > repeatInterval) {
                handleRepeat(lastValidKey);
                lastRepeatTime = millis();
            }
            Serial.println("REPEAT");
            break;
        default:
            Serial.print("Unknown Code: 0x");
            Serial.println(val, HEX);
    }
}

void setup() {
    // Start serial communication
    Serial.begin(115200);

    // Start the IR receiver
    irrecv.enableIRIn();
    Serial.println("IR Receiver Initialized...");

    pinMode(motor1A, OUTPUT);
    pinMode(motor2A, OUTPUT);

    ledcAttach(enableA, freq, resolution);
    ledcWrite(enableA, 0);

    servo1.attach(servoPin);
    servo1.write(133); // Center position
}

void loop() {
    // If an IR signal is received
    if (irrecv.decode(&results)) {
        handleCommand(results.value);
        irrecv.resume(); // Continue to receive the next signal
    }
}

