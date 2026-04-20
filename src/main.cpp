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
const int maxLeft = 117;
const int maxRight = 150;
const int center = 133;
unsigned long turnSpeed = 1000; // Time in milliseconds to consider a turn command as active for repeat handling
int turnStep = 1; // Step size for turning the servo
int currentAngle = 133; // Start at center position


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

void handleTurn(uint32_t key) {
    if (key == 0xFF22DD) {
        turnLeft();
        Serial.println("REPEAT: TURN LEFT");
    } else if (key == 0xFFC23D) {
        turnRight();
        Serial.println("REPEAT: TURN RIGHT");
    }
}

// Function to decode the value of the IR signal
void decodeKeyValue(uint64_t result) {
    uint32_t val = (uint32_t)result;
    static uint32_t lastValidKey = 0;
    static unsigned long lastTurnTime = 0;

    if (val != 0xFFFFFFFF) {
        lastValidKey = val; // Store the last valid key
    }

    switch(val) {
        case 0xFF629D:
            forward(255, 3000);
            Serial.println("FORWARD");
            break;
        case 0xFFA857:
            reverse(255, 3000);
            Serial.println("REVERSE");
            break;
        case 0xFF22DD: 
            Serial.println("TURN LEFT");
            lastTurnTime = millis();
            break;
        case 0xFFC23D:
            Serial.println("TURN RIGHT");
            lastTurnTime = millis();
            break;
        case 0xFF02FD:
            stop();
            Serial.println("STOP");
            break;
        case 0xFFFFFFFF:
            if ((millis() - lastTurnTime) > turnSpeed) {
                handleTurn(lastValidKey);
                lastTurnTime = millis();
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
        decodeKeyValue(results.value);
        irrecv.resume(); // Continue to receive the next signal
    }
}

