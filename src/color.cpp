#include "color.hpp"
using namespace smashpp;

RGB Color::alpha_blend (const RGB& color1, const RGB& color2, float alpha)
const {
  return RGB(static_cast<u8>(color1.r + (color2.r-color1.r) * alpha),
             static_cast<u8>(color1.g + (color2.g-color1.g) * alpha),
             static_cast<u8>(color1.b + (color2.b-color1.b) * alpha));
}
string Color::alpha_blend (const string& hexColor, const RGB& color2, 
float alpha) const {
  RGB color1 {to_rgb(hexColor)};
  return to_hex(alpha_blend(color1, color2, alpha));
}

template <typename OutColor>
OutColor RGB::shade (float alpha) {
  return alpha_blend(*this, to_rgb("black"), alpha);
}

template <typename OutColor>
OutColor RGB::tint (float alpha) {
  return alpha_blend(*this, to_rgb("white"), alpha);
}

template <typename OutColor>
OutColor RGB::tone (float alpha) {
  return alpha_blend(*this, to_rgb("grey"), alpha);
}

namespace smashpp {
// Global functions
// inline static bool is_hex (const string& color) {
//   if (color.front()!='#' || color.size()!=7)          return false;
//   for (auto ch : color.substr(1))  if (!isxdigit(ch)) return false;
//   return true;
// }

// inline static RGB to_rgb (const string& color) {
//   if (is_hex(color)) {
//     return RGB(stoi(color.substr(1,2), 0, 16 /*base*/), 
//                stoi(color.substr(3,2), 0, 16 /*base*/),
//                stoi(color.substr(5,2), 0, 16 /*base*/));
//   }
//   else {
//     if      (color=="black")  return RGB(0,   0,   0);
//     else if (color=="white")  return RGB(255, 255, 255);
//     else if (color=="grey")   return RGB(128, 128, 128);
//     else if (color=="red")    return RGB(255, 0,   0);
//     else if (color=="green")  return RGB(0,   255, 0);
//     else if (color=="blue")   return RGB(0,   0,   255);
//     else                      error("color undefined");
//   }
//   return RGB();
// }

inline static RGB hsv_to_rgb (const HSV& hsv) {
  RGB rgb {};
  if (hsv.s==0) { rgb.r = rgb.g = rgb.b = hsv.v;    return rgb; }

  const auto region    = u8(hsv.h / 43),
             remainder = u8((hsv.h - region*43) * 6),
             p = u8((hsv.v * (255 - hsv.s)) >> 8),
             q = u8((hsv.v * (255 - ((hsv.s*remainder)>>8))) >> 8),
             t = u8((hsv.v * (255 - ((hsv.s*(255-remainder))>>8))) >> 8);

  switch (region) {
  case 0:   rgb.r=hsv.v;  rgb.g=t;      rgb.b=p;      break;
  case 1:   rgb.r=q;      rgb.g=hsv.v;  rgb.b=p;      break;
  case 2:   rgb.r=p;      rgb.g=hsv.v;  rgb.b=t;      break;
  case 3:   rgb.r=p;      rgb.g=q;      rgb.b=hsv.v;  break;
  case 4:   rgb.r=t;      rgb.g=p;      rgb.b=hsv.v;  break;
  default:  rgb.r=hsv.v;  rgb.g=p;      rgb.b=q;      break;
  }
  return rgb;
}

inline static HSV rgb_to_hsv (const RGB& rgb) {
  const u8 rgbMin { min({rgb.r, rgb.g, rgb.b}) };
  const u8 rgbMax { max({rgb.r, rgb.g, rgb.b}) };

  HSV hsv {};
  hsv.v = rgbMax;
  if (hsv.v==0) { hsv.h = hsv.s = 0;    return hsv; }

  hsv.s = u8(255 * u16((rgbMax-rgbMin)/hsv.v));
  if (hsv.s==0) { hsv.h = 0;            return hsv; }

  if      (rgbMax==rgb.r)    hsv.h = u8(43*(rgb.g-rgb.b)/(rgbMax-rgbMin));
  else if (rgbMax==rgb.g)    hsv.h = u8(85 + 43*(rgb.b-rgb.r)/(rgbMax-rgbMin));
  else                       hsv.h = u8(171 + 43*(rgb.r-rgb.g)/(rgbMax-rgbMin));
  
  return hsv;
}
}