/**
 * @file ColorSpace.h
 * @brief Header file for color space conversion utilities.
 *
 * This file contains structs and functions to convert between different
 * color spaces, such as RGB, HSV, and LCH.
 */

#ifndef COLORSPACE_H
#define COLORSPACE_H

#include <Arduino.h>

namespace ColorSpace {

// RGB color representation.
struct RGB {
  uint8_t r, g, b;
};

// HSL (Hue, Saturation, Lightness) color representation.
struct HSL {
  float h, s, l;
};

// LCH (Lightness, Chroma, Hue) color representation.
// Values are floats for interpolation.
struct LCH {
  float l, c, h;
};

/**
 * @brief Overloaded operator for adding two LCH structs.
 */
LCH operator+(const LCH &a, const LCH &b);
/**
 * @brief Overloaded operator for subtracting two LCH structs.
 */
LCH operator-(const LCH &a, const LCH &b);
/**
 * @brief Overloaded operator for multiplying an LCH struct by a scalar.
 */
LCH operator*(const LCH &a, float scalar);
/**
 * @brief Overloaded operator for multiplying a scalar by an LCH struct.
 */
LCH operator*(float scalar, const LCH &a);

/**
 * @brief Converts a Kelvin color temperature to an RGB value.
 * @param kelvin The color temperature in Kelvin (1500-10000).
 * @return The resulting RGB color.
 */
RGB kelvinToRgb(float kelvin);

/**
 * @brief Converts an HSV color to an RGB value.
 * @param h Hue value (0-360).
 * @param s Saturation value (0.0-1.0).
 * @param v Value/Brightness (0.0-1.0).
 * @return The resulting RGB color.
 */
RGB hsvToRgb(float h, float s, float v);

/**
 * @brief Converts an RGB color to LCH.
 * @param rgb Input RGB color.
 * @return The resulting LCH color.
 */
LCH rgbToLch(const RGB &rgb);

/**
 * @brief Converts an LCH color to RGB.
 * @param lch Input LCH color.
 * @return The resulting RGB color.
 */
RGB lchToRgb(const LCH &lch);

/**
 * @brief Converts an RGB color to HSL.
 * @param rgb Input RGB color.
 * @return The resulting HSL color.
 */
HSL rgbToHsl(const RGB &rgb);

/**
 * @brief Converts an HSL color to RGB.
 * @param hsl Input HSL color.
 * @return The resulting RGB color.
 */
RGB hslToRgb(const HSL &hsl);

/**
 * @brief Converts an RGB color to HSV.
 * @param rgb Input RGB color.
 * @return The resulting HSV color.
 */
void rgbToHsv(const RGB &rgb, float &h, float &s, float &v);
} // namespace ColorSpace

#endif
