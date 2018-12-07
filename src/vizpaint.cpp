#include "vizpaint.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "number.hpp"
using namespace smashpp;

void VizPaint::print_plot (VizParam& p) {
  check_file(p.posFile);
  ifstream fPos (p.posFile);
  ofstream fPlot(p.image);

  u64 n_refBases=0, n_tarBases=0;
  string watermark, ref, tar;
  fPos >> watermark >> ref >> n_refBases >> tar >> n_tarBases;
  if (watermark!=POS_HDR)  error("unknown file format for positions.");
  if (p.verbose)           show_info(p, ref, tar, n_refBases, n_tarBases);

  cerr << "Plotting ...\r";
  config(p.width, p.space, n_refBases, n_tarBases);

  auto Paint_Extra = PAINT_EXTRA;
  if (p.showAnnot)  Paint_Extra+=30;

  print_head(fPlot, PAINT_CX+width+space+width+PAINT_CX, maxSize+Paint_Extra);

  auto line   = make_unique<Line>();
  line->width = 2.0;
  auto rect   = make_unique<Rectangle>();
  rect->opacity = p.opacity;
  auto poly   = make_unique<Polygon>();
  auto text   = make_unique<Text>();

  rect->color  = backColor;
  rect->origin = Point(0, 0);
  rect->width  = PAINT_CX + width + space + width + PAINT_CX;
  rect->height = maxSize + Paint_Extra;
  rect->plot(fPlot);

  text->origin = Point(cx + width/2, cy - 15);
  text->label  = ref;
  text->plot_title(fPlot);

  text->origin = Point(cx + width + space + width/2, cy - 15);
  text->label  = tar;
  text->plot_title(fPlot);

  // If min is set to default, reset to base max proportion
  if (p.min==0)    p.min=static_cast<u32>(maxSize / 100);
  // if (!p.manMult)  p.mult = 256 / file_lines(p.posFile);
  mult = p.mult;

  vector<Pos> pos;
  u64 start {p.start};
  i64 br, er, bt, et;
  for (double nr,nt,sr,st; fPos >> br>>er>>nr>>sr >> bt>>et>>nt>>st; ++start)
    pos.emplace_back(Pos(br, er, nr, sr, bt, et, nt, st, start));

  // std::sort(pos.begin(), pos.end(),
  //   [](const Pos &l, const Pos &r) { return l.begRef < r.begRef; });
  // const auto last = unique(pos.begin(), pos.end(),
  //   [](const Pos &l, const Pos &r) { 
  //     return l.begRef==r.begRef && l.endRef==r.endRef; 
  //   });
  // pos.erase(last, pos.end());

  if (p.showPos) {
    double X = 0;
    if (p.showNRC && p.showRedun) 
      X = 2 * (HORIZ_TUNE + width/HORIZ_RATIO);
    else if (p.showNRC ^ p.showRedun) 
      X = HORIZ_TUNE + width/HORIZ_RATIO;

    print_pos(fPlot, pos, X, max(n_refBases,n_tarBases), p.min, "ref");
    // print_pos(fPlot, pos, X, max(n_refBases,n_tarBases), p.min, "tar");//todo
  }

  u64 n_regular=0, n_inverse=0, n_ignore=0;
  for (auto e : pos) {
    const auto plot_main_ref = [&]() {
      rect->width  = width;
      rect->color  = customColor(p.start);
      rect->origin = Point(cx, cy + get_point(e.begRef));
      rect->height = get_point(e.endRef-e.begRef);
      rect->plot(fPlot);
      if (p.showNRC) {
        rect->color = nrc_color(e.entRef, p.color);
        rect->plot_nrc_ref(fPlot);
      }
      if (p.showRedun) {
        rect->color = redun_color(e.selfRef, p.color);
        rect->plot_redun_ref(fPlot, p.showNRC);
      }
    };

    const auto plot_main_tar = [&](bool inverted) {
      rect->color  = customColor(p.start);
      rect->height = get_point(abs(e.begTar-e.endTar));
      if (!inverted) {
        rect->origin = Point(cx+width+space, cy+get_point(e.begTar));
        rect->plot(fPlot);
      }
      else {
        rect->origin = Point(cx+width+space, cy+get_point(e.endTar));
        rect->plot_ir(fPlot);
      }

      if (p.showNRC) {
        rect->color = nrc_color(e.entTar, p.color);
        rect->plot_nrc_tar(fPlot);
      }
      if (p.showRedun) {
        rect->color = redun_color(e.selfTar, p.color);
        rect->plot_redun_tar(fPlot, p.showNRC);
      }
    };
    
    if (abs(e.endRef-e.begRef)<p.min || abs(e.endTar-e.begTar)<p.min) {
      ++n_ignore;
      continue;
    }

    if (e.endTar > e.begTar) {
      if (p.regular) {
        plot_main_ref();
        plot_main_tar(false);

        switch (p.link) {
        case 5:
          poly->lineColor = "grey";
          poly->fillColor = customColor(p.start);
          poly->one   = Point(cx+width,       cy+get_point(e.begRef));
          poly->two   = Point(cx+width,       cy+get_point(e.endRef));
          poly->three = Point(cx+width+space, cy+get_point(e.endTar));
          poly->four  = Point(cx+width+space, cy+get_point(e.begTar));
          poly->plot(fPlot);
          break;
        case 2:
          line->color = customColor(p.start);
          line->beg = Point(
            cx+width, cy+get_point(e.begRef+(e.endRef-e.begRef)/2.0));
          line->end = Point(
            cx+width+space, cy+get_point(e.begTar+(e.endTar-e.begTar)/2.0));
          line->plot(fPlot);
          break;
        case 1:
          line->color = "black";
          line->beg = Point(
            cx+width, cy+get_point(e.begRef+(e.endRef-e.begRef)/2.0));
          line->end = Point(
            cx+width+space, cy+get_point(e.begTar+(e.endTar-e.begTar)/2.0));
          line->plot(fPlot);
          break;
        case 3:
          line->color = "black";
          line->beg = Point(cx+width,       cy+get_point(e.begRef));
          line->end = Point(cx+width+space, cy+get_point(e.begTar));
          line->plot(fPlot);

          line->beg = Point(cx+width,       cy+get_point(e.endRef));
          line->end = Point(cx+width+space, cy+get_point(e.endTar));
          line->plot(fPlot);
          break;
        case 4:
          line->color = customColor(p.start);
          line->beg = Point(cx+width,       cy+get_point(e.begRef));
          line->end = Point(cx+width+space, cy+get_point(e.begTar));
          line->plot(fPlot);

          line->beg = Point(cx+width,       cy+get_point(e.endRef));
          line->end = Point(cx+width+space, cy+get_point(e.endTar));
          line->plot(fPlot);
          break;
        default:break;
        }
        ++n_regular;
      }
    }
    else {
      if (p.inverse) {
        plot_main_ref();
        plot_main_tar(true);

        switch (p.link) {
        case 5:
          poly->lineColor = "grey";
          poly->fillColor = customColor(p.start);
          poly->one   = Point(cx+width,       cy+get_point(e.begRef));
          poly->two   = Point(cx+width,       cy+get_point(e.endRef));
          poly->three = Point(cx+width+space, cy+get_point(e.endTar));
          poly->four  = Point(cx+width+space, cy+get_point(e.begTar));
          poly->plot(fPlot);
          break;
        case 2:
          line->color = customColor(p.start);
          line->beg = Point(
            cx+width, cy+get_point(e.begRef+(e.endRef-e.begRef)/2.0));
          line->end = Point(
            cx+width+space, cy+get_point(e.endTar+(e.begTar-e.endTar)/2.0));
          line->plot(fPlot);
          break;
        case 1:
          line->color = "green";
          line->beg = Point(
            cx+width, cy+get_point(e.begRef+(e.endRef-e.begRef)/2.0));
          line->end = Point(
            cx+width+space, cy+get_point(e.endTar+(e.begTar-e.endTar)/2.0));
          line->plot(fPlot);
          break;
        case 3:
          line->color = "green";
          line->beg = Point(cx+width,       cy+get_point(e.begRef));
          line->end = Point(cx+width+space, cy+get_point(e.begTar));
          line->plot(fPlot);

          line->beg = Point(cx+width,       cy+get_point(e.endRef));
          line->end = Point(cx+width+space, cy+get_point(e.endTar));
          line->plot(fPlot);
          break;
        case 4:
          line->color = customColor(p.start);
          line->beg = Point(cx+width,       cy+get_point(e.begRef));
          line->end = Point(cx+width+space, cy+get_point(e.begTar));
          line->plot(fPlot);

          line->beg = Point(cx+width,       cy+get_point(e.endRef));
          line->end = Point(cx+width+space, cy+get_point(e.endTar));
          line->plot(fPlot);
          break;
        default:  break;
        }
        ++n_inverse;
      }
    }
    ++p.start;
  }
  fPos.seekg(ios::beg);

  // Plot Ns
  save_n_pos(ref);
  ifstream refFile(file_name(ref)+"."+FMT_N);
  for (i64 beg,end; refFile>>beg>>end;) {
    rect->color  = "grey";
    rect->origin = Point(cx, cy+get_point(beg));
    rect->height = get_point(end-beg+1);
    rect->plot(fPlot);
  }
  refFile.close();
  remove((file_name(ref)+"."+FMT_N).c_str());

  save_n_pos(tar);
  ifstream tarFile(file_name(tar)+"."+FMT_N);
  for (i64 beg,end; tarFile>>beg>>end;) {
    rect->color  = "grey";
    rect->origin = Point(cx+width+space, cy+get_point(beg));
    rect->height = get_point(end-beg+1);
    rect->plot(fPlot);
  }
  tarFile.close();
  remove((file_name(tar)+"."+FMT_N).c_str());
  
  rect->width  = width;
  rect->origin = Point(cx, cy);
  rect->height = refSize;
  rect->plot_chromosome(fPlot);

  rect->origin = Point(cx + width + space, cy);
  rect->height = tarSize;
  rect->plot_chromosome(fPlot);

  plot_legend(fPlot, p);

  if (p.showAnnot)  plot_annot(fPlot, max(n_refBases,n_tarBases));

  print_tail(fPlot);

  cerr << "Plotting finished.\n";
  if (p.regular)    cerr << "Found "   << n_regular << " regular regions.\n";
  if (p.inverse)    cerr << "Found "   << n_inverse << " inverted regions.\n";
  if (n_ignore!=0)  cerr << "Ignored " << n_ignore  << " regions.\n";
  cerr << '\n';

  fPos.close();  fPlot.close();
}

inline void VizPaint::show_info (VizParam& p, const string& ref, 
const string& tar, u64 n_refBases, u64 n_tarBases) const {
  const u8 lblWidth=18, colWidth=8;
  u8 tblWidth=58;
  if (max(n_refBases,n_tarBases) > 999999)
    tblWidth = static_cast<u8>(lblWidth+4*colWidth);
  else
    tblWidth = static_cast<u8>(lblWidth+3*colWidth);

  const auto rule = [](u8 n, const string& s) {
    for (auto i=n/s.size(); i--;) { cerr<<s; }    cerr<<'\n';
  };
  const auto toprule = [&]() { rule(tblWidth, "~"); };
  const auto midrule = [&]() { rule(tblWidth, "~"); };
  const auto botrule = [&]() { rule(tblWidth, " "); };
  const auto label   = [&](const string& s){ cerr<<setw(lblWidth)<<left<<s;   };
  const auto header  = [&](const string& s){ cerr<<setw(2*colWidth)<<left<<s; };
  const auto design_vals = [&](char c) {
    cerr << setw(colWidth) << left;
    switch (c) {
    case 'w':  cerr<<p.width;                     break;
    case 's':  cerr<<p.space;                     break;
    case 'f':  cerr<<p.mult;                      break;
    case 'b':  cerr<<p.start;                     break;
    case 'm':  cerr<<p.min;                       break;
    case 'r':  cerr<<(p.regular ? "yes" : "no");  break;
    case 'i':  cerr<<(p.inverse ? "yes" : "no");  break;
    case 'l':  cerr<<p.link;                      break;
    default:                                      break;
    }
    cerr << '\n';
  };
  const auto file_vals = [&](char c) {
    cerr << setw(2*colWidth) << left;
    switch (c) {
    case '1':  cerr.imbue(locale("en_US.UTF8"));  cerr<<n_refBases;  break;
    // case 'r':  cerr<<ref;                                            break;
    case 'r':  cerr<<file_name(ref);                                 break;
    case '2':  cerr.imbue(locale("en_US.UTF8"));  cerr<<n_tarBases;  break;
    // case 't':  cerr<<tar;                                            break;
    case 't':  cerr<<file_name(tar);                                 break;
    case 'i':  cerr<<p.image;                                        break;
    default:   cerr<<'-';                                            break;
    }
  };

  toprule();
  label("Chromosomes design");              cerr<<'\n';
  midrule();
  label("Width");                           design_vals('w');
  label("Space");                           design_vals('s');
  label("Multiplication");                  design_vals('f');
  label("Begin");                           design_vals('b');
  label("Minimum");                         design_vals('m');
  label("Show regular");                    design_vals('r');
  label("Show inversions");                 design_vals('i');
  label("Link type");                       design_vals('l');
  botrule();  //cerr << '\n';

  toprule();
  label("Files");        header("Name");    header("Size (B)");      cerr<<'\n';
  midrule();                                
  label("Reference");    file_vals('r');    file_vals('1');          cerr<<'\n';
  label("Target");       file_vals('t');    file_vals('2');          cerr<<'\n';
  label("Image");        file_vals('i');    file_vals('-');          cerr<<'\n';
  botrule();
}

inline void VizPaint::config (double width_, double space_, u64 refSize_, 
u64 tarSize_) {
  ratio   = static_cast<u32>(max(refSize_,tarSize_) / PAINT_SCALE);
  width   = width_;
  space   = space_;
  refSize = get_point(refSize_);
  tarSize = get_point(tarSize_);
  maxSize = max(refSize, tarSize);
}

inline RgbColor VizPaint::hsv_to_rgb (const HsvColor& HSV) const {
  RgbColor RGB {};
  if (HSV.s==0) { RGB.r = RGB.g = RGB.b = HSV.v;    return RGB; }

  const auto region    = u8(HSV.h / 43),
             remainder = u8((HSV.h - region*43) * 6),
             p = u8((HSV.v * (255 - HSV.s)) >> 8),
             q = u8((HSV.v * (255 - ((HSV.s*remainder)>>8))) >> 8),
             t = u8((HSV.v * (255 - ((HSV.s*(255-remainder))>>8))) >> 8);

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

  HSV.s = u8(255 * u16((rgbMax-rgbMin)/HSV.v));
  if (HSV.s==0) { HSV.h = 0;            return HSV; }

  if      (rgbMax==RGB.r)    HSV.h = u8(43*(RGB.g-RGB.b)/(rgbMax-rgbMin));
  else if (rgbMax==RGB.g)    HSV.h = u8(85 + 43*(RGB.b-RGB.r)/(rgbMax-rgbMin));
  else                       HSV.h = u8(171 + 43*(RGB.r-RGB.g)/(rgbMax-rgbMin));
  
  return HSV;
}

inline string VizPaint::rgb_color (u8 hue) const {
  HsvColor HSV (hue);
  RgbColor RGB {hsv_to_rgb(HSV)};
  return string_format("#%X%X%X", RGB.r, RGB.g, RGB.b);
}

inline string VizPaint::heatmap_color (double lambda, 
const HeatmapColor& heatmap) const {
  // Change behaviour [sensitivity: near low similarity]
  lambda = (1 + pow(lambda, 3) + tanh(8*(lambda-1))) / 2; 

  double phi = 2 * PI * (heatmap.start/3 + heatmap.rotations*lambda),
         lambdaGamma = pow(lambda, heatmap.gamma),
         a = heatmap.hue * lambdaGamma * (1-lambdaGamma)/2,
         R = lambdaGamma - a*0.14861*cos(phi) + a*1.78277*sin(phi),
         G = lambdaGamma - a*0.29227*cos(phi) - a*0.90649*sin(phi),
         B = lambdaGamma + a*1.97294*cos(phi);

  keep_in_range(0.0,R,1.0); keep_in_range(0.0,G,1.0); keep_in_range(0.0,B,1.0);

  return string_format("#%02X%02X%02X", int(R*255), int(G*255), int(B*255));
}

template <typename ValueR, typename ValueG, typename ValueB>
inline string VizPaint::shade_color (ValueR r, ValueG g, ValueB b) const {
  return "rgb("+to_string(static_cast<u8>(r))+","
               +to_string(static_cast<u8>(g))+","
               +to_string(static_cast<u8>(b))+")";
}

inline string VizPaint::customColor (u32 start) const {
  return rgb_color(static_cast<u8>(start * mult));
}

inline string VizPaint::nrc_color (double entropy, u32 colorMode) const {
  keep_in_range(0.0, entropy, 2.0);
  // return heatmap_color(entropy/2 * (width+space+width));
  vector<string> colorSet {};
  switch (colorMode) {
  case 0:
    colorSet = {"#2c7bb6", "#00a6ca", "#00ccbc", "#90eb9d", "#ffff8c",
      "#f9d057", "#f29e2e", "#e76818", "#d7191c"};                        break;
  case 1:
    colorSet = {"#FFFFDD", "#AAF191", "#80D385", "#61B385", "#3E9583",
      "#217681", "#285285", "#1F2D86", "#000086"};                        break;
  case 2:
    colorSet = {"#5E4FA2", "#41799C", "#62A08D", "#9CB598", "#C8CEAD",
      "#E6E6BA", "#E8D499", "#E2B07F", "#E67F5F", "#C55562", "#A53A66"};  break;
  default:
    error("undefined color mode.");
  }

  return colorSet[entropy/2 * (colorSet.size()-1)];
}

inline string VizPaint::redun_color (double entropy, u32 colorMode) const {
  keep_in_range(0.0, entropy, 2.0);
  // return heatmap_color(entropy/2 * (width+space+width));
  vector<string> colorSet {};
  switch (colorMode) {
  case 0:
    colorSet = {"#2c7bb6", "#00a6ca", "#00ccbc", "#90eb9d", "#ffff8c",
      "#f9d057", "#f29e2e", "#e76818", "#d7191c"};                        break;
  case 1:
    colorSet = {"#FFFFDD", "#AAF191", "#80D385", "#61B385", "#3E9583",
      "#217681", "#285285", "#1F2D86", "#000086"};                        break;
  case 2:
    colorSet = {"#5E4FA2", "#41799C", "#62A08D", "#9CB598", "#C8CEAD",
      "#E6E6BA", "#E8D499", "#E2B07F", "#E67F5F", "#C55562", "#A53A66"};  break;
  default:
    error("undefined color mode.");
  }

  return colorSet[entropy/2 * (colorSet.size()-1)];
}

inline void VizPaint::print_head (ofstream& f, double w, double h) const {
  f << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    "<!-- IEETA " << DEV_YEARS << " using Inkscape -->\n""<svg\n"
    "xmlns:osb=\"http://www.openswatchbook.org/uri/2009/osb\"\n"
    "xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
    "xmlns:cc=\"http://creativecommons.org/ns#\"\n"
    "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
    "xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
    "xmlns=\"http://www.w3.org/2000/svg\"\n"
    "xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\"\n"
    "xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n"
    "width=\"" << w << "\"\n"
    "height=\"" << h << "\"\n"
    "id=\"svg2\"\n"
    "version=\"1.1\"\n"
    "inkscape:version=\"0.48.3.1 r9886\"\n"
    "sodipodi:docname=\"chromosomes\">\n"
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

template <typename Value>
inline double VizPaint::get_point (Value p) const {
  return 5 * p / static_cast<double>(ratio);
}

inline void VizPaint::plot_legend (ofstream& f, const VizParam& p) const {
  const auto vert = 24;
  // Relative redundancy
  auto rect = make_unique<Rectangle>();
  if (!p.showNRC && !p.showRedun) {
    rect->origin = Point(cx, vert);
    rect->width = width+space+width;
  }
  else if (p.showNRC ^ p.showRedun) {
    rect->origin = Point(cx-(HORIZ_TUNE+width/HORIZ_RATIO), vert);
    rect->width  = width/HORIZ_RATIO+HORIZ_TUNE+width+space+width+
                   width/HORIZ_RATIO+HORIZ_TUNE;
  }
  else if (p.showNRC && p.showRedun) {
    rect->origin = Point(cx-2*(HORIZ_TUNE+width/HORIZ_RATIO), vert);
    rect->width  = 2*(width/HORIZ_RATIO+HORIZ_TUNE)+width+space+width+
                   2*(width/HORIZ_RATIO+HORIZ_TUNE);
  }
  rect->height = 12;

  auto text = make_unique<Text>();
  text->origin = Point(rect->origin.x+rect->width/2, rect->origin.y);
  text->textAnchor = "middle";
  text->fontWeight = "bold";
  text->dominantBaseline = "text-after-edge";
  text->label = "RELATIVE REDUNDANCY";
  text->fontSize = 9;
  text->plot(f);

  auto grad = make_unique<Gradient>();
  switch (p.color) {
  case 0:
    grad->offsetColor = {"#2c7bb6", "#00a6ca", "#00ccbc", "#90eb9d", "#ffff8c",
      "#f9d057", "#f29e2e", "#e76818", "#d7191c"};                        break;
  case 1:
    grad->offsetColor = {"#FFFFDD", "#AAF191", "#80D385", "#61B385", "#3E9583",
      "#217681", "#285285", "#1F2D86", "#000086"};                        break;
  case 2:
    grad->offsetColor = {"#5E4FA2", "#41799C", "#62A08D", "#9CB598", "#C8CEAD",
      "#E6E6BA", "#E8D499", "#E2B07F", "#E67F5F", "#C55562", "#A53A66"};  break;
  default:
    error("undefined color mode.");
  }

  const auto offset = [&](u8 i) { 
    return to_string(i * 100 / (grad->offsetColor.size() - 1)) + "%";
  };
  auto id = to_string(rect->origin.x) + to_string(rect->origin.y);
  f << "<defs><linearGradient id=\"grad"+id+"\"  x1=\"0%\" y1=\"0%\" "
    "x2=\"100%\" y2=\"0%\"> ";
  for (u8 i=0; i!=grad->offsetColor.size(); ++i) {
    f << "<stop offset=\"" << offset(i) << "\" style=\"stop-color:" 
      << grad->offsetColor[i] << ";stop-opacity:1\"/>";
  }
  f << "</linearGradient> </defs>"
    "<rect fill=\"url(#grad"+id+")\" "
    "width=\""  << PREC << rect->width  << "\" "
    "height=\"" << PREC << rect->height << "\" "
    "x=\"" << PREC << rect->origin.x << "\" "
    "y=\"" << PREC << rect->origin.y << "\" ry=\"3\" />\n";

  // text->dominantBaseline = "text-before-edge";
  text->dominantBaseline = "middle";
  text->fontWeight = "normal";
  text->fontSize   = 9;
  text->textAnchor = "end";
  text->origin = Point(rect->origin.x-2, rect->origin.y+rect->height/2);
  text->label  = "0.0";
  text->fontWeight = "bold";
  text->plot(f);
  text->textAnchor = "middle";
  for (u8 i=1; i!=4; ++i) {
    text->origin = 
      Point(rect->origin.x+(rect->width*i)/4, rect->origin.y+rect->height/2);
    if (p.color==1)  text->color="white";
    text->label = string_format("%.1f", i*0.5);
    text->plot(f);
  }
  text->textAnchor = "start";
  text->origin = Point(rect->origin.x+rect->width+2, 
                       rect->origin.y+rect->height/2);
  text->color = "black";
  text->label = "2.0";
  text->plot(f);
  text->textAnchor = "middle";
  text->fontWeight = "normal";

  // Redundancy
  text->origin = Point(rect->origin.x+rect->width/2, 
                       rect->origin.y+rect->height);
  text->dominantBaseline = "text-before-edge";
  text->label = "REDUNDANCY";
  text->fontSize = 9;
  text->fontWeight = "bold";
  text->plot(f);
}

inline void VizPaint::plot_annot (ofstream& f, i64 maxHeight) const {
  const auto relRedunX1 = cx - HORIZ_TUNE - 0.5*width/HORIZ_RATIO;
  const auto relRedunX2 = 
    cx + 2*width + space + HORIZ_TUNE + 0.5*width/HORIZ_RATIO;
  const auto relRedunY  = cy + get_point(maxHeight) + 20;

  auto horizSize = space/5.0;
  auto vertSize  = relRedunY - cy - get_point(lastPos[0]);
  auto path = make_unique<Path>();
  path->color="black";
  path->origin = Point(relRedunX1, cy+get_point(lastPos[0]));
  path->trace = "v "+to_string(vertSize)+" h "+to_string(horizSize);
  path->plot(f);

  vertSize  = relRedunY - cy - get_point(lastPos[1]);
  path->origin = Point(relRedunX2, cy+get_point(lastPos[1]));
  path->trace = "v "+to_string(vertSize)+" h "+to_string(-horizSize);
  path->plot(f);

  auto text = make_unique<Text>();
  text->fontSize=9;
  text->color="black";
  text->origin=Point((relRedunX1+relRedunX2)/2, relRedunY);
  text->label="Relative Redundancy";
  text->plot(f);

  const auto redunX1 = cx - 2*HORIZ_TUNE - 1.5*width/HORIZ_RATIO;
  const auto redunX2 = 
    cx + 2*width + space + 2*HORIZ_TUNE + 1.5*width/HORIZ_RATIO;
  const auto redunY = relRedunY + 15;

  vertSize  = redunY - cy - get_point(lastPos[0]);
  horizSize += HORIZ_TUNE + width/HORIZ_RATIO + 15;
  path->origin = Point(redunX1, cy+get_point(lastPos[0]));
  path->trace = "v "+to_string(vertSize)+" h "+to_string(horizSize);
  path->plot(f);

  vertSize  = redunY - cy - get_point(lastPos[1]);
  path->origin = Point(redunX2, cy+get_point(lastPos[1]));
  path->trace = "v "+to_string(vertSize)+" h "+to_string(-horizSize);
  path->plot(f);

  text->fontSize=9;
  text->origin=Point((redunX1+redunX2)/2, redunY);
  text->label="Redundancy";
  text->plot(f);
}

inline string VizPaint::tspan (u32 start, i64 pos) const {
  return "<tspan id=\"" + to_string(start) + "\" style=\"fill:" + 
    customColor(start) + "\">" + to_string(pos) + ", </tspan>\n";
}
inline string VizPaint::tspan (u32 start, const string& pos) const {
  return "<tspan id=\"" + to_string(start) + "\" style=\"fill:" + 
    customColor(start) + "\">" + pos + ", </tspan>\n";
}
// inline string VizPaint::tspan (const string& color, i64 pos) const {
//   return "<tspan id=\"" + color.substr(1) + "\" style=\"fill:" + color + 
//     "\">" + to_string(pos) + ", </tspan>\n";
// }
// inline string VizPaint::tspan (const string& color, const string& pos) const {
//   return "<tspan id=\"" + color.substr(1) + "\" style=\"fill:" + color + 
//     "\">" + pos + ", </tspan>\n";
// }

inline void VizPaint::sort_merge (string& s) const {
  istringstream stream(s);

  vector<string> vLine;
  for (string gl; getline(stream, gl);)
    vLine.emplace_back(gl);

  if (vLine.size() == 1) {
    s.erase(s.find(", "), 2);
    return;
  }

  vector<u64> vID;
  for (const auto& l : vLine) {
    auto foundBeg = l.find("id=", 0) + 4;
    auto foundEnd = l.find("\"", foundBeg+1);
    vID.emplace_back(stoull(l.substr(foundBeg, foundEnd)));
  }

  vector<i64> vPos;
  for (const auto& l : vLine) {
    auto foundBeg = l.find(">", 0) + 1;
    auto foundEnd = l.find("<", foundBeg+1);
    auto strPos = l.substr(foundBeg, foundEnd-foundBeg);
    strPos.erase(strPos.find_last_of(", ")-1, 2);
    vPos.emplace_back(stoull(strPos));
  }

  struct Env {
    u64    id;
    string line;
    i64    pos;
  };
  
  vector<Env> vEnv;    vEnv.resize(vLine.size());
  for (size_t i=0; i!=vLine.size(); ++i) {
    vEnv[i].id   = vID[i];
    vEnv[i].line = vLine[i];
    vEnv[i].pos  = vPos[i];
  }
  std::sort(vEnv.begin(), vEnv.end(), 
    [](const Env& l, const Env& r) { return l.id < r.id; });

  s.clear();
  u64 leftOver = vEnv.size();
  for (auto it=vEnv.begin(); it<vEnv.end()-1;) {
    if (it->id == (it+1)->id) {
      s += tspan(it->id, to_string(it->pos)+"-"+to_string((it+1)->pos)) + "\n";
      // s += tspan(customColor(it->id), to_string(it->pos)+"-"+to_string((it+1)->pos)) + "\n";
      leftOver -= 2;
      it       += 2;
    }
    else {
      s += it->line + "\n";
      leftOver -= 1;
      it       += 1;
    }
  }
  if (leftOver == 1)
    s += vEnv.back().line + "\n";

  s.erase(s.find_last_of(", <")-2, 2);
}

inline void VizPaint::save_n_pos (const string& filePath) const {
  ifstream inFile(filePath);
  ofstream NFile(file_name(filePath)+"."+FMT_N);
  u64 pos=0, beg=0, num=0;
  bool begun = false;

  for (char c; inFile.get(c); ++pos) {
    if (c=='N' || c=='n') {
      if (!begun) {
        begun = true;
        beg = pos;
      }
      ++num;
    }
    else {
      begun = false;
      if (num != 0)
        NFile << beg << '\t' << beg+num-1 << '\n';
      num = 0;
      beg = 0;
    }
  }

  inFile.close();  NFile.close();
}

template <typename Position>
inline void VizPaint::print_pos (ofstream& fPlot, Position& pos, double X,
u64 maxBases, u32 min, string&& type) {
  struct Node {
    i64  position;
    char type;
    u64  start;
    // string color;
    Node (i64 p, char t, u64 s) : position(p), type(t), start(s) {}
    // Node (i64 p, char t, const string& c) : position(p), type(t), color(c) {}
  };
  
  // Ignore tiny regions
  for (auto posItr=pos.begin(); posItr!=pos.end(); ++posItr) {
    if (abs(posItr->endRef - posItr->begRef) < min ||
        abs(posItr->endTar - posItr->begTar) < min) {
      posItr->begRef = -1;
      posItr->endRef = -1;
      posItr->begTar = -1;
      posItr->endTar = -1;
    }
  }

  vector<Node> nodes;    nodes.reserve(2*pos.size());
  if (type == "ref") {
    for (u64 i=0; i!=pos.size(); ++i)
      if (pos[i].begRef != -1)
        nodes.emplace_back(Node(pos[i].begRef, 'b', pos[i].start));
        // nodes.emplace_back(Node(pos[i].begRef, 'b', customColor(pos[i].start)));
    for (u64 i=0; i!=pos.size(); ++i)
      if (pos[i].endRef != -1)
        // nodes.emplace_back(Node(pos[i].endRef, 'e', customColor(pos[i].start)));
        nodes.emplace_back(Node(pos[i].endRef, 'e', pos[i].start));
  }
  else if (type == "tar") {
    for (u64 i=0; i!=pos.size(); ++i)
      if (pos[i].begTar != -1)
        // nodes.emplace_back(Node(pos[i].begTar, 
        //   pos[i].endTar>pos[i].begTar ? 'b' : 'e', customColor(pos[i].start)));
        nodes.emplace_back(Node(pos[i].begTar, 
          pos[i].endTar>pos[i].begTar ? 'b' : 'e', pos[i].start));
    for (u64 i=0; i!=pos.size(); ++i)
      if (pos[i].endTar != -1)
        // nodes.emplace_back(Node(pos[i].endTar, 
        //   pos[i].endTar>pos[i].begTar ? 'e' : 'b', customColor(pos[i].start)));
        nodes.emplace_back(Node(pos[i].endTar, 
          pos[i].endTar>pos[i].begTar ? 'e' : 'b', pos[i].start));
  }
  std::sort(nodes.begin(), nodes.end(),
    [](const Node& l, const Node& r) { return l.position < r.position; });

  lastPos.emplace_back(nodes.back().position);

  auto   text = make_unique<Text>();
  string line, lastLine;
  i64    printPos  = 0;
  char   printType = 'b';
  u64    nOverlap  = 0;
  double CX=cx;    type=="ref" ? CX-=X : CX+=width+space+width+X;

  for (auto it=nodes.begin(); it<nodes.end()-1; ++it) {
    if ((it->type=='b' && (it+1)->type=='b') ||
        (it->type=='e' && (it+1)->type=='e')) {
      if ((it+1)->position - it->position < PAINT_SHORT * maxBases) {
        if (++nOverlap == 1) {
          if (it->start == (it+1)->start) {
          // if (it->color == (it+1)->color) {
            printPos = (it->position + (it+1)->position) / 2;
            printType = 'm';
          }
          else {
            printPos = it->position;
            printType = it->type;
          }
        }
        line += tspan(it->start, it->position);
        lastLine = tspan((it+1)->start, (it+1)->position);
        // line += tspan(it->color, it->position);
        // lastLine = tspan((it+1)->color, (it+1)->position);
      }
      else {
        if (nOverlap == 0) {
          printPos = it->position;
          printType = it->type;
        }
        nOverlap = 0;
      }
    }
    else if (it->type=='b' && (it+1)->type=='e') {
      if ((it+1)->position - it->position < PAINT_SHORT * maxBases) {
        if (++nOverlap == 1) {
          if (it->start == (it+1)->start) {
          // if (it->color == (it+1)->color) {
            printPos = (it->position + (it+1)->position) / 2;
            printType = 'm';
          }
          else {
            printPos = (it->position + (it+1)->position) / 2;
            printType = 'm';
          }
        }
        // line += tspan(it->color, it->position);
        // lastLine = tspan((it+1)->color, (it+1)->position);
        line += tspan(it->start, it->position);
        lastLine = tspan((it+1)->start, (it+1)->position);
      }
      else {
        if (nOverlap == 0) {
          printPos = it->position;
          printType = it->type;
        }
        nOverlap = 0;
      }
    }
    else if (it->type=='e' && (it+1)->type=='b') {
      if (nOverlap == 0) {
        printPos = it->position;
        printType = it->type;
      }
      nOverlap = 0;
    }

    if (nOverlap == 0) {
      // lastLine = tspan(it->color, it->position);
      lastLine = tspan(it->start, it->position);
      string finalLine {line+lastLine};
      sort_merge(finalLine);

      // text->fontWeight = "bold";
      if      (printType=='b')  text->dominantBaseline="text-before-edge";
      else if (printType=='m')  text->dominantBaseline="middle";
      else if (printType=='e')  text->dominantBaseline="text-after-edge";
      text->label = finalLine;
      text->origin = Point(CX, cy+get_point(printPos));
      type=="ref" ? text->plot_pos_ref(fPlot) : text->plot_pos_tar(fPlot);

      line.clear();
      lastLine.clear();

      //last
      if (it+2 == nodes.end()) {
        // finalLine = tspan((it+1)->color, (it+1)->position);
        finalLine = tspan((it+1)->start, (it+1)->position);
        sort_merge(finalLine);
        printPos = (it+1)->position;

        text->dominantBaseline="text-after-edge";
        text->label = finalLine;
        text->origin = Point(CX, cy+get_point(printPos));
        type=="ref" ? text->plot_pos_ref(fPlot) : text->plot_pos_tar(fPlot);
      }
    }
    
    if (it+2==nodes.end() && nOverlap!=0) {
      // lastLine = tspan((it+1)->color, (it+1)->position);
      lastLine = tspan((it+1)->start, (it+1)->position);
      string finalLine {line+lastLine};
      sort_merge(finalLine);

      // text->fontWeight = "bold";
      if      (printType=='b')  text->dominantBaseline="text-before-edge";
      else if (printType=='m')  text->dominantBaseline="middle";
      else if (printType=='e')  text->dominantBaseline="text-after-edge";
      text->label = finalLine;
      text->origin = Point(CX, cy+get_point(printPos));
      type=="ref" ? text->plot_pos_ref(fPlot) : text->plot_pos_tar(fPlot);
      break;
    }
  } // for
}