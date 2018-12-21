#ifndef SVG_H_INCLUDED
#define SVG_H_INCLUDED

#include "par.hpp"
#include "exception.hpp"

namespace smashpp {
template <typename Value>
inline static string attrib (const string& name, const Value& value, 
bool precise=false, const string& unit="") {
  stringstream ss;
  if (precise)  ss << name << "=\"" << PREC << value << unit << "\" ";
  else          ss << name << "=\"" << value << unit << "\" ";
  return ss.str();
}

inline static string begin_elem (const string& name) {
  return "\n<" + name + " ";
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
  // float alpha;
  RgbColor () = default;
  RgbColor (u8 r_, u8 g_, u8 b_/*, float a=0.5*/) : r(r_), g(g_), b(b_)
    /*, alpha(a)*/ {}
};

struct HsvColor {
  u8 h, s, v;
  HsvColor () = default;
  explicit HsvColor (u8 hue) : h(hue), s(PAINT_LVL_SATUR), v(PAINT_LVL_VAL) {}
};

inline static bool is_hex (const string& color) {
  if (color.front()!='#' || color.size()!=7)          return false;
  for (auto ch : color.substr(1))  if (!isxdigit(ch)) return false;
  return true;
}

inline static RgbColor to_rgb (const string& color) {
  if (is_hex(color)) {
    constexpr int base = 16;
    const string 
      strR=color.substr(1,2), strG=color.substr(3,2), strB=color.substr(5,2);

    return RgbColor(stoi(strR,0,base), stoi(strG,0,base), stoi(strB,0,base));
  }
  else {
    if      (color=="black")    return RgbColor(0,   0,   0);
    else if (color=="white")    return RgbColor(255, 255, 255);
    else if (color=="grey")     return RgbColor(128, 128, 128);
    else if (color=="red")      return RgbColor(255, 0,   0);
    else if (color=="green")    return RgbColor(0,   255, 0);
    else if (color=="blue")     return RgbColor(0,   0,   255);
    else                        error("color undefined");
  }
  return RgbColor();
}

inline static string to_hex_color (const RgbColor& color) {
  return string_format("#%X%X%X", color.r, color.g, color.b);
}

inline static RgbColor alpha_blend (const RgbColor& color1, 
const RgbColor& color2, float alpha) {
  return RgbColor(static_cast<u8>(color1.r + (color2.r-color1.r) * alpha),
                  static_cast<u8>(color1.g + (color2.g-color1.g) * alpha),
                  static_cast<u8>(color1.b + (color2.b-color1.b) * alpha));
}
inline static string alpha_blend (const string& hexColor, 
const RgbColor& color2, float alpha) {
  RgbColor color1 {to_rgb(hexColor)};
  return to_hex_color(alpha_blend(color1, color2, alpha));
}

// Mix whith black
template <typename InColor>
inline static auto shade (const InColor& color, float alpha=0.5) {
  return alpha_blend(color, to_rgb("black"), alpha);
}

// Mix with white
template <typename InColor>
inline static auto tint (const InColor& color, float alpha=0.5) {
  return alpha_blend(color, to_rgb("white"), alpha);
}

// Mix with grey
template <typename InColor>
inline static auto tone (const InColor& color, float alpha=0.5) {
  return alpha_blend(color, to_rgb("grey"), alpha);
}

struct Gradient {
  string startColor, stopColor;
  vector<string> offsetColor;
  Gradient () = default;
  Gradient (string&& start, string&& stop) : startColor(std::move(start)), 
    stopColor(std::move(stop)) {}
};

struct Point {
  double x, y;
  Point () = default;
  Point (double x_, double y_) : x(x_), y(y_) {}
};

struct Text {
  Point  origin;
  string dx, dy;
  string label, textAnchor, dominantBaseline, transform, color, fontWeight;
  u8     fontSize;
  Text () : dx("0"), dy("0"), textAnchor("middle"), dominantBaseline("middle"),
    color("black"), fontWeight("normal"), fontSize(13) {}
  Text (const string& lbl_, const string& clr_) : label(lbl_), color(clr_) {}
  void plot (ofstream&) const;
  void print_title (ofstream&);
  void print_pos_ref (ofstream&, char c='\0');
  void print_pos_tar (ofstream&, char c='\0');
};

struct Line {
  Point  beg, end;
  double strokeWidth;
  string stroke;
  Line () : strokeWidth(1.0), stroke("black") {}
  void plot (ofstream&) const;
};

struct Ellipse {
  double cx, cy, rx, ry;
  double strokeWidth;
  string stroke, fill;
  float  opacity;
  Ellipse () : rx(2.0), ry(2.0), strokeWidth(1.0), stroke("black"), 
    fill("transparent"), opacity(OPAC) {}
  void plot (ofstream&) const;
};

struct Path {
  Point  origin;
  double strokeWidth;
  string id, stroke, d, trace, strokeLineJoin, strokeDashArray, fill, transform;
  float  opacity;
  Path () : strokeWidth(1), d(""), strokeLineJoin("round"), fill("transparent"),
    opacity(OPAC)  {}
  void plot (ofstream&) const;
};

struct Cylinder {
  Point  origin;
  double width, height, ry, strokeWidth;
  string stroke, fill, strokeLineJoin, strokeDashArray, transform;
  float  opacity;
  Cylinder () : ry(2.0), strokeWidth(1.0), stroke("black"), 
    fill("transparent"), strokeLineJoin("round"), opacity(OPAC) {}
  void plot (ofstream&) const;
  void plot_ir (ofstream&, string&& wave=std::move("#Wavy"));
  void plot_periph (ofstream&, char=' ', u8=0);
};

struct Rectangle {
  Point  origin;
  double width, height, ry;
  string fill, stroke;
  float  opacity, strokeWidth;
  Rectangle () : ry(1), opacity(OPAC) {}
  void plot (ofstream&) const;
};

struct Polygon {
  Point  one, two, three, four;
  string points, lineColor, fillColor;
  float  stroke_width, stroke_opacity, fill_opacity;
  Polygon () : stroke_width(1), stroke_opacity(0.5), fill_opacity(0.5) {}
  void add_point (double, double);
  void plot (ofstream&);
};

struct Pattern {
  string id, patternUnits;
  double x, y, width, height;
  Pattern () = default;
  void set_head (ofstream&) const;
  void set_tail (ofstream&) const;
};

struct Defs {
  string id;
  Defs () = default;
  void set_head (ofstream&) const;
  void set_tail (ofstream&) const;
};

template <typename T>
inline static void make_pattern (ofstream& file, unique_ptr<Defs>& defs, 
unique_ptr<Pattern>& pattern, unique_ptr<T>& figBase) {
  defs->set_head(file);
  pattern->set_head(file);
  figBase->plot(file);
  pattern->set_tail(file);
  defs->set_tail(file);
}
template <typename T, typename... Ts>
inline static void make_pattern (ofstream& file, unique_ptr<Defs>& defs, 
unique_ptr<Pattern>& pattern, unique_ptr<T>& figBase, unique_ptr<Ts...>& fig) {
  defs->set_head(file);
  pattern->set_head(file);
  figBase->plot(file);
  fig->plot(file);
  pattern->set_tail(file);
  defs->set_tail(file);
}
}

#endif