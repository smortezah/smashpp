#ifndef PAINT_H_INCLUDED
#define PAINT_H_INCLUDED

#include "defs.h"
#include "vizparam.hpp"

namespace smashpp {
struct Point {
  double x, y;
};

struct Rectangle {
  double w, h;
  double x, y;
  Rectangle (double w_, double h_, double x_, double y_) {config(w_,h_,x_,y_);}
  void config (double w_, double h_, double x_, double y_) {
    w=w_;  h=h_;  x=x_;  y=y_;
  }
};

struct HsvColor {
  u8 h, s, v;
  explicit HsvColor(u8 h_) : h(h_), s(PAINT_LVL_SATUR), v(PAINT_LVL_VAL) {}
  HsvColor(u8 h_, u8 s_, u8 v_) : h(h_), s(s_), v(v_) {}
};

struct RgbColor {
  u8 r, g, b;
  RgbColor() = default;
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

  void     config      (double, double, u64, u64);
  RgbColor hsv_to_rgb  (HsvColor) const;
  string   rgb_color   (u8) const;
  void     print_final (ofstream&) const;
  void     print_head  (ofstream&, double, double) const;
  void     polygon     (ofstream&, double, double, double, double, double,
                        double, double, double, const string&, const string&)
                        const;
  void     line        (ofstream&, double, double, double, double, double,
                        const string&) const;
  void     rect_oval   (ofstream&, double, double, double, double,
                        const string&) const;
  void     rect        (ofstream&, shared_ptr<Rectangle>, const string&) const;
  void     rect_ir     (ofstream&, const shared_ptr<Rectangle>&, const string&)
                       const;
  void     chromosome  (ofstream&, double, double, double, double) const;
  void     text        (ofstream&, double, double, const string&) const;
  template <typename Value>
  double   get_point   (Value);
};
}

#endif