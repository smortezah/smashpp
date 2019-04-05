// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#include <algorithm>
#include "color.hpp"
#include "string.hpp"
#include "exception.hpp"

namespace smashpp {
// Global functions
inline bool is_hex(std::string color) {
  if (color.front() != '#' || color.size() != 7) return false;

  for (auto ch : color.substr(1))
    if (!std::isxdigit(ch)) return false;

  return true;
}

inline std::string to_hex(const RGB& color) {
  return string_format("#%X%X%X", color.r, color.g, color.b);
}

inline RGB to_rgb(std::string color) {
  if (is_hex(color)) {
    return RGB(std::stoi(color.substr(1, 2), 0, 16 /*base*/),
               std::stoi(color.substr(3, 2), 0, 16 /*base*/),
               std::stoi(color.substr(5, 2), 0, 16 /*base*/));
  } else {
    if (color == "black")
      return RGB(0, 0, 0);
    else if (color == "white")
      return RGB(255, 255, 255);
    else if (color == "grey")
      return RGB(128, 128, 128);
    else if (color == "red")
      return RGB(255, 0, 0);
    else if (color == "green")
      return RGB(0, 255, 0);
    else if (color == "blue")
      return RGB(0, 0, 255);
    else
      error("color undefined");
  }
  return RGB();
}

inline RGB to_rgb(const HSV& hsv) {
  RGB rgb{};
  if (hsv.s == 0) {
    rgb.r = rgb.g = rgb.b = hsv.v;
    return rgb;
  }

  const auto region = static_cast<uint8_t>(hsv.h / 43);
  const auto remainder = static_cast<uint8_t>((hsv.h - region * 43) * 6);
  const auto p = static_cast<uint8_t>((hsv.v * (255 - hsv.s)) >> 8);
  const auto q =
      static_cast<uint8_t>((hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8);
  const auto t = static_cast<uint8_t>(
      (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8);

  switch (region) {
    case 0:
      rgb.r = hsv.v;
      rgb.g = t;
      rgb.b = p;
      break;
    case 1:
      rgb.r = q;
      rgb.g = hsv.v;
      rgb.b = p;
      break;
    case 2:
      rgb.r = p;
      rgb.g = hsv.v;
      rgb.b = t;
      break;
    case 3:
      rgb.r = p;
      rgb.g = q;
      rgb.b = hsv.v;
      break;
    case 4:
      rgb.r = t;
      rgb.g = p;
      rgb.b = hsv.v;
      break;
    default:
      rgb.r = hsv.v;
      rgb.g = p;
      rgb.b = q;
      break;
  }
  return rgb;
}

inline HSV to_hsv(const RGB& rgb) {
  const uint8_t rgbMin{std::min({rgb.r, rgb.g, rgb.b})};
  const uint8_t rgbMax{std::max({rgb.r, rgb.g, rgb.b})};

  HSV hsv{};
  hsv.v = rgbMax;
  if (hsv.v == 0) {
    hsv.h = hsv.s = 0;
    return hsv;
  }

  hsv.s = static_cast<uint8_t>(
      255 * static_cast<uint16_t>((rgbMax - rgbMin) / hsv.v));
  if (hsv.s == 0) {
    hsv.h = 0;
    return hsv;
  }

  if (rgbMax == rgb.r)
    hsv.h = static_cast<uint8_t>(43 * (rgb.g - rgb.b) / (rgbMax - rgbMin));
  else if (rgbMax == rgb.g)
    hsv.h = static_cast<uint8_t>(85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin));
  else
    hsv.h =
        static_cast<uint8_t>(171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin));

  return hsv;
}

inline RGB alpha_blend(const RGB& color1, const RGB& color2, float alpha) {
  return RGB(static_cast<uint8_t>(color1.r + (color2.r - color1.r) * alpha),
             static_cast<uint8_t>(color1.g + (color2.g - color1.g) * alpha),
             static_cast<uint8_t>(color1.b + (color2.b - color1.b) * alpha));
}

inline RGB shade(const RGB& color, float alpha) {
  return alpha_blend(color, to_rgb("black"), alpha);
}

inline std::string shade(std::string color, float alpha) {
  return to_hex(alpha_blend(to_rgb(color), to_rgb("black"), alpha));
}

inline RGB tint(const RGB& color, float alpha) {
  return alpha_blend(color, to_rgb("white"), alpha);
}

inline std::string tint(std::string color, float alpha) {
  return to_hex(alpha_blend(to_rgb(color), to_rgb("white"), alpha));
}

inline RGB tone(const RGB& color, float alpha) {
  return alpha_blend(color, to_rgb("grey"), alpha);
}

inline std::string tone(std::string color, float alpha) {
  return to_hex(alpha_blend(to_rgb(color), to_rgb("grey"), alpha));
}
}  // namespace smashpp