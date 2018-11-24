#include "svg.hpp"
#include "vizdef.hpp"
using namespace smashpp;

/*
 * class Text
 */
void Text::plot (ofstream& f) const {
  f << "<text sodipodi:role=\"line\" id=\"tspan3804\" "
    "x=\"" << PREC << origin.x  << "\" "
    "y=\"" << PREC << origin.y  << "\" "
    "dominant-baseline=\"" << dominantBaseline << "\" "
    "transform=\"" << transform << "\" "
    "style=\"font-size:" << to_string(fontSize) << "px;font-style:normal;"
    "font-variant:normal;font-weight:" << fontWeight << ";font-stretch:normal;"
    "fill:" << color << ";fill-opacity:1;text-align:start;line-height:125%%;"
    "writing-mode:lr-tb;text-anchor:" << textAnchor << ";font-family:Arial;"
    "-inkscape-font-specification:Arial\">" << label << "</text>\n";
}

void Text::plot_title (ofstream& f) {
  textAnchor = "middle";
  fontSize = 14;
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
  f << "<line "
    "x1=\"" << PREC << beg.x << "\" y1=\"" << PREC << beg.y << "\" "
    "x2=\"" << PREC << end.x << "\" y2=\"" << PREC << end.y << "\" "
    "style=\"stroke:" << color << ";"
    "stroke-width:" << PREC << width << "\" />";
}

/*
 * class Rectangle
 */
void Rectangle::plot (ofstream& f) const {
  f << "<rect style=\"fill:" << color << ";stroke:" << color <<
    ";fill-opacity:" << opacity << ";stroke-width:1;stroke-miterlimit:4;"
    "stroke-dasharray:none\" id=\"rect3777\" "
    "width=\"" << PREC << width << "\" height=\"" << PREC << height <<"\" "
    "x=\"" << PREC << origin.x << "\" y=\"" << PREC << origin.y <<"\" "
    "ry=\"3\" />\n";
}

void Rectangle::plot_ir (ofstream& f) const {
  plot(f);
  f << "<rect style=\"fill-opacity:" << opacity << ";stroke-width:2;"
    "stroke-miterlimit:4;stroke-dasharray:none;fill:url(#Wavy);"
    "fill-rule:nonzero;opacity:1\" id=\"rect6217\" "
    "width=\"" << PREC << width << "\" height=\"" << PREC << height <<"\" "
    "x=\"" << PREC << origin.x << "\" y=\"" << PREC << origin.y <<"\" "
    "ry=\"3\" />\n";
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