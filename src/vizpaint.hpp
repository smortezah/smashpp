#ifndef VIZPAINT_H_INCLUDED
#define VIZPAINT_H_INCLUDED

#include "vizdef.hpp"
#include "par.hpp"
#include "svg.hpp"

namespace smashpp {
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
  u32 mult;

  void show_info (VizParam&, const string&, const string&, u64, u64) const;
  void config (double, double, u64, u64);
  RgbColor hsv_to_rgb (const HsvColor&) const;
  HsvColor rgb_to_hsv (const RgbColor&) const;
  string rgb_color (u8) const;
  string heatmap_color (double, const HeatmapColor& h=HeatmapColor()) const;
  template <typename ValueR, typename ValueG, typename ValueB>
  string shade_color (ValueR, ValueG, ValueB) const;
  string customColor (u32) const;
  string nrc_color (double, u32) const;
  string redun_color (double, u32) const;
  void print_head (ofstream&, double, double) const;
  void print_tail (ofstream&) const;
  template <typename Value>
  double get_point (Value) const;
  void plot_legend (ofstream&, const VizParam&) const;
  string tspan (u32, i64) const;
  string tspan (u32, const string&) const;
  void sort_merge (string&) const;
};
}

#endif