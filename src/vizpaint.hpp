#ifndef VIZPAINT_H_INCLUDED
#define VIZPAINT_H_INCLUDED

#include "vizdef.hpp"
//#include "vizparam.hpp"
#include "par.hpp"

namespace smashpp {
struct RgbColor {
  u8 r, g, b;
  RgbColor    () = default;
  RgbColor    (u8 r_, u8 g_, u8 b_) { config(r_,g_,b_); }
  void config (u8 r_, u8 g_, u8 b_) { r=r_;  g=g_;  b=b_; }
};

struct HsvColor {
  u8 h, s, v;
  HsvColor () = default;
  explicit HsvColor (u8 hue) : h(hue), s(PAINT_LVL_SATUR), v(PAINT_LVL_VAL) {}
};

struct Point {
  double x, y;

  Point       () = default;
  Point       (double x_, double y_) { config(x_,y_); }
  void config (double x_, double y_) { x=x_;  y=y_; }
};

class Text {
 public:
  Point  origin;
  string label, textAnchor, transform;
  u8 fontSize;

  Text              () : textAnchor("middle"), fontSize(13) {}
  void plot         (ofstream&) const;
  void plot_title   (ofstream&);
  void plot_pos_ref (ofstream&);
  void plot_pos_tar (ofstream&);
};

class Line {
 public:
  Point  beg, end;
  double width;
  string color;

  Line      () = default;
  void plot (ofstream&) const;
};

class Rectangle {
 public:
  Point  origin;
  double width, height;
  string color;

  Rectangle             () = default;
  void plot             (ofstream&)           const;
  void plot_ir          (ofstream&)           const;
  void plot_oval        (ofstream&)           const;
  void plot_oval_ir     (ofstream&)           const;
  void plot_nrc         (ofstream&, char)     const;
  void plot_nrc_ref     (ofstream&)           const;
  void plot_nrc_tar     (ofstream&)           const;
  void plot_complex     (ofstream&, u8, char) const;
  void plot_complex_ref (ofstream&, bool)     const;
  void plot_complex_tar (ofstream&, bool)     const;
  void plot_chromosome  (ofstream&)           const;
  void plot_legend      (ofstream&, const string&, const string&) const;
};

class Polygon {
 public:
  Point  one, two, three, four;
  string lineColor, fillColor;

  Polygon   () = default;
  void plot (ofstream&) const;
};

class Circle {
 public:
  Circle    () = default;
  void plot (ofstream&) const;

 private:
  Point  origin;
  double radius;
  string fillColor;
};

class VizPaint {
 public:
  double cx, cy;
  string backColor;
  double width;
  double space;
  double refSize, tarSize, maxSize;

  VizPaint() : cx(0.8*PAINT_CX), cy(PAINT_CY), backColor(PAINT_BGCOLOR), width(0.0),
               space(0.0), refSize(0.0), tarSize(0.0), maxSize(0.0), ratio(1) {}
  void print_plot (VizParam&);

 private:
  u32 ratio;

  void     show_info   (VizParam&, const string&, const string&, u64, u64)const;
  void     config      (double, double, u64, u64);
  RgbColor hsv_to_rgb  (const HsvColor&)           const;
  HsvColor rgb_to_hsv  (const RgbColor&)           const;
  string   rgb_color   (u8)                        const;
  template <typename ValueR, typename ValueG, typename ValueB>
  string   shade_color (ValueR, ValueG, ValueB)    const;
  void     print_head  (ofstream&, double, double) const;
  void     print_tail  (ofstream&)                 const;
  template <typename Value>
  double   get_point   (Value)                     const;
};
}

#endif