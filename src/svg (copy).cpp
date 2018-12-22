#include <algorithm>
#include "svg.hpp"
#include "vizdef.hpp"
using namespace smashpp;

template <typename Stream>
void Stop::plot (Stream& s) const {
  s << begin_elem("stop")
    << attrib("offset", offset)
    << attrib("stop-color", stop_color)
    << attrib("stop-opacity", stop_opacity, true)
    << end_empty_elem();
}

template <typename Stream>
void LinearGradient::plot (Stream& s) const {
  auto defs = make_unique<Defs>();
  defs->set_head(s);

  s << begin_elem("linearGradient")
    << attrib("id", id)
    << attrib("x1", x1)
    << attrib("y1", y1)
    << attrib("x2", x2)
    << attrib("y2", y2)
    << mid_elem();

  s << stops;

  s << end_elem("linearGradient");
  defs->set_tail(s);
}

void LinearGradient::add_stop (const Stop& stop) {
  stringstream ss;
  stop.plot(ss);
  stops += ss.str();
}

template <typename Stream>
void Text::plot (Stream& s) {
  s << begin_elem("text")
    << attrib("id", to_string(origin.x)+to_string(origin.y))
    << attrib("x", origin.x, true)//todo remove
    << attrib("y", origin.y, true)//todo remove
    // << attrib("x", x, true)//todo add
    // << attrib("y", y, true)//todo add
    << attrib("dx", dx, true)
    << attrib("dy", dy, true)
    << attrib("dominant-baseline", dominantBaseline)
    << attrib("transform", transform)
    << attrib("font-size", to_string(fontSize), false, "px")
    << attrib("font-weight", fontWeight)
    << attrib("font-family", "Arial")
    << attrib("fill", color)
    << attrib("text-align", "start")
    << attrib("line-height", "125%%")
    << attrib("text-anchor", textAnchor) 
    << mid_elem()
    << label
    << end_elem("text");
}

void Text::print_title (ofstream& f) {
  textAnchor = "middle";
  fontSize = 12;
  // fontWeight = "bold";
  plot(f);
}

void Text::print_pos_ref (ofstream& f, char c) {
  textAnchor = "end";
  origin.x += -5;
  switch (c) {
    case 'b':  dominantBaseline = "hanging";   break;  // begin
    case 'm':  dominantBaseline = "middle";    break;  // middle
    case 'e':  dominantBaseline = "baseline";  break;  // end
    default:                                   break;
  }
  fontSize = 9;
  plot(f);
}

void Text::print_pos_tar (ofstream& f, char c) {
  textAnchor = "start";
  origin.x += 5;
  switch (c) {
    case 'b':  dominantBaseline = "hanging";   break;  // begin
    case 'm':  dominantBaseline = "middle";    break;  // middle
    case 'e':  dominantBaseline = "baseline";  break;  // end
    default:                                   break;
  }
  fontSize = 9;
  plot(f);
}

void Line::plot (ofstream& f) {
  f << begin_elem("line")
    << attrib("id", to_string(beg.x)+to_string(beg.y)+to_string(end.x)+
                    to_string(end.y))
    << attrib("x1", beg.x, true)
    << attrib("y1", beg.y, true)
    << attrib("x2", end.x, true) 
    << attrib("y2", end.y, true)
    << attrib("stroke", stroke) 
    << attrib("stroke-width", strokeWidth, true)
    << end_empty_elem();
}

void Ellipse::plot (ofstream& f) {
  f << begin_elem("ellipse")
    << attrib("cx", cx)
    << attrib("cy", cy)
    << attrib("rx", rx)
    << attrib("ry", ry)
    << attrib("fill", fill)
    << attrib("fill-opacity", opacity)
    << attrib("stroke", stroke)
    << attrib("stroke-opacity", stroke_opacity)
    << attrib("stroke-width", strokeWidth)
    << end_empty_elem();
}

void Path::plot (ofstream& f) {
  f << begin_elem("path")
    << attrib("id", id);
  
  if (!d.empty())
    f << attrib("d", d);
  else
    f << attrib("d","M "+to_string(origin.x)+","+to_string(origin.y)+" "+trace);

  f << attrib("fill", fill)
    << attrib("fill-opacity", opacity)
    << attrib("stroke", stroke)
    << attrib("stroke-opacity", stroke_opacity)
    << attrib("stroke-linejoin", strokeLineJoin)
    << attrib("stroke-dasharray", strokeDashArray)
    << attrib("stroke-width", strokeWidth, true)
    << attrib("transform", transform)
    << end_empty_elem();
}

void Cylinder::plot (ofstream& f) {
  auto path = make_unique<Path>();
  path->origin = Point(origin.x, origin.y);
  path->id = to_string(origin.x)+to_string(origin.y);
  path->trace = " v "+to_string(height)+
    " a "+to_string(width/2)+","+to_string(ry)+" 0 0,0 "+to_string(width)+
    ",0 "+
    " v "+to_string(-height)+
    " a "+to_string(width/2)+","+to_string(ry)+" 0 0,1 "+to_string(-width)+
    ",0 ";
  path->fill = fill;
  path->opacity = opacity;
  path->stroke = stroke;
  path->stroke_opacity = stroke_opacity;
  path->strokeWidth = strokeWidth;
  path->transform = transform;
  path->plot(f);

  auto ellipse = make_unique<Ellipse>();
  ellipse->stroke = path->stroke;
  ellipse->stroke_opacity = stroke_opacity;
  ellipse->strokeWidth = 0.75 * strokeWidth;
  ellipse->fill = fill;
  ellipse->opacity = opacity;
  ellipse->cx = origin.x + width/2;
  ellipse->cy = origin.y;
  ellipse->rx = width/2 + (strokeWidth-ellipse->strokeWidth)/2;
  ellipse->ry = ry;
  ellipse->plot(f);

  ellipse->cy = origin.y + height;
  ellipse->fill = "transparent";
  ellipse->plot(f);
}

void Cylinder::plot_ir (ofstream& f, string&& wave) {
  plot(f);

  fill = "url("+std::move(wave)+")";
  plot(f);
}

void Cylinder::plot_periph (ofstream& f, char refTar, u8 showNRC) {
  const auto mainOriginX = origin.x;
  const auto mainWidth = width;
  const auto mainStrokeWidth = strokeWidth;
  const auto mainRy = ry;

  if (refTar=='r')
    origin.x = origin.x - (1+showNRC) * (HORIZ_TUNE + width/HORIZ_RATIO);
  else
    origin.x = origin.x + width + HORIZ_TUNE + 
               showNRC * (width/HORIZ_RATIO + HORIZ_TUNE);

  width = width/HORIZ_RATIO;
  strokeWidth *= 2;
  // stroke = shade(fill, 0.95);
  ry /= 2;

  plot(f);

  origin.x = mainOriginX;
  width = mainWidth;
  strokeWidth = mainStrokeWidth;
  ry = mainRy;
}

void Rectangle::plot (ofstream& f) {
  f << begin_elem("rect")
    << attrib("fill", fill)
    << attrib("stroke", stroke)
    << attrib("stroke-width", strokeWidth)
    << attrib("fill-opacity", opacity)
    << attrib("width", width, true)
    << attrib("height", height, true)
    << attrib("x", origin.x, true)
    << attrib("y", origin.y, true)
    << attrib("ry", ry)
    << end_empty_elem();
}

void Polygon::add_point (double x, double y) {
  stringstream ss;
  ss << PREC << x << "," << PREC << y << " ";
  points += ss.str();
}

void Polygon::plot (ofstream& f) {
  f << begin_elem("polygon")
    << attrib("points", points)
    << attrib("fill", fillColor)
    << attrib("stroke", fillColor)
    << attrib("stroke-width", stroke_width)
    << attrib("stroke-opacity", stroke_opacity)
    << attrib("fill-opacity", fill_opacity)
    // << attrib("stroke-opacity", 0.4)
    // << attrib("fill-opacity", 0.4)
    << end_empty_elem();

    points.clear();
}

void Pattern::set_head (ofstream& f) {
  f << begin_elem("pattern")
    << attrib("id", id)
    << attrib("x", x, true)
    << attrib("y", y, true)
    << attrib("width", width, true)
    << attrib("height", height, true)
    << attrib("patternUnits", patternUnits)
    << mid_elem();
}

void Pattern::set_tail (ofstream& f) {
  f << end_elem("pattern");
}

void Defs::set_head (ofstream& f) {
  f << begin_elem("defs")
    << attrib("id", id)
    << mid_elem();
}

void Defs::set_tail (ofstream& f) {
  f << end_elem("defs");
}