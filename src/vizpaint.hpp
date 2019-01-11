#ifndef VIZPAINT_H_INCLUDED
#define VIZPAINT_H_INCLUDED

#include "vizdef.hpp"
#include "par.hpp"
#include "svg.hpp"
#include "color.hpp"
#include "color.cpp"

namespace smashpp {
struct Position {
  i64   begRef, endRef,  begTar, endTar;
  prc_t entRef, selfRef, entTar, selfTar;
  u64   start;

  Position (i64 br, i64 er, prc_t nr, prc_t sr, i64 bt, i64 et, prc_t nt, 
    prc_t st, u64 s) : begRef(br), endRef(er), begTar(bt), endTar(et), 
    entRef(nr), selfRef(sr), entTar(nt), selfTar(st), start(s) {}
};

struct PosNode {
  i64  position;
  char type;
  u64  start;

  PosNode (i64 p, char t, u64 s) : position(p), type(t), start(s) {}
};

struct PosPlot {
  u8     n_ranges, n_subranges;
  u16    minorTickSize, majorTickSize, vertSkip, tickLabelSkip;
  bool   vertical, showNRC, showRedun, plotRef, refTick, tarTick;
  float  minorStrokeWidth, majorStrokeWidth;
  double n_bases;

  PosPlot () : n_ranges(10), n_subranges(4), minorTickSize(6),
    majorTickSize(1.75*minorTickSize), vertSkip(13), vertical(false),
    minorStrokeWidth(0.8f), majorStrokeWidth(1.6*minorStrokeWidth) {}
};

struct LegendPlot {
  bool showNRC, showRedun, vertical;
  i64  maxWidth;
  u8   labelShift, colorMode;
  unique_ptr<Rectangle> rect;
  unique_ptr<Path>      path;
  unique_ptr<Text>      text;

  LegendPlot () : labelShift(10), rect(make_unique<Rectangle>()), 
    path(make_unique<Path>()), text(make_unique<Text>()) {}
};

class VizPaint {
 public:
  float  x, y;
  string backColor;
  float  seqWidth, periphWidth, innerSpace;
  float  refSize, tarSize, maxSize;
  unique_ptr<SVG> svg;

  VizPaint() : backColor("white"), svg(make_unique<SVG>()), ratio(1), 
    plottable(true), ry(2.0f) {}
  void plot (VizParam&);

 private:
  u32   ratio;
  u32   mult;
  bool  plottable;
  float ry;
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
  void plot_title_horizontal (ofstream&, const string&, const string&,
    unique_ptr<Text>&) const;
  void plot_title_vertical (ofstream&, const string&, const string&,
    unique_ptr<Text>&) const;
  void plot_legend (ofstream&, const VizParam&, i64) const;
  void plot_legend_horizontal (ofstream&, unique_ptr<LegendPlot>&) const;
  void plot_legend_vertical (ofstream&, unique_ptr<LegendPlot>&) const;
  void plot_legend_gradient (ofstream&, unique_ptr<LegendPlot>&) const;
  auto tspan (u32, i64) const -> string;
  auto tspan (u32, const string&) const -> string;
  void sort_merge (string&) const;
  void save_n_pos (const string&) const;
  void read_pos (ifstream&, vector<Position>&, VizParam&);
  void make_posNode (const vector<Position>&, const VizParam&, string&&);
  void plot_pos_horizontal (ofstream&, unique_ptr<PosPlot>&) const;
  void plot_pos_vertical (ofstream&, unique_ptr<PosPlot>&) const;
  // void print_pos (ofstream&, VizParam&, const vector<Position>&, u64, string&&);
};
}

#endif
