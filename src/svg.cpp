#include <algorithm>
#include "svg.hpp"
#include "vizdef.hpp"
using namespace smashpp;

string SVG::attrib (const string& name, float value, bool precise,
const string& unit) const {
  stringstream ss;
  if (precise)  ss << name << "=\"" << PREC << value << unit << "\" ";
  else          ss << name << "=\"" << value << unit << "\" ";
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
  f << attrib("font-size", to_string(fontSize), false, "px");
  if (!font_weight.empty())  f << attrib("font-weight", font_weight);
  f << attrib("font-family", font_family);
  if (!fill.empty() && fill!="black")  f << attrib("fill", fill);
  f << attrib("text-anchor", text_anchor)
    << attrib("text-align", text_align)
    << attrib("line-height", line_height)
    << mid_elem()
    << Label
    << end_elem("text");
}

void Text::print_title (ofstream& f) {
  text_anchor = "middle";
  fontSize = 12;
  // font_weight = "bold";
  plot(f);
}

void Text::print_pos_ref (ofstream& f, char c) {
  text_anchor = "end";
  x = x - 5;
  switch (c) {
    case 'b':  dominant_baseline = "hanging";   break;  // begin
    case 'm':  dominant_baseline = "middle";    break;  // middle
    case 'e':  dominant_baseline = "baseline";  break;  // end
    default:                                    break;
  }
  fontSize = 9;
  plot(f);
}

void Text::print_pos_tar (ofstream& f, char c) {
  text_anchor = "start";
  x = x + 5;
  switch (c) {
    case 'b':  dominant_baseline = "hanging";   break;  // begin
    case 'm':  dominant_baseline = "middle";    break;  // middle
    case 'e':  dominant_baseline = "baseline";  break;  // end
    default:                                    break;
  }
  fontSize = 9;
  plot(f);
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
    << attrib("fill", fill)
    << end_empty_elem();
}

string Path::M (float x, float y) const {
  stringstream ss;
  ss << "M " << PREC << x << "," << PREC << y << "\n";
  return ss.str();
}

string Path::m (float dx, float dy) const {
  stringstream ss;
  ss << "m " << PREC << dx << "," << PREC << dy << "\n";
  return ss.str();
}

string Path::L (float x, float y) const {
  stringstream ss;
  ss << "L " << PREC << x << "," << PREC << y << "\n";
  return ss.str();
}

string Path::l (float dx, float dy) const {
  stringstream ss;
  ss << "l " << PREC << dx << "," << PREC << dy << "\n";
  return ss.str();
}

string Path::H (float x) const {
  stringstream ss;
  ss << "H " << PREC << x << "\n";
  return ss.str();
}

string Path::h (float dx) const {
  stringstream ss;
  ss << "h " << PREC << dx << "\n";
  return ss.str();
}

string Path::V (float y) const {
  stringstream ss;
  ss << "V " << PREC << y << "\n";
  return ss.str();
}

string Path::v (float dy) const {
  stringstream ss;
  ss << "v " << PREC << dy << "\n";
  return ss.str();
}

string Path::C (float x1, float y1, float x2, float y2, float x, float y) const{
  stringstream ss;
  ss << "C " << PREC << x1 << "," << PREC << y1 << " "
             << PREC << x2 << "," << PREC << y2 << " "
             << PREC << x  << "," << PREC << y  << "\n";
  return ss.str();
}

string Path::c (float dx1, float dy1, float dx2, float dy2, float dx, float dy) 
const {
  stringstream ss;
  ss << "c " << PREC << dx1 << "," << PREC << dy1 << " "
             << PREC << dx2 << "," << PREC << dy2 << " "
             << PREC << dx  << "," << PREC << dy  << "\n";
  return ss.str();
}

string Path::S (float x2, float y2, float x, float y) const {
  stringstream ss;
  ss << "S " << PREC << x2 << "," << PREC << y2 << " "
             << PREC << x  << "," << PREC << y  << "\n";
  return ss.str();
}

string Path::s (float dx2, float dy2, float dx, float dy) const {
  stringstream ss;
  ss << "s " << PREC << dx2 << "," << PREC << dy2 << " "
             << PREC << dx  << "," << PREC << dy  << "\n";
  return ss.str();
}

string Path::Q (float x1, float y1, float x, float y) const {
  stringstream ss;
  ss << "Q " << PREC << x1 << "," << PREC << y1 << " "
             << PREC << x  << "," << PREC << y  << "\n";
  return ss.str();
}

string Path::q (float dx1, float dy1, float dx, float dy) const {
  stringstream ss;
  ss << "q " << PREC << dx1 << "," << PREC << dy1 << " "
             << PREC << dx  << "," << PREC << dy  << "\n";
  return ss.str();
}

string Path::T (float x, float y) const {
  stringstream ss;
  ss << "T " << PREC << x << "," << PREC << y << "\n";
  return ss.str();
}

string Path::t (float dx, float dy) const {
  stringstream ss;
  ss << "t " << PREC << dx << "," << PREC << dy << "\n";
  return ss.str();
}

string Path::A (float rx, float ry, float angle, u8 large_arc_flag, 
u8 sweep_flag, float x, float y) const {
  stringstream ss;
  ss << "A " << PREC << rx << "," << PREC << ry << " "
             << PREC << angle << " "
             << u16(large_arc_flag) << "," << u16(sweep_flag) << " "
             << PREC << x << "," << PREC << y << "\n";
  return ss.str();
}

string Path::a (float rx, float ry, float angle, u8 large_arc_flag, 
u8 sweep_flag, float dx, float dy) const {
  stringstream ss;
  ss << "a " << PREC << rx << "," << PREC << ry << " "
             << PREC << angle << " "
             << u16(large_arc_flag) << "," << u16(sweep_flag) << " "
             << PREC << dx << "," << PREC << dy << "\n";
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
    << attrib("fill", fill)
    << attrib("fill-opacity", fill_opacity, true)
    << attrib("stroke", stroke)
    << attrib("stroke-opacity", stroke_opacity, true)
    << attrib("stroke-linejoin", stroke_lineJoin);
  if (!stroke_dasharray.empty())  
    f << attrib("stroke-dasharray", stroke_dasharray);
  f << attrib("stroke-width", stroke_width, true);
  if (!transform.empty())  f << attrib("transform", transform);
  f << end_empty_elem();
}

void Cylinder::plot (ofstream& f) const {
  auto path = make_unique<Path>();
  path->id = to_string(x)+to_string(y);
  path->d = path->M(x, y) + path->v(height) + 
            path->a(width/2, ry, 0, 0, 0, width ,0) + path->v(-height) + 
            path->a(width/2, ry, 0, 0, 1, -width, 0);
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
  ellipse->stroke_width = 0.5 * stroke_width;
  ellipse->fill = fill;
  ellipse->fill_opacity = fill_opacity;
  ellipse->cx = x + width/2;
  ellipse->cy = y;
  ellipse->rx = width/2 + (stroke_width-ellipse->stroke_width)/2;
  ellipse->ry = ry;
  ellipse->plot(f);

  ellipse->cy = y + height;
  ellipse->fill = "transparent";
  ellipse->plot(f);
}

void Cylinder::plot_ir (ofstream& f, string&& wave) {
  plot(f);

  fill = "url("+std::move(wave)+")";
  plot(f);
}

void Cylinder::plot_periph (ofstream& f, char refTar, u8 showNRC) {
  const auto mainOriginX = x;
  const auto mainWidth = width;
  const auto mainStrokeWidth = stroke_width;
  const auto mainRy = ry;

  if (refTar=='r')
    x = x - (1+showNRC) * (HORIZ_TUNE + width/HORIZ_RATIO);
  else
    x = x + width + HORIZ_TUNE + showNRC * (width/HORIZ_RATIO + HORIZ_TUNE);

  width = width/HORIZ_RATIO;
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
  ss << PREC << x << "," << PREC << y << " ";
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