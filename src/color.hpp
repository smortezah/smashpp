#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include "par.hpp"

namespace smashpp {
// class RGB;
// class HSV;

class Color {
 public:
  Color () = default;
};

class RGB : public Color {
 public:
  u8 r, g, b;

  RGB () = default;
  RGB (u8 r_, u8 g_, u8 b_) : r(r_), g(g_), b(b_) {}
  RGB alpha_blend (const RGB&, const RGB&, float) const;
  string alpha_blend (const string&, const RGB&, float) const;
  template <typename InColor>
  auto shade (const InColor&, float=0.5);  // Mix whith black
  template <typename InColor>
  auto tint (const InColor&, float=0.5);   // Mix with white
  template <typename InColor>
  auto tone (const InColor&, float=0.5);   // Mix with grey
};

class HSV : public Color {
 public:
  u8 h, s, v;

  HSV () = default;
  explicit HSV (u8 hue) : h(hue), s(PAINT_LVL_SATUR), v(PAINT_LVL_VAL) {}
};

// General functions
bool is_hex (const string&);
string to_hex (const RGB&);
RGB to_rgb (const string&);
RGB hsv_to_rgb (const HSV&);
HSV rgb_to_hsv (const RGB&);
}

#endif