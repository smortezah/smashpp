#include <algorithm>
#include "svg.hpp"
#include "vizdef.hpp"
#include "color.hpp"
using namespace smashpp;

string SVG::attrib (const string& name, float value, bool precise,
const string& unit) const {
  stringstream ss;
  if (precise)  
    ss << name << "=\"" << fixed_precision(PREC_VIZ) << value << unit << "\" ";
  else          
    ss << name << "=\"" << value << unit << "\" ";
  return ss.str();
}
string SVG::attrib (const string& name, const string& value, bool precise,
const string& unit) const {
  stringstream ss;
  ss << name << "=\"" << value << unit << "\" ";
  return ss.str();
}

string SVG::begin_elem (const string& name) const {
  return "<" + name + " ";
}

string SVG::mid_elem () const {
  return ">\n";
}

string SVG::end_elem (const string& name) const {
  return "</" + name + ">\n";
}

string SVG::end_empty_elem () const {
  return "/>\n";
}

void SVG::print_header (ofstream& f) const {
  f << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    << "<!-- Morteza Hosseini, IEETA " << DEV_YEARS << " -->\n"
    << begin_elem("svg")
    << attrib("xmlns", "http://www.w3.org/2000/svg")
    << attrib("xmlns:xlink", "http://www.w3.org/1999/xlink")
    << attrib("width", width, true)
    << attrib("height", height, true)
    << mid_elem();
}

void SVG::print_tailer (ofstream& f) const {
  f << "</svg>";
  // f << "</g>\n</svg>";
}

void Stop::plot (ofstream& f) const {
  f << begin_elem("stop")
    << attrib("offset", offset)
    << attrib("stop-color", stop_color)
    << attrib("stop-opacity", stop_opacity, true)
    << end_empty_elem();
}
void Stop::plot (stringstream& s) const {
  s << begin_elem("stop")
    << attrib("offset", offset)
    << attrib("stop-color", stop_color)
    << attrib("stop-opacity", stop_opacity, true)
    << end_empty_elem();
}

void LinearGradient::plot (ofstream& f) const {
  auto defs = make_unique<Defs>();
  defs->set_head(f);

  f << begin_elem("linearGradient")
    << attrib("id", id)
    << attrib("x1", x1)
    << attrib("y1", y1)
    << attrib("x2", x2)
    << attrib("y2", y2)
    << mid_elem();

  f << stops;

  f << end_elem("linearGradient");
  defs->set_tail(f);
}

void LinearGradient::add_stop (unique_ptr<Stop>& stop) {
  stringstream ss;
  stop->plot(ss);
  stops += ss.str();
}
void LinearGradient::add_stop (const string& offset, const string& stop_color) {
  auto stop = make_unique<Stop>();
  stop->offset = offset;
  stop->stop_color = stop_color;
  
  stringstream ss;
  stop->plot(ss);

  stops += ss.str();
}

void Text::plot (ofstream& f) const {
  f << begin_elem("text")
    << attrib("id", to_string(x)+to_string(y))
    << attrib("x", x, true)
    << attrib("y", y, true);
  if (dx!=0.0f)  f << attrib("dx", dx, true);
  if (dy!=0.0f)  f << attrib("dy", dy, true);
  f << attrib("dominant-baseline", dominant_baseline);
  if (!transform.empty())    f << attrib("transform", transform);
  f << attrib("font-size", to_string(font_size), false, "px");
  if (!font_weight.empty())  f << attrib("font-weight", font_weight);
  f << attrib("font-family", font_family);
  if (!fill.empty() && fill!="black")  f << attrib("fill", fill);
  f << attrib("text-anchor", text_anchor)
    << attrib("text-align", text_align)
    << attrib("line-height", line_height);
  if (!filter.empty())  f << attrib("filter", "url(#"+filter+")");
  f << mid_elem()
    << Label
    << end_elem("text");
}

void Text::plot_shadow (ofstream& f, const string& shadowFill) {
  // Filter
  filter = text_shadow(f);
  fill = shadowFill;
  plot(f);

  // Essential
  filter.clear();
  fill = "black";
}

void Line::plot (ofstream& f) const {
  f << begin_elem("line")
    << attrib("id", to_string(x1)+to_string(y1)+to_string(x2)+to_string(y2))
    << attrib("x1", x1, true)
    << attrib("y1", y1, true)
    << attrib("x2", x2, true) 
    << attrib("y2", y2, true)
    << attrib("stroke-width", stroke_width, true)
    << attrib("stroke", stroke)
    << end_empty_elem();
}

void Ellipse::plot (ofstream& f) const {
  f << begin_elem("ellipse")
    << attrib("cx", cx, true)
    << attrib("cy", cy, true)
    << attrib("rx", rx, true)
    << attrib("ry", ry, true)
    << attrib("stroke-width", stroke_width, true)
    << attrib("fill-opacity", fill_opacity, true)
    << attrib("stroke-opacity", stroke_opacity, true)
    << attrib("stroke", stroke)
    << attrib("fill", fill);
  if (!transform.empty())  f << attrib("transform", transform);
  f << end_empty_elem();
}

string Path::M (float x, float y) const {
  stringstream ss;
  ss << "M " << fixed_precision(PREC_VIZ) << x << "," 
             << fixed_precision(PREC_VIZ) << y << "\n";
  return ss.str();
}

string Path::m (float dx, float dy) const {
  stringstream ss;
  ss << "m " << fixed_precision(PREC_VIZ) << dx << "," 
             << fixed_precision(PREC_VIZ) << dy << "\n";
  return ss.str();
}

string Path::L (float x, float y) const {
  stringstream ss;
  ss << "L " << fixed_precision(PREC_VIZ) << x << "," 
             << fixed_precision(PREC_VIZ) << y << "\n";
  return ss.str();
}

string Path::l (float dx, float dy) const {
  stringstream ss;
  ss << "l " << fixed_precision(PREC_VIZ) << dx << "," 
             << fixed_precision(PREC_VIZ) << dy << "\n";
  return ss.str();
}

string Path::H (float x) const {
  stringstream ss;
  ss << "H " << fixed_precision(PREC_VIZ) << x << "\n";
  return ss.str();
}

string Path::h (float dx) const {
  stringstream ss;
  ss << "h " << fixed_precision(PREC_VIZ) << dx << "\n";
  return ss.str();
}

string Path::V (float y) const {
  stringstream ss;
  ss << "V " << fixed_precision(PREC_VIZ) << y << "\n";
  return ss.str();
}

string Path::v (float dy) const {
  stringstream ss;
  ss << "v " << fixed_precision(PREC_VIZ) << dy << "\n";
  return ss.str();
}

string Path::C (float x1, float y1, float x2, float y2, float x, float y) const{
  stringstream ss;
  ss << "C " << fixed_precision(PREC_VIZ) << x1 << "," 
             << fixed_precision(PREC_VIZ) << y1 << " "
             << fixed_precision(PREC_VIZ) << x2 << ","
             << fixed_precision(PREC_VIZ) << y2 << " "
             << fixed_precision(PREC_VIZ) << x  << ","
             << fixed_precision(PREC_VIZ) << y  << "\n";
  return ss.str();
}

string Path::c (float dx1, float dy1, float dx2, float dy2, float dx, float dy) 
const {
  stringstream ss;
  ss << "c " << fixed_precision(PREC_VIZ) << dx1 << "," 
             << fixed_precision(PREC_VIZ) << dy1 << " "
             << fixed_precision(PREC_VIZ) << dx2 << "," 
             << fixed_precision(PREC_VIZ) << dy2 << " "
             << fixed_precision(PREC_VIZ) << dx  << "," 
             << fixed_precision(PREC_VIZ) << dy  << "\n";
  return ss.str();
}

string Path::S (float x2, float y2, float x, float y) const {
  stringstream ss;
  ss << "S " << fixed_precision(PREC_VIZ) << x2 << "," 
             << fixed_precision(PREC_VIZ) << y2 << " "
             << fixed_precision(PREC_VIZ) << x  << "," 
             << fixed_precision(PREC_VIZ) << y  << "\n";
  return ss.str();
}

string Path::s (float dx2, float dy2, float dx, float dy) const {
  stringstream ss;
  ss << "s " << fixed_precision(PREC_VIZ) << dx2 << ","
             << fixed_precision(PREC_VIZ) << dy2 << " "
             << fixed_precision(PREC_VIZ) << dx  << "," 
             << fixed_precision(PREC_VIZ) << dy  << "\n";
  return ss.str();
}

string Path::Q (float x1, float y1, float x, float y) const {
  stringstream ss;
  ss << "Q " << fixed_precision(PREC_VIZ) << x1 << "," 
             << fixed_precision(PREC_VIZ) << y1 << " "
             << fixed_precision(PREC_VIZ) << x  << "," 
             << fixed_precision(PREC_VIZ) << y  << "\n";
  return ss.str();
}

string Path::q (float dx1, float dy1, float dx, float dy) const {
  stringstream ss;
  ss << "q " << fixed_precision(PREC_VIZ) << dx1 << ","
             << fixed_precision(PREC_VIZ) << dy1 << " "
             << fixed_precision(PREC_VIZ) << dx  << "," 
             << fixed_precision(PREC_VIZ) << dy  << "\n";
  return ss.str();
}

string Path::T (float x, float y) const {
  stringstream ss;
  ss << "T " << fixed_precision(PREC_VIZ) << x << "," 
             << fixed_precision(PREC_VIZ) << y << "\n";
  return ss.str();
}

string Path::t (float dx, float dy) const {
  stringstream ss;
  ss << "t " << fixed_precision(PREC_VIZ) << dx << "," 
             << fixed_precision(PREC_VIZ) << dy << "\n";
  return ss.str();
}

string Path::A (float rx, float ry, float angle, u8 large_arc_flag, 
u8 sweep_flag, float x, float y) const {
  stringstream ss;
  ss << "A " << fixed_precision(PREC_VIZ) << rx << "," 
             << fixed_precision(PREC_VIZ) << ry << " "
             << fixed_precision(PREC_VIZ) << angle << " "
             << u16(large_arc_flag) << "," << u16(sweep_flag) << " "
             << fixed_precision(PREC_VIZ) << x << "," 
             << fixed_precision(PREC_VIZ) << y << "\n";
  return ss.str();
}

string Path::a (float rx, float ry, float angle, u8 large_arc_flag, 
u8 sweep_flag, float dx, float dy) const {
  stringstream ss;
  ss << "a " << fixed_precision(PREC_VIZ) << rx << "," 
             << fixed_precision(PREC_VIZ) << ry << " "
             << fixed_precision(PREC_VIZ) << angle << " "
             << u16(large_arc_flag) << "," << u16(sweep_flag) << " "
             << fixed_precision(PREC_VIZ) << dx << "," 
             << fixed_precision(PREC_VIZ) << dy << "\n";
  return ss.str();
}

string Path::Z () const {
  return "Z\n";
}

string Path::z () const {
  return "z\n";
}

void Path::plot (ofstream& f) const {
  f << begin_elem("path")
    << attrib("id", id)
    << attrib("d", d)
    << attrib("fill", fill);
  if (!filter.empty())  f << attrib("filter", "url(#"+filter+")");
  f << attrib("fill-opacity", fill_opacity, true)
    << attrib("stroke", stroke)
    << attrib("stroke-opacity", stroke_opacity, true)
    << attrib("stroke-linejoin", stroke_lineJoin);
  if (!stroke_dasharray.empty())  
    f << attrib("stroke-dasharray", stroke_dasharray);
  f << attrib("stroke-width", stroke_width, true);
  if (!transform.empty())  f << attrib("transform", transform);
  f << end_empty_elem();
}

void Path::plot_shadow (ofstream& f, const string& shadowFill) {
  // Filter
  filter = path_shadow(f);
  plot(f);

  // Essential
  filter.clear();
}

void Cylinder::plot (ofstream& f) const {
  auto path = make_unique<Path>();
  path->id = to_string(x)+to_string(y);
  path->d = path->M(x, y) + path->v(height) + 
            path->a(width/2, ry, 0, 0, 0, width ,0) + path->v(-height) + 
            path->a(width/2, ry, 0, 0, 0 /*1*/, -width, 0) + path->z();
  path->fill = fill;
  path->fill_opacity = fill_opacity;
  path->stroke = stroke;
  path->stroke_opacity = stroke_opacity;
  path->stroke_width = stroke_width;
  path->transform = transform;
  path->plot(f);

  auto ellipse = make_unique<Ellipse>();
  ellipse->stroke = path->stroke;
  ellipse->stroke_opacity = stroke_opacity;
  ellipse->stroke_width = 0.4 * stroke_width;
  ellipse->fill = fill;
  ellipse->fill_opacity = fill_opacity;
  ellipse->cx = x + width/2;
  ellipse->cy = y;
  ellipse->rx = width/2 + 0.4*(stroke_width-ellipse->stroke_width);
  ellipse->ry = ry;
  ellipse->transform = transform;
  ellipse->plot(f);

  ellipse->cy = y + height;
  ellipse->fill = "transparent";
  ellipse->transform = transform;
  ellipse->plot(f);
}

void Cylinder::plot_ir (ofstream& f, const string& wave) {
  plot(f);

  // Plot pattern
  auto pattern = make_unique<Pattern>();
  pattern->id = wave + to_string(x) + to_string(y);
  pattern->patternUnits = "userSpaceOnUse";
  pattern->x = x;
  pattern->y = y;
  pattern->width = width;
  pattern->height = 14;

  auto path = make_unique<Path>();
  path->fill = "transparent";
  path->stroke_width = 0.3 * pattern->height;
  path->d = 
    path->m(-path->stroke_width/2, pattern->height - path->stroke_width/2) +
    path->l(pattern->width/2 + path->stroke_width/2, 
            -pattern->height + path->stroke_width) + 
    path->l(pattern->width/2 + path->stroke_width/2, 
            pattern->height  - path->stroke_width);
  // path->stroke_width = 1.3 * stroke_width;
  // path->d = path->m(0, 0) + 
  //           path->q(pattern->width/2, 1.5*ry, pattern->width, 0);
  if      (wave=="Wavy")       path->stroke=shade(stroke, 0.95);
  else if (wave=="WavyWhite")  path->stroke="white";
  make_pattern(f, pattern, path);
  
  fill = "url(#" + pattern->id + ")";
  plot(f);
}

void Cylinder::plot_periph (ofstream& f, char refTar, u8 showNRC) {
  const auto mainOriginX = x;
  const auto mainWidth = width;
  const auto mainStrokeWidth = stroke_width;
  const auto mainRy = ry;

  if (refTar=='r')
    x = x + width + TITLE_SPACE + VERT_TUNE + 
        showNRC*(width/VERT_RATIO + VERT_TUNE);
  else
    x = x - (width/VERT_RATIO + TITLE_SPACE + VERT_TUNE + 
        showNRC*(VERT_TUNE + width/VERT_RATIO));

  width = width/VERT_RATIO;
  stroke_width *= 2;
  ry /= 2;

  plot(f);

  x = mainOriginX;
  width = mainWidth;
  stroke_width = mainStrokeWidth;
  ry = mainRy;
}

void Rectangle::plot (ofstream& f) const {
  f << begin_elem("rect")
    << attrib("x", x, true)
    << attrib("y", y, true)
    << attrib("width", width, true)
    << attrib("height", height, true);
  if (rx!=0.0f)  f << attrib("rx", rx, true);
  f << attrib("ry", ry, true)
    << attrib("fill", fill)
    << attrib("fill-opacity", fill_opacity, true)
    << attrib("stroke", stroke)
    << attrib("stroke-width", stroke_width, true)
    << end_empty_elem();
}

string Polygon::point (float x, float y) const {
  stringstream ss;
  ss << fixed_precision(PREC_VIZ) << x << "," 
     << fixed_precision(PREC_VIZ) << y << " ";
  return ss.str();
}

void Polygon::plot (ofstream& f) {
  f << begin_elem("polygon")
    << attrib("points", points)
    << attrib("fill", fill)
    << attrib("stroke", stroke)
    << attrib("stroke-width", stroke_width, true)
    << attrib("stroke-opacity", stroke_opacity, true)
    << attrib("fill-opacity", fill_opacity, true)
    << end_empty_elem();

  points.clear();
}

void Pattern::set_head (ofstream& f) const {
  f << begin_elem("pattern")
    << attrib("id", id)
    << attrib("x", x, true)
    << attrib("y", y, true)
    << attrib("width", width, true)
    << attrib("height", height, true)
    << attrib("patternUnits", patternUnits)
    << mid_elem();
}

void Pattern::set_tail (ofstream& f) const {
  f << end_elem("pattern");
}

void Defs::set_head (ofstream& f) const {
  f << begin_elem("defs")
    << attrib("id", id)
    << mid_elem();
}

void Defs::set_tail (ofstream& f) const {
  f << end_elem("defs");
}

void FilterSVG::set_head (ofstream& f) const {
  f << begin_elem("filter")
    << attrib("id", id)
    << attrib("x", x)
    << attrib("y", y)
    << attrib("width", width)
    << attrib("height", height)
    << mid_elem();
}

void FilterSVG::set_tail (ofstream& f) const {
  f << end_elem("filter");
}

void FeGaussianBlur::plot (ofstream& f) const {
  f << begin_elem("feGaussianBlur")
    << attrib("stdDeviation", stdDeviation)
    << attrib("result", result)
    << end_empty_elem();
}

void FeOffset::plot (ofstream& f) const {
  f << begin_elem("feOffset")
    << attrib("dx", dx, true)
    << attrib("dy", dy, true)
    << end_empty_elem();
}

void FeMerge::set_head (ofstream& f) const {
  f << begin_elem("feMerge")
    << mid_elem();
}

void FeMerge::set_tail (ofstream& f) const {
  f << end_elem("feMerge");
}

void FeMergeNode::plot (ofstream& f) const {
  f << begin_elem("feMergeNode")
    << attrib("in", in)
    << end_empty_elem();
}