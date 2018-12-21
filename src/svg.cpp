#include <algorithm>
#include "svg.hpp"
#include "vizdef.hpp"
using namespace smashpp;

void Text::plot (ofstream& f) const {
  f << begin_elem("text")
    << attrib("id", to_string(origin.x)+to_string(origin.y))
    << attrib("x", origin.x, true)
    << attrib("y", origin.y, true)
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

void Line::plot (ofstream& f) const {
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

void Ellipse::plot (ofstream& f) const {
  f << begin_elem("ellipse")
    << attrib("cx", cx)
    << attrib("cy", cy)
    << attrib("rx", rx)
    << attrib("ry", ry)
    << attrib("fill", fill)
    << attrib("fill-opacity", opacity)
    << attrib("stroke", stroke)
    << attrib("stroke-width", strokeWidth)
    << end_empty_elem();
}

void Path::plot (ofstream& f) const {
  f << begin_elem("path");
  
  if (!d.empty())
    f << attrib("d", d);
  else
    f << attrib("d","M "+to_string(origin.x)+","+to_string(origin.y)+" "+trace);

  f << attrib("fill", fill)
    << attrib("fill-opacity", opacity)
    << attrib("stroke", stroke)
    << attrib("stroke-linejoin", strokeLineJoin)
    << attrib("stroke-dasharray", strokeDashArray)
    << attrib("stroke-width", strokeWidth, true)
    << end_empty_elem();
}

void Cylinder::plot (ofstream& f) const {
  auto path = make_unique<Path>();
  path->origin = Point(origin.x, origin.y);
  path->trace = " v "+to_string(height)+
    " a "+to_string(width/2)+","+to_string(ry)+" 0 0,0 "+to_string(width)+
    ",0 "+
    " v "+to_string(-height)+
    " a "+to_string(width/2)+","+to_string(ry)+" 0 0,0 "+to_string(-width)+
    ",0 "+
    " a "+to_string(width/2)+","+to_string(ry)+" 0 0,0 "+to_string(width)+
    ",0 ";
  path->fill = fill;
  path->opacity = opacity;
  path->stroke = stroke;
  path->strokeWidth = strokeWidth;
  path->plot(f);

  path->origin = Point(origin.x, origin.y+height);
  path->trace = " a "+to_string(width/2)+","+to_string(ry)+" 0 0,1 "+
    to_string(width)+",0 ";
  path->fill = "transparent";
  path->opacity = opacity;
  path->stroke = stroke;
  path->strokeWidth = strokeWidth/2;
  path->strokeDashArray = "4 1";
  path->plot(f);
}

void Cylinder::plot_ir (ofstream& f, string&& wave) {
  plot(f);

  fill = "url("+std::move(wave)+")";
  plot(f);
}

void Cylinder::plot_nrc (ofstream& f, char refTar=' ') {
  const auto mainOriginX = origin.x;
  const auto mainWidth   = width;

  if (refTar=='r')  origin.x = origin.x - HORIZ_TUNE - width/HORIZ_RATIO;
  else              origin.x = origin.x + width + HORIZ_TUNE;
  width = width/HORIZ_RATIO;
  strokeWidth = 0.5;
  stroke = shade(fill, 0.95);

  plot(f);

  origin.x = mainOriginX;
  width    = mainWidth;
}

void Cylinder::plot_redun (ofstream& f, u8 showNRC, char refTar=' ') {
  const auto mainOriginX = origin.x;
  const auto mainWidth   = width;

  if (refTar=='r')
    origin.x = origin.x - (1+showNRC) * (HORIZ_TUNE + width/HORIZ_RATIO);
  else
    origin.x = origin.x + width + HORIZ_TUNE + 
               showNRC * (width/HORIZ_RATIO + HORIZ_TUNE);

  width = width/HORIZ_RATIO;
  strokeWidth = 0.5;
  stroke = shade(fill, 0.95);

  plot(f);

  origin.x = mainOriginX;
  width    = mainWidth;
}

void Rectangle::plot (ofstream& f) const {
  f << begin_elem("rect")
    << attrib("fill", fill)
    << attrib("stroke", stroke)
    << attrib("fill-opacity", opacity)
    << attrib("width", width, true)
    << attrib("height", height, true)
    << attrib("x", origin.x, true)
    << attrib("y", origin.y, true)
    << attrib("ry", 3)
    << end_empty_elem();
}

void Polygon::plot (ofstream& f) const {
  f << begin_elem("polygon")
    << "points=\""
    << PREC << one.x   << "," << PREC << one.y   << " "
    << PREC << two.x   << "," << PREC << two.y   << " "
    << PREC << three.x << "," << PREC << three.y << " "
    << PREC << four.x  << "," << PREC << four.y  << "\" "
    << attrib("fill", fillColor)
    << attrib("stroke", fillColor)
    << attrib("stroke-width", 1)
    << attrib("stroke-opacity", 0.4)
    << attrib("fill-opacity", 0.4)
    << end_empty_elem();
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