#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "vizpaint.h"
#include "mem.h"
#include <fstream>
using namespace smashpp;
//namespace smashpp { u32 ratio; }

inline RgbColor VizPaint::hsv_to_rgb (HsvColor HSV) const {
  RgbColor RGB {};
  if (HSV.s == 0) {
    RGB.r = HSV.v;
    RGB.g = HSV.v;
    RGB.b = HSV.v;
    return RGB;
  }

  auto region    = static_cast<u8>(HSV.h / 43);
  auto remainder = static_cast<u8>((HSV.h - region*43) * 6);
  auto p = static_cast<u8>((HSV.v * (255 - HSV.s)) >> 8);
  auto q = static_cast<u8>((HSV.v * (255 - ((HSV.s*remainder)>>8))) >> 8);
  auto t = static_cast<u8>((HSV.v * (255 - ((HSV.s*(255-remainder))>>8))) >> 8);

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

inline string VizPaint::rgb_color (u8 hue) const {
  HsvColor HSV(hue);
  RgbColor RGB = hsv_to_rgb(HSV);
  return string_format("#%X%X%X", RGB.r, RGB.g, RGB.b);
}

inline void VizPaint::polygon (ofstream& f, double x1, double y1, double x2,
                               double y2, double x3, double y3, double x4,
                               double y4, const string& colorf,
                               const string& colorb) const {
  f << "<polygon points=\""
    << setprecision(2) << x1 << "," << setprecision(2) << y1 << " "
    << setprecision(2) << x2 << "," << setprecision(2) << y2 << " "
    << setprecision(2) << x3 << "," << setprecision(2) << y3 << " "
    << setprecision(2) << x4 << "," << setprecision(2) << y4 << "\" "
      "style=\"fill:" << colorf << ";stroke:" << colorb << ";"
      "stroke-width:1;fill-opacity:0.7\" />";
}

inline void VizPaint::line (ofstream& f, double width, double x1, double y1,
                            double x2, double y2, const string& color) const {
  f << "<line x1=\"" << setprecision(2) << x1 << "\" "
       "y1=\"" << setprecision(2) << y1 << "\" "
       "x2=\"" << setprecision(2) << x2 << "\" "
       "y2=\"" << setprecision(2) << y2 << "\" "
       "style=\"stroke:" << color << ";"
       "stroke-width:" << setprecision(2) << width << "\" />";
}

inline void VizPaint::rect_oval (ofstream& f, double w, double h, double x,
                                 double y, const string& color) const {
  f << "<rect style=\"fill:" << color << ";fill-opacity:1;stroke-width:2;"
       "stroke-miterlimit:4;stroke-dasharray:none\" id=\"rectx\" "
       "width=\""  << setprecision(2) << w << "\" "
       "height=\"" << setprecision(2) << h << "\" "
       "x=\""      << setprecision(2) << x << "\" "
       "y=\""      << setprecision(2) << y << "\" ry=\"12.5\" />\n";
}

inline void VizPaint::rect
(ofstream& f, shared_ptr<Rectangle> rectangle, const string& color) const {
  f << "<rect style=\"fill:" << color << ";fill-opacity:1;stroke-width:2;"
       "stroke-miterlimit:4;stroke-dasharray:none\" id=\"rect3777\" "
       "width=\""  << setprecision(2) << rectangle->w << "\" "
       "height=\"" << setprecision(2) << rectangle->h << "\" "
       "x=\""      << setprecision(2) << rectangle->x << "\" "
       "y=\""      << setprecision(2) << rectangle->y << "\" ry=\"0\" />\n";
}

inline void VizPaint::rect_ir
(ofstream& f, const shared_ptr<Rectangle>& rectangle, const string& color)
const {
  rect(f, rectangle, color);
  f << "<rect style=\"fill-opacity:1;stroke-width:2;stroke-miterlimit:4;"
       "stroke-dasharray:none;fill:url(#Wavy);fill-rule:nonzero;opacity:1\" "
       "id=\"rect6217\" "
       "width=\""  << setprecision(2) << rectangle->w << "\" "
       "height=\"" << setprecision(2) << rectangle->h << "\" "
       "x=\""      << setprecision(2) << rectangle->x << "\" "
       "y=\""      << setprecision(2) << rectangle->y << "\" ry=\"0\" />\n";
}

inline void VizPaint::chromosome
(ofstream& f, double w, double h, double x, double y) const {
  static const string borderColor {"#000000"};
//  double  wk = w / 2 + 0.5;
/*
  fprintf(f, "<path "
         "d=\"m %.2lf,"
         "%.2lf 0,"
         "%.2lf c 0, -8.31 6.69, -%.2lf %.2lf, -%.2lf l -%.2lf,0 z m %.2lf,"
         "0 c 8.31,0 %.2lf,6.69 %.2lf,%.2lf l 0,-%.2lf -%.2lf,0 z\" "
         "id=\"rect3787\" style=\"fill:#fff;fill-opacity:1;fill-rule:"
         "nonzero;stroke:none\" />", x-0.5, y-0.5, 
         wk, wk, wk, wk, wk, wk, wk, wk, wk, wk, wk);

  fprintf(f, "<path "
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
       "width=\""  << setprecision(2) << w << "\" "
       "height=\"" << setprecision(2) << h << "\" "
       "x=\""      << setprecision(2) << x << "\" "
       "y=\""      << setprecision(2) << y << "\" ry=\"1\" />\n";
}

inline void VizPaint::text (ofstream& f, double x, double y, const string& name)
const {
  f << "<text xml:space=\"preserve\" style=\"font-size:40px;font-style:normal;"
       "font-weight:normal;line-height:125%%;letter-spacing:0px;"
       "word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;"
       "font-family:Sans\" "
       "x=\"" << setprecision(2) << x << "\" "
       "y=\"" << setprecision(2) << y << "\" "
       "id=\"corben\" sodipodi:linespacing=\"125%%\"><tspan sodipodi:"
       "role=\"line\" id=\"tspan3804\" "
       "x=\"" << setprecision(2) << x << "\" "
       "y=\"" << setprecision(2) << y << "\" "
       "style=\"font-size:18px;font-style:normal;font-variant:normal;"
       "font-weight:normal;font-stretch:normal;text-align:start;line-height:"
       "125%%;writing-mode:lr-tb;text-anchor:start;font-family:Arial;"
       "-inkscape-font-specification:Arial\">" << name << "</tspan>\n</text>\n";
}

template <typename Value>
double VizPaint::get_point (Value point) {
  return 5 * point/static_cast<double>(ratio);
}

inline void VizPaint::print_head (ofstream& f, double w, double u) const {
  f << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
       "<!-- IEETA 2018 using Inkscape -->\n""<svg\n"
       "xmlns:osb=\"http://www.openswatchbook.org/uri/2009/osb\"\n"
       "xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
       "xmlns:cc=\"http://creativecommons.org/ns#\"\n"
       "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
       "xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
       "xmlns=\"http://www.w3.org/2000/svg\"\n"
       "xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\"\n"
       "xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n"
       "width=\""  << setprecision(1) << w << "\"\n"
       "height=\"" << setprecision(1) << u << "\"\n"
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
       "<g inkscape:label=\"Camada 1\" inkscape:groupmode=\"layer\" "
       "id=\"layer1\" >\n";

  f << "<defs id=\"defs6211\"><pattern inkscape:stockid=\"Polka dots, large\" "
       "id=\"Polkadots-large\" patternTransform=\"translate(0,0)scale(10,10)\" "
       "height=\"10\" width=\"10\" patternUnits=\"userSpaceOnUse\" inkscape:"
       "collect=\"always\"> "
       "<circle id=\"circle4936\" r=\"0.45\" cy=\"0.810\" "
       "cx=\"2.567\" style=\"fill:black;stroke:none\" />"
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
       "style=\"fill:black;stroke:none\" /></pattern></defs>";

  f << "<defs id=\"ffff\"><pattern inkscape:stockid=\"Wavy\" id=\"Wavy\" "
       "height=\"5.1805778\" width=\"30.0\" patternUnits=\"userSpaceOnUse\" "
       "inkscape:collect=\"always\"><path id=\"path5114\" d=\"M 7.597,0.061 "
       "C 5.079,-0.187 2.656,0.302 -0.01,1.788 L -0.01,3.061 C 2.773,1.431 "
       "5.173,1.052 7.472,1.280 C 9.770,1.508 11.969,2.361 14.253,3.218 "
       "C 18.820,4.931 23.804,6.676 30.066,3.061 L 30.062,1.788 C 23.622,5.497 "
       "19.246,3.770 14.691,2.061 C 12.413,1.207 10.115,0.311 7.597,0.061 z \" "
       "style=\"fill:black;stroke:none;\" /></pattern></defs>";

  f << "<defs id=\"defs6219\"><pattern inkscape:stockid=\"xtrace\" id="
       "\"xtrace\" height=\"20.0\" width=\"20.0\" patternUnits="
       "\"userSpaceOnUse\" inkscape:collect=\"always\"><path style=\"fill:"
       "#000000;stroke:#000000;stroke-width:0.30;stroke-linecap:butt;"
       "stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:0;"
       "stroke-dasharray:none;fill-opacity:1\" d=\"m 0.0,10.0 25.00000,-25.0 "
       "0,5 -25.00000,25.00000 z\" id=\"path7213\" inkscape:"
       "connector-curvature=\"0\" /><path style=\"fill:#000000;fill-opacity:1;"
       "stroke:#000000;stroke-width:0.30;stroke-linecap:butt;stroke-linejoin:"
       "miter;stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" "
       "d=\"m 0.0,30.0 25.00000,-25.0 0,5 -25.00000,25.00000 z\" id="
       "\"path7213-8\" inkscape:connector-curvature=\"0\" /><path style="
       "\"fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0.30;"
       "stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:0;"
       "stroke-opacity:1;stroke-dasharray:none\" d=\"m 0.0,50.0 25.00000,-25.0 "
       "0,5 -25.00000,25.00000 z\" id=\"path7213-9\" inkscape:"
       "connector-curvature=\"0\" /><path style=\"fill:#000000;fill-opacity:1;"
       "stroke:#000000;stroke-width:0.30;stroke-linecap:butt;stroke-linejoin:"
       "miter;stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d="
       "\"m 0.0,70.0 25.00000,-25.0 0,5 -25.00000,25.00000 z\" id="
       "\"path7213-10\" inkscape:connector-curvature=\"0\" /><path style="
       "\"fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0.30;"
       "stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:0;"
       "stroke-opacity:1;stroke-dasharray:none\" d=\"m 0.0,90.0 25.00000,-25.0 "
       "0,5 -25.00000,25.00000 z\" id=\"path7213-11\" inkscape:"
       "connector-curvature=\"0\" /><path style=\"fill:#000000;fill-opacity:1;"
       "stroke:#000000;stroke-width:0.30;stroke-linecap:butt;stroke-linejoin:"
       "miter;stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d="
       "\"m m 0.0,110.0 25.00000,-25.0 0,05 -25.00000,25.00000 z\" id="
       "\"path7213-12\" inkscape:connector-curvature=\"0\" /></pattern></defs>";

  f << "<defs id=\"defs4\"><pattern id=\"dallas\" patternTransform="
       "\"translate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" "
       "patternUnits=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:"
       "#000000;stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;"
       "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d="
       "\"m -0.00788,2.37557 4.76251,0\" id=\"path2985\" /><path style="
       "\"fill:none;stroke:#000000;stroke-width:1;stroke-linecap:butt;"
       "stroke-linejoin:miter;stroke-miterlimit:0;stroke-opacity:1;"
       "stroke-dasharray:none\" d=\"m 2.37338,-0.00568 0,4.76251\" id="
       "\"path2985-1\" /></pattern></defs>";

  f << "<defs id=\"defs4\"><pattern id=\"lineX\" patternTransform="
       "\"translate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" "
       "patternUnits=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:"
       "#000000;stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;"
       "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d="
       "\"m -0.00788,2.37557 4.76251,0\" id=\"path2985\" /></pattern></defs>";

  f << "<defs id=\"defs4\"><pattern id=\"stripeX\" patternTransform="
       "\"translate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" "
       "patternUnits=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:"
       "#000000;stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;"
       "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d=\"m "
       "2.37338,-0.00568 0,4.76251\" id=\"path2985-1\" /></pattern></defs>";
}

inline void VizPaint::print_final (ofstream& f) const {
  f << "</g>\n</svg>";
  f.close();
}

inline void VizPaint::config
(double width_, double space_, u64 refSize_, u64 tarSize_) {
  width   = static_cast<double>(width_);
  space   = static_cast<double>(space_);
  refSize = get_point(refSize_);
  tarSize = get_point(tarSize_);
  maxSize = max(refSize, tarSize);
  ratio   = static_cast<u32>(maxSize / PAINT_SCALE);
}

void VizPaint::print_plot (VizParam& p) {
//  Painter* Paint;
  check_file(p.posFile);
  check_file(p.image);
  ifstream fPos(p.posFile);
  ofstream fPlot(p.image);
  string watermark;
  u64 tarNoBases=0, refNoBases=0;

  fPos >> watermark >> refNoBases >> tarNoBases;
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

  print_head(fPlot, 2*PAINT_CX + 2*(width+space) - space,
                    maxSize + PAINT_EXTRA);

  auto rectangle = make_shared<Rectangle>(
    2*PAINT_CX+2*(width+space)-space, maxSize+PAINT_EXTRA, 0, 0);

  rect(fPlot, rectangle, backColor);
  rect_oval(fPlot, width, refSize, cx, cy, backColor);
  rect_oval(fPlot, width, tarSize, cx, cy, backColor);
  text(fPlot, cx,             cy-15, PAINT_REF);
  text(fPlot, cx+width+space, cy-15, PAINT_TAR);

  // IF MINIMUM IS SET AS DEFAULT, RESET TO BASE MAX PROPORTION
  if (p.minimum == 0)
    p.minimum = static_cast<u32>(maxSize / 100);

  i64 begPosTar, endPosTar, begPosRef, endPosRef;
  string entTar, entRef;
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
          line(fPlot, 2,
            cx+width,       cy+get_point(begPosRef+(endPosRef-begPosRef)/2.0),
            cx+space+width, cy+get_point(begPosTar+(endPosTar-begPosTar)/2.0),
            "black");
          break;
        case 2:
          line(fPlot, 2,
            cx+width,       cy+get_point(begPosRef+(endPosRef-begPosRef)/2.0),
            cx+space+width, cy+get_point(begPosTar+(endPosTar-begPosTar)/2.0),
            rgb_color(static_cast<u8>(p.start*p.mult)));
          break;
        case 3:
          line(fPlot, 2,
            cx+width,       cy+get_point(begPosRef),
            cx+space+width, cy+get_point(begPosTar), "black");
          line(fPlot, 2,
            cx+width,       cy+get_point(endPosRef),
            cx+space+width, cy+get_point(endPosTar), "black");
          break;
        case 4:
          line(fPlot, 2,
            cx+width,       cy+get_point(begPosRef),
            cx+space+width, cy+get_point(begPosTar),
            rgb_color(static_cast<u8>(p.start*p.mult)));
          line(fPlot, 2,
            cx+width,       cy+get_point(endPosRef),
            cx+space+width, cy+get_point(endPosTar),
            rgb_color(static_cast<u8>(p.start*p.mult)));
          break;
        case 5:
          polygon(fPlot,
            cx+width,       cy+get_point(begPosRef),
            cx+width,       cy+get_point(endPosRef),
            cx+space+width, cy+get_point(endPosTar),
            cx+space+width, cy+get_point(begPosTar),
            rgb_color(static_cast<u8>(p.start*p.mult)), "grey");
          break;
        default:break;
        }

        rectangle->config(width, get_point(endPosRef-begPosRef),
                          cx, cy+get_point(begPosRef));
        rect(fPlot, rectangle, rgb_color(static_cast<u8>(p.start*p.mult)));

        rectangle->config(width, get_point(endPosTar-begPosTar),
                          cx+space+width, cy+get_point(begPosTar));
        rect(fPlot, rectangle, rgb_color(static_cast<u8>(p.start*p.mult)));

        ++no_regular;
      }
    }
    else {
      if (p.inversion) {
        switch (p.link) {
        case 1:
          line(fPlot, 2,
            cx+width,       cy+get_point(endPosRef+(begPosRef-endPosRef)/2.0),
            cx+space+width, cy+get_point(endPosTar+(begPosTar-endPosTar)/2.0),
            "green");
          break;
        case 2:
          line(fPlot, 2,
            cx+width,       cy+get_point(endPosRef+(begPosRef-endPosRef)/2.0),
            cx+space+width, cy+get_point(endPosTar+(begPosTar-endPosTar)/2.0),
            rgb_color(static_cast<u8>(p.start*p.mult)));
          break;
        case 3:
          line(fPlot, 2,
            cx+width,       cy+get_point(endPosRef),
            cx+space+width, cy+get_point(endPosTar), "green");
          line(fPlot, 2,
            cx+width,       cy+get_point(begPosRef),
            cx+space+width, cy+get_point(begPosTar), "green");
          break;
        case 4:
          line(fPlot, 2,
            cx+width,       cy+get_point(endPosRef),
            cx+space+width, cy+get_point(endPosTar),
            rgb_color(static_cast<u8>(p.start*p.mult)));
          line(fPlot, 2,
            cx+width,       cy+get_point(begPosRef),
            cx+space+width, cy+get_point(begPosTar),
            rgb_color(static_cast<u8>(p.start*p.mult)));
          break;
        case 5:
          polygon(fPlot,
            cx+width,       cy+get_point(endPosRef),
            cx+width,       cy+get_point(begPosRef),
            cx+space+width, cy+get_point(begPosTar),
            cx+space+width, cy+get_point(endPosTar),
            rgb_color(static_cast<u8>(p.start*p.mult)), "grey");
          break;
        default:break;
        }

        rectangle->config(width, get_point(begPosRef-endPosRef),
                          cx, cy+get_point(endPosRef));
        rect(fPlot, rectangle, rgb_color(static_cast<u8>(p.start*p.mult)));

        rectangle->config(width, get_point(endPosTar-begPosTar),
                          cx+space+width, cy+get_point(begPosTar));
        rect_ir(fPlot, rectangle, rgb_color(static_cast<u8>(p.start*p.mult)));

        ++no_inverse;
      }
    }

    ++p.start;
  }
  fPos.seekg(ios::beg);

  if (p.regular)    cerr << "Found "   << no_regular << " regular regions.\n";
  if (p.inversion)  cerr << "Found "   << no_inverse << " inverted regions.\n";
  if (p.verbose)    cerr << "Ignored " << no_ignored << " regions.\n";

  chromosome(fPlot, width, refSize, cx, cy);
  chromosome(fPlot, width, tarSize, cx+space+width, cy);
  print_final(fPlot);

  cerr << "Done!                       \n";
  fPos.close();
}