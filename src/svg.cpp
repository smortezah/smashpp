#include <algorithm>
#include "svg.hpp"
#include "vizdef.hpp"
using namespace smashpp;

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
    << attrib("y", y, true)
    << attrib("dx", dx, true)
    << attrib("dy", dy, true)
    << attrib("dominant-baseline", dominant_baseline)
    << attrib("transform", transform)
    << attrib("font-size", to_string(fontSize), false, "px")
    << attrib("font-weight", font_weight)
    << attrib("font-family", font_family)
    << attrib("fill", fill)
    << attrib("text-anchor", text_anchor)
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
    default:                                   break;
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
    default:                                   break;
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

void Path::plot (ofstream& f) const {
  f << begin_elem("path")
    << attrib("id", id)
    << attrib("d", d)
    << attrib("fill", fill)
    << attrib("fill-opacity", fill_opacity, true)
    << attrib("stroke", stroke)
    << attrib("stroke-opacity", stroke_opacity, true)
    << attrib("stroke-linejoin", stroke_lineJoin)
    << attrib("stroke-dasharray", stroke_dasharray)
    << attrib("stroke-width", stroke_width, true)
    << attrib("transform", transform)
    << end_empty_elem();
}

void Cylinder::plot (ofstream& f) const {
  auto path = make_unique<Path>();
  path->id = to_string(x)+to_string(y);
  path->d = "M "+to_string(x)+","+to_string(y)+" "+
    " v "+to_string(height)+
    " a "+to_string(width/2)+","+to_string(ry)+" 0 0,0 "+to_string(width)+
    ",0 "+
    " v "+to_string(-height)+
    " a "+to_string(width/2)+","+to_string(ry)+" 0 0,1 "+to_string(-width)+
    ",0 ";
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
  ellipse->stroke_width = 0.75 * stroke_width;
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
  // stroke = shade(fill, 0.95);
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
    << attrib("height", height, true)
    << attrib("rx", rx, true)
    << attrib("ry", ry, true)
    << attrib("fill", fill)
    << attrib("fill-opacity", fill_opacity, true)
    << attrib("stroke", stroke)
    << attrib("stroke-width", stroke_width, true)
    << end_empty_elem();
}

void Polygon::add_point (float x, float y) {
  stringstream ss;
  ss << PREC << x << "," << PREC << y << " ";
  points += ss.str();
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
