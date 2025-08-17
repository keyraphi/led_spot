/**
 * @file spot.cpp
 * @brief Main sketch for the ESP8266 Spotlight controller.
 *
 * This file demonstrates how to use the Spotlight class to control a tri-color
 * LED spotlight. It includes a simple state machine in the main loop to cycle
 * through the different modes (fixed color, color temperature, color wheel,
 * and color cycle mode).
 */

#include "Constants.h"
#include "Spotlight.h"

// Define the pins for your RGB LEDs.
// You must connect the anodes of the LEDs to VCC and the cathodes to the
// respective pins through a resistor.
const int RED_PIN = D5;   // Example GPIO pin for the Red LED
// const int GREEN_PIN = D6; // Example GPIO pin for the Green LED
const int GREEN_PIN = LED_BUILTIN;
const int BLUE_PIN = D7;  // Example GPIO pin for the Blue LED

// Create an instance of the Spotlight class.
Spotlight spotlight(RED_PIN, GREEN_PIN, BLUE_PIN);

// State machine variables for cycling through the modes.
enum Mode { RGB_MODE, KELVIN_MODE, COLOR_WHEEL_MODE, COLOR_CYCLE_MODE };

Mode currentMode = RGB_MODE;
unsigned long modeStartTime = 0;
const unsigned long modeDuration = 10000; // 10 seconds per mode

void setup() {
  Serial.begin(115200);
  Serial.println("\nSpotlight Controller Started!");

  // Initialize the spotlight and set the initial mode.
  spotlight.begin();
  spotlight.setRGB(255, 0, 0); // Start with a fixed red color
  modeStartTime = millis();
}

void loop() {
  // Always call the update() method to handle animations and transitions.
  spotlight.update();

  // Simple state machine to cycle through modes every 10 seconds.
  if (millis() - modeStartTime > modeDuration) {
    switch (currentMode) {
    case RGB_MODE: {
      Serial.println("Switching to Kelvin Mode...");
      spotlight.setColorTemperature(3000, 1.0); // Warm white
      currentMode = KELVIN_MODE;
      break;
    }

    case KELVIN_MODE: {
      Serial.println("Switching to Color Wheel Mode...");
      spotlight.enableColorWheelMode(
          15.0, Spotlight::RotationDirection::Clockwise); // 15-second rotation
      currentMode = COLOR_WHEEL_MODE;
      break;
    }

    case COLOR_WHEEL_MODE: {
      Serial.println("Switching to Color Cycle Mode...");
      // Define an array of colors using the ColorSpace::RGB struct.
      ColorSpace::RGB colors[] = {{255, 0, 0},   {255, 255, 0}, {0, 255, 0},
                                  {0, 255, 255}, {0, 0, 255},   {255, 0, 255}};
      // Set a duration and easing before starting the animation.
      spotlight.setCycleDuration(2.0);
      spotlight.setCycleEasing(Easing::EasingFunction::SineInOut);
      spotlight.enableColorCycleMode(colors, sizeof(colors) / sizeof(colors[0]),
                                     false); // Not random
      currentMode = COLOR_CYCLE_MODE;
      break;
    }
    case COLOR_CYCLE_MODE: {
      Serial.println("Switching back to RGB Mode...");
      spotlight.setRGB(0, 255, 0); // Fixed green
      currentMode = RGB_MODE;
      break;
    }
    }
    modeStartTime = millis();
  }
}
