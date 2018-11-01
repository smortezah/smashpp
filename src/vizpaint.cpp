#include "vizpaint.hpp"
#include "fn.hpp"
using namespace smashpp;

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

inline void Text::plot (ofstream& f) const {
  f << "<text xml:space=\"preserve\" style=\"font-size:40px;font-style:normal;"
    "font-weight:normal;line-height:125%%;letter-spacing:0px;word-spacing:0px;"
    "fill:#000000;fill-opacity:1;stroke:none;font-family:Sans\" "
    "x=\"" << std::fixed << setprecision(2) << origin.x << "\" "
    "y=\"" << std::fixed << setprecision(2) << origin.y << "\" "
    "id=\"corben\" sodipodi:linespacing=\"125%%\"><tspan sodipodi:role=\""
    "line\" id=\"tspan3804\" "
    "x=\"" << std::fixed << setprecision(2) << origin.x << "\" "
    "y=\"" << std::fixed << setprecision(2) << origin.y << "\" "
    "style=\"font-size:18px;font-style:normal;font-variant:normal;font-weight:"
    "normal;font-stretch:normal;text-align:start;line-height:125%%;"
    "writing-mode:lr-tb;text-anchor:start;font-family:Arial;"
    "-inkscape-font-specification:Arial\">" << label << "</tspan>\n</text>\n";
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
    "x1=\"" << std::fixed << setprecision(2) << beg.x << "\" "
    "y1=\"" << std::fixed << setprecision(2) << beg.y << "\" "
    "x2=\"" << std::fixed << setprecision(2) << end.x << "\" "
    "y2=\"" << std::fixed << setprecision(2) << end.y << "\" "
    "style=\"stroke:" << color << ";"
    "stroke-width:" << std::fixed << setprecision(2) << width << "\" />";
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
  f << "<rect style=\"fill:" << color << ";fill-opacity:1;stroke-width:2;"
    "stroke-miterlimit:4;stroke-dasharray:none\" id=\"rect3777\" "
    "width=\""  << std::fixed << setprecision(2) << width << "\" "
    "height=\"" << std::fixed << setprecision(2) << height << "\" "
    "x=\""      << std::fixed << setprecision(2) << origin.x << "\" "
    "y=\""      << std::fixed << setprecision(2) << origin.y << "\" "
    "ry=\"0\" />\n";
}

inline void Rectangle::plot_ir (ofstream& f) const {
  plot(f);
  f << "<rect style=\"fill-opacity:1;stroke-width:2;stroke-miterlimit:4;"
    "stroke-dasharray:none;fill:url(#Wavy);fill-rule:nonzero;opacity:1\" "
    "id=\"rect6217\" "
    "width=\""  << std::fixed << setprecision(2) << width << "\" "
    "height=\"" << std::fixed << setprecision(2) << height << "\" "
    "x=\""      << std::fixed << setprecision(2) << origin.x << "\" "
    "y=\""      << std::fixed << setprecision(2) << origin.y << "\" "
    "ry=\"0\" />\n";
}

inline void Rectangle::plot_oval (ofstream& f) const {
  f << "<rect style=\"fill:" << color << ";fill-opacity:1;stroke-width:2;"
    "stroke-miterlimit:4;stroke-dasharray:none\" id=\"rectx\" "
    "width=\""  << std::fixed << setprecision(2) << width << "\" "
    "height=\"" << std::fixed << setprecision(2) << height << "\" "
    "x=\""      << std::fixed << setprecision(2) << origin.x << "\" "
    "y=\""      << std::fixed << setprecision(2) << origin.y << "\" "
    "rx=\"12.5\" ry=\"12.5\" />\n";
}

inline void Rectangle::plot_oval_ir (ofstream& f) const {
  plot_oval(f);
  f << "<rect style=\"fill-opacity:1;stroke-width:2;stroke-miterlimit:4;"
    "stroke-dasharray:nonestroke-dasharray:none;fill:url(#xtrace);"
    "fill-rule:nonzero;opacity:1\" id=\"recty\" "
    "width=\""  << std::fixed << setprecision(2) << width << "\" "
    "height=\"" << std::fixed << setprecision(2) << height << "\" "
    "x=\""      << std::fixed << setprecision(2) << origin.x << "\" "
    "y=\""      << std::fixed << setprecision(2) << origin.y << "\" "
    "ry=\"12.5\" />\n";
}

inline void Rectangle::plot_chromosome (ofstream& f) const {
  const string borderColor {"#000000"};
//  double  wk = w / 2 + 0.5;
/*
  fprintf(F, "<path "
         "d=\"m %.2lf,"
         "%.2lf 0,"
         "%.2lf c 0, -8.31 6.69, -%.2lf %.2lf, -%.2lf l -%.2lf,0 z m %.2lf,"
         "0 c 8.31,0 %.2lf,6.69 %.2lf,%.2lf l 0,-%.2lf -%.2lf,0 z\" "
         "id=\"rect3787\" style=\"fill:#fff;fill-opacity:1;fill-rule:"
         "nonzero;stroke:none\" />", x-0.5, y-0.5,
         wk, wk, wk, wk, wk, wk, wk, wk, wk, wk, wk);

  fprintf(F, "<path "
         "d=\"m %.2lf,"
         "%.2lf 0,"
         "-%.2lf c 0,8.31 -6.69, %.2lf -%.2lf, %.2lf l %.2lf,0 z m -%.2lf,"
         "0 c -8.31,0 -%.2lf,-6.69 -%.2lf,-%.2lf l 0,%.2lf %.2lf,0 z\" "
         "id=\"rect3787\" style=\"fill:#fff;fill-opacity:1;fill-rule:"
         "nonzero;stroke:none\" />", x+0.5+w, y+0.5+h,
         wk, wk, wk, wk, wk, wk, wk, wk, wk, wk, wk);
*/

  f << "<rect style=\"fill:none;stroke:" << borderColor << ";stroke-width:2;"
    "stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;"
    "stroke-opacity:1;stroke-dasharray:none\" id=\"rect2985\" "
    "width=\""  << std::fixed << setprecision(2) << width << "\" "
    "height=\"" << std::fixed << setprecision(2) << height << "\" "
    "x=\""      << std::fixed << setprecision(2) << origin.x << "\" "
    "y=\""      << std::fixed << setprecision(2) << origin.y << "\" "
    "rx=\"20\" ry=\"20\" />\n";
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
    << std::fixed << setprecision(2) << one.x   << ","
    << std::fixed << setprecision(2) << one.y   << " "
    << std::fixed << setprecision(2) << two.x   << ","
    << std::fixed << setprecision(2) << two.y   << " "
    << std::fixed << setprecision(2) << three.x << ","
    << std::fixed << setprecision(2) << three.y << " "
    << std::fixed << setprecision(2) << four.x  << ","
    << std::fixed << setprecision(2) << four.y  << "\" "
    << "style=\"fill:" << fillColor << ";stroke:" << lineColor << ";"
    << "stroke-width:1;fill-opacity:0.7\" />";
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
    "cx=\"" << std::fixed << setprecision(2) << origin.x << "\" "
    "cy=\"" << std::fixed << setprecision(2) << origin.y << "\" "
    "r=\""  << std::fixed << setprecision(2) << radius   << "\" "
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

inline void VizPaint::print_tail (ofstream& f) const
{
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

////////////////////////////////////////////////////////////////////////////////
//// - - - - - - - - - - - - - - - - - - P L O T - - - - - - - - - - - - - - - -
void VizPaint::print_plot (VizParam& p) {
  check_file(p.posFile);
  ifstream fPos(p.posFile);
  ofstream fPlot(p.image);

  u64 tarNoBases=0, refNoBases=0;
  string watermark;
  fPos >> watermark >> tarNoBases >> refNoBases;
  if (watermark != "#SCF")
    error("unknown file format for positions.");
  if (p.verbose) {
    cerr <<
      "==[ CONFIGURATION ]================="                              <<"\n"
      "Verbose mode ....................... yes"                          <<"\n"
      "Reference number of bases .......... " << refNoBases               <<"\n"
      "Target number of bases ............. " << tarNoBases               <<"\n"
      "Link type .......................... " << p.link                   <<"\n"
      "Chromosomes design characteristics:"                               <<"\n"
      "  [+] Width ........................ " << p.width                  <<"\n"
      "  [+] Space ........................ " << p.space                  <<"\n"
      "  [+] Multiplication factor ........ " << p.mult                   <<"\n"
      "  [+] Begin ........................ " << p.start                  <<"\n"
      "  [+] Minimum ...................... " << p.minimum                <<"\n"
      "  [+] Show regular ................. " <<(p.regular   ?"yes":"no") <<"\n"
      "  [+] Show inversions .............. " <<(p.inversion ?"yes":"no") <<"\n"
      "Output map filename ................ " << p.image                  <<"\n"
                                                                        << endl;
  }

  cerr << "==[ PROCESSING ]====================\n"
          "Printing plot ...\n";

  config(p.width, p.space, refNoBases, tarNoBases);

  print_head(fPlot, 2*PAINT_CX + 2*width + space, maxSize + PAINT_EXTRA);

  auto line      = make_shared<Line>();
  auto rectangle = make_shared<Rectangle>();
  auto polygon   = make_shared<Polygon>();
  auto text      = make_shared<Text>();

  const string backColor {"#ffffff"};
  rectangle->config(Point(0, 0), 2*PAINT_CX + 2*width + space,
                    maxSize + PAINT_EXTRA, backColor);
  rectangle->plot(fPlot);

  rectangle->config(Point(cx, cy), width, refSize, backColor);
  rectangle->plot_oval(fPlot);

  rectangle->config(Point(cx, cy), width, tarSize, backColor);
  rectangle->plot_oval(fPlot);

  text->config(Point(cx, cy - 15), "REF");
  text->plot(fPlot);

  text->config(Point(cx + width + space, cy - 15), "TAR");
  text->plot(fPlot);

  // IF MINIMUM IS SET DEFAULT, RESET TO BASE MAX PROPORTION
  if(p.minimum==0)  p.minimum = static_cast<u32>(maxSize / 100);

  i64 begPosTar, endPosTar, begPosRef, endPosRef;
  double entTar, entRef;
  u64 no_regular=0, no_inverse=0, no_ignored=0;
  while (fPos >> begPosTar>>endPosTar>>entTar>>begPosRef>>endPosRef>>entRef) {
    if (abs(endPosRef-begPosRef) < p.minimum ||
        abs(begPosTar-endPosTar) < p.minimum) {
      ++no_ignored;
      continue;
    }

    if (endPosRef > begPosRef) {
      if (p.regular) {
        switch (p.link) {
          case 1:
            line->config(
                    Point(cx + width,
                          cy + get_point(begPosRef+(endPosRef-begPosRef)/2.0)),
                    Point(cx + space + width,
                          cy + get_point(begPosTar+(endPosTar-begPosTar)/2.0)),
                    2, "black");
            line->plot(fPlot);
            break;
          case 2:
            line->config(
                    Point(cx + width,
                          cy + get_point(begPosRef+(endPosRef-begPosRef)/2.0)),
                    Point(cx + space + width,
                          cy + get_point(begPosTar+(endPosTar-begPosTar)/2.0)),
                    2, rgb_color(static_cast<u8>(p.start*p.mult)));
            line->plot(fPlot);
            break;
          case 3:
            line->config(Point(cx + width,         cy + get_point(begPosRef)),
                         Point(cx + space + width, cy + get_point(begPosTar)),
                         2, "black");
            line->plot(fPlot);

            line->config(Point(cx + width,         cy + get_point(endPosRef)),
                         Point(cx + space + width, cy + get_point(endPosTar)),
                         2, "black");
            line->plot(fPlot);
            break;
          case 4:
            line->config(Point(cx + width,         cy + get_point(begPosRef)),
                         Point(cx + space + width, cy + get_point(begPosTar)),
                         2, rgb_color(static_cast<u8>(p.start*p.mult)));
            line->plot(fPlot);

            line->config(Point(cx + width,         cy + get_point(endPosRef)),
                         Point(cx + space + width, cy + get_point(endPosTar)),
                         2, rgb_color(static_cast<u8>(p.start*p.mult)));
            line->plot(fPlot);
            break;
          case 5:
            polygon->config(Point(cx + width,        cy + get_point(begPosRef)),
                            Point(cx + width,        cy + get_point(endPosRef)),
                            Point(cx + space +width, cy + get_point(endPosTar)),
                            Point(cx + space +width, cy + get_point(begPosTar)),
                            "grey", rgb_color(static_cast<u8>(p.start*p.mult)));
            polygon->plot(fPlot);
            break;
          default:break;
        }
        rectangle->config(Point(cx, cy + get_point(begPosRef)),
                          width, get_point(endPosRef-begPosRef),
                          rgb_color(static_cast<u8>(p.start*p.mult)));
        rectangle->plot(fPlot);

        rectangle->config(Point(cx + space + width, cy + get_point(begPosTar)),
                          width, get_point(endPosTar-begPosTar),
                          rgb_color(static_cast<u8>(p.start*p.mult)));
        rectangle->plot(fPlot);

        ++no_regular;
      }
    }
    else {
      if (p.inversion) {
        switch (p.link) {
          case 1:
            line->config(
                    Point(cx + width,
                          cy + get_point(endPosRef+(begPosRef-endPosRef)/2.0)),
                    Point(cx + space + width,
                          cy + get_point(endPosTar+(begPosTar-endPosTar)/2.0)),
                    2, "green");
            line->plot(fPlot);
            break;
          case 2:
            line->config(
                    Point(cx + width,
                          cy + get_point(endPosRef+(begPosRef-endPosRef)/2.0)),
                    Point(cx + space + width,
                          cy + get_point(endPosTar+(begPosTar-endPosTar)/2.0)),
                    2, rgb_color(static_cast<u8>(p.start*p.mult)));
            line->plot(fPlot);
            break;
          case 3:
            line->config(Point(cx + width,         cy + get_point(endPosRef)),
                         Point(cx + space + width, cy + get_point(endPosTar)),
                         2, "green");
            line->plot(fPlot);

            line->config(Point(cx + width,         cy + get_point(begPosRef)),
                         Point(cx + space + width, cy + get_point(begPosTar)),
                         2, "green");
            line->plot(fPlot);
            break;
          case 4:
            line->config(Point(cx + width,         cy + get_point(endPosRef)),
                         Point(cx + space + width, cy + get_point(endPosTar)),
                         2, rgb_color(static_cast<u8>(p.start*p.mult)));
            line->plot(fPlot);

            line->config(Point(cx + width,         cy + get_point(begPosRef)),
                         Point(cx + space + width, cy + get_point(begPosTar)),
                         2, rgb_color(static_cast<u8>(p.start*p.mult)));
            line->plot(fPlot);
            break;
          case 5:
            polygon->config(Point(cx + width,        cy + get_point(endPosRef)),
                            Point(cx + width,        cy + get_point(begPosRef)),
                            Point(cx + space +width, cy + get_point(begPosTar)),
                            Point(cx + space +width, cy + get_point(endPosTar)),
                            "grey", rgb_color(static_cast<u8>(p.start*p.mult)));
            polygon->plot(fPlot);
            break;
          default:break;
        }
        rectangle->config(Point(cx, cy + get_point(endPosRef)),
                          width, get_point(begPosRef-endPosRef),
                          rgb_color(static_cast<u8>(p.start*p.mult)));
        rectangle->plot(fPlot);

        rectangle->config(Point(cx + space + width, cy + get_point(begPosTar)),
                          width, get_point(endPosTar-begPosTar),
                          rgb_color(static_cast<u8>(p.start*p.mult)));
        rectangle->plot_ir(fPlot);

        ++no_inverse;
      }
    }

    ++p.start;
  }
  fPos.seekg(ios::beg);

  if (p.regular)    cerr << "Found "   << no_regular << " regular regions.\n";
  if (p.inversion)  cerr << "Found "   << no_inverse << " inverted regions.\n";
  if (p.verbose)    cerr << "Ignored " << no_ignored << " regions.\n";

  rectangle->config(Point(cx, cy), width, refSize);
  rectangle->plot_chromosome(fPlot);

  rectangle->config(Point(cx + space + width, cy), width, tarSize);
  rectangle->plot_chromosome(fPlot);

  print_tail(fPlot);

  cerr << "Done!                       \n";
  fPos.close();
  fPlot.close();
}