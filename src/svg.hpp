#ifndef SVG_H_INCLUDED
#define SVG_H_INCLUDED

#include "par.hpp"
#include "exception.hpp"
#include "number.hpp"

namespace smashpp {
class SVG {
 public:
  string id, filter;
  float  width, height;

  SVG () = default;
  auto attr (const string&, float, bool=false, const string& ="") const
    -> string;
  auto attr (const string&, const string&, bool=false, const string& ="") const
    -> string;
  auto begin_elem (const string&) const -> string;
  auto mid_elem () const -> string;
  auto end_elem (const string&) const -> string;
  auto end_empty_elem () const -> string;
  void print_header (ofstream&) const;
  void print_tailer (ofstream&) const;
};

class Stop : public SVG {
 public:
  string offset, stop_color;
  float  stop_opacity;

  Stop () : stop_opacity(1) {}
  void plot (ofstream&) const;
  void plot (stringstream&) const;
};

class LinearGradient : public SVG {
 public:
  string x1, y1, x2, y2;
  
  LinearGradient () : x1("0%"), y1("0%"), x2("100%"), y2("0%") {}
  void plot (ofstream&) const;
  void add_stop (unique_ptr<Stop>&);
  void add_stop (const string&, const string&);

 private:
  string stops;
};

class Text : public SVG {
 public:
  float  x, y, dx, dy;
  string dominant_baseline, transform, font_weight, font_family, fill, 
         text_anchor, text_align, line_height;
  u8     font_size;
  string Label;       // Not in standard

  Text () : dx(0), dy(0), dominant_baseline("middle"), font_family("Arial"),
    text_anchor("middle"), text_align("start"), line_height("125%%"), 
    font_size(13) {}
  void plot (ofstream&) const;
  void plot_shadow (ofstream&, const string& ="grey");
};

class Line : public SVG {
 public:
  float  x1, y1, x2, y2, stroke_width;
  string stroke;

  Line () : stroke_width(1.0f), stroke("black") {}
  void plot (ofstream&) const;
};

class Ellipse : public SVG {
 public:
  float  cx, cy, rx, ry, stroke_width, fill_opacity, stroke_opacity;
  string stroke, fill, stroke_dasharray, transform;
  
  Ellipse () : rx(2.0f), ry(2.0f), stroke_width(1.0f), fill_opacity(OPAC),
    stroke_opacity(1.0f), stroke("black"), fill("transparent"), 
    stroke_dasharray(""), transform("") {}
  void plot (ofstream&) const;
};

class Path : public SVG {
 public:
  string id, d, fill, stroke, stroke_lineJoin, stroke_linecap, stroke_dasharray,
         transform, filter;
  float  fill_opacity, stroke_opacity, stroke_width;

  Path () : fill("transparent"), stroke_lineJoin("round"), 
    stroke_linecap("butt"), stroke_dasharray(""), filter(""), 
    fill_opacity(OPAC), stroke_opacity(1.0f), stroke_width(1.0f) {}
  auto M (float, float)                              const -> string;
  auto m (float, float)                              const -> string;
  auto L (float, float)                              const -> string;
  auto l (float, float)                              const -> string;
  auto H (float)                                     const -> string;
  auto h (float)                                     const -> string;
  auto V (float)                                     const -> string;
  auto v (float)                                     const -> string;
  auto C (float, float, float, float, float, float)  const -> string;
  auto c (float, float, float, float, float, float)  const -> string;
  auto S (float, float, float, float)                const -> string;
  auto s (float, float, float, float)                const -> string;
  auto Q (float, float, float, float)                const -> string;
  auto q (float, float, float, float)                const -> string;
  auto T (float, float)                              const -> string;
  auto t (float, float)                              const -> string;
  auto A (float, float, float, u8, u8, float, float) const -> string;
  auto a (float, float, float, u8, u8, float, float) const -> string;
  auto Z ()                                          const -> string;
  auto z ()                                          const -> string;
  void plot (ofstream&) const;
  void plot_shadow (ofstream&, const string& ="#d0d0ff");
};

// Not in standard
class Cylinder : public SVG {
 public:
  float  x, y, width, height, ry, stroke_width, fill_opacity, stroke_opacity;
  string id, stroke, fill, stroke_lineJoin, stroke_dasharray, transform;

  Cylinder () : ry(2.0f), stroke_width(1.0f), fill_opacity(OPAC), 
    stroke_opacity(1.0f), stroke("black"), fill("transparent"), 
    stroke_lineJoin("round"), stroke_dasharray("") {}
  void plot (ofstream&) const;
  void plot_ir (ofstream&, const string& ="Wavy");
};

class Rectangle : public SVG {
 public:
  float  x, y, width, height, rx, ry, fill_opacity, stroke_width;
  string fill, stroke;

  Rectangle () : rx(1.0f), ry(1.0f), fill_opacity(OPAC) {}
  void plot (ofstream&) const;
};

class Polygon : public SVG {
 public:
  float  stroke_width, stroke_opacity, fill_opacity;
  string points, fill, stroke;
 
  Polygon () : stroke_width(1.0f), stroke_opacity(0.5f), fill_opacity(0.5f) {}
  string point (float, float) const;
  void plot (ofstream&);
};

class Pattern : public SVG {
 public:
  float  x, y, width, height;
  string id, patternUnits;

  Pattern () = default;
  void set_head (ofstream&) const;
  void set_tail (ofstream&) const;
};

class Defs : public SVG {
 public:
  Defs () = default;
  void set_head (ofstream&) const;
  void set_tail (ofstream&) const;
};

// Standard name of SVG element is filter
class FilterSVG : public SVG {
 public:
  string x, y, width, height;

  FilterSVG () : x("0%"), y("0%"), width("100%"), height("100%") {}
  void set_head (ofstream&) const;
  void set_tail (ofstream&) const;
};

class FeGaussianBlur : public SVG {
 public:
  string in, stdDeviation, result;

  FeGaussianBlur () : in("SourceAlpha"), stdDeviation("3") {}
  void plot (ofstream&) const;
};

class FeOffset : public SVG {
 public:
  float dx, dy;

  FeOffset () = default;
  void plot (ofstream&) const;
};

class FeMerge : public SVG {
 public:
  FeMerge () = default;
  void set_head (ofstream&) const;
  void set_tail (ofstream&) const;
};

class FeMergeNode : public SVG {
 public:
  string in;

  FeMergeNode () : in("SourceGraphic") {}
  void plot (ofstream&) const;
};

template <typename T>
void make_pattern (ofstream& file, unique_ptr<Pattern>& pattern, 
unique_ptr<T>& figBase) {
  auto defs = make_unique<Defs>();
  defs->set_head(file);
  pattern->set_head(file);
  figBase->plot(file);
  pattern->set_tail(file);
  defs->set_tail(file);
}
template <typename T, typename... Ts>
void make_pattern (ofstream& file, unique_ptr<Pattern>& pattern, 
unique_ptr<T>& figBase, unique_ptr<Ts...>& fig) {
  auto defs = make_unique<Defs>();
  defs->set_head(file);
  pattern->set_head(file);
  figBase->plot(file);
  fig->plot(file);
  pattern->set_tail(file);
  defs->set_tail(file);
}

inline string text_shadow (ofstream& file) {
  auto filter = make_unique<FilterSVG>();
  filter->id = "textShadow";
  filter->x = "-20%";
  filter->y = "-20%";
  filter->width = "140%";
  filter->height = "140%";
  filter->set_head(file);

  auto feGaussianBlur = make_unique<FeGaussianBlur>();
  feGaussianBlur->stdDeviation = "0.0";
  feGaussianBlur->result = "textShadow";
  feGaussianBlur->plot(file);

  auto feOffset = make_unique<FeOffset>();
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

inline string path_shadow (ofstream& file) {
  auto filter = make_unique<FilterSVG>();
  filter->id = "pathShadow";
  filter->x = "-3%";
  filter->y = "-3%";
  filter->width = "106%";
  filter->height = "106%";
  filter->set_head(file);

  auto feGaussianBlur = make_unique<FeGaussianBlur>();
  feGaussianBlur->stdDeviation = "0.5";
  feGaussianBlur->result = "pathShadow";
  feGaussianBlur->plot(file);

  auto feOffset = make_unique<FeOffset>();
  feOffset->dx = 0;
  feOffset->dy = 0;
  feOffset->plot(file);

  filter->set_tail(file);

  return filter->id;
}
}

#endif