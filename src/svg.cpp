#include <algorithm>
#include "svg.hpp"
#include "vizdef.hpp"
using namespace smashpp;

RgbColor RgbColor::alpha_blend (const RgbColor& color1, const RgbColor& color2)
const {
  return RgbColor(static_cast<u8>(color1.r + (color2.r - color1.r) * alpha),
                  static_cast<u8>(color1.g + (color2.g - color1.g) * alpha),
                  static_cast<u8>(color1.b + (color2.b - color1.b) * alpha));
}

RgbColor RgbColor::shade (const RgbColor& color) const {
  return alpha_blend(color, RgbColor(0, 0, 0));
}

RgbColor RgbColor::tint (const RgbColor& color) const {
  return alpha_blend(color, RgbColor(255, 255, 255));
}

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
  f << begin_elem("path")
    << attrib("d", "M "+to_string(origin.x)+","+to_string(origin.y)+" "+trace)
    << attrib("fill", fill)
    << attrib("fill-opacity", opacity)
    << attrib("stroke", stroke)
    << attrib("stroke-linejoin", strokeLineJoin)
    << attrib("stroke-dasharray", strokeDashArray)
    << attrib("stroke-width", strokeWidth, true)
    << end_empty_elem();
}

void Cyllinder::plot (ofstream& f) const {
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

void Rectangle::plot_ir (ofstream& f, string&& wave) const {
  plot(f);
  f << begin_elem("rect")
    << attrib("fill-opacity", opacity)
    << attrib("stroke-width", 2)
    << attrib("fill", "url("+wave+")")
    << attrib("width", width, true)
    << attrib("height", height, true)
    << attrib("x", origin.x, true)
    << attrib("y", origin.y, true)
    << attrib("ry", 3)
    << end_empty_elem();
}

void Rectangle::plot_nrc (ofstream& f, char refTar=' ') const {
  f << begin_elem("rect")
    << attrib("id", "rect3777")
    << attrib("fill", fill)
    << attrib("stroke", stroke)
    << attrib("fill-opacity", opacity)
    << attrib("width", width/HORIZ_RATIO, true)
    << attrib("height", height, true);

  if (refTar=='r')
    f << attrib("x", origin.x - HORIZ_TUNE - width/HORIZ_RATIO, true);
  else
    f << attrib("x", origin.x + width + HORIZ_TUNE, true);

  f << attrib("y", origin.y, true)
    << attrib("ry", 2)
    << end_empty_elem();
}

void Rectangle::plot_nrc_ref (ofstream& f) const {
  plot_nrc(f, 'r');
}

void Rectangle::plot_nrc_tar (ofstream& f) const {
  plot_nrc(f, 't');
}

void Rectangle::plot_redun (ofstream& f, u8 showNRC, char refTar=' ') const {
  f << begin_elem("rect")
    << attrib("fill", fill)
    << attrib("stroke", stroke)
    << attrib("fill-opacity", opacity)
    << attrib("width", width/HORIZ_RATIO, true)
    << attrib("height", height, true);

  if (refTar=='r')
    f << attrib("x", origin.x-(1+showNRC)*(HORIZ_TUNE+width/HORIZ_RATIO), true);
  else
    f << attrib("x", origin.x + width + HORIZ_TUNE + 
                     showNRC*(width/HORIZ_RATIO+HORIZ_TUNE), true);
                     
  f << attrib("y", origin.y, true)
    << attrib("ry", 2)
    << end_empty_elem();
}

void Rectangle::plot_redun_ref (ofstream& f, bool showNRC) const {
  showNRC ? plot_redun(f, 1, 'r') : plot_redun(f, 0, 'r');
}

void Rectangle::plot_redun_tar (ofstream& f, bool showNRC) const {
  showNRC ? plot_redun(f, 1, 't') : plot_redun(f, 0, 't');
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

void Chromosome::plot (ofstream& f) const {
  auto cyllinder = make_unique<Cyllinder>();
  cyllinder->width = width;
  cyllinder->height = height;
  cyllinder->stroke = stroke;
  cyllinder->fill = fill;
  cyllinder->opacity = opacity;
  cyllinder->strokeWidth = strokeWidth;;
  cyllinder->origin = origin;
  cyllinder->plot(f);
}

void Chromosome::plot_ir (ofstream& f, string&& wave) const {

  plot(f);

  // auto cyllinder = make_unique<Cyllinder>();
  // cyllinder->width = width;
  // cyllinder->height = height;
  // cyllinder->stroke = stroke;
  // cyllinder->fill = "url("+wave+")";
  // cyllinder->opacity = opacity;
  // cyllinder->strokeWidth = strokeWidth;;
  // cyllinder->origin = origin;
  // cyllinder->plot(f);

  // f << begin_elem("rect")
  //   << attrib("fill-opacity", opacity)
  //   << attrib("stroke-width", 2)
  //   << attrib("fill", "url("+wave+")")
  //   << attrib("width", width, true)
  //   << attrib("height", height, true)
  //   << attrib("x", origin.x, true)
  //   << attrib("y", origin.y, true)
  //   << attrib("ry", 3)
  //   << end_empty_elem();
}

DEFCOLORS to_defColors (const string& color) {
  
}

RgbColor to_rgb (const string& rgbColor) {
  const bool isHex = (rgbColor[0]=='#');
  if (isHex) {
  // return RgbColor(static_cast<u8>(stoi(rgbColor.substr(1,2))),
  //                 static_cast<u8>(stoi(rgbColor.substr(3,2))),
  //                 static_cast<u8>(stoi(rgbColor.substr(5,2))));
  }
  else {
    if      (*pos=="black")  return RgbColor(0,   0,   0);
    else if (*pos=="white")  return RgbColor(255, 255, 255);
    else if (*pos=="red")    return RgbColor(255, 0,   0);
    else if (*pos=="green")  return RgbColor(0,   255, 0);
    else if (*pos=="blue")   return RgbColor(0,   0,   255);
    else                     error("color undefined");
  }
}