/**
 * @file Spotlight.h
 * @brief Header file for the Spotlight control class.
 */

#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "ColorSpace.h"
#include "Constants.h"
#include "Easing.h"
#include <Arduino.h>

/**
 * @class Spotlight
 * @brief A class to control a tri-color LED spotlight with various modes.
 *
 * This class provides methods to set the color of an RGB LED spotlight
 * using different color spaces and animation modes, all in a non-blocking
 * manner using timer-based updates.
 */
class Spotlight {
public:
  /**
   * @brief Defines the direction of rotation for the color wheel.
   */
  enum class RotationDirection { Clockwise, CounterClockwise };

  /**
   * @brief Constructor for the Spotlight class.
   * @param redPin The GPIO pin connected to the red LED channel.
   * @param greenPin The GPIO pin connected to the green LED channel.
   * @param bluePin The GPIO pin connected to the blue LED channel.
   */
  Spotlight(int redPin, int greenPin, int bluePin);

  /**
   * @brief Initializes the LED pins as outputs.
   */
  void begin();

  /**
   * @brief Main update method for animations.
   *
   * This method should be called in the main `loop()` to handle all
   * time-based animations and transitions without using `delay()`.
   */
  void update();

  /**
   * @brief Sets a fixed RGB color with a simple fade transition.
   * @param r Red value (0-255).
   * @param g Green value (0-255).
   * @param b Blue value (0-255).
   */
  void setRGB(uint8_t r, uint8_t g, uint8_t b);

  /**
   * @brief Sets the color based on a Kelvin color temperature.
   * @param kelvin The color temperature in Kelvin (1500-10000).
   * @param brightness The brightness scalar (0.0-1.0).
   */
  void setColorTemperature(float kelvin, float brightness);

  /**
   * @brief Enables a continuous color wheel rotation mode.
   * @param periodSeconds The duration of one full rotation in seconds.
   * @param direction The direction of the rotation.
   */
  void enableColorWheelMode(float periodSeconds, RotationDirection direction);

  /**
   * @brief Enables a mode to cycle through a list of colors with blending.
   * @param colors An array of ColorSpace::RGB structs.
   * @param count The number of colors in the array. Max `maxColors` from
   * constructor.
   * @param isRandom If true, the order of colors is randomized.
   */
  void enableColorCycleMode(const ColorSpace::RGB *colors, size_t count,
                            bool isRandom);

  /**
   * @brief Sets the duration for each transition in color cycle mode.
   * @param duration The duration in seconds.
   */
  void setCycleDuration(float duration);

  /**
   * @brief Sets the easing function for each transition in color cycle mode.
   * @param easing The easing function to use.
   */
  void setCycleEasing(Easing::EasingFunction easing);

  /**
   * @brief Sets the duration for the smooth transition to a new fixed color.
   * @param duration The duration in seconds.
   */
  void setTransitionDuration(float duration);

  /**
   * @brief Sets the easing function for the smooth transition to a new fixed
   * color.
   * @param easing The easing function to use.
   */
  void setTransitionEasing(Easing::EasingFunction easing);

private:
  int _redPin, _greenPin, _bluePin;

  // Current color state variables.
  ColorSpace::RGB _currentRGB;

  // Animation state variables.
  unsigned long _animationStartTime;
  bool _isAnimating;

  // Color Wheel Mode variables.
  float _currentHue;
  float _startHue;
  float _saturation;
  float _value;
  float _rotationPeriod;
  RotationDirection _rotationDirection;

  // Color Cycle Mode variables.
  ColorSpace::LCH _colorCycleList[Constants::MAX_COLORS];
  size_t _colorCycleCount;
  size_t _currentColorIndex;
  ColorSpace::LCH _startLCH;
  float _transitionDuration;
  Easing::EasingFunction _currentEasing;
  bool _isRandom;

  // Smooth transition variables for fixed colors
  bool _isTransitioning;
  unsigned long _transitionStartTime;
  float _fixedTransitionDuration;
  Easing::EasingFunction _fixedTransitionEasing;
  ColorSpace::LCH _fixedStartLCH;
  ColorSpace::LCH _fixedEndLCH;

  // Private helper methods.
  void stopAllAnimations();
  void writeLeds(const ColorSpace::RGB &color);
  ColorSpace::RGB getCurrentRGB();
};

#endif
