/**
 * @file Easing.cpp
 * @brief Implementation file for easing functions.
 */

#include "Easing.h"
#include <cmath>

namespace Easing {

float getEasedValue(EasingFunction func, float t) {
  switch (func) {
  case EasingFunction::Linear:
    return easeLinear(t);
  case EasingFunction::SineInOut:
    return easeSineInOut(t);
  case EasingFunction::QuadInOut:
    return easeQuadInOut(t);
  case EasingFunction::CubicInOut:
    return easeCubicInOut(t);
  case EasingFunction::QuartInOut:
    return easeQuartInOut(t);
  case EasingFunction::QuintInOut:
    return easeQuintInOut(t);
  case EasingFunction::CircInOut:
    return easeCircInOut(t);
  case EasingFunction::ElasticInOut:
    return easeElasticInOut(t);
  case EasingFunction::BackInOut:
    return easeBackInOut(t);
  case EasingFunction::BounceInOut:
    return easeBounceInOut(t);
  default:
    return easeLinear(t);
  }
}

// Simple linear interpolation.
float easeLinear(float t) { return t; }

// Sine easing.
float easeSineInOut(float t) { return -(cos(PI * t) - 1.0) / 2.0; }

// Quad easing.
float easeQuadInOut(float t) {
  return t < 0.5 ? 2.0 * t * t : -1.0 + (4.0 - 2.0 * t) * t;
}

// Cubic easing.
float easeCubicInOut(float t) {
  return t < 0.5 ? 4.0 * t * t * t
                 : (t - 1.0) * (2.0 * t - 2.0) * (2.0 * t - 2.0) + 1.0;
}

// Quart easing.
float easeQuartInOut(float t) {
  float f = t * 2.0;
  if (f < 1.0)
    return 0.5 * f * f * f * f;
  float f2 = f - 2.0;
  return -0.5 * (f2 * f2 * f2 * f2 - 2.0);
}

// Quint easing.
float easeQuintInOut(float t) {
  float f = t * 2.0;
  if (f < 1.0)
    return 0.5 * f * f * f * f * f;
  float f2 = f - 2.0;
  return 0.5 * (f2 * f2 * f2 * f2 * f2 + 2.0);
}

// Circ easing.
float easeCircInOut(float t) {
  float f = t * 2.0;
  if (f < 1.0)
    return -0.5 * (sqrt(1.0 - f * f) - 1.0);
  float f2 = f - 2.0;
  return 0.5 * (sqrt(1.0 - f2 * f2) + 1.0);
}

// Elastic easing.
float easeElasticInOut(float t) {
  if (t == 0.0 || t == 1.0)
    return t;
  float p = 0.3 * 1.5;
  float s = p / 4.0;
  float f = t * 2.0 - 1.0;
  if (f < 0.0)
    return -0.5 * (pow(2.0, 10.0 * f) * sin((f - s) * (2.0 * PI) / p));
  float f2 = f - 1.0;
  return pow(2.0, -10.0 * f2) * sin((f2 - s) * (2.0 * PI) / p) * 0.5 + 1.0;
}

// Back easing.
float easeBackInOut(float t) {
  float s = 1.70158 * 1.525;
  float f = t * 2.0;
  if (f < 1.0)
    return 0.5 * (f * f * ((s + 1.0) * f - s));
  float f2 = f - 2.0;
  return 0.5 * (f2 * f2 * ((s + 1.0) * f2 + s) + 2.0);
}

// Bounce easing.
float easeBounceOut(float t) {
  if (t < 1.0 / 2.75) {
    return 7.5625 * t * t;
  } else if (t < 2.0 / 2.75) {
    t -= 1.5 / 2.75;
    return 7.5625 * t * t + 0.75;
  } else if (t < 2.5 / 2.75) {
    t -= 2.25 / 2.75;
    return 7.5625 * t * t + 0.9375;
  } else {
    t -= 2.625 / 2.75;
    return 7.5625 * t * t + 0.984375;
  }
}
float easeBounceIn(float t) { return 1.0 - easeBounceOut(1.0 - t); }
float easeBounceInOut(float t) {
  if (t < 0.5)
    return easeBounceIn(t * 2.0) * 0.5;
  return easeBounceOut(t * 2.0 - 1.0) * 0.5 + 0.5;
}
} // namespace Easing
