#ifndef SVG_H_INCLUDED
#define SVG_H_INCLUDED

#include "par.hpp"
#include "exception.hpp"

namespace smashpp {
class SVG {
 public:
  string id;
  float  width, height;

  SVG () = default;
  string attrib (const string&, float, bool=false, const string& ="") const;
  string attrib (const string&, const string&, bool=false, 
    const string& ="") const;
  string begin_elem (const string&) const;
  string mid_elem () const;
  string end_elem (const string&) const;
  string end_empty_elem () const;

  void print_header (ofstream&) const;
  void print_tailer (ofstream&) const;
};

// struct RgbColor : public SVG {
//   u8 r, g, b;
//   // float alpha;
//   RgbColor () = default;
//   RgbColor (u8 r_, u8 g_, u8 b_/*, float a=0.5*/) : r(r_), g(g_), b(b_)
//     /*, alpha(a)*/ {}
// };

// struct HsvColor : public SVG {
//   u8 h, s, v;
//   HsvColor () = default;
//   explicit HsvColor (u8 hue) : h(hue), s(PAINT_LVL_SATUR), v(PAINT_LVL_VAL) {}
// };

// inline static bool is_hex (const string& color) {
//   if (color.front()!='#' || color.size()!=7)          return false;
//   for (auto ch : color.substr(1))  if (!isxdigit(ch)) return false;
//   return true;
// }

// inline static RgbColor to_rgb (const string& color) {
//   if (is_hex(color)) {
//     constexpr int base = 16;
//     const string 
//       strR=color.substr(1,2), strG=color.substr(3,2), strB=color.substr(5,2);

//     return RgbColor(stoi(strR,0,base), stoi(strG,0,base), stoi(strB,0,base));
//   }
//   else {
//     if      (color=="black")    return RgbColor(0,   0,   0);
//     else if (color=="white")    return RgbColor(255, 255, 255);
//     else if (color=="grey")     return RgbColor(128, 128, 128);
//     else if (color=="red")      return RgbColor(255, 0,   0);
//     else if (color=="green")    return RgbColor(0,   255, 0);
//     else if (color=="blue")     return RgbColor(0,   0,   255);
//     else                        error("color undefined");
//   }
//   return RgbColor();
// }

// inline static string to_hex (const RgbColor& color) {
//   return string_format("#%X%X%X", color.r, color.g, color.b);
// }

// inline static RgbColor alpha_blend (const RgbColor& color1, 
// const RgbColor& color2, float alpha) {
//   return RgbColor(static_cast<u8>(color1.r + (color2.r-color1.r) * alpha),
//                   static_cast<u8>(color1.g + (color2.g-color1.g) * alpha),
//                   static_cast<u8>(color1.b + (color2.b-color1.b) * alpha));
// }
// inline static string alpha_blend (const string& hexColor, 
// const RgbColor& color2, float alpha) {
//   RgbColor color1 {to_rgb(hexColor)};
//   return to_hex(alpha_blend(color1, color2, alpha));
// }

// // Mix whith black
// template <typename InColor>
// inline static auto shade (const InColor& color, float alpha=0.5) {
//   return alpha_blend(color, to_rgb("black"), alpha);
// }

// // Mix with white
// template <typename InColor>
// inline static auto tint (const InColor& color, float alpha=0.5) {
//   return alpha_blend(color, to_rgb("white"), alpha);
// }

// // Mix with grey
// template <typename InColor>
// inline static auto tone (const InColor& color, float alpha=0.5) {
//   return alpha_blend(color, to_rgb("grey"), alpha);
// }

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
  u8     fontSize;
  string Label;  // Not in standard

  Text () : dx(0), dy(0), dominant_baseline("middle"), font_weight("normal"),
    font_family("Arial"), fill("black"), text_anchor("middle"), 
    text_align("start"), line_height("125%%"), fontSize(13) {}
  void plot (ofstream&) const;
  void print_title (ofstream&);
  void print_pos_ref (ofstream&, char c='\0');
  void print_pos_tar (ofstream&, char c='\0');
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
  string stroke, fill;
  
  Ellipse () : rx(2.0f), ry(2.0f), stroke_width(1.0f), fill_opacity(OPAC),
    stroke_opacity(1.0f), stroke("black"), fill("transparent") {}
  void plot (ofstream&) const;
};

class Path : public SVG {
 public:
  string id, d, fill, stroke, stroke_lineJoin, stroke_dasharray, transform;
  float  fill_opacity, stroke_opacity, stroke_width;

  Path () : fill("transparent"), stroke_lineJoin("round"), fill_opacity(OPAC),
    stroke_opacity(1.0f), stroke_width(1.0f) {}
  void plot (ofstream&) const;
};

// Not in standard
class Cylinder : public SVG {
 public:
  float  x, y, width, height, ry, stroke_width, fill_opacity, stroke_opacity;
  string id, stroke, fill, stroke_lineJoin, stroke_dasharray, transform;

  Cylinder () : ry(2.0f), stroke_width(1.0f), fill_opacity(OPAC), 
    stroke_opacity(1.0f), stroke("black"), fill("transparent"), 
    stroke_lineJoin("round") {}
  void plot (ofstream&) const;
  void plot_ir (ofstream&, string&& wave=std::move("#Wavy"));
  void plot_periph (ofstream&, char=' ', u8=0);
};

class Rectangle : public SVG {
 public:
  float  x, y, width, height, rx, ry, fill_opacity, stroke_width;
  string fill, stroke;

  Rectangle () : ry(1.0f), fill_opacity(OPAC) {}
  void plot (ofstream&) const;
};

class Polygon : public SVG {
 public:
  float  stroke_width, stroke_opacity, fill_opacity;
  string fill, stroke;
 
  Polygon () : stroke_width(1.0f), stroke_opacity(0.5f), fill_opacity(0.5f) {}
  void add_point (float, float);
  void plot (ofstream&);

 private:
  string points;
};

class Pattern : public SVG {
 public:
  float x, y, width, height;
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