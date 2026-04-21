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

void handleCommand(uint64_t result);  // FORWARD DECLARATION


void handleRepeat(uint32_t key) {
    switch (key) {
        case FORWARD:
            handleCommand(FORWARD);
            Serial.println("REPEAT: FORWARD");
            break;
        case REVERSE:
            Serial.println("REPEAT: REVERSE");
            break;
        case LEFT:
            Serial.println("REPEAT: TURN LEFT");
            break;
        case RIGHT:
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

    // if (val != REPEAT) {
    //     lastValidKey = val; // Store the last valid key
    // }

    switch(val) {
        case FORWARD:
            if (lastValidKey == REVERSE) speed = minSpeed; // Direction change
            if (speed == 255) break;
            if (speed < 255) speed += speedStep;
            if (speed > 255) speed = 255; // Cap the speed at 255
            if (speed < minSpeed) speed = minSpeed; // Ensure speed is above minimum threshold
            lastValidKey = val; // Store the last valid key

            digitalWrite(motor1A, HIGH);
            digitalWrite(motor2A, LOW);
            ledcWrite(enableA, speed);
            Serial.println("FORWARD");
            break;
        case REVERSE:
            if (lastValidKey == FORWARD) speed = minSpeed; // Direction change
            if (speed == 255) break;
            if (speed < 255) speed += speedStep;
            if (speed > 255) speed = 255; // Cap the speed at 255
            if (speed < minSpeed) speed = minSpeed; // Ensure speed is above minimum threshold
            lastValidKey = val; // Store the last valid key

            digitalWrite(motor1A, LOW);
            digitalWrite(motor2A, HIGH);
            ledcWrite(enableA, speed);
            Serial.println("REVERSE");
            break;
        case LEFT: 
            if (currentAngle == maxLeft) break;
            if (currentAngle < maxLeft) currentAngle = maxLeft;
            if (currentAngle > maxLeft) currentAngle -= turnStep;
            lastValidKey = val; // Store the last valid key

            servo1.write(currentAngle);
            Serial.println(currentAngle);
            Serial.println("TURN LEFT");
            lastRepeatTime = millis();
            break;
        case RIGHT:
            if (currentAngle == maxRight) break;
            if (currentAngle > maxRight) currentAngle = maxRight;
            if (currentAngle < maxRight) currentAngle += turnStep;
            lastValidKey = val; // Store the last valid key
            servo1.write(currentAngle);
            Serial.println(currentAngle);
            Serial.println("TURN RIGHT");
            lastRepeatTime = millis();
            break;
        case STOP:
            digitalWrite(motor1A, LOW);
            digitalWrite(motor2A, LOW);
            ledcWrite(enableA, 0);
            Serial.println("STOP");
            break;
        case REPEAT:
            if ((millis() - lastRepeatTime) > repeatInterval) {
                handleRepeat(lastValidKey);
                lastRepeatTime = millis();
            }
            Serial.println("REPEAT");
            break;
        default:
            Serial.print("Unknown Code: 0x");
            Serial.println(val, HEX);
            lastValidKey = val; // Store the last valid key
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

