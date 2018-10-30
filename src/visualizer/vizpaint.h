#ifndef PAINT_H_INCLUDED
#define PAINT_H_INCLUDED

#include "defs.h"
#include "vizparam.hpp"

namespace smashpp {
struct RgbColor { u8 r, g, b; };
struct HsvColor {
  u8 h, s, v;
  HsvColor() : s(PAINT_LVL_SATUR), v(PAINT_LVL_VAL) {}
};

class VizPaint
{
 public:
  double  cx, cy;
  double  tx, ty;
  string  backColor;
  double  width;
  double  space;
  double  refSize, tarSize, maxSize;

  VizPaint() : cx(PAINT_CX), cy(PAINT_CY), tx(PAINT_TX), ty(PAINT_TY),
               backColor(PAINT_BGCOLOR), width(0.0), space(0.0), refSize(0.0),
               tarSize(0.0), maxSize(0.0) {}
  void print_plot (VizParam&);

 private:
  u32 ratio;

  void     config         (double, double, u64, u64);
  RgbColor hsv_to_rgb     (HsvColor);
  string   get_rgb_color  (u8);
  void     print_final    (ofstream&);
  void     print_head     (ofstream&, double, double) const;
  void     polygon        (ofstream&, double, double, double, double, double,
                           double, double, double, string, string);
  void     line           (ofstream&, double, double, double, double, double,
                           const string&) const;
  void     rect_oval      (ofstream&, double, double, double, double,
                           const string&) const;
  void     rect           (ofstream&, double, double, double, double,
                           const string&) const;
  void     rect_ir        (ofstream&, double, double, double, double, string);
  void     chromosome     (ofstream&, double, double, double, double);
  void     text           (ofstream&, double, double, const string&) const;
  template <typename Value>
  double   get_point      (Value);
};
}

#endif