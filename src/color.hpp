#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include "par.hpp"

namespace smashpp {
class RGB;
class HSV;

// Global functions
bool is_hex (const string&);
string to_hex (const RGB&);
RGB to_rgb (const string&);
HSV to_hsv (const RGB&);
RGB alpha_blend (const RGB&, const RGB&, float);
string alpha_blend (const string&, const RGB&, float);
RGB shade (const RGB&, float=0.5);       // Mix whith black
string shade (const string&, float=0.5);
RGB tint (const RGB&, float=0.5);        // Mix with white
string tint (const string&, float=0.5);
RGB tone (const RGB&, float=0.5);        // Mix with grey
string tone (const string&, float=0.5);

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