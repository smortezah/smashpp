#ifndef VIZPAINT_H_INCLUDED
#define VIZPAINT_H_INCLUDED

#include "vizdef.hpp"
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

struct HeatmapColor {
  double start, rotations, hue, gamma;
  // HeatmapColor () = default;
  HeatmapColor () : start(HEAT_START), rotations(HEAT_ROT), hue(HEAT_HUE),
                    gamma(HEAT_GAMMA) {};
};

struct Gradient {
  string startColor, stopColor;
  vector<string> offsetColor;
  Gradient () = default;
  Gradient (string start, string stop)
    : startColor(std::move(start)), stopColor(std::move(stop)) {}
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
  string label, textAnchor, dominantBaseline, transform, color, fontWeight;
  u8     fontSize;

  Text              () : textAnchor("middle"), dominantBaseline("middle"), 
                         color("black"), fontWeight("normal"), fontSize(13) {}
  Text (const string& lbl_, const string& clr_) { label=lbl_;  color=clr_; }//todo
  void plot         (ofstream&) const;
  void plot_title   (ofstream&);
  void plot_pos_ref (ofstream&, char);
  void plot_pos_tar (ofstream&, char);
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
  float  opacity;

  Rectangle             () : opacity(DEF_OPAC) {}
  void plot             (ofstream&)           const;
  void plot_ir          (ofstream&)           const;
  void plot_oval        (ofstream&)           const;
  void plot_oval_ir     (ofstream&)           const;
  void plot_nrc         (ofstream&, char)     const;
  void plot_nrc_ref     (ofstream&)           const;
  void plot_nrc_tar     (ofstream&)           const;
  void plot_redun       (ofstream&, u8, char) const;
  void plot_redun_ref   (ofstream&, bool)     const;
  void plot_redun_tar   (ofstream&, bool)     const;
  void plot_chromosome  (ofstream&)           const;
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

  VizPaint() : cx(PAINT_CX), cy(PAINT_CY), backColor(PAINT_BGCOLOR), width(0.0),
               space(0.0), refSize(0.0), tarSize(0.0), maxSize(0.0), ratio(1) {}
  void print_plot (VizParam&);

 private:
  u32 ratio;

  void     show_info   (VizParam&, const string&, const string&, u64, u64)const;
  void     config      (double, double, u64, u64);
  RgbColor hsv_to_rgb  (const HsvColor&)           const;
  HsvColor rgb_to_hsv  (const RgbColor&)           const;
  string   rgb_color   (u8)                        const;
  string   heatmap_color (double, const HeatmapColor& h=HeatmapColor()) const;
  template <typename ValueR, typename ValueG, typename ValueB>
  string   shade_color (ValueR, ValueG, ValueB)    const;
  string   nrc_color   (double, u32)               const;
  string   redun_color (double, u32)               const;
  void     print_head  (ofstream&, double, double) const;
  void     print_tail  (ofstream&)                 const;
  template <typename Value>
  double   get_point   (Value)                     const;
  void     plot_legend (ofstream&, const VizParam&) const;
  void     sort_by_id  (string&, string&&)         const;
};
}

#endif