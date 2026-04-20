// https://docs.sunfounder.com/projects/esp32-starter-kit/en/latest/arduino/basic_projects/ar_irremote.html

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// Define the IR receiver pin (GPIO 14)
const uint16_t IR_RECEIVE_PIN = 14;

// Create an IRrecv object
IRrecv irrecv(IR_RECEIVE_PIN);

// Create a decode_results object
decode_results results;

// Function prototype (required in PlatformIO/C++)
String decodeKeyValue(uint64_t result);

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Start the IR receiver
  irrecv.enableIRIn();
  Serial.println("IR Receiver Initialized...");
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
    case 0xFFE21D: return "FUNC/STOP";
    case 0xFF629D: return "VOL+";
    case 0xFF22DD: return "FAST BACK";
    case 0xFF02FD: return "PAUSE";
    case 0xFFC23D: return "FAST FORWARD";
    case 0xFFE01F: return "DOWN";
    case 0xFFA857: return "VOL-";
    case 0xFF906F: return "UP";
    case 0xFF9867: return "EQ";
    case 0xFFB04F: return "ST/REPT";
    case 0xFF6897: return "0";
    case 0xFF30CF: return "1"; 
    case 0xFF18E7: return "2"; 
    case 0xFF7A85: return "3"; 
    case 0xFF10EF: return "4"; 
    case 0xFF38C7: return "5"; 
    case 0xFF5AA5: return "6"; 
    case 0xFF42BD: return "7"; 
    case 0xFF4AB5: return "8"; 
    case 0xFF52AD: return "9"; 
    default:       return "ERROR";
  }
}