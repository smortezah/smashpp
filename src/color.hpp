// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_COLOR_HPP
#define SMASHPP_COLOR_HPP

#include "par.hpp"

namespace smashpp {
class RGB;
class HSV;

// Global functions
bool is_hex(std::string);
auto to_hex(const RGB&) -> std::string;
auto to_rgb(std::string) -> RGB;
auto to_rgb(const HSV&) -> RGB;
auto to_hsv(const RGB&) -> HSV;
auto alpha_blend(const RGB&, const RGB&, float) -> RGB;
auto shade(const RGB&, float = 0.5) -> RGB;  // Mix whith black
auto shade(std::string, float = 0.5) -> std::string;
auto tint(const RGB&, float = 0.5) -> RGB;  // Mix with white
auto tint(std::string, float = 0.5) -> std::string;
auto tone(const RGB&, float = 0.5) -> RGB;  // Mix with grey
auto tone(std::string, float = 0.5) -> std::string;

class Color {
 public:
  Color() = default;
};

class RGB : public Color {
 public:
  uint8_t r;
  uint8_t g;
  uint8_t b;

  RGB() = default;
  RGB(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
  explicit RGB(std::string color)
      : r(to_rgb(color).r), g(to_rgb(color).g), b(to_rgb(color).b) {}
};

class HSV : public Color {
 public:
  uint8_t h;
  uint8_t s;
  uint8_t v;

  HSV() = default;
  explicit HSV(uint8_t hue) : h(hue), s(160), v(160) {}
};
}  // namespace smashpp

#endif  //SMASHPP_COLOR_HPP