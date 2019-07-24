// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_VIZPAINT_HPP
#define SMASHPP_VIZPAINT_HPP

#include "color.cpp"
#include "color.hpp"
#include "def.hpp"
#include "par.hpp"
#include "svg.hpp"

namespace smashpp {
static const std::string FMT_N{"n"};  // Format - position of N files
static constexpr uint32_t PAINT_SCALE{100};
static constexpr auto PAINT_SHORT{0.035};
static constexpr auto SPACE_TUNE{6.0f};
static constexpr auto TITLE_SPACE{16.0f};
static constexpr auto VERT_MIDDLE{0.35f};
static constexpr auto VERT_BOTTOM{0.7f};

struct Position {
  int64_t begRef;
  int64_t endRef;
  int64_t begTar;
  int64_t endTar;
  prc_t entRef;
  prc_t selfRef;
  prc_t entTar;
  prc_t selfTar;
  uint64_t start;
  uint8_t n_color;

  Position(int64_t br, int64_t er, prc_t nr, prc_t sr, int64_t bt, int64_t et,
           prc_t nt, prc_t st, uint64_t s)
      : begRef(br),
        endRef(er),
        begTar(bt),
        endTar(et),
        entRef(nr),
        selfRef(sr),
        entTar(nt),
        selfTar(st),
        start(s),
        n_color(1) {}
};

struct PosNode {
  int64_t position;
  char type;
  uint64_t start;

  PosNode(int64_t p, char t, uint64_t s) : position(p), type(t), start(s) {}
};

struct PosPlot {
  uint8_t n_ranges;
  uint8_t n_subranges;
  uint16_t minorTickSize;
  uint16_t majorTickSize;
  uint16_t vertSkip;
  uint16_t tickLabelSkip;
  bool vertical;
  bool showNRC;
  bool showRedun;
  bool plotRef;
  uint64_t refTick;
  uint64_t tarTick;
  bool tickHumanRead;
  float minorStrokeWidth;
  float majorStrokeWidth;
  double n_bases;

  PosPlot()
      : n_ranges(10),
        n_subranges(4),
        minorTickSize(4), // 6
        majorTickSize(1.5 * minorTickSize), // 1.75
        vertSkip(13),
        vertical(false),
        minorStrokeWidth(0.8f),
        majorStrokeWidth(1.6 * minorStrokeWidth) {}
};

struct LegendPlot {
  bool showNRC;
  bool showRedun;
  bool vertical;
  int64_t maxWidth;
  uint8_t labelShift;
  uint8_t colorMode;
  std::unique_ptr<Rectangle> rect;
  std::unique_ptr<Path> path;
  std::vector<std::unique_ptr<Text>> text;

  LegendPlot()
      : labelShift(10),
        rect(std::make_unique<Rectangle>()),
        path(std::make_unique<Path>()) {}
};

class VizPaint {
 public:
  float x;
  float y;
  float seqWidth;
  float periphWidth;
  float innerSpace;
  float refSize;
  float tarSize;
  float maxSize;
  std::unique_ptr<SVG> svg;

  VizPaint()
      : svg(std::make_unique<SVG>()), ratio(1), plottable(true), ry(2.0f) {}
  void plot(std::unique_ptr<VizParam>&);

 private:
  std::string ref;
  std::string tar;
  uint32_t ratio;
  uint32_t mult;
  uint64_t n_refBases;
  uint64_t n_tarBases;
  bool plottable;
  float ry;
  std::vector<int64_t> lastPos;
  std::vector<PosNode> nodes;

  void read_matadata(std::ifstream&, std::unique_ptr<VizParam>&);
  void show_info(std::unique_ptr<VizParam>&) const;
  void config(double, double, uint32_t);
  void set_page(bool);
  auto rgb_color(uint32_t) const -> std::string;
  auto make_color(uint8_t, uint8_t) const -> std::string;
  auto nrc_color(double, uint32_t) const -> std::string;
  auto redun_color(double, uint32_t) const -> std::string;
  auto seq_gradient(std::ofstream&, std::string, std::string) const
      -> std::string;
  auto periph_gradient(std::ofstream&, std::string, std::string) const
      -> std::string;
  template <typename Value>
  auto get_point(Value) const -> double;
  auto get_index(double point) const -> uint64_t;
  void plot_background(std::ofstream&) const;
  void plot_seq_ref(std::ofstream&, const std::vector<Position>::iterator&,
                    std::unique_ptr<VizParam>&) const;
  void plot_seq_tar(std::ofstream&, const std::vector<Position>::iterator&,
                    std::unique_ptr<VizParam>&, bool) const;
  void plot_periph(std::ofstream&, std::unique_ptr<Rectangle>&, bool, char,
                   uint8_t) const;
  void plot_periph(std::ofstream&, std::unique_ptr<Cylinder>&, bool, char,
                   uint8_t) const;
  void plot_connector(std::ofstream&, const std::vector<Position>::iterator&,
                      std::unique_ptr<VizParam>&, bool) const;
  void plot_title(std::ofstream&, std::string, std::string, bool) const;
  void plot_legend(std::ofstream&, std::unique_ptr<VizParam>&, int64_t) const;
  void set_legend_rect(std::ofstream&, std::unique_ptr<LegendPlot>&,
                       char) const;
  void plot_legend_gradient(std::ofstream&, std::unique_ptr<LegendPlot>&) const;
  void plot_legend_text_horiz(std::ofstream&,
                              std::unique_ptr<LegendPlot>&) const;
  void plot_legend_path_horiz(std::ofstream&,
                              std::unique_ptr<LegendPlot>&) const;
  void plot_legend_text_vert(std::ofstream&,
                             std::unique_ptr<LegendPlot>&) const;
  void plot_legend_path_vert(std::ofstream&,
                             std::unique_ptr<LegendPlot>&) const;
  auto tspan(uint32_t, int64_t) const -> std::string;
  auto tspan(uint32_t, std::string) const -> std::string;
  void sort_merge(std::string&) const;
  void save_n_pos(std::string) const;
  void read_pos(std::ifstream&, std::vector<Position>&,
                std::unique_ptr<VizParam>&) const;
  void make_posNode(const std::vector<Position>&, std::unique_ptr<VizParam>&,
                    std::string&&);
  void plot_pos(std::ofstream&, std::ifstream&, std::vector<Position>&,
                std::unique_ptr<VizParam>&);
  void plot_pos_horizontal(std::ofstream&, std::unique_ptr<PosPlot>&) const;
  void plot_pos_vertical(std::ofstream&, std::unique_ptr<PosPlot>&) const;
  void plot_Ns(std::ofstream&, float, bool) const;
  void plot_seq_borders(std::ofstream&, bool) const;
  void print_log(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) const;
  // void print_pos (ofstream&, VizParam&, const vector<Position>&, uint64_t,
  // string&&);
  void set_n_color(std::vector<Position>&);
};
}  // namespace smashpp

#endif  // SMASHPP_VIZPAINT_HPP