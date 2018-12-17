#ifndef SVG_H_INCLUDED
#define SVG_H_INCLUDED

#include "par.hpp"
#include "exception.hpp"

namespace smashpp {
template <typename Value>
inline static string attrib (const string& name, const Value& value, 
bool precise=false, const string& unit="") {
  stringstream ss;
  if (precise)
    ss << name << "=\"" << PREC << value << unit << "\" ";
  else
    ss << name << "=\"" << value << unit << "\" ";
  return ss.str();
}

inline static string begin_elem (const string& name) {
  return "\t<" + name + " ";
}
inline static string mid_elem () {
  return ">";
}
inline static string end_elem (const string& name) {
  return "</" + name + ">\n";
}
inline static string end_empty_elem () {
  return "/>\n";
}

struct RgbColor {
  u8 r, g, b;
  RgbColor () = default;
  RgbColor (u8 r_, u8 g_, u8 b_) { config(r_,g_,b_); }
  void config (u8 r_, u8 g_, u8 b_) { r=r_;  g=g_;  b=b_; }
};

struct HsvColor {
  u8 h, s, v;
  HsvColor () = default;
  explicit HsvColor (u8 hue) : h(hue), s(PAINT_LVL_SATUR), v(PAINT_LVL_VAL) {}
};

struct HeatmapColor {
  double start, rotations, hue, gamma;
  HeatmapColor () : start(HEAT_START), rotations(HEAT_ROT), hue(HEAT_HUE),
    gamma(HEAT_GAMMA) {}
};

struct Gradient {
  string         startColor, stopColor;
  vector<string> offsetColor;
  Gradient () = default;
  Gradient (string&& start, string&& stop) : startColor(std::move(start)), 
    stopColor(std::move(stop)) {}
};

struct Point {
  double x, y;
  Point () = default;
  Point (double x_, double y_) { config(x_,y_); }
  void config (double x_, double y_) { x=x_;  y=y_; }
};

struct Text {
  Point  origin;
  string label, textAnchor, dominantBaseline, transform, color, fontWeight;
  u8     fontSize;
  Text () : textAnchor("middle"), dominantBaseline("middle"), color("black"),
    fontWeight("normal"), fontSize(13) {}
  Text (const string& lbl_, const string& clr_) : label(lbl_), color(clr_) {}
  void plot (ofstream&) const;
  void plot_title (ofstream&);
  void plot_pos_ref (ofstream&, char c='\0');
  void plot_pos_tar (ofstream&, char c='\0');
};

struct Line {
  Point  beg, end;
  double width;
  string color;
  Line () = default;
  void plot (ofstream&) const;
};

struct Path {
  Point  origin;
  double width;
  string color, trace, strokeLineJoin, strokeDashArray;
  Path () : width(1), strokeLineJoin("round"), strokeDashArray("8 3") {}
  void plot (ofstream&) const;
};

struct Rectangle {
  Point  origin;
  double width, height;
  string color;
  float  opacity;
  Rectangle () : opacity(OPAC) {}
  void plot (ofstream&) const;
  void plot_ir (ofstream&, string&& wave=std::move("#Wavy")) const;
  void plot_oval (ofstream&) const;
  void plot_oval_ir (ofstream&) const;
  void plot_nrc (ofstream&, char) const;
  void plot_nrc_ref (ofstream&) const;
  void plot_nrc_tar (ofstream&) const;
  void plot_redun (ofstream&, u8, char) const;
  void plot_redun_ref (ofstream&, bool) const;
  void plot_redun_tar (ofstream&, bool) const;
  void plot_chromosome (ofstream&) const;
};

struct Polygon {
  Point  one, two, three, four;
  string lineColor, fillColor;
  Polygon () = default;
  void plot (ofstream&) const;
};
}

#endif