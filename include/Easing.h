/**
 * @file Easing.h
 * @brief Header file for easing functions.
 */

#ifndef EASING_H
#define EASING_H

#include <Arduino.h>

// Easing functions based on Robert Penner's easing equations.
namespace Easing {

/**
 * @brief A collection of easing functions for smooth color transitions.
 *
 * These functions accept a time parameter (t, from 0.0 to 1.0) and return
 * an eased value.
 */
enum EasingFunction {
  Linear,
  SineInOut,
  QuadInOut,
  CubicInOut,
  QuartInOut,
  QuintInOut,
  CircInOut,
  ElasticInOut,
  BackInOut,
  BounceInOut
};

/**
 * @brief Gets the eased value for a given time and easing function.
 * @param func The easing function to use.
 * @param t The time value (0.0-1.0).
 * @return The eased value.
 */
float getEasedValue(EasingFunction func, float t);

// --- Specific Easing Function Implementations ---
float easeLinear(float t);
float easeSineInOut(float t);
float easeQuadInOut(float t);
float easeCubicInOut(float t);
float easeQuartInOut(float t);
float easeQuintInOut(float t);
float easeCircInOut(float t);
float easeElasticInOut(float t);
float easeBackInOut(float t);
float easeBounceInOut(float t);
} // namespace Easing

#endif
