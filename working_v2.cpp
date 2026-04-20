// https://docs.sunfounder.com/projects/esp32-starter-kit/en/latest/arduino/basic_projects/ar_irremote.html

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ESP32Servo.h>

static const int servoPin = 14;

Servo servo1;

int motor1A = 13;
int motor2A = 14;
int enableA = 27;

const int freq = 20000;
const int resolution = 8;

// Define the IR receiver pin (GPIO 14)
const uint16_t IR_RECEIVE_PIN = 19;

// Create an IRrecv object
IRrecv irrecv(IR_RECEIVE_PIN);

// Create a decode_results object
decode_results results;

// Function prototype (required in PlatformIO/C++)
String decodeKeyValue(uint64_t result);

void stop() {
    digitalWrite(motor1A, LOW);
    digitalWrite(motor2A, LOW);
    ledcWrite(enableA, 0);
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
}

void loop() {
  // If an IR signal is received
  if (irrecv.decode(&results)) {
    // result.value is a uint64_t in this library
    String key = decodeKeyValue(results.value);
    
    if (key != "ERROR") {
      Serial.print("Received: ");
      Serial.println(key);
    } else {
      // Optional: Print unknown codes to help you map them
      Serial.print("Unknown Code: 0x");
      Serial.println((uint32_t)results.value, HEX);
    }
    
    irrecv.resume(); // Continue to receive the next signal
  }
}

// Function to decode the value of the IR signal
String decodeKeyValue(uint64_t result) {
  // Note: Standard NEC codes are usually 32-bit. 
  // We cast to uint32_t for the switch case to match your hex codes.
  uint32_t val = (uint32_t)result;

  switch(val) {
case 0xFFA25D: return "POWER";
    case 0xFF629D:
      Forward(255, 3000);
      return "VOL+";
    case 0xFF22DD: return "FAST BACK";
    case 0xFF02FD: 
      stop();
      return "PAUSE";
    case 0xFFC23D: return "FAST FORWARD";
    case 0xFFA857: 
      reverse(255, 3000);
      return "VOL-";
    default:       return "ERROR";
  }
}