/**
 * @file Constants.h
 * @brief Global constants for the Spotlight project.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

namespace Constants {
// The maximum number of colors for the color cycle mode.
// This value is defined here so that memory can be statically allocated
// at compile time, avoiding dynamic allocation issues.
const size_t MAX_COLORS = 32;
} // namespace Constants

#endif
