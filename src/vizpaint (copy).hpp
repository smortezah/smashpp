#ifndef VIZPAINT_H_INCLUDED
#define VIZPAINT_H_INCLUDED

#include "vizdef.hpp"
#include "par.hpp"
#include "svg.hpp"

namespace smashpp {
struct Position {
  i64   begRef, endRef;
  prc_t entRef, selfRef;
  i64   begTar, endTar;
  prc_t entTar, selfTar;
  u64   start;
  Position (i64 br, i64 er, prc_t nr, prc_t sr, i64 bt, i64 et, prc_t nt, 
    prc_t st, u64 s) : begRef(br), endRef(er), entRef(nr), selfRef(sr), 
    begTar(bt), endTar(et), entTar(nt), selfTar(st), start(s) {}
};

struct PosNode {
  i64  position;
  char type;
  u64  start;
  PosNode (i64 p, char t, u64 s) : position(p), type(t), start(s) {}
};

class VizPaint {
 public:
  double cx, cy;
  string backColor;
  double width, space;
  double refSize, tarSize, maxSize;

  VizPaint() : cx(PAINT_CX), cy(PAINT_CY), backColor(PAINT_BGCOLOR), width(0.0),
    space(0.0), refSize(0.0), tarSize(0.0), maxSize(0.0), ratio(1), 
    plottable(true), ry(2) {}
    
  void print_plot (VizParam&);

 private:
  u32   ratio;
  u32   mult;
  bool  plottable;
  float ry;
  vector<i64>     lastPos;
  vector<PosNode> nodes;

  void show_info (VizParam&, const string&, const string&, u64, u64) const;
  void config (double, double, u32, u64, u64);
  auto hsv_to_rgb (const HsvColor&) const -> RgbColor;
#ifdef EXTEND
  auto rgb_to_hsv (const RgbColor&) const -> HsvColor;
#endif
  auto rgb_color (u32) const -> string;
  auto nrc_color (double, u32) const -> string;
  auto redun_color (double, u32) const -> string;
  void print_head (ofstream&, double, double);
  void print_tail (ofstream&) const;
  template <typename Value>
  auto get_point (Value) const -> double;
  void plot_legend (ofstream&, const VizParam&);
  template <typename Rect>
  void plot_legend_gradient (ofstream&, const Rect&, u8);
  void plot_annot (ofstream&, i64, bool, bool) const;
  auto tspan (u32, i64) const -> string;
  auto tspan (u32, const string&) const -> string;
  void sort_merge (string&) const;
  void save_n_pos (const string&) const;
  void read_pos (ifstream&, vector<Position>&, VizParam&);
  void make_posNode (const vector<Position>&, const VizParam&, string&&);
  void print_pos (ofstream&, VizParam&, const vector<Position>&, u64, string&&);
};
}

#endif