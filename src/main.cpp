// https://docs.sunfounder.com/projects/esp32-starter-kit/en/latest/arduino/basic_projects/ar_irremote.html

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ESP32Servo.h>


const uint16_t IR_RECEIVE_PIN = 19;
const int servoPin = 25;
int motor1A = 13;
int motor2A = 26;
int enableA = 27;

const int freq = 20000;
const int resolution = 8;
const int minSpeed = 200; // Minimum speed to ensure the motor starts moving, adjust as needed
const int maxLeft = 117;
const int maxRight = 150;
const int center = 133;
unsigned long repeatInterval = 10; // Time in milliseconds to consider a repeat command as active.
int turnStep = 1; // Step size for turning the servo
int speedStep = 1; // Step size for adjusting speed
int currentAngle = 133; // Start at center position

// TO DO: Create an enum for KeyCodes to improve readability.
enum KeyCodes : uint32_t {
    FORWARD = 0xFF629D,
    REVERSE = 0xFFA857,
    LEFT = 0xFF22DD,
    RIGHT = 0xFFC23D,
    STOP = 0xFF02FD,
    REPEAT = 0xFFFFFFFF
};

Servo servo1;
IRrecv irrecv(IR_RECEIVE_PIN);

// Create a decode_results object
decode_results results;


// Function to decode the value of the IR signal
void handleCommand(uint64_t result) {
    uint32_t val = (uint32_t)result;
    static uint32_t lastValidKey = 0;
    static unsigned long lastExecutionTime = 0;
    static int speed = 0;


    if (val == REPEAT) {
            val = lastValidKey;
        } else {
            lastValidKey = val;
        }

    if (millis() - lastExecutionTime < 50) return;
        lastExecutionTime = millis();

    switch(val) {
        case FORWARD:
            if (lastValidKey == REVERSE) speed = minSpeed; // Direction change
            if (speed == 255) break;
            if (speed < 255) speed += speedStep;
            if (speed > 255) speed = 255; // Cap the speed at 255
            if (speed < minSpeed) speed = minSpeed; // Ensure speed is above minimum threshold

            digitalWrite(motor1A, HIGH);
            digitalWrite(motor2A, LOW);
            ledcWrite(enableA, speed);
            Serial.print("FORWARD: ");
            Serial.println(speed);
            break;
        case REVERSE:
            if (lastValidKey == FORWARD) speed = minSpeed; // Direction change
            if (speed == 255) break;
            if (speed < 255) speed += speedStep;
            if (speed > 255) speed = 255; // Cap the speed at 255
            if (speed < minSpeed) speed = minSpeed; // Ensure speed is above minimum threshold

            digitalWrite(motor1A, LOW);
            digitalWrite(motor2A, HIGH);
            ledcWrite(enableA, speed);
            Serial.print("REVERSE: ");
            Serial.println(speed);
            break;
        case LEFT: 
            if (currentAngle == maxLeft) break;
            if (currentAngle < maxLeft) currentAngle = maxLeft;
            if (currentAngle > maxLeft) currentAngle -= turnStep;

            servo1.write(currentAngle);
            Serial.print("LEFT: ");
            Serial.println(currentAngle);
            break;
        case RIGHT:
            if (currentAngle == maxRight) break;
            if (currentAngle > maxRight) currentAngle = maxRight;
            if (currentAngle < maxRight) currentAngle += turnStep;
            servo1.write(currentAngle);
            Serial.print("RIGHT: ");
            Serial.println(currentAngle);
            break;
        case STOP:
            digitalWrite(motor1A, LOW);
            digitalWrite(motor2A, LOW);
            ledcWrite(enableA, 0);
            Serial.println("STOP");
            break;
        default:
            Serial.print("Unknown Code: 0x");
            Serial.println(val, HEX);
    }
}

void setup() {
    Serial.begin(115200);

    // CLEAR ALL TIMERS FIRST
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    // INITIALIZE SERVO FIRST (Force it to grab its resources before LEDC starts)
    servo1.setPeriodHertz(50); 
    // We attach it here. If this fails, nothing else will work.
    if (!servo1.attach(servoPin, 500, 2400)) {
        Serial.println("Servo attach failed!");
    }
    servo1.write(center);

    // INITIALIZE MOTORS WITH A SPECIFIC CHANNEL
    pinMode(motor1A, OUTPUT);
    pinMode(motor2A, OUTPUT);
    
    ledcAttach(enableA, freq, resolution);
    ledcWrite(enableA, 0);

    // INITIALIZE IR
    irrecv.enableIRIn();
    Serial.println("IR Receiver Initialized...");
}

void loop() {
    // If an IR signal is received
    if (irrecv.decode(&results)) {
        handleCommand(results.value);
        irrecv.resume(); // Continue to receive the next signal
    }
}

