#include "svg.hpp"
#include "vizdef.hpp"
using namespace smashpp;

/*
 * class Text
 */
void Text::plot (ofstream& f) const {
  f << begin_elem("text")
    << attrib("id", to_string(origin.x)+to_string(origin.y))
    << attrib("x", origin.x, true)
    << attrib("y", origin.y, true)
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

/*
 * class Line
 */
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

/*
* class Ellipse
*/
void Ellipse::plot (ofstream& f) const {
  f << begin_elem("ellipse")
    << attrib("cx", cx)
    << attrib("cy", cy)
    << attrib("rx", rx)
    << attrib("ry", ry)
    << attrib("fill", fill)
    << attrib("stroke", stroke)
    << attrib("stroke-width", strokeWidth)
    << end_empty_elem();
}

/*
 * class Path
 */
void Path::plot (ofstream& f) const {
  f << begin_elem("path")
    << attrib("d", "M "+to_string(origin.x)+","+to_string(origin.y)+" "+trace)
    << attrib("fill", fill)
    << attrib("stroke", stroke)
    << attrib("stroke-linejoin", strokeLineJoin)
    << attrib("stroke-dasharray", strokeDashArray)
    << attrib("stroke-width", strokeWidth, true)
    << end_empty_elem();
}

/*
* class Cyllinder
*/
void Cyllinder::plot (ofstream& f) const {
  auto line = make_unique<Line>();
  line->stroke = stroke;
  line->strokeWidth = strokeWidth;
  line->beg = Point(origin.x, origin.y);
  line->end = Point(origin.x, origin.y+height);
  line->plot(f);

  line->beg = Point(origin.x+width, origin.y);
  line->end = Point(origin.x+width, origin.y+height);
  line->plot(f);

  auto ellipse = make_unique<Ellipse>();
  ellipse->cx = origin.x + width/2;
  ellipse->cy = origin.y;
  ellipse->rx = width/2;
  ellipse->ry = ry;
  ellipse->fill = fill;
  ellipse->stroke = stroke;
  ellipse->strokeWidth = strokeWidth;
  ellipse->plot(f);

  auto path = make_unique<Path>();
  path->origin = Point(origin.x, origin.y+height);
  path->trace = "M"+to_string(origin.x)+" "+to_string(origin.y+height)+
    " a "+to_string(width/2)+" "+to_string(ry)+", 0, 0 0, "+to_string(width)+
    " 0";
  path->stroke = stroke;
  path->fill = fill;
  path->strokeWidth = strokeWidth;
  path->plot(f);
}

/*
 * class Rectangle
 */
void Rectangle::plot (ofstream& f) const {
  f << begin_elem("rect")
    << attrib("fill", color)
    << attrib("stroke", color)
    << attrib("fill-opacity", opacity)
    << attrib("width", width, true)
    << attrib("height", height, true)
    << attrib("x", origin.x, true)
    << attrib("y", origin.y, true)
    << attrib("ry", 3)
    << end_empty_elem();


  // auto path = make_unique<Path>();
  // path->origin = Point(origin.x, origin.y+height);
  // path->trace = "M"+to_string(origin.x)+" "+to_string(origin.y+height)+
  //   " a "+to_string(width/2)+" "+to_string(ry)+", 0, 0 0, "+to_string(width)+
  //   " 0";
  // path->stroke = stroke;
  // path->fill = fill;
  // path->strokeWidth = strokeWidth;
  // path->plot(f);
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
    << attrib("fill", color)
    << attrib("stroke", color)
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
    << attrib("fill", color)
    << attrib("stroke", color)
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

/*
 * class Polygon
 */
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

/*
* class Chromosome
*/
void Chromosome::plot (ofstream& f) const {
  auto cyllinder = make_unique<Cyllinder>();
  cyllinder->width = width;
  cyllinder->height = height;
  cyllinder->stroke = stroke;
  cyllinder->fill = fill;
  cyllinder->strokeWidth = strokeWidth;;
  cyllinder->origin = origin;
  cyllinder->plot(f);
}
