/**
 * @file Spotlight.cpp
 * @brief Implementation file for the Spotlight control class.
 */

#include "Spotlight.h"
#include "Constants.h"
#include <algorithm>
#include <cmath>

// Constructor
Spotlight::Spotlight(int redPin, int greenPin, int bluePin)
    : _redPin(redPin), _greenPin(greenPin), _bluePin(bluePin),
      _isAnimating(false), _currentHue(0.0), _startHue(0.0), _saturation(1.0),
      _value(1.0), _rotationPeriod(0.0),
      _rotationDirection(RotationDirection::Clockwise), _colorCycleCount(0),
      _currentColorIndex(0), _transitionDuration(2.0),
      _currentEasing(Easing::Linear), _isRandom(false), _isTransitioning(false),
      _fixedTransitionDuration(0.2),
      _fixedTransitionEasing(Easing::EasingFunction::CubicInOut) {}

// Initializes the pins.
void Spotlight::begin() {
  pinMode(_redPin, OUTPUT);
  pinMode(_greenPin, OUTPUT);
  pinMode(_bluePin, OUTPUT);

  Serial.begin(115200);
}

// Main update method.
void Spotlight::update() {
  // --- Smooth Transition for Fixed Colors ---
  if (_isTransitioning) {
    unsigned long now = millis();
    unsigned long elapsedTime = now - _transitionStartTime;
    float t =
        static_cast<float>(elapsedTime) / (_fixedTransitionDuration * 1000.0);
    float easedT = Easing::getEasedValue(_fixedTransitionEasing, t);

    if (t >= 1.0) {
      // Transition is complete, jump to the final color and stop.
      writeLeds(ColorSpace::lchToRgb(_fixedEndLCH));
      _isTransitioning = false;
    } else {
      // Blending is still in progress.
      ColorSpace::LCH interpolatedLCH =
          _fixedStartLCH + (_fixedEndLCH - _fixedStartLCH) * easedT;
      writeLeds(ColorSpace::lchToRgb(interpolatedLCH));
    }
    return;
  }

  if (!_isAnimating) {
    return;
  }

  unsigned long now = millis();
  unsigned long elapsedTime = now - _animationStartTime;

  // --- Color Wheel Mode ---
  if (_rotationPeriod > 0) {
    float hueDelta = (float)elapsedTime / (_rotationPeriod * 1000.0) * 360.0;
    if (_rotationDirection == RotationDirection::CounterClockwise) {
      hueDelta *= -1.0;
    }

    _currentHue = fmod(_startHue + hueDelta, 360.0);
    if (_currentHue < 0) {
      _currentHue += 360.0;
    }

    writeLeds(ColorSpace::hsvToRgb(_currentHue, _saturation, _value));
  }

  // --- Color Cycle Mode ---
  else if (_colorCycleCount > 0) {
    if (elapsedTime >= _transitionDuration * 1000.0) {
      // Transition is complete, move to the next color.
      _startLCH = _colorCycleList[_currentColorIndex];

      // Move to the next index, handling randomness if enabled.
      if (_colorCycleCount > 1 && _isRandom) {
        size_t newIndex;
        do {
          newIndex = random(0, _colorCycleCount);
        } while (newIndex == _currentColorIndex);
        _currentColorIndex = newIndex;
      } else {
        _currentColorIndex = (_currentColorIndex + 1) % _colorCycleCount;
      }

      _animationStartTime = now;
    } else {
      // Blending is still in progress.
      float t = (float)elapsedTime / (_transitionDuration * 1000.0);
      float easedT = Easing::getEasedValue(_currentEasing, t);

      ColorSpace::LCH start = _startLCH;
      ColorSpace::LCH end = _colorCycleList[_currentColorIndex];

      // Use the new overloaded operators for simplified interpolation.
      ColorSpace::LCH interpolatedLCH = start + (end - start) * easedT;

      writeLeds(ColorSpace::lchToRgb(interpolatedLCH));
    }
  }
}

// Writes the given RGB color to the LED pins.
void Spotlight::writeLeds(const ColorSpace::RGB &color) {
  _currentRGB = color;
  analogWrite(_redPin, color.r);
  analogWrite(_greenPin, color.g);
  analogWrite(_bluePin, color.b);
}

// Stops all running animations/modes.
void Spotlight::stopAllAnimations() {
  _isAnimating = false;
  _isTransitioning = false;
  _rotationPeriod = 0.0;
  _colorCycleCount = 0;
}

// Gets the current color, regardless of the active mode.
ColorSpace::RGB Spotlight::getCurrentRGB() {
  if (_isTransitioning) {
    unsigned long now = millis();
    unsigned long elapsedTime = now - _transitionStartTime;
    float t =
        static_cast<float>(elapsedTime) / (_fixedTransitionDuration * 1000.0);
    t = std::min(1.0f, t); // Clamp to prevent overshooting
    float easedT = Easing::getEasedValue(_fixedTransitionEasing, t);

    ColorSpace::LCH interpolatedLCH =
        _fixedStartLCH + (_fixedEndLCH - _fixedStartLCH) * easedT;
    return ColorSpace::lchToRgb(interpolatedLCH);
  } else if (_rotationPeriod > 0) {
    return ColorSpace::hsvToRgb(_currentHue, _saturation, _value);
  } else if (_colorCycleCount > 0) {
    unsigned long now = millis();
    unsigned long elapsedTime = now - _animationStartTime;
    float t = static_cast<float>(elapsedTime) / (_transitionDuration * 1000.0);
    t = std::min(1.0f, t);
    float easedT = Easing::getEasedValue(_currentEasing, t);

    ColorSpace::LCH start = _startLCH;
    ColorSpace::LCH end = _colorCycleList[_currentColorIndex];
    ColorSpace::LCH interpolatedLCH = start + (end - start) * easedT;

    return ColorSpace::lchToRgb(interpolatedLCH);
  } else {
    return _currentRGB;
  }
}

// Sets a fixed RGB color with a smooth transition.
void Spotlight::setRGB(uint8_t r, uint8_t g, uint8_t b) {
  ColorSpace::RGB startColor = getCurrentRGB();

  stopAllAnimations(); // This will also stop the current transition.

  _fixedStartLCH = ColorSpace::rgbToLch(startColor);
  _fixedEndLCH = ColorSpace::rgbToLch({r, g, b});

  _isTransitioning = true;
  _transitionStartTime = millis();
}

// Sets color based on Kelvin temperature.
void Spotlight::setColorTemperature(float kelvin, float brightness) {
  stopAllAnimations();
  ColorSpace::RGB rgb = ColorSpace::kelvinToRgb(kelvin);

  // Apply brightness scalar.
  rgb.r = static_cast<uint8_t>(rgb.r * brightness);
  rgb.g = static_cast<uint8_t>(rgb.g * brightness);
  rgb.b = static_cast<uint8_t>(rgb.b * brightness);

  _currentRGB = rgb;
  writeLeds(_currentRGB);
}

// Enables continuous color wheel mode.
void Spotlight::enableColorWheelMode(float periodSeconds,
                                     RotationDirection direction) {
  stopAllAnimations();

  ColorSpace::RGB startColor = getCurrentRGB();
  float h, s, v;
  ColorSpace::rgbToHsv(startColor, h, s, v);
  _startHue = h;
  _rotationPeriod = periodSeconds;
  _rotationDirection = direction;
  _isAnimating = true;
  _animationStartTime = millis();
}

// Enables color cycle mode.
void Spotlight::enableColorCycleMode(const ColorSpace::RGB *colors,
                                     size_t count, bool isRandom) {
  stopAllAnimations();

  // Populate fixed-size array and convert colors to LCH.
  _colorCycleCount = std::min(count, Constants::MAX_COLORS);
  _isRandom = isRandom;
  for (size_t i = 0; i < _colorCycleCount; ++i) {
    _colorCycleList[i] = ColorSpace::rgbToLch(colors[i]);
  }

  if (_colorCycleCount == 0) {
    return; // Nothing to animate.
  }

  // Manual shuffle using Arduino's random() function.
  if (_isRandom) {
    for (size_t i = 0; i < _colorCycleCount - 1; ++i) {
      size_t j = random(i, _colorCycleCount);
      if (i != j) {
        // Swap elements.
        ColorSpace::LCH temp = _colorCycleList[i];
        _colorCycleList[i] = _colorCycleList[j];
        _colorCycleList[j] = temp;
      }
    }
  }

  _currentColorIndex = 0;
  _startLCH = _colorCycleList[_currentColorIndex];

  _isAnimating = true;
  _animationStartTime = millis();
}

// Sets the duration for each color cycle transition.
void Spotlight::setCycleDuration(float duration) {
  _transitionDuration = duration;
}

// Sets the easing function for each color cycle transition.
void Spotlight::setCycleEasing(Easing::EasingFunction easing) {
  _currentEasing = easing;
}

void Spotlight::setTransitionDuration(float duration) {
  _fixedTransitionDuration = duration;
}

void Spotlight::setTransitionEasing(Easing::EasingFunction easing) {
  _fixedTransitionEasing = easing;
}
