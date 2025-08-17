/**
 * @file spot.cpp
 * @brief Main sketch for the ESP8266 Spotlight controller.
 *
 * This file sets up the Spotlight hardware and the web server,
 * then continuously runs their update loops.
 */

#include <Arduino.h>
#include "Spotlight.h"
#include "SpotlightServer.h"
#include "pins_arduino.h"

// Define the pins for your RGB LEDs.
// You must connect the anodes of the LEDs to VCC and the cathodes to the
// respective pins through a resistor.
const int RED_PIN = D5;
const int GREEN_PIN = D6;
const int BLUE_PIN = D7;

// Create instances of the Spotlight and SpotlightServer classes.
// The SpotlightServer is passed a reference to the Spotlight object
// so it can control the hardware.
Spotlight spotlight(RED_PIN, GREEN_PIN, BLUE_PIN);
SpotlightServer spotlightServer(&spotlight);

/**
 * @brief Arduino setup function.
 *
 * This function runs once when the ESP8266 boots up. It initializes
 * serial communication, the spotlight hardware, and the web server.
 */
void setup() {
  // Initialize Serial communication for debugging output.
  Serial.begin(115200);
  Serial.println("\nSpotlight Controller starting up...");

  // Initialize the spotlight's hardware pins.
  spotlight.begin();

  // Start the web server and connect to WiFi.
  // This function also initializes LittleFS and mDNS.
  spotlightServer.begin();

  Serial.println("Setup complete. Ready to serve clients.");
}

/**
 * @brief Arduino main loop function.
 *
 * This function runs continuously after setup completes. It calls the
 * update methods for both the server and the spotlight, ensuring both
 * are responsive and animations continue smoothly.
 */
void loop() {
  // Handle any incoming HTTP requests. This must be called frequently.
  spotlightServer.update();

  // Update the spotlight's animation state. This handles all
  // smooth color transitions and animations without using delay().
  spotlight.update();
}
