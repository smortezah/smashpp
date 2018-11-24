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

  auto show_info (VizParam&, const string&, const string&, u64, u64) const ->
    void;
  auto config (double, double, u64, u64) -> void;
  auto hsv_to_rgb (const HsvColor&) const -> RgbColor;
  auto rgb_to_hsv (const RgbColor&) const -> HsvColor;
  auto rgb_color (u8) const -> string;
  auto heatmap_color (double, const HeatmapColor& h=HeatmapColor()) const ->
    string;
  template <typename ValueR, typename ValueG, typename ValueB>
  auto shade_color (ValueR, ValueG, ValueB) const -> string;
  auto customColor (u32) const -> string;
  auto nrc_color (double, u32) const -> string;
  auto redun_color (double, u32) const -> string;
  auto print_head (ofstream&, double, double) const -> void;
  auto print_tail (ofstream&) const -> void;
  template <typename Value>
  auto get_point (Value) const -> double;
  auto plot_legend (ofstream&, const VizParam&) const -> void;
  auto tspan (u32, i64) const -> string;
  auto tspan (u32, const string&) const -> string;
  auto sort_merge (string&) const -> void;
  // template <typename Position>
  // auto plot_pos (const Position&, bool, bool, u64) const -> void;
};
}

#endif