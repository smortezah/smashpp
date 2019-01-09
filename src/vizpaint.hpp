#ifndef VIZPAINT_H_INCLUDED
#define VIZPAINT_H_INCLUDED

#include "vizdef.hpp"
#include "par.hpp"
#include "svg.hpp"
#include "color.hpp"
#include "color.cpp"

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
  float  x, y;
  string backColor;
  float  seqWidth, innerSpace;
  float  refSize, tarSize, maxSize;
  unique_ptr<SVG> svg;

  VizPaint() : /*x(20.0f), y(100.0f),*/ backColor("white"), 
    svg(make_unique<SVG>()), ratio(1), plottable(true), ry(2.0f) {}
  void plot (VizParam&);

 private:
  u32   ratio;
  u32   mult;
  bool  plottable;
  float ry;
  // bool  vertical;//todo
  vector<i64>     lastPos;
  vector<PosNode> nodes;

  void show_info (VizParam&, const string&, const string&, u64, u64) const;
  void config (double, double, u32, u64, u64);
  auto rgb_color (u32) const -> string;
  auto nrc_color (double, u32) const -> string;
  auto redun_color (double, u32) const -> string;
  template <typename Value>
  auto get_point (Value) const -> double;
  auto get_index (double point) const -> u64;
  void plot_title (ofstream&, const string&, const string&, bool) const;
  void plot_legend (ofstream&, const VizParam&, i64);
  template <typename Rect>
  void plot_legend_gradient (ofstream&, const Rect&, u8);
  auto tspan (u32, i64) const -> string;
  auto tspan (u32, const string&) const -> string;
  void sort_merge (string&) const;
  void save_n_pos (const string&) const;
  void read_pos (ifstream&, vector<Position>&, VizParam&);
  void make_posNode (const vector<Position>&, const VizParam&, string&&);
  // void print_pos (ofstream&, VizParam&, const vector<Position>&, u64, string&&);
  void plot_pos (ofstream&, VizParam&, u64, bool);
};
}

#endif
