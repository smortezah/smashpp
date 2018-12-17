#include "svg.hpp"
#include "vizdef.hpp"
using namespace smashpp;

/*
 * class Text
 */
void Text::plot (ofstream& f) const {
  f << begin_elem("text")
    << attrib("id",                to_string(origin.x)+to_string(origin.y))
    << attrib("x",                 origin.x, true)
    << attrib("y",                 origin.y, true)
    << attrib("dominant-baseline", dominantBaseline)
    << attrib("transform",         transform)
    << attrib("font-size",         to_string(fontSize), false, "px")
    << attrib("font-weight",       fontWeight)
    << attrib("font-family",       "Arial")
    // << attrib("font-family",       "-apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, Helvetica, Arial, sans-serif, \"Apple Color Emoji\", \"Segoe UI Emoji\", \"Segoe UI Symbol\"")
    << attrib("fill",              color)
    << attrib("text-align",        "start")
    << attrib("line-height",       "125%%")
    << attrib("text-anchor",       textAnchor) 
    << mid_elem()
    << label
    << end_elem("text");
}

void Text::plot_title (ofstream& f) {
  textAnchor = "middle";
  fontSize = 12;
  plot(f);
}

void Text::plot_pos_ref (ofstream& f, char c) {
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

void Text::plot_pos_tar (ofstream& f, char c) {
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
    << attrib("id",           to_string(beg.x)+to_string(beg.y)+
                              to_string(end.x)+to_string(end.y))
    << attrib("x1",           beg.x, true)
    << attrib("y1",           beg.y, true)
    << attrib("x2",           end.x, true) 
    << attrib("y2",           end.y, true)
    << attrib("stroke",       color) 
    << attrib("stroke-width", width, true)
    << end_empty_elem();
}

/*
 * class Path
 */
void Path::plot (ofstream& f) const {
  f << begin_elem("path")
    << attrib("d", "M "+to_string(origin.x)+","+to_string(origin.y)+" "+trace)
    << attrib("fill",             "none")
    << attrib("stroke",           color)
    << attrib("stroke-linejoin",  strokeLineJoin)
    << attrib("stroke-dasharray", strokeDashArray)
    << attrib("stroke-width",     width, true)
    << end_empty_elem();
  // f << "<path d=\"M " << origin.x << "," << origin.y << " " << trace << "\" "
  // "style=\"fill:none;stroke:" << color << ";stroke-linejoin:" << 
  // strokeLineJoin << ";stroke-dasharray:" << strokeDashArray << 
  // ";stroke-width:" << PREC << width << "\" />";
}

/*
 * class Rectangle
 */
void Rectangle::plot (ofstream& f) const {
  f << begin_elem("rect")
    << attrib("fill",         color)
    << attrib("stroke",       color)
    << attrib("fill-opacity", opacity)
    << attrib("width",        width,    true)
    << attrib("height",       height,   true)
    << attrib("x",            origin.x, true)
    << attrib("y",            origin.y, true)
    << attrib("ry",           3)
    << end_empty_elem();
  // f << "<rect style=\"fill:" << color << ";stroke:" << color <<
  //   ";fill-opacity:" << opacity << ";stroke-width:1;stroke-miterlimit:4;"
  //   "stroke-dasharray:none\" id=\"rect3777\" "
  //   "width=\"" << PREC << width << "\" height=\"" << PREC << height <<"\" "
  //   "x=\"" << PREC << origin.x << "\" y=\"" << PREC << origin.y <<"\" "
  //   "ry=\"3\" />\n";
}

void Rectangle::plot_ir (ofstream& f, string&& wave) const {
  plot(f);
  f << begin_elem("rect")
    << attrib("fill-opacity", opacity)
    << attrib("stroke-width", 2)
    << attrib("fill",         "url("+wave+")")
    << attrib("width",        width, true)
    << attrib("height",       height, true)
    << attrib("x",            origin.x, true)
    << attrib("y",            origin.y, true)
    << attrib("ry",           3)
    << end_empty_elem();
  // f << "<rect style=\"fill-opacity:" << opacity << ";stroke-width:2;"
  //   "stroke-miterlimit:4;stroke-dasharray:none;fill:url(" << wave << ");"
  //   "fill-rule:nonzero;opacity:1;\" id=\"rect6217\" "
  //   "width=\"" << PREC << width << "\" height=\"" << PREC << height <<"\" "
  //   "x=\"" << PREC << origin.x << "\" y=\"" << PREC << origin.y <<"\" "
  //   "ry=\"3\" />\n";
}

void Rectangle::plot_oval (ofstream& f) const {
  f << "<rect style=\"fill:" << color << ";fill-opacity:1;stroke-width:2;"
    "stroke-miterlimit:4;stroke-dasharray:none\" id=\"rectx\" "
    "width=\"" << PREC << width << "\" height=\"" << PREC << height <<"\" "
    "x=\"" << PREC << origin.x << "\" y=\"" << PREC << origin.y <<"\" "
    "ry=\"12.5\" />\n";
}

void Rectangle::plot_oval_ir (ofstream& f) const {
  plot_oval(f);
  f << "<rect style=\"fill-opacity:1;stroke-width:2;stroke-miterlimit:4;"
    "stroke-dasharray:nonestroke-dasharray:none;fill:url(#xtrace);"
    "fill-rule:nonzero;opacity:1\" id=\"recty\" "
    "width=\"" << PREC << width << "\" height=\"" << PREC << height <<"\" "
    "x=\"" << PREC << origin.x << "\" y=\"" << PREC << origin.y <<"\" "
    "ry=\"12.5\" />\n";
}

void Rectangle::plot_nrc (ofstream& f, char refTar=' ') const {
  f << "<rect style=\"fill:" << color << ";stroke:" << color <<
    ";fill-opacity:" << opacity << ";stroke-width:1;stroke-miterlimit:4;"
    "stroke-dasharray:none\" id=\"rect3777\" "
    "width=\"" << PREC << width/HORIZ_RATIO << "\" "
    "height=\"" << PREC << height <<"\" "
    "x=\"" << PREC << (refTar=='r' ? origin.x - HORIZ_TUNE - width/HORIZ_RATIO
                                   : origin.x + width + HORIZ_TUNE) << "\" "
    "y=\"" << PREC << origin.y <<"\" ry=\"2\" />\n";
}

void Rectangle::plot_nrc_ref (ofstream& f) const {
  plot_nrc(f, 'r');
}

void Rectangle::plot_nrc_tar (ofstream& f) const {
  plot_nrc(f, 't');
}

void Rectangle::plot_redun
(ofstream& f, u8 showNRC, char refTar = ' ') const {
  f << "<rect style=\"fill:" << color << ";stroke:" << color <<
    ";fill-opacity:" << opacity << ";stroke-width:1;stroke-miterlimit:4;"
    "stroke-dasharray:none\" id=\"rect3777\" "
    "width=\"" << PREC << width/HORIZ_RATIO << "\" "
    "height=\"" << PREC << height <<"\" "
    "x=\"";
  if (refTar=='r')
    f << PREC << origin.x - (1+showNRC)*(HORIZ_TUNE+width/HORIZ_RATIO);
  else if (refTar=='t')
    f << PREC << origin.x + width + HORIZ_TUNE
                          + showNRC*(width/HORIZ_RATIO + HORIZ_TUNE);
  f << "\" y=\"" << PREC << origin.y <<"\" ry=\"2\" />\n";
}

void Rectangle::plot_redun_ref (ofstream& f, bool showNRC) const {
  showNRC ? plot_redun(f, 1, 'r') : plot_redun(f, 0, 'r');
}

void Rectangle::plot_redun_tar (ofstream& f, bool showNRC) const {
  showNRC ? plot_redun(f, 1, 't') : plot_redun(f, 0, 't');
}

void Rectangle::plot_chromosome (ofstream& f) const {
  const string borderColor {"#000000"};
//  double  wk = width / 2 + 0.5;
//
//  f << "<path d=\"m " << PREC << origin.x - 1 << ","
//    << PREC << origin.y - 1 << " 0," << PREC << wk << " c 0, -8.31 6.69, "
//    << PREC << -wk << " " << PREC << wk << ", " << PREC << -wk << " l "
//    << PREC << -wk << ",0 z m " << PREC << wk << ",0 c 8.31,0 "
//    << PREC << wk << ",6.69 " << PREC << wk << "," << PREC << wk << " l 0,"
//    << PREC << -wk << " " << PREC << -wk << ",0 z\" id=\"rect3787\" "
//    << "style=\"fill:#fff;fill-opacity:1;fill-rule:nonzero;stroke:none\" />";
//
//  f << "<path d=\"m " << PREC << origin.x + 1 + width << ","
//    << PREC << origin.y + 1 + height << " 0," << PREC << -wk
//    << " c 0,8.31 -6.69, " << PREC << wk << " " << PREC << -wk << ", "
//    << PREC << wk << " l " << PREC << wk << ",0 z m " << PREC << -wk
//    << ",0 c -8.31,0 " << PREC << -wk << ",-6.69 " << PREC << -wk << ","
//    << PREC << -wk << " l 0," << PREC << wk << " " << PREC << wk << ",0 z\" "
//    << "id=\"rect3787\" style=\"fill:#fff;fill-opacity:1;fill-rule:nonzero;"
//    << "stroke:none\" />";

  f << "<rect style=\"fill:none;stroke:" << borderColor << ";stroke-width:2;"
    "stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;"
    "stroke-opacity:1;stroke-dasharray:none\" id=\"rect2985\" "
    "width=\"" << PREC << width << "\" height=\"" << PREC << height <<"\" "
    "x=\"" << PREC << origin.x << "\" y=\"" << PREC << origin.y <<"\" "
    "ry=\"3\" />\n";
}

/*
 * class Polygon
 */
void Polygon::plot (ofstream& f) const {
  f << "<polygon points=\""
    << PREC << one.x   << "," << PREC << one.y   << " "
    << PREC << two.x   << "," << PREC << two.y   << " "
    << PREC << three.x << "," << PREC << three.y << " "
    << PREC << four.x  << "," << PREC << four.y  << "\" "
    << "style=\"fill:" << fillColor << ";stroke:" << fillColor << ";"
    << "stroke-width:1;stroke-opacity:0.4;fill-opacity:0.4\" />";
}