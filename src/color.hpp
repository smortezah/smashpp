#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include "par.hpp"

namespace smashpp {
class RGB;
class HSV;

// Global functions
auto is_hex (const string&) -> bool;
auto to_hex (const RGB&) -> string;
auto to_rgb (const string&) -> RGB;
auto to_rgb (const HSV&) -> RGB;
auto to_hsv (const RGB&) -> HSV;
auto alpha_blend (const RGB&, const RGB&, float) -> RGB;
auto alpha_blend (const string&, const RGB&, float) -> string;
auto shade (const RGB&, float=0.5) -> RGB;        // Mix whith black
auto shade (const string&, float=0.5) -> string;
auto tint (const RGB&, float=0.5) -> RGB;         // Mix with white
auto tint (const string&, float=0.5) -> string;
auto tone (const RGB&, float=0.5) -> RGB;         // Mix with grey
auto tone (const string&, float=0.5) -> string;

class Color {
 public:
  Color () = default;
};

class RGB : public Color {
 public:
  u8 r, g, b;

  RGB () = default;
  RGB (u8 r_, u8 g_, u8 b_) : r(r_), g(g_), b(b_) {}
  RGB (const string& color) : r(to_rgb(color).r), g(to_rgb(color).g), 
    b(to_rgb(color).b) {}
};

class HSV : public Color {
 public:
  u8 h, s, v;

  HSV () = default;
  explicit HSV (u8 hue) : h(hue), s(PAINT_LVL_SATUR), v(PAINT_LVL_VAL) {}
};
}

#endif