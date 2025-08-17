/**
 * @file ColorSpace.cpp
 * @brief Implementation file for color space conversions.
 */

#include "ColorSpace.h"
#include <cmath>
#include <algorithm>

namespace ColorSpace {

    // Overloaded operators for LCH struct.
    LCH operator+(const LCH& a, const LCH& b) {
        return { a.l + b.l, a.c + b.c, a.h + b.h };
    }

    LCH operator-(const LCH& a, const LCH& b) {
        return { a.l - b.l, a.c - b.c, a.h - b.h };
    }

    LCH operator*(const LCH& a, float scalar) {
        return { a.l * scalar, a.c * scalar, a.h * scalar };
    }

    LCH operator*(float scalar, const LCH& a) {
        return a * scalar;
    }
    
    RGB kelvinToRgb(float kelvin) {
        float temp = kelvin / 100.0;
        float red, green, blue;

        // Red
        if (temp <= 66.0) {
            red = 255.0;
        } else {
            red = temp - 60.0;
            red = 329.698727446 * pow(red, -0.1332047592);
            red = std::min(255.0f, red);
            red = std::max(0.0f, red);
        }

        // Green
        if (temp <= 66.0) {
            green = temp;
            green = 99.4708025861 * log(green) - 161.1195681661;
        } else {
            green = temp - 60.0;
            green = 288.1221695283 * pow(green, -0.0755148492);
        }
        green = std::min(255.0f, green);
        green = std::max(0.0f, green);

        // Blue
        if (temp >= 66.0) {
            blue = 255.0;
        } else {
            if (temp <= 19.0) {
                blue = 0.0;
            } else {
                blue = temp - 10.0;
                blue = 138.5177312231 * log(blue) - 305.0447927307;
                blue = std::min(255.0f, blue);
                blue = std::max(0.0f, blue);
            }
        }
        return { static_cast<uint8_t>(red), static_cast<uint8_t>(green), static_cast<uint8_t>(blue) };
    }

    RGB hslToRgb(const HSL& hsl) {
        float r_f, g_f, b_f;
        if (hsl.s == 0.0f) {
            r_f = g_f = b_f = hsl.l;
        } else {
            auto hue2rgb = [](float p, float q, float t) {
                if (t < 0.0f) t += 1.0f;
                if (t > 1.0f) t -= 1.0f;
                if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
                if (t < 1.0f / 2.0f) return q;
                if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
                return p;
            };
            float q = hsl.l < 0.5f ? hsl.l * (1.0f + hsl.s) : hsl.l + hsl.s - hsl.l * hsl.s;
            float p = 2.0f * hsl.l - q;
            r_f = hue2rgb(p, q, hsl.h / 360.0f + 1.0f / 3.0f);
            g_f = hue2rgb(p, q, hsl.h / 360.0f);
            b_f = hue2rgb(p, q, hsl.h / 360.0f - 1.0f / 3.0f);
        }
        return { static_cast<uint8_t>(r_f * 255.0f), static_cast<uint8_t>(g_f * 255.0f), static_cast<uint8_t>(b_f * 255.0f) };
    }

    void rgbToHsv(const RGB& rgb, float& h, float& s, float& v) {
        float r_f = rgb.r / 255.0f;
        float g_f = rgb.g / 255.0f;
        float b_f = rgb.b / 255.0f;

        float max_c = std::max({r_f, g_f, b_f});
        float min_c = std::min({r_f, g_f, b_f});
        float delta = max_c - min_c;

        v = max_c;
        if (max_c == 0) {
            s = 0;
            h = 0;
            return;
        }

        s = delta / max_c;

        if (max_c == r_f) {
            h = fmod((g_f - b_f) / delta, 6.0f);
        } else if (max_c == g_f) {
            h = ((b_f - r_f) / delta) + 2.0f;
        } else {
            h = ((r_f - g_f) / delta) + 4.0f;
        }
        h *= 60.0f;
        if (h < 0) {
            h += 360.0f;
        }
    }

    RGB hsvToRgb(float h, float s, float v) {
        float c = v * s;
        float h_prime = fmod(h / 60.0f, 6.0f);
        float x = c * (1.0f - std::abs(fmod(h_prime, 2.0f) - 1.0f));
        float m = v - c;

        float r_f = 0, g_f = 0, b_f = 0;

        if (h_prime >= 0 && h_prime < 1) {
            r_f = c;
            g_f = x;
            b_f = 0;
        } else if (h_prime >= 1 && h_prime < 2) {
            r_f = x;
            g_f = c;
            b_f = 0;
        } else if (h_prime >= 2 && h_prime < 3) {
            r_f = 0;
            g_f = c;
            b_f = x;
        } else if (h_prime >= 3 && h_prime < 4) {
            r_f = 0;
            g_f = x;
            b_f = c;
        } else if (h_prime >= 4 && h_prime < 5) {
            r_f = x;
            g_f = 0;
            b_f = c;
        } else if (h_prime >= 5 && h_prime < 6) {
            r_f = c;
            g_f = 0;
            b_f = x;
        }

        return { static_cast<uint8_t>((r_f + m) * 255.0f), static_cast<uint8_t>((g_f + m) * 255.0f), static_cast<uint8_t>((b_f + m) * 255.0f) };
    }
    
    // Convert RGB to LCH (Luminance, Chroma, Hue)
    // This is a simplified conversion, not a full CIE LCH implementation
    LCH rgbToLch(const RGB& rgb) {
        float r = rgb.r / 255.0f;
        float g = rgb.g / 255.0f;
        float b = rgb.b / 255.0f;
        float max_val = std::max({r, g, b});
        float min_val = std::min({r, g, b});
        
        float l = (max_val + min_val) / 2.0f;
        float c = 0.0f;
        float h = 0.0f;

        if (max_val != min_val) {
            float delta = max_val - min_val;
            c = l > 0.5f ? delta / (2.0f - max_val - min_val) : delta / (max_val + min_val);
            if (max_val == r) {
                h = fmod((g - b) / delta + (g < b ? 6.0f : 0.0f), 6.0f);
            } else if (max_val == g) {
                h = ((b - r) / delta) + 2.0f;
            } else {
                h = ((r - g) / delta) + 4.0f;
            }
            h *= 60.0f;
            if (h < 0.0f) { h += 360.0f; }
        }
        return { l, c, h };
    }
    
    // Convert LCH to RGB
    RGB lchToRgb(const LCH& lch) {
        float s = lch.c;
        if (lch.l < 0.5f) {
            s = (lch.c == 0.0f) ? 0.0f : lch.c / (2.0f * lch.l);
        } else {
            s = (lch.c == 0.0f) ? 0.0f : lch.c / (2.0f - 2.0f * lch.l);
        }
        
        return hslToRgb({lch.h, s, lch.l});
    }
    
    HSL rgbToHsl(const RGB& rgb) {
        float r_f = rgb.r / 255.0f;
        float g_f = rgb.g / 255.0f;
        float b_f = rgb.b / 255.0f;

        float max_c = std::max({r_f, g_f, b_f});
        float min_c = std::min({r_f, g_f, b_f});
        float h, s, l;

        l = (max_c + min_c) / 2.0f;

        if (max_c == min_c) {
            h = 0.0f;
            s = 0.0f;
        } else {
            float delta = max_c - min_c;
            s = l > 0.5f ? delta / (2.0f - max_c - min_c) : delta / (max_c + min_c);
            if (max_c == r_f) {
                h = (g_f - b_f) / delta + (g_f < b_f ? 6.0f : 0.0f);
            } else if (max_c == g_f) {
                h = (b_f - r_f) / delta + 2.0f;
            } else {
                h = (r_f - g_f) / delta + 4.0f;
            }
            h *= 60.0f;
        }
        return {h, s, l};
    }
}
