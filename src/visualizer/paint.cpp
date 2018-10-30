#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "paint.h"
#include "common.h"
#include "mem.h"
#include <fstream>
using namespace smashpp;
namespace smashpp { u32 ratio; }

RgbColor smashpp::HsvToRgb (HsvColor hsv) {
  RgbColor rgb;
  u8 region, remainder, p, q, t;

  if (hsv.s == 0) {
    rgb.r = hsv.v;
    rgb.g = hsv.v;
    rgb.b = hsv.v;
    return rgb;
  }

  region = hsv.h / 43;
  remainder = (hsv.h - (region * 43)) * 6;

  p = (hsv.v * (255 - hsv.s)) >> 8;
  q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
  t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

  switch (region) {
  case 0:   rgb.r=hsv.v;  rgb.g=t;      rgb.b=p;      break;
  case 1:   rgb.r=q;      rgb.g=hsv.v;  rgb.b=p;      break;
  case 2:   rgb.r=p;      rgb.g=hsv.v;  rgb.b=t;      break;
  case 3:   rgb.r=p;      rgb.g=q;      rgb.b=hsv.v;  break;
  case 4:   rgb.r=t;      rgb.g=p;      rgb.b=hsv.v;  break;
  default:  rgb.r=hsv.v;  rgb.g=p;      rgb.b=q;      break;
  }

  return rgb;
}

string smashpp::GetRgbColor (u8 hue) {
  RgbColor RGB;
  HsvColor HSV;
  char* color = (char*) Malloc(8 * sizeof(char));

  HSV.h = hue;
  HSV.s = LEVEL_SATURATION;
  HSV.v = LEVEL_VALUE;

  RGB = smashpp::HsvToRgb(HSV);

  sprintf(color, "#%X%X%X", RGB.r, RGB.g, RGB.b);

  return string(color);
}

Painter* smashpp::CreatePainter
(double refSize, double tarSize, double width, double space, string color) {
  Painter* P = (Painter*) Malloc(sizeof(Painter));
  P->backColor = color;
  P->refSize   = refSize;
  P->tarSize   = tarSize;
  P->cx        = DEFAULT_CX;
  P->cy        = DEFAULT_CY;
  P->tx        = DEFAULT_TX;
  P->ty        = DEFAULT_TY;
  P->width     = width;
  P->space     = space;
  P->maxSize   = refSize>tarSize ? refSize : tarSize;
  return P;
}

void smashpp::Polygon (ofstream& F, double x1, double y1, double x2, double y2,
                                    double x3, double y3, double x4, double y4,
                                    string colorf, string colorb) {
  F << "<polygon points=\""
    <<           setprecision(2) << x1 << "," << setprecision(2) << y1 << " "
    <<           setprecision(2) << x2 << "," << setprecision(2) << y2 << " "
    <<           setprecision(2) << x3 << "," << setprecision(2) << y3 << " "
    <<           setprecision(2) << x4 << "," << setprecision(2) << y4 << "\" "
                "style=\"fill:" << colorf << ";"
                "stroke:" << colorb << ";stroke-width:1;fill-opacity:0.7\" />";
}

void smashpp::Line (ofstream& F, double width, double x1, double y1, double x2,
                                 double y2, string color) {
  F << "<line x1=\"" << setprecision(2) << x1 << "\" "
             "y1=\"" << setprecision(2) << y1 << "\" "
             "x2=\"" << setprecision(2) << x2 << "\" "
             "y2=\"" << setprecision(2) << y2 << "\" "
             "style=\"stroke:" << color << ";"
             "stroke-width:" << setprecision(2) << width << "\" />";
}

void smashpp::RectOval
(ofstream& F, double w, double h, double x, double y, string color) {
  F << "<rect style=\"fill:" << color << ";fill-opacity:1;stroke-width:2;"
       "stroke-miterlimit:4;stroke-dasharray:none\" id=\"rectx\" "
       "width=\""  << setprecision(2) << w << "\" "
       "height=\"" << setprecision(2) << h << "\" "
       "x=\""      << setprecision(2) << x << "\" "
       "y=\""      << setprecision(2) << y << "\" ry=\"12.5\" />\n";
}

void smashpp::Rect
(ofstream& F, double w, double h, double x, double y, string color) {
  F << "<rect style=\"fill:" << color << ";fill-opacity:1;stroke-width:2;"
       "stroke-miterlimit:4;stroke-dasharray:none\" id=\"rect3777\" "
       "width=\""  << setprecision(2) << w << "\" "
       "height=\"" << setprecision(2) << h << "\" "
       "x=\""      << setprecision(2) << x << "\" "
       "y=\""      << setprecision(2) << y << "\" ry=\"0\" />\n";
}

void smashpp::RectIR
(ofstream& F, double w, double h, double x, double y, string color) {
  smashpp::Rect(F, w, h, x, y, color);
  F << "<rect style=\"fill-opacity:1;stroke-width:2;stroke-miterlimit:4;"
       "stroke-dasharray:none;fill:url(#Wavy);fill-rule:nonzero;opacity:1\" "
       "id=\"rect6217\" "
       "width=\""  << setprecision(2) << w << "\" "
       "height=\"" << setprecision(2) << h << "\" "
       "x=\""      << setprecision(2) << x << "\" "
       "y=\""      << setprecision(2) << y << "\" ry=\"0\" />\n";
}

void smashpp::Chromosome (ofstream& F, double w, double h, double x, double y) {
  char borderColor[] = "#000000";
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

  F << "<rect style=\"fill:none;stroke:" << borderColor << ";stroke-width:2;"
       "stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;"
       "stroke-opacity:1;stroke-dasharray:none\" id=\"rect2985\" "
       "width=\""  << setprecision(2) << w << "\" "
       "height=\"" << setprecision(2) << h << "\" "
       "x=\""      << setprecision(2) << x << "\" "
       "y=\""      << setprecision(2) << y << "\" ry=\"1\" />\n";
}

void smashpp::Text (ofstream& F, double x, double y, string name) {
  F << "<text xml:space=\"preserve\" style=\"font-size:40px;font-style:normal;"
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

void smashpp::SetRatio (u32 r) {
  smashpp::ratio = r;
}

double smashpp::GetPoint (u64 p) {
  return p / (double) smashpp::ratio * 5;
}

void smashpp::PrintHead (ofstream& F, double w, double u) {
  F << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
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

  F << "<defs id=\"defs6211\"><pattern inkscape:stockid=\"Polka dots, large\" "
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

  F << "<defs id=\"ffff\"><pattern inkscape:stockid=\"Wavy\" id=\"Wavy\" "
       "height=\"5.1805778\" width=\"30.0\" patternUnits=\"userSpaceOnUse\" "
       "inkscape:collect=\"always\"><path id=\"path5114\" d=\"M 7.597,0.061 "
       "C 5.079,-0.187 2.656,0.302 -0.01,1.788 L -0.01,3.061 C 2.773,1.431 "
       "5.173,1.052 7.472,1.280 C 9.770,1.508 11.969,2.361 14.253,3.218 "
       "C 18.820,4.931 23.804,6.676 30.066,3.061 L 30.062,1.788 C 23.622,5.497 "
       "19.246,3.770 14.691,2.061 C 12.413,1.207 10.115,0.311 7.597,0.061 z \" "
       "style=\"fill:black;stroke:none;\" /></pattern></defs>";

  F << "<defs id=\"defs6219\"><pattern inkscape:stockid=\"xtrace\" id="
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

  F << "<defs id=\"defs4\"><pattern id=\"dallas\" patternTransform="
       "\"translate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" "
       "patternUnits=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:"
       "#000000;stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;"
       "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d="
       "\"m -0.00788,2.37557 4.76251,0\" id=\"path2985\" /><path style="
       "\"fill:none;stroke:#000000;stroke-width:1;stroke-linecap:butt;"
       "stroke-linejoin:miter;stroke-miterlimit:0;stroke-opacity:1;"
       "stroke-dasharray:none\" d=\"m 2.37338,-0.00568 0,4.76251\" id="
       "\"path2985-1\" /></pattern></defs>";

  F << "<defs id=\"defs4\"><pattern id=\"lineX\" patternTransform="
       "\"translate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" "
       "patternUnits=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:"
       "#000000;stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;"
       "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d="
       "\"m -0.00788,2.37557 4.76251,0\" id=\"path2985\" /></pattern></defs>";

  F << "<defs id=\"defs4\"><pattern id=\"stripeX\" patternTransform="
       "\"translate(106.59375,206.90625)\" height=\"4.75\" width=\"4.75\" "
       "patternUnits=\"userSpaceOnUse\"> <path style=\"fill:none;stroke:"
       "#000000;stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;"
       "stroke-miterlimit:0;stroke-opacity:1;stroke-dasharray:none\" d=\"m "
       "2.37338,-0.00568 0,4.76251\" id=\"path2985-1\" /></pattern></defs>";
}

void smashpp::PrintFinal (ofstream& F) {
  F << "</g>\n</svg>";
  F.close();
}