// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_SVG_HPP
#define SMASHPP_SVG_HPP

#include "par.hpp"
#include "exception.hpp"
#include "number.hpp"
// #include "color.hpp"

namespace smashpp {  
static constexpr uint8_t PREC_VIZ{2};  // Precision

class SVG {
 public:
  std::string id;
  std::string filter;
  float width;
  float height;
  std::string viewBox;

  SVG() : width(0.0f), height(0.0f), viewBox("0 0 100% 100%") {}
  auto attr(std::string, float, bool = false, std::string = "") const
      -> std::string;
  auto attr(std::string, const std::string&, bool = false,
            std::string = "") const -> std::string;
  auto begin_elem(std::string) const -> std::string;
  auto mid_elem() const -> std::string;
  auto end_elem(std::string) const -> std::string;
  auto end_empty_elem() const -> std::string;
  void print_header(std::ofstream&) const;
  void print_tailer(std::ofstream&) const;
};

class Stop : public SVG {
 public:
  std::string offset;
  std::string stop_color;
  float stop_opacity;

  Stop() : stop_opacity(1) {}
  void plot(std::ofstream&) const;
  void plot(std::stringstream&) const;
};

class LinearGradient : public SVG {
 public:
  std::string x1;
  std::string y1;
  std::string x2;
  std::string y2;

  LinearGradient() : x1("0%"), y1("0%"), x2("100%"), y2("0%") {}
  void plot(std::ofstream&) const;
  void add_stop(std::unique_ptr<Stop>&);
  void add_stop(std::string, std::string);

 private:
  std::string stops;
};

class Text : public SVG {
 public:
  float x;
  float y;
  float dx;
  float dy;
  std::string dominant_baseline;
  std::string baseline_shift;
  std::string alignment_baseline;
  std::string transform;
  std::string font_weight;
  std::string font_family;
  std::string font_variant;
  std::string fill;
  std::string text_anchor;
  std::string text_align;
  std::string line_height;
  uint8_t font_size;
  std::string Label;  // Not in standard

  Text()
      : dx(0),
        dy(0),
        font_family("Helvetica,Arial"),
        font_variant("normal"),
        text_anchor("middle"),
        text_align("start"),
        line_height("125%%"),
        font_size(13) {}
  void plot(std::ofstream&) const;
  void plot_shadow(std::ofstream&, std::string = "grey");
};

class Line : public SVG {
 public:
  float x1;
  float y1;
  float x2;
  float y2;
  float stroke_width;
  std::string stroke;

  Line() : stroke_width(1.0f), stroke("black") {}
  void plot(std::ofstream&) const;
};

class Ellipse : public SVG {
 public:
  float cx;
  float cy;
  float rx;
  float ry;
  float stroke_width;
  float fill_opacity;
  float stroke_opacity;
  std::string stroke;
  std::string fill;
  std::string stroke_dasharray;
  std::string transform;

  Ellipse()
      : rx(2.0f),
        ry(2.0f),
        stroke_width(1.0f),
        fill_opacity(OPAC),
        stroke_opacity(1.0f),
        stroke("black"),
        fill("none"),
        stroke_dasharray(""),
        transform("") {}
  void plot(std::ofstream&) const;
};

class Path : public SVG {
 public:
  std::string id;
  std::string d;
  std::string fill;
  std::string stroke;
  std::string stroke_lineJoin;
  std::string stroke_linecap;
  std::string stroke_dasharray;
  std::string transform;
  std::string filter;
  float fill_opacity;
  float stroke_opacity;
  float stroke_width;

  Path()
      : fill("none"),
        stroke_lineJoin("round"),
        stroke_linecap("butt"),
        stroke_dasharray(""),
        filter(""),
        fill_opacity(OPAC),
        stroke_opacity(1.0f),
        stroke_width(1.0f) {}
  auto M(float, float) const -> std::string;
  auto m(float, float) const -> std::string;
  auto L(float, float) const -> std::string;
  auto l(float, float) const -> std::string;
  auto H(float) const -> std::string;
  auto h(float) const -> std::string;
  auto V(float) const -> std::string;
  auto v(float) const -> std::string;
  auto C(float, float, float, float, float, float) const -> std::string;
  auto c(float, float, float, float, float, float) const -> std::string;
  auto S(float, float, float, float) const -> std::string;
  auto s(float, float, float, float) const -> std::string;
  auto Q(float, float, float, float) const -> std::string;
  auto q(float, float, float, float) const -> std::string;
  auto T(float, float) const -> std::string;
  auto t(float, float) const -> std::string;
  auto A(float, float, float, uint8_t, uint8_t, float, float) const
      -> std::string;
  auto a(float, float, float, uint8_t, uint8_t, float, float) const
      -> std::string;
  auto Z() const -> std::string;
  auto z() const -> std::string;
  void plot(std::ofstream&) const;
  void plot_shadow(std::ofstream&, std::string = "#d0d0ff");
};

// Not in standard
class Cylinder : public SVG {
 public:
  float x;
  float y;
  float width;
  float height;
  float ry;
  float stroke_width;
  float fill_opacity;
  float stroke_opacity;
  std::string id;
  std::string stroke;
  std::string fill;
  std::string stroke_lineJoin;
  std::string stroke_dasharray;
  std::string transform;

  Cylinder()
      // : ry(2.0f),
      : ry(5.f),//todo
        stroke_width(1.0f),
        fill_opacity(OPAC),
        stroke_opacity(1.0f),
        stroke("black"),
        fill("none"),
        stroke_lineJoin("round"),
        stroke_dasharray("") {}
  void plot(std::ofstream&) const;
  void plot_ir(std::ofstream&, std::string = "Wavy");
};

class Rectangle : public SVG {
 public:
  float x;
  float y;
  float width;
  float height;
  float rx;
  float ry;
  float fill_opacity;
  float stroke_width;
  float stroke_opacity;
  std::string fill;
  std::string stroke;
  std::string transform;

  Rectangle() : rx(1.0f), ry(1.0f), fill_opacity(OPAC), stroke_opacity(1.0f) {}
  void plot(std::ofstream&) const;
  void plot_ir(std::ofstream&, std::string = "Wavy", std::string="");
};

class Polygon : public SVG {
 public:
  float stroke_width;
  float stroke_opacity;
  float fill_opacity;
  std::string points;
  std::string fill;
  std::string stroke;

  Polygon() : stroke_width(1.0f), stroke_opacity(0.5f), fill_opacity(0.5f) {}
  auto point(float, float) const -> std::string;
  void plot(std::ofstream&);
};

class Pattern : public SVG {
 public:
  float x;
  float y;
  float width;
  float height;
  std::string id;
  std::string patternUnits;

  Pattern() = default;
  void set_head(std::ofstream&) const;
  void set_tail(std::ofstream&) const;
};

class Defs : public SVG {
 public:
  Defs() = default;
  void set_head(std::ofstream&) const;
  void set_tail(std::ofstream&) const;
};

// Standard name of SVG element is "filter"
class FilterSVG : public SVG {
 public:
  std::string x;
  std::string y;
  std::string width;
  std::string height;

  FilterSVG() : x("0%"), y("0%"), width("100%"), height("100%") {}
  void set_head(std::ofstream&) const;
  void set_tail(std::ofstream&) const;
};

class FeGaussianBlur : public SVG {
 public:
  std::string in;
  std::string stdDeviation;
  std::string result;

  FeGaussianBlur() : in("SourceAlpha"), stdDeviation("3") {}
  void plot(std::ofstream&) const;
};

class FeOffset : public SVG {
 public:
  float dx;
  float dy;

  FeOffset() = default;
  void plot(std::ofstream&) const;
};

class FeMerge : public SVG {
 public:
  FeMerge() = default;
  void set_head(std::ofstream&) const;
  void set_tail(std::ofstream&) const;
};

class FeMergeNode : public SVG {
 public:
  std::string in;

  FeMergeNode() : in("SourceGraphic") {}
  void plot(std::ofstream&) const;
};

template <typename T>
void make_pattern(std::ofstream& file, std::unique_ptr<Pattern>& pattern,
                  std::unique_ptr<T>& figBase) {
  auto defs = std::make_unique<Defs>();
  defs->set_head(file);
  pattern->set_head(file);
  figBase->plot(file);
  pattern->set_tail(file);
  defs->set_tail(file);
}
template <typename T, typename... Ts>
void make_pattern(std::ofstream& file, std::unique_ptr<Pattern>& pattern,
                  std::unique_ptr<T>& figBase, std::unique_ptr<Ts...>& fig) {
  auto defs = std::make_unique<Defs>();
  defs->set_head(file);
  pattern->set_head(file);
  figBase->plot(file);
  fig->plot(file);
  pattern->set_tail(file);
  defs->set_tail(file);
}

inline std::string text_shadow(std::ofstream& file) {
  auto filter = std::make_unique<FilterSVG>();
  filter->id = "textShadow";
  filter->x = "-20%";
  filter->y = "-20%";
  filter->width = "140%";
  filter->height = "140%";
  filter->set_head(file);

  auto feGaussianBlur = std::make_unique<FeGaussianBlur>();
  feGaussianBlur->stdDeviation = "0.0";
  feGaussianBlur->result = "textShadow";
  feGaussianBlur->plot(file);

  auto feOffset = std::make_unique<FeOffset>();
  feOffset->dx = 0;
  feOffset->dy = 0;
  feOffset->plot(file);

  filter->set_tail(file);

  // // Apply filter on object (text, shape, ...)
  // auto filter = make_unique<FilterSVG>();
  // filter->id = "dropShadow";
  // filter->set_head(file);

  // auto feGaussianBlur = make_unique<FeGaussianBlur>();
  // feGaussianBlur->in = "SourceAlpha";
  // feGaussianBlur->stdDeviation = "3";
  // feGaussianBlur->plot(file);

  // auto feOffset = make_unique<FeOffset>();
  // feOffset->dx = 2;
  // feOffset->dy = 4;
  // feOffset->plot(file);

  // auto feMerge = make_unique<FeMerge>();
  // feMerge->set_head(file);

  // auto feMergeNode = make_unique<FeMergeNode>();
  // feMergeNode->plot(file);

  // feMergeNode->in = "SourceGraphic";
  // feMergeNode->plot(file);

  // feMerge->set_tail(file);
  // filter->set_tail(file);

  return filter->id;
}

inline std::string path_shadow(std::ofstream& file) {
  auto filter = std::make_unique<FilterSVG>();
  filter->id = "pathShadow";
  filter->x = "-3%";
  filter->y = "-3%";
  filter->width = "106%";
  filter->height = "106%";
  filter->set_head(file);

  auto feGaussianBlur = std::make_unique<FeGaussianBlur>();
  feGaussianBlur->stdDeviation = "0.5";
  feGaussianBlur->result = "pathShadow";
  feGaussianBlur->plot(file);

  auto feOffset = std::make_unique<FeOffset>();
  feOffset->dx = 0;
  feOffset->dy = 0;
  feOffset->plot(file);

  filter->set_tail(file);

  return filter->id;
}
}  // namespace smashpp

#endif // SMASHPP_SVG_HPP