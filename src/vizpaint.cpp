#include "vizpaint.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "number.hpp"
using namespace smashpp;

void VizPaint::print_plot (VizParam& p) {
  check_file(p.posFile);
  ifstream fPos (p.posFile);
  ofstream fPlot(p.image);

  // Read watermark, ref & tar names and sizes from file
  u64 n_refBases=0, n_tarBases=0;
  string watermark, ref, tar;
  fPos >> watermark >> ref >> n_refBases >> tar >> n_tarBases;
  if (watermark!=POS_HDR)  error("unknown file format for positions.");
  if (p.verbose)           show_info(p, ref, tar, n_refBases, n_tarBases);

  cerr << "Plotting ...\r";
  config(p.width, p.space, p.mult, n_refBases, n_tarBases);

  auto Paint_Extra = PAINT_EXTRA;
  if (p.showAnnot)  Paint_Extra+=30;

  print_head(fPlot, PAINT_CX+width+space+width+PAINT_CX, maxSize+Paint_Extra);

  auto poly = make_unique<Polygon>();
  auto text = make_unique<Text>();
  auto line = make_unique<Line>();
  line->strokeWidth = 2.0;
  auto rect = make_unique<Rectangle>();
  rect->opacity = p.opacity;

  // Plot background page
  rect->fill = rect->stroke = backColor;
  rect->origin = Point(0, 0);
  rect->width  = PAINT_CX + width + space + width + PAINT_CX;
  rect->height = maxSize + Paint_Extra;
  rect->plot(fPlot);

  // Print titles
  text->origin = Point(cx + width/2, cy - 15);
  text->label  = ref;
  text->print_title(fPlot);
  text->origin = Point(cx + width + space + width/2, cy - 15);
  text->label  = tar;
  text->print_title(fPlot);

  // If min is set to default, reset to base max proportion
  if (p.min==0)    p.min=static_cast<u32>(maxSize / 100);

  // Read positions from file and Print them
  vector<Position> pos;
  read_pos(fPos, pos, p);

  make_posNode(pos, p, "ref");
  if (p.showPos)
    print_pos(fPlot, p, pos, max(n_refBases,n_tarBases), "ref");

  make_posNode(pos, p, "tar");
  if (p.showPos)
    print_pos(fPlot, p, pos, max(n_refBases,n_tarBases), "tar");

  if (p.showPos)
    if (!plottable)  error("not plottable positions.");

  // Plot
  u64 n_regular=0, n_regularSolo=0, n_inverse=0, n_inverseSolo=0, n_ignore=0;
  // for (auto e : pos) {
  for (auto e=pos.rbegin(); e!=pos.rend(); ++e) {
    if (abs(e->endTar-e->begTar) <= p.min) {
      ++n_ignore;    continue;
    }
    else if (e->begRef!=DBLANK && e->endRef-e->begRef<=p.min) {
      ++n_ignore;    continue;
    }

    if (e->begRef==DBLANK) {e->endTar>e->begTar? ++n_regularSolo: ++n_inverseSolo;}

    const auto plot_main = [&](auto& cylinder) {
      cylinder->width = width;
      cylinder->strokeWidth = 0.5;
      cylinder->opacity = p.opacity;
    };

    const auto plot_main_ref = [&]() {
      if (e->begRef != DBLANK) {
        auto cylinder = make_unique<Cylinder>();
        plot_main(cylinder);
        cylinder->height = get_point(e->endRef-e->begRef);
        cylinder->fill = rgb_color(e->start);
        cylinder->stroke = shade(cylinder->fill);
        cylinder->origin = Point(cx, cy + get_point(e->begRef));
        cylinder->plot(fPlot);

        cylinder->strokeWidth = 0.7;
        if (p.showNRC) {
          cylinder->fill = nrc_color(e->entRef, p.colorMode);
          cylinder->plot_periph(fPlot, 'r');
        }
        if (p.showRedun) {
          cylinder->fill = redun_color(e->selfRef, p.colorMode);
          cylinder->plot_periph(fPlot, 'r', u8(p.showNRC));
        }
      }
    };

    const auto plot_main_tar = [&](bool inverted) {
      auto cylinder = make_unique<Cylinder>();
      plot_main(cylinder);
      cylinder->height = get_point(abs(e->begTar-e->endTar));
      if (e->begRef == DBLANK) {
        cylinder->fill = "black";
        cylinder->stroke = "white";
      } else {
        cylinder->fill = rgb_color(e->start);
        cylinder->stroke = shade(cylinder->fill);
      }

      if (!inverted) {
        cylinder->origin = Point(cx+width+space, cy+get_point(e->begTar));
        cylinder->plot(fPlot);
      } else {
        cylinder->origin = Point(cx+width+space, cy+get_point(e->endTar));
        if (e->begRef==DBLANK)  cylinder->plot_ir(fPlot, "#WavyWhite");
        else                   cylinder->plot_ir(fPlot);
      }

      cylinder->strokeWidth = 0.7;
      if (p.showNRC) {
        cylinder->fill = nrc_color(e->entTar, p.colorMode);
        cylinder->plot_periph(fPlot, 't');
      }
      if (p.showRedun) {
        cylinder->fill = redun_color(e->selfTar, p.colorMode);
        cylinder->plot_periph(fPlot, 't', u8(p.showNRC));
      }
    };

    if (e->endTar > e->begTar) {
      if (p.regular) {
        plot_main_ref();
        plot_main_tar(false);

        if (e->begRef != DBLANK) {
          switch (p.link) {
          case 1:
            poly->fillColor = rgb_color(e->start);
            poly->stroke_opacity = poly->fill_opacity = 0.5 * p.opacity;
            poly->add_point(cx+width,       cy+get_point(e->begRef));
            poly->add_point(cx+width,       cy+get_point(e->endRef));
            poly->add_point(cx+width+space, cy+get_point(e->endTar));
            poly->add_point(cx+width+space, cy+get_point(e->begTar));
            poly->plot(fPlot);
            break;
          case 2:
            line->stroke = rgb_color(e->start);
            line->beg = Point(
              cx+width, cy+get_point(e->begRef+(e->endRef-e->begRef)/2.0));
            line->end = Point(
              cx+width+space, cy+get_point(e->begTar+(e->endTar-e->begTar)/2.0));
            line->plot(fPlot);
            break;
          case 3:
            line->stroke = "black";
            line->beg = Point(
              cx+width, cy+get_point(e->begRef+(e->endRef-e->begRef)/2.0));
            line->end = Point(
              cx+width+space, cy+get_point(e->begTar+(e->endTar-e->begTar)/2.0));
            line->plot(fPlot);
            break;
          case 4:
            line->stroke = rgb_color(e->start);
            line->beg = Point(cx+width,       cy+get_point(e->begRef));
            line->end = Point(cx+width+space, cy+get_point(e->begTar));
            line->plot(fPlot);
            line->beg = Point(cx+width,       cy+get_point(e->endRef));
            line->end = Point(cx+width+space, cy+get_point(e->endTar));
            line->plot(fPlot);
            break;
          case 5:
            line->stroke = "black";
            line->beg = Point(cx+width,       cy+get_point(e->begRef));
            line->end = Point(cx+width+space, cy+get_point(e->begTar));
            line->plot(fPlot);
            line->beg = Point(cx+width,       cy+get_point(e->endRef));
            line->end = Point(cx+width+space, cy+get_point(e->endTar));
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

        if (e->begRef != DBLANK) {
          switch (p.link) {
          case 1:
            poly->fillColor = rgb_color(e->start);
            poly->stroke_opacity = poly->fill_opacity = 0.5 * p.opacity;
            poly->add_point(cx+width,       cy+get_point(e->begRef));
            poly->add_point(cx+width,       cy+get_point(e->endRef));
            poly->add_point(cx+width+space, cy+get_point(e->endTar));
            poly->add_point(cx+width+space, cy+get_point(e->begTar));
            poly->plot(fPlot);
            break;
          case 2:
            line->stroke = rgb_color(e->start);
            line->beg = Point(
              cx+width, cy+get_point(e->begRef+(e->endRef-e->begRef)/2.0));
            line->end = Point(
              cx+width+space, cy+get_point(e->endTar+(e->begTar-e->endTar)/2.0));
            line->plot(fPlot);
            break;
          case 3:
            line->stroke = "green";
            line->beg = Point(
              cx+width, cy+get_point(e->begRef+(e->endRef-e->begRef)/2.0));
            line->end = Point(
              cx+width+space, cy+get_point(e->endTar+(e->begTar-e->endTar)/2.0));
            line->plot(fPlot);
            break;
          case 4:
            line->stroke = rgb_color(e->start);
            line->beg = Point(cx+width,       cy+get_point(e->begRef));
            line->end = Point(cx+width+space, cy+get_point(e->begTar));
            line->plot(fPlot);
  
            line->beg = Point(cx+width,       cy+get_point(e->endRef));
            line->end = Point(cx+width+space, cy+get_point(e->endTar));
            line->plot(fPlot);
            break;
          case 5:
            line->stroke = "green";
            line->beg = Point(cx+width,       cy+get_point(e->begRef));
            line->end = Point(cx+width+space, cy+get_point(e->begTar));
            line->plot(fPlot);
  
            line->beg = Point(cx+width,       cy+get_point(e->endRef));
            line->end = Point(cx+width+space, cy+get_point(e->endTar));
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
    rect->fill = rect->stroke  = "grey";
    rect->origin = Point(cx, cy+get_point(beg));
    rect->height = get_point(end-beg+1);
    rect->plot(fPlot);
  }
  refFile.close();
  remove((file_name(ref)+"."+FMT_N).c_str());

  save_n_pos(tar);
  ifstream tarFile(file_name(tar)+"."+FMT_N);
  for (i64 beg,end; tarFile>>beg>>end;) {
    rect->fill = rect->stroke  = "grey";
    rect->origin = Point(cx+width+space, cy+get_point(beg));
    rect->height = get_point(end-beg+1);
    rect->plot(fPlot);
  }
  tarFile.close();
  remove((file_name(tar)+"."+FMT_N).c_str());
  
  // Plot chromosomes
  auto cylinder = make_unique<Cylinder>();
  cylinder->width = width;
  cylinder->height = refSize;
  cylinder->strokeWidth = 2;
  cylinder->origin = Point(cx, cy);
  cylinder->plot(fPlot);

  cylinder->height = tarSize;
  cylinder->origin = Point(cx+width+space, cy);
  cylinder->plot(fPlot);

  // Plot legend and annotation
  plot_legend(fPlot, p);
  if (p.showAnnot)
    plot_annot(fPlot, max(n_refBases,n_tarBases), p.showNRC, p.showRedun);

  print_tail(fPlot);

  // Log
  cerr << "Plotting finished.\n";
  cerr << "Found ";
  if (p.regular)         cerr <<         n_regular     << " regular";
  if (n_regularSolo!=0)  cerr << ", " << n_regularSolo << " solo regular";
  if (p.inverse)         cerr << ", " << n_inverse     << " inverted";
  if (n_inverseSolo!=0)  cerr << ", " << n_inverseSolo << " solo inverted";
  cerr << " region" <<
    (n_regular+n_regularSolo+n_inverse+n_inverseSolo>1 ? "s" : "") << ".\n";

  if (n_ignore!=0)
    cerr << "Ignored " << n_ignore << " region" << (n_ignore>1 ? "s" : "") 
         << ".\n";
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

inline void VizPaint::config (double width_, double space_, u32 mult_,
u64 refSize_, u64 tarSize_) {
  ratio   = static_cast<u32>(max(refSize_,tarSize_) / PAINT_SCALE);
  width   = width_;
  space   = space_;
  mult    = mult_;
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
string VizPaint::rgb_color (u32 start) const {
  const auto hue = static_cast<u8>(start * mult);
  HsvColor HSV (hue);
  RgbColor RGB {hsv_to_rgb(HSV)};
  return to_hex_color(RGB);
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
  // Header
  f << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    << "<!-- Morteza Hosseini, IEETA " << DEV_YEARS << " -->\n"
    << begin_elem("svg")
    << attrib("xmlns", "http://www.w3.org/2000/svg")
    << attrib("xmlns:xlink", "http://www.w3.org/1999/xlink")
    << attrib("width", w)
    << attrib("height", h)
    << mid_elem();
  
  // Patterns
  auto defs = make_unique<Defs>();
  auto pattern = make_unique<Pattern>();
  auto path = make_unique<Path>();
  defs->id = "ffff";
  pattern->id = "Wavy";
  pattern->patternUnits = "userSpaceOnUse";
  pattern->x=-width/2;
  pattern->y=0;
  pattern->width = width;
  pattern->height = 7;
  path->d = "m0,0 a "+to_string(pattern->width/2)+","+to_string(ry)+
    " 0 0,0 "+to_string(width)+",0 ";
  path->stroke = "black";
  path->fill = "transparent";
  // pattern->width = width;
  // pattern->height = 49;
  // path->d = "M13.99 9.25l13 7.5v15l-13 7.5L1 31.75v-15l12.99-7.5zM3 17.9v12.7l10.99 6.34 11-6.35V17.9l-11-6.34L3 17.9zM0 15l12.98-7.5V0h-2v6.35L0 12.69v2.3zm0 18.5L12.98 41v8h-2v-6.85L0 35.81v-2.3zM15 0v7.5L27.99 15H28v-2.31h-.01L17 6.35V0h-2zm0 49v-8l12.99-7.5H28v2.31h-.01L17 42.15V49h-2z";
  // path->fill = "black";
  make_pattern(f, defs, pattern, path);

  pattern->id = "WavyWhite";
  path->stroke = "white";
  make_pattern(f, defs, pattern, path);
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
  const string shiftX="2.5", shiftY="3";
  auto rect = make_shared<Rectangle>();
  rect->height = 12;

  auto text = make_unique<Text>();
  text->textAnchor = "middle";
  // text->fontWeight = "bold";
  text->fontSize = 9;

  if (p.showNRC && !p.showRedun) {
    rect->origin = Point(cx-(HORIZ_TUNE+width/HORIZ_RATIO), vert);
    rect->width  = width/HORIZ_RATIO+HORIZ_TUNE+width+space+width+
                   HORIZ_TUNE+width/HORIZ_RATIO;

    text->origin = Point(rect->origin.x+rect->width/2, rect->origin.y);
    text->dy = "-"+shiftY;
    text->dominantBaseline = "text-after-edge";
    text->label = "RELATIVE REDUNDANCY";
    text->plot(f);
  }
  else if (!p.showNRC && p.showRedun) {
    rect->origin = Point(cx-(HORIZ_TUNE+width/HORIZ_RATIO), vert);
    rect->width  = width/HORIZ_RATIO+HORIZ_TUNE+width+space+width+
                   HORIZ_TUNE+width/HORIZ_RATIO;

    text->origin = Point(rect->origin.x+rect->width/2, rect->origin.y);
    text->dy = "-"+shiftY;
    text->dominantBaseline = "text-after-edge";
    text->label = "REDUNDANCY";
    text->plot(f);
  }
  else if (p.showNRC && p.showRedun) {
    rect->origin = Point(cx-2*(HORIZ_TUNE+width/HORIZ_RATIO), vert);
    rect->width  = 2*width/HORIZ_RATIO+2*HORIZ_TUNE+width+space+width+
                   2*width/HORIZ_RATIO+2*HORIZ_TUNE;

    text->origin = Point(rect->origin.x+rect->width/2, rect->origin.y);
    text->dy = "-"+shiftY;
    text->dominantBaseline = "text-after-edge";
    text->label = "RELATIVE REDUNDANCY";
    text->plot(f);

    // Redundancy
    text->origin = Point(rect->origin.x+rect->width/2, 
                         rect->origin.y+rect->height);
    text->dy = shiftY;
    text->dominantBaseline = "text-before-edge";
    text->label = "REDUNDANCY";
    text->plot(f);
  }

  plot_legend_gradient(f, rect, p.colorMode);

  // Print numbers (measures)
  text->dy = "0";
  text->dominantBaseline = "middle";
  text->fontWeight = "normal";
  text->fontSize = 9;
  text->textAnchor = "end";
  text->origin = Point(rect->origin.x-2, rect->origin.y+rect->height/2);
  text->dx = "-"+shiftX;
  text->label = "0.0";
  // text->fontWeight = "bold";
  text->plot(f);
  text->textAnchor = "middle";
  for (u8 i=1; i!=4; ++i) {
    text->origin = 
      Point(rect->origin.x+(rect->width*i)/4, rect->origin.y+rect->height/2);
    text->dx = "0";
    if (p.colorMode==1 && i==3)  text->color="white";
    text->label = string_format("%.1f", i*0.5);
    text->plot(f);
  }
  text->textAnchor = "start";
  text->origin = Point(rect->origin.x+rect->width+2, 
                       rect->origin.y+rect->height/2);
  text->dx = shiftX;
  text->color = "black";
  text->label = "2.0";
  text->plot(f);
  text->textAnchor = "middle";
  text->fontWeight = "normal";
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
    // << attrib("x2", "0%")
    // << attrib("y2", "100%")
    << attrib("x2", "100%")
    << attrib("y2", "0%")
    << mid_elem();
  for (u8 i=0; i!=grad->offsetColor.size(); ++i) {
    f << begin_elem("stop") 
      << attrib("offset", 
                to_string(i*100/(grad->offsetColor.size()-1))+"%")
      << attrib("stop-color", grad->offsetColor[i]) 
      << attrib("stop-opacity", 1)
      << end_empty_elem();
  }
  f << end_elem("linearGradient")
    << end_elem("defs");

  rect->stroke = "black";
  rect->strokeWidth = 0.35;
  rect->fill = "url(#grad"+id+")";
  rect->plot(f);

  // auto cylinder = make_unique<Cylinder>();
  // cylinder->width = rect->height;
  // cylinder->height = rect->width;
  // cylinder->origin = Point(rect->origin.x, rect->origin.y+rect->height);
  // cylinder->transform = "rotate(-90 " + to_string(cylinder->origin.x) + " " +
  //   to_string(cylinder->origin.y) + ")";
  // cylinder->fill = "url(#grad" + id + ")";
  // cylinder->ry = 0;
  // cylinder->strokeWidth = 0.35;
  // cylinder->stroke = "black";
  // cylinder->plot(f);
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
  path->stroke="black";
  path->strokeDashArray = "8 3";
  
  if (lastPos.size() == 1) {
    path->origin = Point(X2, cy+get_point(lastPos[0]));
    path->trace  = "v "+to_string(vertSize)+" h "+to_string(-horizSize);
    path->plot(f);
  }
  else if (lastPos.size() == 2) {
    path->origin = Point(X1, cy+get_point(lastPos[0]));
    path->trace  = "v "+to_string(vertSize)+" h "+to_string(horizSize);
    path->plot(f);

    vertSize = Y - cy - get_point(lastPos[1]);
    path->origin = Point(X2, cy+get_point(lastPos[1]));
    path->trace  = "v "+to_string(vertSize)+" h "+to_string(-horizSize);
    path->plot(f);
  }

  auto text = make_unique<Text>();
  text->fontSize=9;
  text->color="black";

  if (showNRC && !showRedun) {
    text->origin = Point((X1+X2)/2, Y);
    text->label  = "Relative Redundancy";
    text->plot(f);
  }
  else if (!showNRC && showRedun) {
    text->origin = Point((X1+X2)/2, Y);
    text->label  = "Redundancy";
    text->plot(f);
  }
  else if (showNRC && showRedun) {
    text->origin = Point((X1+X2)/2, Y);
    text->label  = "Relative Redundancy";
    text->plot(f);

    const auto redunX1 = cx - 2*HORIZ_TUNE - 1.5*width/HORIZ_RATIO;
    const auto redunX2 = cx + 2*width + space + 2*HORIZ_TUNE + 
                         1.5*width/HORIZ_RATIO;
    const auto redunY = Y + 15;

    if (lastPos.size() == 1) {
      vertSize     = redunY - cy - get_point(lastPos[0]);
      horizSize   += HORIZ_TUNE + width/HORIZ_RATIO + 15;
      path->origin = Point(redunX2, cy+get_point(lastPos[0]));
      path->trace  = "v "+to_string(vertSize)+" h "+to_string(-horizSize);
      path->plot(f);
    }
    else if (lastPos.size() == 2) {
      vertSize     = redunY - cy - get_point(lastPos[0]);
      horizSize   += HORIZ_TUNE + width/HORIZ_RATIO + 15;
      path->origin = Point(redunX1, cy+get_point(lastPos[0]));
      path->trace  = "v "+to_string(vertSize)+" h "+to_string(horizSize);
      path->plot(f);

      vertSize = redunY - cy - get_point(lastPos[1]);
      path->origin = Point(redunX2, cy+get_point(lastPos[1]));
      path->trace  = "v "+to_string(vertSize)+" h "+to_string(-horizSize);
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
  for (string gl; getline(stream, gl);)  vLine.emplace_back(gl);

  if (vLine.size()==1) { s.erase(s.find(", "), 2);    return; }

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
    } else {
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

inline void VizPaint::read_pos (ifstream& fPos, vector<Position>& pos, 
VizParam& par) {
  double nr,nt,sr,st;
  for (i64 br, er, bt, et; fPos >> br>>er>>nr>>sr >> bt>>et>>nt>>st; 
       ++par.start)
    pos.emplace_back(Position(br, er, nr, sr, bt, et, nt, st, par.start));

  if (sr==DBLANK && st==DBLANK)
    par.showRedun=false;

  // std::sort(pos.begin(), pos.end(),
  //   [](const Position& l, const Position& r) { return l.begRef < r.begRef; });
  // const auto last = unique(pos.begin(), pos.end(),
  //   [](const Position &l, const Position &r) { 
  //     return l.begRef==r.begRef && l.endRef==r.endRef; 
  //   });
  // pos.erase(last, pos.end());
}

inline void VizPaint::make_posNode (const vector<Position>& pos, 
const VizParam& par, string&& type) {
  nodes.clear();
  nodes.reserve(2*pos.size());

  if (type == "ref") {
    for (auto e : pos)
      if (e.endRef-e.begRef>par.min && abs(e.endTar-e.begTar)>par.min)
        nodes.emplace_back(PosNode(e.begRef, 'b', e.start));
    for (auto e : pos)
      if (e.endRef-e.begRef>par.min && abs(e.endTar-e.begTar)>par.min)
        nodes.emplace_back(PosNode(e.endRef, 'e', e.start));
  }
  else if (type == "tar") {
    for (auto e : pos)
      if ((abs(e.endTar-e.begTar)>par.min && e.begRef==DBLANK) ||
          (abs(e.endTar-e.begTar)>par.min && e.endRef-e.begRef>par.min))
        nodes.emplace_back(PosNode(e.begTar, 
          e.endTar>e.begTar ? 'b' : 'e', e.start));
    for (auto e : pos)
      if ((abs(e.endTar-e.begTar)>par.min && e.begRef==DBLANK) ||
          (abs(e.endTar-e.begTar)>par.min && e.endRef-e.begRef>par.min))
        nodes.emplace_back(PosNode(e.endTar, 
          e.endTar>e.begTar ? 'e' : 'b', e.start));
  }

  std::sort(nodes.begin(), nodes.end(),
    [](const PosNode& l, const PosNode& r) { return l.position < r.position; });

  plottable = static_cast<bool>(nodes.size());
  if (!plottable)  return;

  // if (!par.manMult)  par.mult = 512 / nodes.size();  // 256/(size/2)
  // mult = par.mult;

  lastPos.emplace_back(nodes.back().position);
}

inline void VizPaint::print_pos (ofstream& fPlot, VizParam& par, 
const vector<Position>& pos, u64 maxBases, string&& type) {
  auto   text = make_unique<Text>();
  string line, lastLine;
  i64    printPos  = 0;
  char   printType = 'b';
  u64    nOverlap  = 0;
  double shiftX    = 4;
  if (par.showNRC && par.showRedun)
    shiftX += 2 * (HORIZ_TUNE + width/HORIZ_RATIO);
  else if (par.showNRC ^ par.showRedun)
    shiftX += HORIZ_TUNE + width/HORIZ_RATIO;
  double CX=cx;    type=="ref" ? CX-=shiftX : CX+=width+space+width+shiftX;

  const auto set_dominantBaseline = [&](char type) {
    switch (type) {
    case 'b':  text->dominantBaseline="text-before-edge";  break;
    case 'm':  text->dominantBaseline="middle";            break;
    case 'e':  text->dominantBaseline="text-after-edge";   break;
    default:   text->dominantBaseline="middle";            break;
    }
  };

  for (auto it=nodes.begin(); it<nodes.end()-1; ++it) {
    if ((it->type=='b' && (it+1)->type=='b') ||
        (it->type=='e' && (it+1)->type=='e')) {
      if ((it+1)->position - it->position < PAINT_SHORT * maxBases) {
        if (++nOverlap == 1) {
          if (it->start == (it+1)->start) {
            printPos  = (it->position + (it+1)->position) / 2;
            printType = 'm';
          } else {
            printPos  = it->position;
            printType = it->type;
          }
        }
        line    += tspan(it->start, it->position);
        lastLine = tspan((it+1)->start, (it+1)->position);
      }
      else {
        if (nOverlap == 0) {
          printPos  = it->position;
          printType = it->type;
        }
        nOverlap = 0;
      }
    }
    else if (it->type=='b' && (it+1)->type=='e') {
      if ((it+1)->position - it->position < PAINT_SHORT * maxBases) {
        if (++nOverlap == 1) {
          if (it->start == (it+1)->start) {
            printPos  = (it->position + (it+1)->position) / 2;
            printType = 'm';
          } else {
            printPos  = (it->position + (it+1)->position) / 2;
            printType = 'm';
          }
        }
        line    += tspan(it->start, it->position);
        lastLine = tspan((it+1)->start, (it+1)->position);
      }
      else {
        if (nOverlap == 0) {
          printPos  = it->position;
          printType = it->type;
        }
        nOverlap = 0;
      }
    }
    else if (it->type=='e' && (it+1)->type=='b') {
      if (nOverlap == 0) {
        printPos  = it->position;
        printType = it->type;
      }
      nOverlap = 0;
    }

    if (nOverlap == 0) {
      lastLine = tspan(it->start, it->position);
      string finalLine {line+lastLine};
      sort_merge(finalLine);

      // text->fontWeight = "bold";
      set_dominantBaseline(printType);
      text->label  = finalLine;
      text->origin = Point(CX, cy+get_point(printPos));
      type=="ref" ? text->print_pos_ref(fPlot) : text->print_pos_tar(fPlot);

      line.clear();
      lastLine.clear();

      //last
      if (it+2 == nodes.end()) {
        finalLine = tspan((it+1)->start, (it+1)->position);
        sort_merge(finalLine);
        printPos = (it+1)->position;

        text->dominantBaseline="text-after-edge";
        text->label  = finalLine;
        text->origin = Point(CX, cy+get_point(printPos));
        type=="ref" ? text->print_pos_ref(fPlot) : text->print_pos_tar(fPlot);
      }
    }
    
    if (it+2==nodes.end() && nOverlap!=0) {
      lastLine = tspan((it+1)->start, (it+1)->position);
      string finalLine {line+lastLine};
      sort_merge(finalLine);

      // text->fontWeight = "bold";
      set_dominantBaseline(printType);
      text->label  = finalLine;
      text->origin = Point(CX, cy+get_point(printPos));
      type=="ref" ? text->print_pos_ref(fPlot) : text->print_pos_tar(fPlot);
      break;
    }
  } // for
}