#include "vizpaint.hpp"
#include "fn.hpp"
using namespace smashpp;
#define PREC  std::fixed << std::setprecision(2)

/*
 * class Text
 */
inline Text::Text (Point origin, const string& label) {
  config(origin, label);
}

inline void Text::config (Point origin_, const string& label_) {
  origin = origin_;
  label  = label_;
}

inline void Text::plot (ofstream& f, int hTune=0, int vTune=0, u8 fontSize=17,
                        const string& textAnchor="start") const {
  f << "<text xml:space=\"preserve\" style=\"font-size:40px;font-style:normal;"
    "font-weight:normal;line-height:125%%;letter-spacing:0px;word-spacing:0px;"
    "fill:#000000;fill-opacity:1;stroke:none;font-family:Sans\" "
    "x=\"" << PREC << origin.x + hTune << "\" "
    "y=\"" << PREC << origin.y + vTune << "\" "
    "id=\"corben\" sodipodi:linespacing=\"125%%\"><tspan sodipodi:role=\""
    "line\" id=\"tspan3804\" "
    "x=\"" << PREC << origin.x + hTune << "\" "
    "y=\"" << PREC << origin.y + vTune << "\" "
    "style=\"font-size:" << to_string(fontSize) << "px;font-style:normal;"
    "font-variant:normal;font-weight:normal;font-stretch:normal;"
    "text-align:start;line-height:125%%;writing-mode:lr-tb;"
    "text-anchor:" << textAnchor << ";font-family:Arial;"
    "-inkscape-font-specification:Arial\">" << label << "</tspan>\n</text>\n";
}

//todo age baze kuchik bud, bege e.g 100-200
inline void Text::plot_pos_ref (ofstream& f) const {
  plot(f, -5, 3, 10, "end");
}

inline void Text::plot_pos_tar (ofstream& f) const {
  plot(f, 5, 3, 10, "start");
}

/*
 * class Line
 */
inline Line::Line (Point beg, Point end_, double width, const string& color) {
  config(beg, end_, width, color);
}

inline void Line::config
(Point beg_, Point end_, double width_=2.0, const string& color_="black") {
  beg   = beg_;
  end   = end_;
  width = width_;
  color = color_;
}

inline void Line::plot (ofstream& f) const {
  f << "<line "
    "x1=\""           << PREC << beg.x << "\" y1=\"" << PREC << beg.y << "\" "
    "x2=\""           << PREC << end.x << "\" y2=\"" << PREC << end.y << "\" "
    "style=\"stroke:" <<         color << ";"
    "stroke-width:"   << PREC << width << "\" />";
}

/*
 * class Rectangle
 */
inline Rectangle::Rectangle
(Point origin, double width, double height, const string& color) {
  config(origin, width, height, color);
}

inline void Rectangle::config
(Point origin_, double width_, double height_, const string& color_="white") {
  origin = origin_;
  width  = width_;
  height = height_;
  color  = color_;
}

inline void Rectangle::plot (ofstream& f) const {
  f << "<rect style=\"fill:" << color << ";stroke:" << color <<
    ";fill-opacity:1;stroke-width:1;stroke-miterlimit:4;"
    "stroke-dasharray:none\" id=\"rect3777\" "
    "width=\"" << PREC << width    << "\" height=\"" << PREC << height   <<"\" "
    "x=\""     << PREC << origin.x << "\" y=\""      << PREC << origin.y <<"\" "
    "ry=\"3\" />\n";
}

inline void Rectangle::plot_ir (ofstream& f) const {
  plot(f);
  f << "<rect style=\"fill-opacity:1;stroke-width:2;stroke-miterlimit:4;"
    "stroke-dasharray:none;fill:url(#Wavy);fill-rule:nonzero;opacity:1\" "
    "id=\"rect6217\" "
    "width=\"" << PREC << width    << "\" height=\"" << PREC << height   <<"\" "
    "x=\""     << PREC << origin.x << "\" y=\""      << PREC << origin.y <<"\" "
    "ry=\"3\" />\n";
}

inline void Rectangle::plot_oval (ofstream& f) const {
  f << "<rect style=\"fill:" << color << ";fill-opacity:1;stroke-width:2;"
    "stroke-miterlimit:4;stroke-dasharray:none\" id=\"rectx\" "
    "width=\"" << PREC << width    << "\" height=\"" << PREC << height   <<"\" "
    "x=\""     << PREC << origin.x << "\" y=\""      << PREC << origin.y <<"\" "
    "ry=\"12.5\" />\n";
}

inline void Rectangle::plot_oval_ir (ofstream& f) const {
  plot_oval(f);
  f << "<rect style=\"fill-opacity:1;stroke-width:2;stroke-miterlimit:4;"
    "stroke-dasharray:nonestroke-dasharray:none;fill:url(#xtrace);"
    "fill-rule:nonzero;opacity:1\" id=\"recty\" "
    "width=\"" << PREC << width    << "\" height=\"" << PREC << height   <<"\" "
    "x=\""     << PREC << origin.x << "\" y=\""      << PREC << origin.y <<"\" "
    "ry=\"12.5\" />\n";
}

inline void Rectangle::plot_nrc (ofstream& f, char refTar=' ') const {
  f << "<rect style=\"fill:" << color << ";stroke:" << color <<
    ";fill-opacity:1;stroke-width:1;stroke-miterlimit:4;"
    "stroke-dasharray:none\" id=\"rect3777\" "
    "width=\""  << PREC << width/HORIZ_RATIO << "\" "
    "height=\"" << PREC << height            <<"\" "
    "x=\"" << PREC << (refTar=='r' ? origin.x - HORIZ_TUNE - width/HORIZ_RATIO
                                   : origin.x + width + HORIZ_TUNE) << "\" "
    "y=\"" << PREC << origin.y <<"\" ry=\"2\" />\n";
}

inline void Rectangle::plot_nrc_ref (ofstream& f) const {
  plot_nrc(f, 'r');
}

inline void Rectangle::plot_nrc_tar (ofstream& f) const {
  plot_nrc(f, 't');
}

inline void Rectangle::plot_complex
(ofstream& f, u8 showNRC, char refTar=' ') const {
  f << "<rect style=\"fill:" << color << ";stroke:" << color <<
    ";fill-opacity:1;stroke-width:1;stroke-miterlimit:4;"
    "stroke-dasharray:none\" id=\"rect3777\" "
    "width=\""  << PREC << width/HORIZ_RATIO << "\" "
    "height=\"" << PREC << height            <<"\" "
    "x=\"";
  if (refTar=='r')
    f << PREC << origin.x - (1+showNRC)*(HORIZ_TUNE+width/HORIZ_RATIO);
  else if (refTar=='t')
    f << PREC << origin.x + width + HORIZ_TUNE
                          + showNRC*(width/HORIZ_RATIO + HORIZ_TUNE);
  f << "\" y=\"" << PREC << origin.y <<"\" ry=\"2\" />\n";
}

inline void Rectangle::plot_complex_ref (ofstream& f, bool showNRC) const {
  showNRC ? plot_complex(f, 1, 'r') : plot_complex(f, 0, 'r');
}

inline void Rectangle::plot_complex_tar (ofstream& f, bool showNRC) const {
  showNRC ? plot_complex(f, 1, 't') : plot_complex(f, 0, 't');
}

inline void Rectangle::plot_chromosome (ofstream& f) const {
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
    "width=\"" << PREC << width    << "\" height=\"" << PREC << height   <<"\" "
    "x=\""     << PREC << origin.x << "\" y=\""      << PREC << origin.y <<"\" "
    "ry=\"3\" />\n";
}

/*
 * class Polygon
 */
inline Polygon::Polygon (Point one, Point two, Point three, Point four,
                         const string& lineColor,
                         const string& fillColor) {
  config(one, two, three, four, lineColor, fillColor);
}

inline void Polygon::config (Point one_, Point two_, Point three_, Point four_,
                             const string& lineColor_="black",
                             const string& fillColor_="white") {
  one       = one_;
  two       = two_;
  three     = three_;
  four      = four_;
  lineColor = lineColor_;
  fillColor = fillColor_;
}

inline void Polygon::plot (ofstream& f) const {
  f << "<polygon points=\""
    << PREC << one.x   << "," << PREC << one.y   << " "
    << PREC << two.x   << "," << PREC << two.y   << " "
    << PREC << three.x << "," << PREC << three.y << " "
    << PREC << four.x  << "," << PREC << four.y  << "\" "
    << "style=\"fill:" << fillColor   << ";stroke:" << fillColor << ";"
//    << "style=\"fill:" << fillColor   << ";stroke:" << lineColor << ";"
    << "stroke-width:1;stroke-opacity:0.4;fill-opacity:0.4\" />";
}

/*
 * class Circle
 */
inline Circle::Circle (Point origin, double radius, const string& fillColor) {
  config(origin, radius, fillColor);
}

inline void Circle::config
(Point origin_, double radius_, const string& fillColor_) {
  origin    = origin_;
  radius    = radius_;
  fillColor = fillColor_;
}

inline void Circle::plot (ofstream& f) const {
  f << "<circle "
    "cx=\"" << PREC << origin.x << "\" cy=\"" << PREC << origin.y << "\" "
    "r=\""  << PREC << radius   << "\" "
    "fill=\"" << fillColor << "\"/>";
}

/*
 * class VizPaint
 */
inline RgbColor VizPaint::hsv_to_rgb (const HsvColor& HSV) const {
  RgbColor RGB {};
  if (HSV.s==0) { RGB.r = RGB.g = RGB.b = HSV.v;    return RGB; }

  const auto
      region    = static_cast<u8>(HSV.h / 43),
      remainder = static_cast<u8>((HSV.h - region*43) * 6),
      p = static_cast<u8>((HSV.v * (255 - HSV.s)) >> 8),
      q = static_cast<u8>((HSV.v * (255 - ((HSV.s*remainder)>>8))) >> 8),
      t = static_cast<u8>((HSV.v * (255 - ((HSV.s*(255-remainder))>>8))) >> 8);

  switch (region) {
  case 0:   RGB.r=HSV.v;  RGB.g=t;      RGB.b=p;      break;
  case 1:   RGB.r=q;      RGB.g=HSV.v;  RGB.b=p;      break;
  case 2:   RGB.r=p;      RGB.g=HSV.v;  RGB.b=t;      break;
  case 3:   RGB.r=p;      RGB.g=q;      RGB.b=HSV.v;  break;
  case 4:   RGB.r=t;      RGB.g=p;      RGB.b=HSV.v;  break;
  default:  RGB.r=HSV.v;  RGB.g=p;      RGB.b=q;      break;
  }
  return RGB;
}

inline HsvColor VizPaint::rgb_to_hsv (const RgbColor& RGB) const {
  const u8 rgbMin { min({RGB.r, RGB.g, RGB.b}) };
  const u8 rgbMax { max({RGB.r, RGB.g, RGB.b}) };

  HsvColor HSV {};
  HSV.v = rgbMax;
  if (HSV.v==0) { HSV.h = HSV.s = 0;    return HSV; }

  HSV.s = static_cast<u8>(255 * static_cast<u16>((rgbMax-rgbMin)/HSV.v));
  if (HSV.s==0) { HSV.h = 0;            return HSV; }

  if (rgbMax == RGB.r)
    HSV.h = static_cast<u8>(43*(RGB.g-RGB.b)/(rgbMax-rgbMin));
  else if (rgbMax == RGB.g)
    HSV.h = static_cast<u8>(85 + 43*(RGB.b-RGB.r)/(rgbMax-rgbMin));
  else
    HSV.h = static_cast<u8>(171 + 43*(RGB.r-RGB.g)/(rgbMax-rgbMin));
  
  return HSV;
}

inline string VizPaint::rgb_color (u8 hue) const {
  HsvColor HSV (hue);
  RgbColor RGB {hsv_to_rgb(HSV)};
  return string_format("#%X%X%X", RGB.r, RGB.g, RGB.b);
}

template <typename ValueR, typename ValueG, typename ValueB>
inline string VizPaint::shade_color (ValueR r, ValueG g, ValueB b) const {
  return "rgb("+to_string(static_cast<u8>(r))+","
               +to_string(static_cast<u8>(g))+","
               +to_string(static_cast<u8>(b))+")";
}

template <typename Value>
inline double VizPaint::get_point (Value p) const {
  return 5 * p / static_cast<double>(ratio);
}

inline void VizPaint::print_head (ofstream& f, double w, double h) const {
  f << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    "<!-- IEETA 2014 using Inkscape -->\n""<svg\n"
    "xmlns:osb=\"http://www.openswatchbook.org/uri/2009/osb\"\n"
    "xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
    "xmlns:cc=\"http://creativecommons.org/ns#\"\n"
    "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
    "xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
    "xmlns=\"http://www.w3.org/2000/svg\"\n"
    "xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\"\n"
    "xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n"
    "width=\""  << w << "\"\n"
    "height=\"" << h << "\"\n"
    "id=\"svg2\"\n"
    "version=\"1.1\"\n"
    "inkscape:version=\"0.48.3.1 r9886\"\n"
    "sodipodi:docname=\"chromosomes\">\n"
    "<defs\n"
    "id=\"defs4\">\n"
      "<linearGradient id=\"linearGradient3755\" osb:paint=\"solid\">\n"
        "<stop style=\"stop-color:#aa00ff;stop-opacity:1;\" offset=\"0\"\n"
        "id=\"stop3757\" />\n"
      "</linearGradient>\n"
    "</defs>\n"
    "<sodipodi:namedview id=\"base\" pagecolor=\"#ffffff\"\n"
    "bordercolor=\"#666666\" borderopacity=\"1.0\"\n"
    "inkscape:pageopacity=\"0.0\" inkscape:pageshadow=\"2\"\n"
    "inkscape:zoom=\"0.49497475\" inkscape:cx=\"159.73474\"\n"
    "inkscape:cy=\"681.53008\" inkscape:document-units=\"px\"\n"
    "inkscape:current-layer=\"layer1\" showgrid=\"false\"\n"
    "inkscape:window-width=\"1066\" inkscape:window-height=\"790\"\n"
    "inkscape:window-x=\"278\" inkscape:window-y=\"1106\"\n"
    "inkscape:window-maximized=\"1\" />\n"
    "<metadata id=\"metadata7\">\n"
      "<rdf:RDF>\n"
        "<cc:Work rdf:about=\"\">\n"
          "<dc:format>image/svg+xml</dc:format>\n"
          "<dc:type rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\""
          " />\n"
          "<dc:title></dc:title>\n"
        "</cc:Work>\n"
      "</rdf:RDF>\n"
    "</metadata>\n"
    "<g inkscape:label=\"Camada 1\" inkscape:groupmode=\"layer\" id=\"layer1\" "
    ">\n";

  f << "<defs id=\"defs6211\"><pattern inkscape:stockid=\"Polka dots, large\" "
    "id=\"Polkadots-large\" patternTransform=\"translate(0,0)scale(10,10)\" "
    "height=\"10\" width=\"10\" patternUnits=\"userSpaceOnUse\" inkscape:"
    "collect=\"always\"> "
    "<circle id=\"circle4936\" r=\"0.45\" cy=\"0.810\" cx=\"2.567\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4938\" r=\"0.45\" cy=\"2.33\" cx=\"3.048\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4940\" r=\"0.45\" cy=\"2.415\" cx=\"4.418\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4942\" r=\"0.45\" cy=\"3.029\" cx=\"1.844\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4944\" r=\"0.45\" cy=\"1.363\" cx=\"6.08\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4946\" r=\"0.45\" cy=\"4.413\" cx=\"5.819\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4948\" r=\"0.45\" cy=\"4.048\" cx=\"4.305\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4950\" r=\"0.45\" cy=\"3.045\" cx=\"5.541\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4952\" r=\"0.45\" cy=\"5.527\" cx=\"4.785\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4954\" r=\"0.45\" cy=\"5.184\" cx=\"2.667\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4956\" r=\"0.45\" cy=\"1.448\" cx=\"7.965\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4958\" r=\"0.45\" cy=\"5.049\" cx=\"7.047\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4960\" r=\"0.45\" cy=\"0.895\" cx=\"4.340\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4962\" r=\"0.45\" cy=\"0.340\" cx=\"7.125\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4964\" r=\"0.45\" cy=\"1.049\" cx=\"9.553\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4966\" r=\"0.45\" cy=\"2.689\" cx=\"7.006\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4968\" r=\"0.45\" cy=\"2.689\" cx=\"8.909\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4970\" r=\"0.45\" cy=\"4.407\" cx=\"9.315\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4972\" r=\"0.45\" cy=\"3.870\" cx=\"7.820\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4974\" r=\"0.45\" cy=\"5.948\" cx=\"8.270\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4976\" r=\"0.45\" cy=\"7.428\" cx=\"7.973\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4978\" r=\"0.45\" cy=\"8.072\" cx=\"9.342\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4980\" r=\"0.45\" cy=\"9.315\" cx=\"8.206\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4982\" r=\"0.45\" cy=\"9.475\" cx=\"9.682\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4984\" r=\"0.45\" cy=\"6.186\" cx=\"9.688\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4986\" r=\"0.45\" cy=\"6.296\" cx=\"3.379\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4988\" r=\"0.45\" cy=\"8.204\" cx=\"2.871\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4990\" r=\"0.45\" cy=\"8.719\" cx=\"4.59\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4992\" r=\"0.45\" cy=\"9.671\" cx=\"3.181\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4994\" r=\"0.45\" cy=\"7.315\" cx=\"5.734\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4996\" r=\"0.45\" cy=\"6.513\" cx=\"6.707\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle4998\" r=\"0.45\" cy=\"9.670\" cx=\"5.730\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5000\" r=\"0.45\" cy=\"8.373\" cx=\"6.535\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5002\" r=\"0.45\" cy=\"7.154\" cx=\"4.37\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5004\" r=\"0.45\" cy=\"7.25\" cx=\"0.622\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5006\" r=\"0.45\" cy=\"5.679\" cx=\"0.831\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5008\" r=\"0.45\" cy=\"8.519\" cx=\"1.257\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5010\" r=\"0.45\" cy=\"6.877\" cx=\"1.989\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5012\" r=\"0.45\" cy=\"3.181\" cx=\"0.374\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5014\" r=\"0.45\" cy=\"1.664\" cx=\"1.166\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5016\" r=\"0.45\" cy=\"0.093\" cx=\"1.151\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5018\" r=\"0.45\" cy=\"10.093\" cx=\"1.151\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5020\" r=\"0.45\" cy=\"4.451\" cx=\"1.302\" "
    "style=\"fill:black;stroke:none\" />"
    "<circle id=\"circle5022\" r=\"0.45\" cy=\"3.763\" cx=\"3.047\" "
    "style=\"fill:black;stroke:none\" />"
    "</pattern></defs>";

  f << "<defs id=\"ffff\"><pattern inkscape:stockid=\"Wavy\" id=\"Wavy\" "
    "height=\"5.1805778\" width=\"30.0\" patternUnits=\"userSpaceOnUse\" "
    "inkscape:collect=\"always\"><path id=\"path5114\" d=\"M 7.597,0.061 "
    "C 5.079,-0.187 2.656,0.302 -0.01,1.788 L -0.01,3.061 C 2.773,1.431 "
    "5.173,1.052 7.472,1.280 C 9.770,1.508 11.969,2.361 14.253,3.218 "
    "C 18.820,4.931 23.804,6.676 30.066,3.061 L 30.062,1.788 C 23.622,5.497 "
    "19.246,3.770 14.691,2.061 C 12.413,1.207 10.115,0.311 7.597,0.061 z \" "
    "style=\"fill:black;stroke:none;\" /></pattern></defs>";

  f << "<defs id=\"defs6219\"><pattern inkscape:stockid=\"xtrace\" id="
    "\"xtrace\" height=\"20.0\" width=\"20.0\" patternUnits=\"userSpaceOnUse\" "
    "inkscape:collect=\"always\"><path style=\"fill:#000000;stroke:#000000;"
    "stroke-width:0.30;stroke-linecap:butt;stroke-linejoin:miter;"
    "stroke-opacity:1;stroke-miterlimit:0;stroke-dasharray:none;"
    "fill-opacity:1\" d=\"m 0.0,10.0 25.00000,-25.0 0,5 -25.00000,25.00000 z\" "
    "id=\"path7213\" inkscape:connector-curvature=\"0\" /><path style=\"fill:"
    "#000000;fill-opacity:1;stroke:#000000;stroke-width:0.30;stroke-linecap:"
    "butt;stroke-linejoin:miter;stroke-miterlimit:0;stroke-opacity:1;"
    "stroke-dasharray:none\" d=\"m 0.0,30.0 25.00000,-25.0 0,5 "
    "-25.00000,25.00000 z\" id=\"path7213-8\" inkscape:connector-curvature="
    "\"0\" /><path style=\"fill:#000000;fill-opacity:1;stroke:#000000;"
    "stroke-width:0.30;stroke-linecap:butt;stroke-linejoin:miter;"
    "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d=\"m "
    "0.0,50.0 25.00000,-25.0 0,5 -25.00000,25.00000 z\" id=\"path7213-9\" "
    "inkscape:connector-curvature=\"0\" /><path style=\"fill:#000000;"
    "fill-opacity:1;stroke:#000000;stroke-width:0.30;stroke-linecap:butt;"
    "stroke-linejoin:miter;stroke-miterlimit:0;stroke-opacity:1;"
    "stroke-dasharray:none\" d=\"m 0.0,70.0 25.00000,-25.0 0,5 "
    "-25.00000,25.00000 z\" id=\"path7213-10\" inkscape:connector-curvature="
    "\"0\" /><path style=\"fill:#000000;fill-opacity:1;stroke:#000000;"
    "stroke-width:0.30;stroke-linecap:butt;stroke-linejoin:miter;"
    "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d=\"m "
    "0.0,90.0 25.00000,-25.0 0,5 -25.00000,25.00000 z\" id=\"path7213-11\" "
    "inkscape:connector-curvature=\"0\" /><path style=\"fill:#000000;"
    "fill-opacity:1;stroke:#000000;stroke-width:0.30;stroke-linecap:butt;"
    "stroke-linejoin:miter;stroke-miterlimit:0;stroke-opacity:1;"
    "stroke-dasharray:none\" d=\"m m 0.0,110.0 25.00000,-25.0 0,05 "
    "-25.00000,25.00000 z\" id=\"path7213-12\" inkscape:connector-curvature="
    "\"0\" /></pattern></defs>";

  f << "<defs id=\"defs4\"><pattern id=\"dallas\" patternTransform=\""
    "translate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" "
    "patternUnits=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:#000000;"
    "stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;"
    "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d=\"m "
    "-0.00788,2.37557 4.76251,0\" id=\"path2985\" /><path style=\"fill:none;"
    "stroke:#000000;stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;"
    "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d=\"m "
    "2.37338,-0.00568 0,4.76251\" id=\"path2985-1\" /></pattern></defs>";

  f << "<defs id=\"defs4\"><pattern id=\"lineX\" patternTransform=\""
    "translate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" "
    "patternUnits=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:#000000;"
    "stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;"
    "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d=\"m "
    "-0.00788,2.37557 4.76251,0\" id=\"path2985\" /></pattern></defs>";

  f << "<defs id=\"defs4\"><pattern id=\"stripeX\" patternTransform=\""
    "translate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" "
    "patternUnits=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:#000000;"
    "stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;"
    "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d=\"m "
    "2.37338,-0.00568 0,4.76251\" id=\"path2985-1\" /></pattern></defs>";
}

inline void VizPaint::print_tail (ofstream& f) const {
  f << "</g>\n</svg>";
}

inline void VizPaint::config (double width_, double space_, u64 refSize_, u64 tarSize_) {
  ratio   = static_cast<u32>(max(refSize_,tarSize_) / PAINT_SCALE);
  width   = width_;
  space   = space_;
  refSize = get_point(refSize_);
  tarSize = get_point(tarSize_);
  maxSize = max(refSize, tarSize);
}

void VizPaint::print_plot (Param& p) {
  check_file(p.viz_posFile);
  ifstream fPos (p.viz_posFile);
  ofstream fPlot(p.viz_image);

  u64 n_refBases=0, n_tarBases=0;
  string watermark;
  fPos >> watermark >> n_refBases >> n_tarBases;
  if (watermark != POS_HDR)
    error("unknown file format for positions.");
  if (p.verbose) {
    cerr <<
      "==[ CONFIGURATION ]================="                              <<"\n"
      "Verbose mode ....................... yes"                          <<"\n"
      "Reference number of bases .......... " << n_refBases               <<"\n"
      "Target number of bases ............. " << n_tarBases               <<"\n"
      "Link type .......................... " << p.viz_link                   <<"\n"
      "Chromosomes design characteristics:"                               <<"\n"
      "  [+] Width ........................ " << p.viz_width                  <<"\n"
      "  [+] Space ........................ " << p.viz_space                  <<"\n"
      "  [+] Multiplication factor ........ " << p.viz_mult                   <<"\n"
      "  [+] Begin ........................ " << p.viz_start                  <<"\n"
      "  [+] Minimum ...................... " << p.viz_min                    <<"\n"
      "  [+] Show regular ................. " <<(p.viz_regular ? "yes" : "no")<<"\n"
      "  [+] Show inversions .............. " <<(p.viz_inverse ? "yes" : "no")<<"\n"
      "Output map filename ................ " << p.viz_image                  <<"\n"
                                                                        << endl;
  }

  cerr << "==[ PROCESSING ]====================\n"
          "Printing plot ...\n";

  config(p.viz_width, p.viz_space, n_refBases, n_tarBases);

  print_head(fPlot, PAINT_CX + width + space + width + PAINT_CX,
                    maxSize + PAINT_EXTRA);

  auto line   = make_shared<Line>();
  line->width = 2.0;
  auto rect   = make_shared<Rectangle>();
  auto poly   = make_shared<Polygon>();
  auto text   = make_shared<Text>();

  rect->color  = "#ffffff";
  rect->origin = Point(0, 0);
  rect->width  = PAINT_CX + width + space + width + PAINT_CX;
  rect->height = maxSize + PAINT_EXTRA;
  rect->plot(fPlot);

//  rect->origin = Point(cx, cy);
//  rect->width  = width;
//  rect->height = refSize;
//  rect->plot_oval(fPlot);
//
//  rect->height = tarSize;
//  rect->plot_oval(fPlot);

  text->origin = Point(cx, cy - 15);
  //todo
//  if (p.test) { text->label=ref name }
  text->label  = "REF";
  text->plot(fPlot);

  text->origin = Point(cx + width + space, cy - 15);
  //todo
//  if (p.test) { text->label=tar name }
  text->label  = "TAR";
  text->plot(fPlot);

  // IF MINIMUM IS SET DEFAULT, RESET TO BASE MAX PROPORTION
  if (p.viz_min==0)  p.viz_min=static_cast<u32>(maxSize / 100);

  auto customColor = [=] (u32 start) {
    return rgb_color(static_cast<u8>(start * p.viz_mult));
  };
  auto nrcColor = [=] (double entropy) {
    keep_in_range(entropy, 0.0, 2.0);
    return shade_color(0, ceil(entropy*100+55), 55);
  };
  auto complColor = [=] (double entropy) {
    keep_in_range(entropy, 0.0, 2.0);
    return shade_color(0, 55, ceil(entropy*100+55));
  };
  i64 begRef, endRef, begTar, endTar;
  double entRef, entTar;
  string complRef, complTar;
  u64 n_regular=0, n_inverse=0, n_ignored=0;
  for (; fPos >> begRef>>endRef>>entRef>>complRef
              >> begTar>>endTar>>entTar>>complTar; ++p.viz_start) {
    if (abs(endRef-begRef)<p.viz_min || abs(begTar-endTar)<p.viz_min) {
      ++n_ignored;
      continue;
    }

    if (p.viz_showPos) {
      double X = 0;
      if     (p.viz_showNRC && p.viz_showComplex)  X=2*(HORIZ_TUNE + width/HORIZ_RATIO);
      else if(p.viz_showNRC ^  p.viz_showComplex)  X=   HORIZ_TUNE + width/HORIZ_RATIO;

      text->origin = Point(cx - X, cy + get_point(begRef));
      text->label  = to_string(begRef);
      text->plot_pos_ref(fPlot);
      text->origin = Point(cx - X, cy + get_point(endRef));
      text->label  = to_string(endRef);
      text->plot_pos_ref(fPlot);

      text->origin = Point(cx + width + space + width + X,
                           cy + get_point(begTar));
      text->label  = to_string(begTar);
      text->plot_pos_tar(fPlot);
      text->origin = Point(cx + width + space + width + X,
                           cy + get_point(endTar));
      text->label  = to_string(endTar);
      text->plot_pos_tar(fPlot);
    }

    if (endTar > begTar) {
      if (p.viz_regular) {
        rect->width  = width;
        rect->color  = customColor(p.viz_start);
        rect->origin = Point(cx, cy + get_point(begRef));
        rect->height = get_point(endRef-begRef);
        rect->plot(fPlot);
        if (p.viz_showNRC) {
          rect->color = nrcColor(entRef);
          rect->plot_nrc_ref(fPlot);
        }
        if (p.viz_showComplex) {//todo
          rect->color = complColor(entRef);
          rect->plot_complex_ref(fPlot, p.viz_showNRC);
        }

        rect->color  = customColor(p.viz_start);
        rect->origin = Point(cx + width + space, cy + get_point(begTar));
        rect->height = get_point(endTar-begTar);
        rect->plot(fPlot);
        if (p.viz_showNRC) {
          rect->color = nrcColor(entTar);
          rect->plot_nrc_tar(fPlot);
        }
        if (p.viz_showComplex) {//todo
          rect->color = complColor(entTar);
          rect->plot_complex_tar(fPlot, p.viz_showNRC);
        }

        switch (p.viz_link) {
        case 5:
          poly->lineColor = "grey";
          poly->fillColor = customColor(p.viz_start);
          poly->one   = Point(cx + width,         cy + get_point(begRef));
          poly->two   = Point(cx + width,         cy + get_point(endRef));
          poly->three = Point(cx + width + space, cy + get_point(endTar));
          poly->four  = Point(cx + width + space, cy + get_point(begTar));
          poly->plot(fPlot);
          break;
        case 1:
          line->color = "black";
          line->beg = Point(cx + width,
                            cy + get_point(begRef+(endRef-begRef)/2.0));
          line->end = Point(cx + width + space,
                            cy + get_point(begTar+(endTar-begTar)/2.0));
          line->plot(fPlot);
          break;
        case 2:
          line->color = customColor(p.viz_start);
          line->beg = Point(cx + width,
                            cy + get_point(begRef+(endRef-begRef)/2.0));
          line->end = Point(cx + width + space,
                            cy + get_point(begTar+(endTar-begTar)/2.0));
          line->plot(fPlot);
          break;
        case 3:
          line->color = "black";
          line->beg = Point(cx + width,         cy + get_point(begRef));
          line->end = Point(cx + width + space, cy + get_point(begTar));
          line->plot(fPlot);

          line->beg = Point(cx + width,         cy + get_point(endRef));
          line->end = Point(cx + width + space, cy + get_point(endTar));
          line->plot(fPlot);
          break;
        case 4:
          line->color = customColor(p.viz_start);
          line->beg = Point(cx + width,         cy + get_point(begRef));
          line->end = Point(cx + width + space, cy + get_point(begTar));
          line->plot(fPlot);

          line->beg = Point(cx + width,         cy + get_point(endRef));
          line->end = Point(cx + width + space, cy + get_point(endTar));
          line->plot(fPlot);
          break;
        default:break;
        }
        ++n_regular;
      }
    }
    else {
      if (p.viz_inverse) {
        rect->width  = width;
        rect->color  = customColor(p.viz_start);
        rect->origin = Point(cx, cy + get_point(begRef));
        rect->height = get_point(endRef-begRef);
        rect->plot(fPlot);
        if (p.viz_showNRC) {
          rect->color = nrcColor(entRef);
          rect->plot_nrc_ref(fPlot);
        }
        if (p.viz_showComplex) {//todo
          rect->color = complColor(entRef);
          rect->plot_complex_ref(fPlot, p.viz_showNRC);
        }

        rect->color  = customColor(p.viz_start);
        rect->origin = Point(cx + width + space, cy + get_point(endTar));
        rect->height = get_point(begTar-endTar);
        rect->plot_ir(fPlot);
        if (p.viz_showNRC) {
          rect->color = nrcColor(entTar);
          rect->plot_nrc_tar(fPlot);
        }
        if (p.viz_showComplex) {//todo
          rect->color = complColor(entTar);
          rect->plot_complex_tar(fPlot, p.viz_showNRC);
        }

        switch (p.viz_link) {
        case 5:
          poly->lineColor = "grey";
          poly->fillColor = customColor(p.viz_start);
          poly->one   = Point(cx + width,         cy + get_point(begRef));
          poly->two   = Point(cx + width,         cy + get_point(endRef));
          poly->three = Point(cx + width + space, cy + get_point(endTar));
          poly->four  = Point(cx + width + space, cy + get_point(begTar));
          poly->plot(fPlot);
          break;
        case 1:
          line->color = "green";
          line->beg = Point(cx + width,
                            cy + get_point(begRef+(endRef-begRef)/2.0));
          line->end = Point(cx + width + space,
                            cy + get_point(endTar+(begTar-endTar)/2.0));
          line->plot(fPlot);
          break;
        case 2:
          line->color = customColor(p.viz_start);
          line->beg = Point(cx + width,
                            cy + get_point(begRef+(endRef-begRef)/2.0));
          line->end = Point(cx + width + space,
                            cy + get_point(endTar+(begTar-endTar)/2.0));
          line->plot(fPlot);
          break;
        case 3:
          line->color = "green";
          line->beg = Point(cx + width,         cy + get_point(begRef));
          line->end = Point(cx + width + space, cy + get_point(begTar));
          line->plot(fPlot);

          line->beg = Point(cx + width,         cy + get_point(endRef));
          line->end = Point(cx + width + space, cy + get_point(endTar));
          line->plot(fPlot);
          break;
        case 4:
          line->color = customColor(p.viz_start);
          line->beg = Point(cx + width,         cy + get_point(begRef));
          line->end = Point(cx + width + space, cy + get_point(begTar));
          line->plot(fPlot);

          line->beg = Point(cx + width,         cy + get_point(endRef));
          line->end = Point(cx + width + space, cy + get_point(endTar));
          line->plot(fPlot);
          break;
        default:break;
        }
        ++n_inverse;
      }
    }
  }
  fPos.seekg(ios::beg);

  if (p.viz_regular)    cerr << "Found "   << n_regular << " regular regions.\n";
  if (p.viz_inverse)    cerr << "Found "   << n_inverse << " inverted regions.\n";
  if (p.verbose)    cerr << "Ignored " << n_ignored << " regions.\n";

  rect->width  = width;
  rect->origin = Point(cx, cy);
  rect->height = refSize;
  rect->plot_chromosome(fPlot);

  rect->origin = Point(cx + width + space, cy);
  rect->height = tarSize;
  rect->plot_chromosome(fPlot);

  print_tail(fPlot);

  cerr << "Done!                       \n";
  fPos.close();
  fPlot.close();
}