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

  vector<Pos> pos;
  u64 start {p.start};
  i64 br, er, bt, et;
  for (double nr,nt,sr,st; fPos >> br>>er>>nr>>sr >> bt>>et>>nt>>st; ++start)
    pos.emplace_back(Pos(br, er, nr, sr, bt, et, nt, st, start));
  p.start = start;

  // std::sort(pos.begin(), pos.end(),
  //   [](const Pos &l, const Pos &r) { return l.begRef < r.begRef; });
  // const auto last = unique(pos.begin(), pos.end(),
  //   [](const Pos &l, const Pos &r) { 
  //     return l.begRef==r.begRef && l.endRef==r.endRef; 
  //   });
  // pos.erase(last, pos.end());

  if (p.showPos) {
    print_pos(fPlot, p, pos, max(n_refBases,n_tarBases), "ref");
    print_pos(fPlot, p, pos, max(n_refBases,n_tarBases), "tar");

    if (!plottable)  error("not plottable positions.");
  }

  u64 n_regular=0, n_regularSolo=0, n_inverse=0, n_inverseSolo=0, n_ignore=0;
  for (auto e : pos) {
    if (abs(e.endTar-e.begTar) <= p.min) {
      ++n_ignore;
      continue;
    }
    else if (e.begRef!=-2 && e.endRef-e.begRef<=p.min) {
      ++n_ignore;
      continue;
    }

    if (e.begRef == -2) {
      if (e.endTar>e.begTar)  ++n_regularSolo;
      else                    ++n_inverseSolo;
    }

    const auto plot_main_ref = [&]() {
      if (e.begRef != -2) {
        rect->width  = width;
        rect->color  = rgb_color(e.start);
        rect->origin = Point(cx, cy + get_point(e.begRef));
        rect->height = get_point(e.endRef-e.begRef);
        rect->plot(fPlot);
        
        if (p.showNRC) {
          rect->color = nrc_color(e.entRef, p.colorMode);
          rect->plot_nrc_ref(fPlot);
        }
        if (p.showRedun) {
          rect->color = redun_color(e.selfRef, p.colorMode);
          rect->plot_redun_ref(fPlot, p.showNRC);
        }
      }
    };

    const auto plot_main_tar = [&](bool inverted) {
      if (e.begRef==-2)  rect->color="black";
      else               rect->color=rgb_color(e.start);
      rect->width=width;
      rect->height = get_point(abs(e.begTar-e.endTar));
      if (!inverted) {
        rect->origin = Point(cx+width+space, cy+get_point(e.begTar));
        rect->plot(fPlot);
      }
      else {
        rect->origin = Point(cx+width+space, cy+get_point(e.endTar));
        if (e.begRef==-2)  rect->plot_ir(fPlot, "#WavyWhite");
        else               rect->plot_ir(fPlot);
      }

      if (p.showNRC) {
        rect->color = nrc_color(e.entTar, p.colorMode);
        rect->plot_nrc_tar(fPlot);
      }
      if (p.showRedun) {
        rect->color = redun_color(e.selfTar, p.colorMode);
        rect->plot_redun_tar(fPlot, p.showNRC);
      }
    };

    if (e.endTar > e.begTar) {
      if (p.regular) {
        plot_main_ref();
        plot_main_tar(false);

        if (e.begRef != -2) {
          switch (p.link) {
          case 5:
            poly->lineColor = "grey";
            poly->fillColor = rgb_color(e.start);
            poly->one   = Point(cx+width,       cy+get_point(e.begRef));
            poly->two   = Point(cx+width,       cy+get_point(e.endRef));
            poly->three = Point(cx+width+space, cy+get_point(e.endTar));
            poly->four  = Point(cx+width+space, cy+get_point(e.begTar));
            poly->plot(fPlot);
            break;
          case 2:
            line->color = rgb_color(e.start);
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
            line->color = rgb_color(e.start);
            line->beg = Point(cx+width,       cy+get_point(e.begRef));
            line->end = Point(cx+width+space, cy+get_point(e.begTar));
            line->plot(fPlot);
            line->beg = Point(cx+width,       cy+get_point(e.endRef));
            line->end = Point(cx+width+space, cy+get_point(e.endTar));
            line->plot(fPlot);
            break;
          default: break;
          }
          ++n_regular;
        }
      }
    }
    else {
      if (p.inverse) {
        plot_main_ref();
        plot_main_tar(true);

        if (e.begRef != -2) {
          switch (p.link) {
          case 5:
            poly->lineColor = "grey";
            poly->fillColor = rgb_color(e.start);
            poly->one   = Point(cx+width,       cy+get_point(e.begRef));
            poly->two   = Point(cx+width,       cy+get_point(e.endRef));
            poly->three = Point(cx+width+space, cy+get_point(e.endTar));
            poly->four  = Point(cx+width+space, cy+get_point(e.begTar));
            poly->plot(fPlot);
            break;
          case 2:
            line->color = rgb_color(e.start);
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
            line->color = rgb_color(e.start);
            line->beg = Point(cx+width,       cy+get_point(e.begRef));
            line->end = Point(cx+width+space, cy+get_point(e.begTar));
            line->plot(fPlot);
  
            line->beg = Point(cx+width,       cy+get_point(e.endRef));
            line->end = Point(cx+width+space, cy+get_point(e.endTar));
            line->plot(fPlot);
            break;
          default: break;
          }
          ++n_inverse;
        }
      }
    }
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

  if (p.showAnnot)
    plot_annot(fPlot, max(n_refBases,n_tarBases), p.showNRC, p.showRedun);

  print_tail(fPlot);

  cerr << "Plotting finished.\n";
  cerr << "Found ";
  if (p.regular)
    cerr << n_regular << " regular";
  if (n_regularSolo != 0)
    cerr << ", " << n_regularSolo << " solo regular";
  if (p.inverse)
    cerr << ", " << n_inverse << " inverted";
  if (n_inverseSolo != 0)
    cerr << ", " << n_inverseSolo << " solo inverted";
  cerr << " region" << 
    (n_regular+n_regularSolo+n_inverse+n_inverseSolo > 1 ? "s" : "") << ".\n";
  if (n_ignore != 0)
    cerr << "Ignored " << n_ignore << " region" << 
      (n_ignore>1 ? "s" : "") << ".\n";
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

#ifdef EXTEND
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
#endif

#ifdef EXTEND
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
#endif

inline string VizPaint::rgb_color (u32 start) const {
  const auto hue = static_cast<u8>(start * mult);
  HsvColor HSV (hue);
  RgbColor RGB {hsv_to_rgb(HSV)};
  return string_format("#%X%X%X", RGB.r, RGB.g, RGB.b);
}

inline string VizPaint::nrc_color (double entropy, u32 colorMode) const {
  keep_in_range(0.0, entropy, 2.0);
#ifdef EXTEND
  // return heatmap_color(entropy/2 * (width+space+width));
#endif

  switch (colorMode) {
  case 0:   return COLORSET[0][entropy/2 * (COLORSET[0].size()-1)];
  case 1:   return COLORSET[1][entropy/2 * (COLORSET[1].size()-1)];
  case 2:   return COLORSET[2][entropy/2 * (COLORSET[2].size()-1)];
  default:  error("undefined color mode.");
  }
  
  return "";
}

inline string VizPaint::redun_color (double entropy, u32 colorMode) const {
  keep_in_range(0.0, entropy, 2.0);
#ifdef EXTEND
  // return heatmap_color(entropy/2 * (width+space+width));
#endif
  return nrc_color(entropy, colorMode);
}

inline void VizPaint::print_head (ofstream& f, double w, double h) const {
  f << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    "<!-- Morteza Hosseini, IEETA " << DEV_YEARS << " -->\n"
    << begin_elem("svg")
    << attrib("xmlns", "http://www.w3.org/2000/svg")
    << attrib("width", w)
    << attrib("height", h)
    << mid_elem();
    
  f << begin_elem("defs")
    << attrib("id", "ffff")
    << mid_elem()
    << begin_elem("pattern")
    << attrib("id", "Wavy")
    << attrib("height", 5.1805778)
    << attrib("width", 30.0)
    << attrib("patternUnits", "userSpaceOnUse")
    << mid_elem()
    << begin_elem("path")
    << attrib("d", "M 7.597,0.061 C 5.079,-0.187 2.656,0.302 -0.01,1.788 L "
         "-0.01,3.061 C 2.773,1.431 5.173,1.052 7.472,1.280 C 9.770,1.508 "
         "11.969,2.361 14.253,3.218 C 18.820,4.931 23.804,6.676 30.066,3.061 L "
         "30.062,1.788 C 23.622,5.497 19.246,3.770 14.691,2.061 C 12.413,1.207 "
         "10.115,0.311 7.597,0.061 z")
    << attrib("fill", "black")
    << attrib("stroke", "none")
    << end_empty_elem()
    << end_elem("pattern")
    << end_elem("defs");

  f << begin_elem("defs")
    << attrib("id", "ffff")
    << mid_elem()
    << begin_elem("pattern")
    << attrib("id", "WavyWhite")
    << attrib("height", 5.1805778)
    << attrib("width", 30.0)
    << attrib("patternUnits", "userSpaceOnUse")
    << mid_elem()
    << begin_elem("path")
    << attrib("d", "M 7.597,0.061 C 5.079,-0.187 2.656,0.302 -0.01,1.788 L "
         "-0.01,3.061 C 2.773,1.431 5.173,1.052 7.472,1.280 C 9.770,1.508 "
         "11.969,2.361 14.253,3.218 C 18.820,4.931 23.804,6.676 30.066,3.061 "
         "L 30.062,1.788 C 23.622,5.497 19.246,3.770 14.691,2.061 "
         "C 12.413,1.207 10.115,0.311 7.597,0.061 z")
    << attrib("fill", "white")
    << attrib("stroke", "none")
    << end_empty_elem()
    << end_elem("pattern")
    << end_elem("defs");
}

inline void VizPaint::print_tail (ofstream& f) const {
  f << "</svg>";
  // f << "</g>\n</svg>";
}

template <typename Value>
inline double VizPaint::get_point (Value p) const {
  return 5 * p / static_cast<double>(ratio);
}

inline void VizPaint::plot_legend (ofstream& f, const VizParam& p) const {
  if (!p.showNRC && !p.showRedun)  return;
  
  const auto vert = 24;
  auto rect = make_shared<Rectangle>();
  rect->height = 12;

  auto text = make_shared<Text>();
  // text->textAnchor = "middle";
  // // text->fontWeight = "bold";
  // text->fontSize = 9;

  if (p.showNRC && !p.showRedun) {
    rect->origin = Point(cx-(HORIZ_TUNE+width/HORIZ_RATIO), vert);
    rect->width  = width/HORIZ_RATIO+HORIZ_TUNE+width+space+width+
                   width/HORIZ_RATIO+HORIZ_TUNE;

    plot_legend_label(f, "RELATIVE REDUNDANCY", 
      Point(rect->origin.x+rect->width/2, rect->origin.y), "text-after-edge");

    // text->origin = Point(rect->origin.x+rect->width/2, rect->origin.y);
    // text->dominantBaseline = "text-after-edge";
    // text->label = "RELATIVE REDUNDANCY";
    // text->plot(f);
  }
  else if (!p.showNRC && p.showRedun) {
    rect->origin = Point(cx-(HORIZ_TUNE+width/HORIZ_RATIO), vert);
    rect->width  = width/HORIZ_RATIO+HORIZ_TUNE+width+space+width+
                   width/HORIZ_RATIO+HORIZ_TUNE;

    plot_legend_label(f, "REDUNDANCY", 
      Point(rect->origin.x+rect->width/2, rect->origin.y), "text-after-edge");

    // text->origin = Point(rect->origin.x+rect->width/2, rect->origin.y);
    // text->dominantBaseline = "text-after-edge";
    // text->label = "REDUNDANCY";
    // text->plot(f);
  }
  else if (p.showNRC && p.showRedun) {
    rect->origin = Point(cx-(HORIZ_TUNE+width/HORIZ_RATIO), vert);
    rect->width  = width/HORIZ_RATIO+HORIZ_TUNE+width+space+width+
                   width/HORIZ_RATIO+HORIZ_TUNE;

    plot_legend_label(f, "RELATIVE REDUNDANCY", 
      Point(rect->origin.x+rect->width/2, rect->origin.y), "text-after-edge");

    // text->origin = Point(rect->origin.x+rect->width/2, rect->origin.y);
    // text->dominantBaseline = "text-after-edge";
    // text->label = "RELATIVE REDUNDANCY";
    // text->plot(f);

    // Redundancy
    plot_legend_label(f, "REDUNDANCY", 
      Point(rect->origin.x+rect->width/2, rect->origin.y+rect->height),
      "text-before-edge");

    // text->origin = Point(rect->origin.x+rect->width/2, 
    //                      rect->origin.y+rect->height);
    // text->dominantBaseline = "text-before-edge";
    // text->label = "REDUNDANCY";
    // text->plot(f);
  }

  plot_legend_gradient(f, rect, p.colorMode);

  // text->dominantBaseline = "middle";
  // text->fontWeight = "normal";
  // text->fontSize   = 9;
  // text->textAnchor = "end";
  // text->origin = Point(rect->origin.x-2, rect->origin.y+rect->height/2);
  // text->label  = "0.0";
  // // text->fontWeight = "bold";
  // text->plot(f);
  // text->textAnchor = "middle";
  // for (u8 i=1; i!=4; ++i) {
  //   text->origin = 
  //     Point(rect->origin.x+(rect->width*i)/4, rect->origin.y+rect->height/2);
  //   if (p.colorMode==1 && i==3)  text->color="white";
  //   text->label = string_format("%.1f", i*0.5);
  //   text->plot(f);
  // }
  // text->textAnchor = "start";
  // text->origin = Point(rect->origin.x+rect->width+2, 
  //                      rect->origin.y+rect->height/2);
  // text->color = "black";
  // text->label = "2.0";
  // text->plot(f);
  // text->textAnchor = "middle";
  // text->fontWeight = "normal";
}

template <typename Rect>
inline void VizPaint::plot_legend_gradient (ofstream& f, const Rect& rect, 
u8 colorMode) const {
  auto grad = make_unique<Gradient>();
  switch (colorMode) {
  case 0:   grad->offsetColor=COLORSET[0];   break;
  case 1:   grad->offsetColor=COLORSET[1];   break;
  case 2:   grad->offsetColor=COLORSET[2];   break;
  default:  error("undefined color mode.");
  }

  auto id = to_string(rect->origin.x) + to_string(rect->origin.y);
  f << begin_elem("defs")
    << mid_elem()
    << begin_elem("linearGradient")
    << attrib("id", "grad"+id)
    << attrib("x1", "0%")
    << attrib("y1", "0%")
    << attrib("x2", "100%")
    << attrib("y2", "0%")
    << mid_elem();
  for (u8 i=0; i!=grad->offsetColor.size(); ++i) {
    f << begin_elem("stop") 
      << attrib("offset", to_string(i*100/(grad->offsetColor.size()-1))+"%")
      << attrib("stop-color", grad->offsetColor[i]) 
      << attrib("stop-opacity", 1)
      << end_empty_elem();
  }
  f << end_elem("linearGradient")
    << end_elem("defs")
    << begin_elem("rect")
    << attrib("fill", "url(#grad"+id+")")
    << attrib("width", rect->width, true)
    << attrib("height", rect->height, true)
    << attrib("x", rect->origin.x, true)
    << attrib("y", rect->origin.y, true)
    << attrib("ry", 3)
    << end_empty_elem();
}

inline void VizPaint::plot_legend_label (ofstream& f, const string& label, 
const Point& origin, const string& dominantBaseline) const {
  auto text = make_unique<Text>();
  text->textAnchor = "middle";
  // text->fontWeight = "bold";
  text->fontSize = 9;

  text->origin = origin;
  text->dominantBaseline = dominantBaseline;
  text->label = label;
  text->plot(f);

  text->dominantBaseline = "middle";
  text->fontWeight = "normal";
  text->fontSize   = 9;
  text->textAnchor = "end";
  text->origin = Point(rect->origin.x-2, rect->origin.y+rect->height/2);
  text->label  = "0.0";
  // text->fontWeight = "bold";
  text->plot(f);
  text->textAnchor = "middle";
  for (u8 i=1; i!=4; ++i) {
    text->origin = 
      Point(rect->origin.x+(rect->width*i)/4, rect->origin.y+rect->height/2);
    if (p.colorMode==1 && i==3)  text->color="white";
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
}

inline void VizPaint::plot_annot (ofstream& f, i64 maxHeight, bool showNRC,
bool showRedun) const {
  if (!showNRC && !showRedun)  return;

  double horizSize=0.0, vertSize=0.0;
  const auto X1 = cx - HORIZ_TUNE - 0.5*width/HORIZ_RATIO;
  const auto X2 = cx + 2*width + space + HORIZ_TUNE + 0.5*width/HORIZ_RATIO;
  const auto Y  = cy + get_point(maxHeight) + 20;
  horizSize = (!showNRC && showRedun) ? HORIZ_TUNE + width/HORIZ_RATIO + 15
                                      : space/5.0;
  vertSize  = Y - cy - get_point(lastPos[0]);

  auto path = make_unique<Path>();
  path->color="black";
  
  if (lastPos.size() == 1) {
    path->origin = Point(X2, cy+get_point(lastPos[0]));
    path->trace = "v "+to_string(vertSize)+" h "+to_string(-horizSize);
    path->plot(f);
  }
  else if (lastPos.size() == 2) {
    path->origin = Point(X1, cy+get_point(lastPos[0]));
    path->trace = "v "+to_string(vertSize)+" h "+to_string(horizSize);
    path->plot(f);

    vertSize = Y - cy - get_point(lastPos[1]);
    path->origin = Point(X2, cy+get_point(lastPos[1]));
    path->trace = "v "+to_string(vertSize)+" h "+to_string(-horizSize);
    path->plot(f);
  }

  auto text = make_unique<Text>();
  text->fontSize=9;
  text->color="black";

  if (showNRC && !showRedun) {
    text->origin=Point((X1+X2)/2, Y);
    text->label="Relative Redundancy";
    text->plot(f);
  }
  else if (!showNRC && showRedun) {
    text->origin=Point((X1+X2)/2, Y);
    text->label="Redundancy";
    text->plot(f);
  }
  else if (showNRC && showRedun) {
    text->origin=Point((X1+X2)/2, Y);
    text->label="Relative Redundancy";
    text->plot(f);

    const auto redunX1 = cx - 2*HORIZ_TUNE - 1.5*width/HORIZ_RATIO;
    const auto redunX2 = 
      cx + 2*width + space + 2*HORIZ_TUNE + 1.5*width/HORIZ_RATIO;
    const auto redunY = Y + 15;

    if (lastPos.size() == 1) {
      vertSize  = redunY - cy - get_point(lastPos[0]);
      horizSize += HORIZ_TUNE + width/HORIZ_RATIO + 15;
      path->origin = Point(redunX2, cy+get_point(lastPos[0]));
      path->trace = "v "+to_string(vertSize)+" h "+to_string(-horizSize);
      path->plot(f);
    }
    else if (lastPos.size() == 2) {
      vertSize  = redunY - cy - get_point(lastPos[0]);
      horizSize += HORIZ_TUNE + width/HORIZ_RATIO + 15;
      path->origin = Point(redunX1, cy+get_point(lastPos[0]));
      path->trace = "v "+to_string(vertSize)+" h "+to_string(horizSize);
      path->plot(f);

      vertSize = redunY - cy - get_point(lastPos[1]);
      path->origin = Point(redunX2, cy+get_point(lastPos[1]));
      path->trace = "v "+to_string(vertSize)+" h "+to_string(-horizSize);
      path->plot(f);
    }

    text->origin=Point((redunX1+redunX2)/2, redunY);
    text->label="Redundancy";
    text->plot(f);
  }
}

inline string VizPaint::tspan (u32 start, i64 pos) const {
  return "<tspan id=\"" + to_string(start) + "\" style=\"fill:" + 
    rgb_color(start) + "\">" + to_string(pos) + ", </tspan>\n";
}
inline string VizPaint::tspan (u32 start, const string& pos) const {
  return "<tspan id=\"" + to_string(start) + "\" style=\"fill:" + 
    rgb_color(start) + "\">" + pos + ", </tspan>\n";
}

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
inline void VizPaint::print_pos (ofstream& fPlot, VizParam& par, Position& pos,
u64 maxBases, string&& type) {
  struct Node {
    i64  position;
    char type;
    u64  start;
    Node (i64 p, char t, u64 s) : position(p), type(t), start(s) {}
  };
  
  vector<Node> nodes;    nodes.reserve(2*pos.size());
  if (type == "ref") {
    for (auto e : pos)
      if (e.endRef-e.begRef>par.min && abs(e.endTar-e.begTar)>par.min)
        nodes.emplace_back(Node(e.begRef, 'b', e.start));
    for (auto e : pos)
      if (e.endRef-e.begRef>par.min && abs(e.endTar-e.begTar)>par.min)
        nodes.emplace_back(Node(e.endRef, 'e', e.start));
  }
  else if (type == "tar") {
    for (auto e : pos)
      if ((abs(e.endTar-e.begTar)>par.min && e.begRef==-2) ||
          (abs(e.endTar-e.begTar)>par.min && e.endRef-e.begRef>par.min))
        nodes.emplace_back(Node(e.begTar, 
          e.endTar>e.begTar ? 'b' : 'e', e.start));
    for (auto e : pos)
      if ((abs(e.endTar-e.begTar)>par.min && e.begRef==-2) ||
          (abs(e.endTar-e.begTar)>par.min && e.endRef-e.begRef>par.min))
        nodes.emplace_back(Node(e.endTar, 
          e.endTar>e.begTar ? 'e' : 'b', e.start));
  }
  std::sort(nodes.begin(), nodes.end(),
    [](const Node& l, const Node& r) { return l.position < r.position; });

  plottable = static_cast<bool>(nodes.size());
  if (!plottable)  return;

  if (!par.manMult)  par.mult = 512 / nodes.size();  // 256/(size/2)
  mult = par.mult;

  lastPos.emplace_back(nodes.back().position);

  auto   text = make_unique<Text>();
  string line, lastLine;
  i64    printPos  = 0;
  char   printType = 'b';
  u64    nOverlap  = 0;
  double X         = 0;
  if (par.showNRC && par.showRedun)
    X = 2 * (HORIZ_TUNE + width/HORIZ_RATIO);
  else if (par.showNRC ^ par.showRedun)
    X = HORIZ_TUNE + width/HORIZ_RATIO;
  double CX=cx;    type=="ref" ? CX-=X : CX+=width+space+width+X;

  for (auto it=nodes.begin(); it<nodes.end()-1; ++it) {
    if ((it->type=='b' && (it+1)->type=='b') ||
        (it->type=='e' && (it+1)->type=='e')) {
      if ((it+1)->position - it->position < PAINT_SHORT * maxBases) {
        if (++nOverlap == 1) {
          if (it->start == (it+1)->start) {
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
            printPos = (it->position + (it+1)->position) / 2;
            printType = 'm';
          }
          else {
            printPos = (it->position + (it+1)->position) / 2;
            printType = 'm';
          }
        }
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