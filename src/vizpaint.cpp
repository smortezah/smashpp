#include "vizpaint.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "number.hpp"
using namespace smashpp;

// todo
void VizPaint::plot (VizParam& p) {
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
  // if (p.showAnnot)//todo
    Paint_Extra+=30;

  svg->width = maxSize + Paint_Extra;
  svg->height = 2*y + 2*chromHeight + innerSpace;
  print_header(fPlot);

  auto poly = make_unique<Polygon>();
  auto text = make_unique<Text>();
  auto line = make_unique<Line>();
  line->stroke_width = 2.0;
  auto rect = make_unique<Rectangle>();
  rect->fill_opacity = p.opacity;

  // Plot background page
  rect->fill = rect->stroke = backColor;
  rect->x = 0;
  rect->y = 0;
  rect->width  = svg->width;
  rect->height = svg->height;
  rect->plot(fPlot);

  // If min is set to default, reset to base max proportion
  if (p.min==0)    p.min=static_cast<u32>(maxSize / 100);

  // Read positions from file and Print them
  vector<Position> pos;
  read_pos(fPos, pos, p);

  make_posNode(pos, p, "ref");
  if (p.showPos)
    plot_pos_axes(fPlot, n_refBases, p.majorTick, p.minorTick);
    // print_pos(fPlot, p, pos, max(n_refBases,n_tarBases), "ref");

  make_posNode(pos, p, "tar");
  if (p.showPos)
    print_pos(fPlot, p, pos, max(n_refBases,n_tarBases), "tar");

  if (p.showPos)
    if (!plottable)  error("not plottable positions.");

  // Plot
  u64 n_regular=0, n_regularSolo=0, n_inverse=0, n_inverseSolo=0, n_ignore=0;
  std::sort(pos.begin(), pos.end(),
    [](const Position& l, const Position& r) { return l.begRef > r.begRef; });

  // for (auto e=pos.rbegin(); e!=pos.rend(); ++e) {
  for (auto e=pos.begin(); e!=pos.end(); ++e) {
    if (abs(e->endTar-e->begTar) <= p.min) {
      ++n_ignore;    continue;
    }
    else if (e->begRef!=DBLANK && e->endRef-e->begRef<=p.min) {
      ++n_ignore;    continue;
    }

    if (e->begRef == DBLANK)
      e->endTar > e->begTar ? ++n_regularSolo : ++n_inverseSolo;

    const auto make_gradient = 
      [&](const string& color, char c, const string& inId) {
      auto grad = make_unique<LinearGradient>();
      grad->id = "grad"+inId;
      grad->add_stop("30%", shade(color, 0.25));
      grad->add_stop("100%", color);
      // c=='r' ? grad->add_stop("30%", shade(color, 0.25)) 
      //        : grad->add_stop("30%", color);
      // c=='r' ? grad->add_stop("100%", color)
      //        : grad->add_stop("100%", shade(color, 0.25));
      grad->plot(fPlot);
      return "url(#"+grad->id+")";
    };

    const auto make_gradient_periph = 
      [&](const string& color, char c, const string& inId) {
      auto grad = make_unique<LinearGradient>();
      grad->id = "grad"+inId;
      grad->add_stop("30%", tone(color, 0.4));
      grad->add_stop("100%", color);
      // c=='r' ? grad->add_stop("30%", tone(color, 0.4)) 
      //        : grad->add_stop("30%", color);
      // c=='r' ? grad->add_stop("100%", color)
      //        : grad->add_stop("100%", tone(color, 0.4));
      grad->plot(fPlot);
      return "url(#"+grad->id+")";
    };

    const auto plot_main = [&](auto& cylinder) {
      cylinder->width = chromHeight;
      cylinder->stroke_width = 0.75;
      cylinder->fill_opacity = cylinder->stroke_opacity = p.opacity;
    };

    const auto plot_main_ref = [&]() {
      if (e->begRef != DBLANK) {
        auto cylinder = make_unique<Cylinder>();
        plot_main(cylinder);
        cylinder->height = get_point(e->endRef-e->begRef);
        cylinder->stroke = shade(rgb_color(e->start));
        cylinder->x = x + get_point(e->begRef);
        cylinder->y = y + chromHeight;
        cylinder->id = to_string(cylinder->x)+to_string(cylinder->y);
        cylinder->fill = make_gradient(rgb_color(e->start), 'r', cylinder->id);
        cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " + 
                              to_string(cylinder->y) + ")";
        cylinder->plot(fPlot);

        cylinder->stroke_width = 0.7;
        if (p.showNRC) {
          cylinder->id += "NRC";
          cylinder->fill = make_gradient_periph(
            nrc_color(e->entRef, p.colorMode), 'r', cylinder->id);
          cylinder->stroke = shade(nrc_color(e->entRef, p.colorMode), 0.96);
          cylinder->plot_periph(fPlot, 'r');
        }
        if (p.showRedun) {
          cylinder->id += "Redun";
          cylinder->fill = make_gradient_periph(
            redun_color(e->selfRef, p.colorMode), 'r', cylinder->id);
          cylinder->stroke = shade(redun_color(e->selfRef, p.colorMode), 0.95);
          cylinder->plot_periph(fPlot, 'r', u8(p.showNRC));
        }
      }
    };

    const auto plot_main_tar = [&](bool inverted) {
      auto cylinder = make_unique<Cylinder>();
      plot_main(cylinder);
      cylinder->height = get_point(abs(e->begTar-e->endTar));
      cylinder->x = !inverted ? x+get_point(e->begTar) : x+get_point(e->endTar);
      cylinder->y = y + 2*chromHeight + innerSpace;
      cylinder->id = to_string(cylinder->x) + to_string(cylinder->y);
      cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " + 
                            to_string(cylinder->y) + ")";
      if (e->begRef == DBLANK) {
        cylinder->fill = "black";
        cylinder->stroke = "white";
      } else {
        cylinder->fill = make_gradient(rgb_color(e->start), 't', cylinder->id);
        cylinder->stroke = shade(rgb_color(e->start));
      }

      if (!inverted) {
        cylinder->plot(fPlot);
      } else {
        if (e->begRef==DBLANK)  cylinder->plot_ir(fPlot, "#WavyWhite");
        else                    cylinder->plot_ir(fPlot);
      }

      cylinder->stroke_width = 0.7;
      if (p.showNRC) {
        cylinder->id += "NRC";
        cylinder->fill = make_gradient_periph(
          nrc_color(e->entTar, p.colorMode), 'r', cylinder->id);
        // cylinder->fill = nrc_color(e->entTar, p.colorMode);
        cylinder->stroke = shade(nrc_color(e->entTar, p.colorMode), 0.96);
        cylinder->plot_periph(fPlot, 't');
      }
      if (p.showRedun) {
        // cylinder->fill = redun_color(e->selfTar, p.colorMode);
        cylinder->id += "Redun";
        cylinder->fill = make_gradient_periph(
          redun_color(e->selfTar, p.colorMode), 'r', cylinder->id);
        cylinder->stroke = shade(redun_color(e->selfTar, p.colorMode), 0.95);
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
            poly->stroke = poly->fill = rgb_color(e->start);
            poly->stroke_opacity = poly->fill_opacity = 0.5 * p.opacity;
            poly->points 
              = poly->point(x+get_point(e->begRef), y+chromHeight) +
                poly->point(x+get_point(e->endRef), y+chromHeight) +
                poly->point(x+get_point(e->endTar), y+chromHeight+innerSpace) +
                poly->point(x+get_point(e->begTar), y+chromHeight+innerSpace);
            poly->plot(fPlot);
            break;
          case 2:
            line->stroke = rgb_color(e->start);
            line->x1 = x + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->begTar+(e->endTar-e->begTar)/2.0);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
            break;
          case 3:
            line->stroke = "black";
            line->x1 = x + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->begTar+(e->endTar-e->begTar)/2.0);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
            break;
          case 4:
            line->stroke = rgb_color(e->start);
            line->x1 = x + get_point(e->begRef);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->begTar);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
            line->x1 = x + get_point(e->endRef);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->endTar);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
            break;
          case 5:
            line->stroke = "black";
            line->x1 = x + get_point(e->begRef);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->begTar);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
            line->x1 = x + get_point(e->endRef);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->endTar);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
            break;
          case 6:
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
            poly->stroke = poly->fill = rgb_color(e->start);
            poly->stroke_opacity = poly->fill_opacity = 0.5 * p.opacity;
            poly->points
              = poly->point(x+get_point(e->begRef), y+chromHeight) +
                poly->point(x+get_point(e->endRef), y+chromHeight) +
                poly->point(x+get_point(e->endTar), y+chromHeight+innerSpace) +
                poly->point(x+get_point(e->begTar), y+chromHeight+innerSpace);
            poly->plot(fPlot);
            break;
          case 2:
            line->stroke = rgb_color(e->start);
            line->x1 = x + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->endTar+(e->begTar-e->endTar)/2.0);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
            break;
          case 3:
            line->stroke = "green";
            line->x1 = x + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->endTar+(e->begTar-e->endTar)/2.0);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
            break;
          case 4:
            line->stroke = rgb_color(e->start);
            line->x1 = x + get_point(e->begRef);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->begTar);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
  
            line->x1 = x + get_point(e->endRef);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->endTar);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
            break;
          case 5:
            line->stroke = "green";
            line->x1 = x + get_point(e->begRef);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->begTar);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
  
            line->x1 = x + get_point(e->endRef);
            line->y1 = y + chromHeight;
            line->x2 = x + get_point(e->endTar);
            line->y2 = y + chromHeight + innerSpace;
            line->plot(fPlot);
            break;
          case 6:
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
    rect->x = x + get_point(beg);
    rect->y = y;
    rect->width = get_point(end-beg+1);
    rect->plot(fPlot);
  }
  refFile.close();
  remove((file_name(ref)+"."+FMT_N).c_str());

  save_n_pos(tar);
  ifstream tarFile(file_name(tar)+"."+FMT_N);
  for (i64 beg,end; tarFile>>beg>>end;) {
    rect->fill = rect->stroke  = "grey";
    rect->x = x + get_point(beg);
    rect->y = y + chromHeight + innerSpace;
    rect->width = get_point(end-beg+1);
    rect->plot(fPlot);
  }
  tarFile.close();
  remove((file_name(tar)+"."+FMT_N).c_str());
  
  // Plot chromosomes
  auto cylinder = make_unique<Cylinder>();
  cylinder->width = chromHeight;
  cylinder->height = refSize;
  cylinder->stroke_width = 2;
  cylinder->x = x;
  cylinder->y = y + chromHeight;
  cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " + 
                        to_string(cylinder->y) + ")";
  cylinder->plot(fPlot);

  cylinder->height = tarSize;
  cylinder->x = x;
  cylinder->y = y + 2*chromHeight + innerSpace;
  cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " + 
                        to_string(cylinder->y) + ")";
  cylinder->plot(fPlot);

  // Plot title, legend and annotation
  plot_title(fPlot, ref, tar);
  plot_legend(fPlot, p, max(n_refBases,n_tarBases));
  // if (p.showAnnot)//todo
    plot_annot(fPlot, max(n_refBases,n_tarBases), p.showNRC, p.showRedun);

  print_tailer(fPlot);

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
  ratio = static_cast<u32>(max(refSize_,tarSize_) / PAINT_SCALE);
  chromHeight = width_;
  innerSpace = space_;
  mult = mult_;
  refSize = get_point(refSize_);
  tarSize = get_point(tarSize_);
  maxSize = max(refSize, tarSize);
}

string VizPaint::rgb_color (u32 start) const {
  const auto hue = static_cast<u8>(start * mult);
  HSV hsv (hue);
  RGB rgb {to_rgb(hsv)};
  return to_hex(rgb);
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

inline void VizPaint::print_header (ofstream& f) const {
  svg->print_header(f);

  // Patterns
  auto pattern = make_unique<Pattern>();
  pattern->id = "Wavy";
  pattern->patternUnits = "userSpaceOnUse";
  pattern->x = chromHeight/2 - 2;
  pattern->y = y + chromHeight + innerSpace;
  pattern->width = chromHeight;
  pattern->height = 7;

  auto path = make_unique<Path>();
  path->fill = "transparent";
  path->stroke_width = 0.75;
  path->d = path->m(0, 0) + 
            path->q(pattern->width/2, 1.5*ry, pattern->width, 0);
  path->stroke = "black";
  make_pattern(f, pattern, path);

  pattern->id = "WavyWhite";
  path->stroke = "white";
  make_pattern(f, pattern, path);
}

inline void VizPaint::print_tailer (ofstream& f) const {
  svg->print_tailer(f);
}

template <typename Value>
inline double VizPaint::get_point (Value p) const {
  return 5 * p / static_cast<double>(ratio);
}

inline void VizPaint::plot_title (ofstream& f, const string& ref, 
const string& tar) const {
  auto text = make_unique<Text>();
  text->font_weight = "bold";
  text->font_size = 10;
  text->text_anchor = "start";
  text->x = x;

  text->y = y - TITLE_SPACE;
  text->dominant_baseline = "text-before-edge";
  text->Label = ref;
  text->plot(f);

  text->y = y + 2*chromHeight + innerSpace + TITLE_SPACE;
  text->dominant_baseline = "text-after-edge";
  text->Label = tar;
  text->plot(f);
}

inline void VizPaint::plot_legend (ofstream& f, const VizParam& p, 
i64 maxWidth) {
  if (!p.showNRC && !p.showRedun)  return;
  
  const auto horiz = x + get_point(maxWidth) + 40;
  auto rect = make_shared<Rectangle>();
  rect->width = 15;

  auto text = make_unique<Text>();
  text->text_anchor = "middle";
  // text->font_weight = "bold";
  text->font_size = 9;

  if (p.showNRC && !p.showRedun) {
    rect->x = horiz;
    rect->y = y - (TITLE_SPACE + VERT_TUNE);
    rect->height = 2*(TITLE_SPACE + VERT_TUNE) + 2*chromHeight + innerSpace;

    text->x = rect->x - 8;
    text->y = rect->y + rect->height/2;
    text->transform = "rotate(90 " + to_string(text->x) + " " +
                      to_string(text->y) + ")";
    text->Label = "Relative Redundancy";
    // text->plot_shadow(f);
    text->plot(f);

    text->transform.clear();
  }
  else if (!p.showNRC && p.showRedun) {
    rect->x = horiz;
    rect->y = y - (TITLE_SPACE + VERT_TUNE);
    rect->height = 2*(TITLE_SPACE + VERT_TUNE) + 2*chromHeight + innerSpace;

    text->x = rect->x - 8;
    text->y = rect->y + rect->height/2;
    text->transform = "rotate(90 " + to_string(text->x) + " " +
                      to_string(text->y) + ")";
    text->Label = "Redundancy";
    // text->plot_shadow(f);
    text->plot(f);

    text->transform.clear();
  }
  else if (p.showNRC && p.showRedun) {
    rect->x = horiz;
    rect->y = y - (TITLE_SPACE + VERT_TUNE);
    rect->height = 2*(TITLE_SPACE + VERT_TUNE) + 2*chromHeight + innerSpace;

    text->x = rect->x - 8;
    text->y = rect->y + rect->height/2;
    text->transform = "rotate(90 " + to_string(text->x) + " " +
                      to_string(text->y) + ")";
    text->Label = "Relative Redundancy";
    // text->plot_shadow(f);
    text->plot(f);

    // Redundancy
    text->x = rect->x + rect->width + 8;
    text->y = rect->y + rect->height/2;
    text->transform = "rotate(90 " + to_string(text->x) + " " +
                      to_string(text->y) + ")";
    text->Label = "Redundancy";
    // text->plot_shadow(f);
    text->plot(f);

    text->transform.clear();
  }

  plot_legend_gradient(f, rect, p.colorMode);

  // Print numbers (measures)
  text->font_weight = "normal";
  text->text_align = "middle";
  text->dominant_baseline = "middle";
  text->font_size = 8;
  text->x = rect->x + rect->width/2;
  text->y = rect->y + rect->height + 6;
  text->Label = "0.0";
  text->plot(f);
  for (u8 i=1; i!=4; ++i) {
    text->x = rect->x + rect->width/2;
    text->y = rect->y + rect->height - rect->height*i/4;
    if (p.colorMode==1 && i==3)  text->fill="white";
    text->Label = string_format("%.1f", i*0.5);
    text->plot(f);
  }
  text->x = rect->x + rect->width/2;
  text->y = rect->y + -6;
  text->fill = "black";
  text->Label = "2.0";
  text->plot(f);
}

template <typename Rect>
inline void VizPaint::plot_legend_gradient (ofstream& f, const Rect& rect, 
u8 colorMode) {
  vector<string> colorset;
  switch (colorMode) {
  case 0:   colorset = COLORSET[0];  break;
  case 1:   colorset = COLORSET[1];  break;
  case 2:   colorset = COLORSET[2];  break;
  default:  error("undefined color mode.");
  }
  auto id = to_string(rect->x) + to_string(rect->y);

  auto grad = make_unique<LinearGradient>();
  grad->id = "grad"+id;
  grad->x1 = "0%";
  grad->y1 = "100%";
  grad->x2 = grad->y2 = "0%";
  for (u8 i=0; i!=colorset.size(); ++i)
    grad->add_stop(to_string(i*100/(colorset.size()-1))+"%", colorset[i]);
  grad->plot(f);

  rect->stroke = "black";
  rect->stroke_width = 0.5;
  rect->rx = rect->ry = 2;
  rect->fill = "url(#grad"+id+")";
  rect->plot(f);

  // auto cylinder = make_unique<Cylinder>();
  // cylinder->width = rect->height;
  // cylinder->height = rect->width;
  // cylinder->x = rect->x;
  // cylinder->y = rect->y + rect->height;
  // cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " +
  //   to_string(cylinder->y) + ")";
  // cylinder->fill = "url(#grad" + id + ")";
  // cylinder->ry = 0;
  // cylinder->stroke_width = 0.35;
  // cylinder->stroke = "black";
  // cylinder->plot(f);
}

inline void VizPaint::plot_annot (ofstream& f, i64 maxHeight, bool showNRC,
bool showRedun) const {
  if (!showNRC && !showRedun)  return;

  float H=32.0f, YTopRelRedun=0, YBottomRelRedun=0, YTopRedun=0, YBottomRedun=0,
        XRelRedun=0, XRedun=0;

  auto path = make_unique<Path>();
  path->stroke = "black";
  path->stroke_width = 0.5;
  // path->stroke_dasharray = "8 3";
  
  auto text = make_unique<Text>();
  text->font_size = 9;

  if (showNRC && !showRedun) {
    XRelRedun = x + get_point(maxHeight) + H;
    YTopRelRedun = y - (TITLE_SPACE + VERT_TUNE + 0.5*chromHeight/VERT_RATIO);
    YBottomRelRedun = y + 2*chromHeight + innerSpace + TITLE_SPACE + VERT_TUNE +
                      0.5*chromHeight/VERT_RATIO;
    // Top wing
    if (lastPos.size() == 2) {
      path->d = path->M(x+get_point(lastPos[0]), YTopRelRedun) + 
                path->H(XRelRedun) + 
                path->V((YTopRelRedun+YBottomRelRedun)/2 - 47);
      // path->plot_shadow(f);
      path->plot(f);
    }
    // Bottom wing
    path->d = path->M(x+get_point(lastPos[1]), YBottomRelRedun) +
              path->H(XRelRedun) + 
              path->V((YTopRelRedun+YBottomRelRedun)/2 + 47);
    // path->plot_shadow(f);
    path->plot(f);
  }
  else if (!showNRC && showRedun) {
    XRedun = x + get_point(maxHeight) + H;
    YTopRedun = y - (TITLE_SPACE + VERT_TUNE + 0.5*chromHeight/VERT_RATIO);
    YBottomRedun = y + 2*chromHeight + innerSpace + TITLE_SPACE + VERT_TUNE +
                   0.5*chromHeight/VERT_RATIO;
    // Top wing
    if (lastPos.size() == 2) {
      path->d = path->M(x+get_point(lastPos[0]), YTopRedun) + 
                path->H(XRedun) + 
                path->V((YTopRedun+YBottomRedun)/2 - 32);
      // path->plot_shadow(f);
      path->plot(f);
    }
    // Bottom wing
    path->d = path->M(x+get_point(lastPos[1]), YBottomRedun) +
              path->H(XRedun) + 
              path->V((YTopRedun+YBottomRedun)/2 + 32);
    // path->plot_shadow(f);
    path->plot(f);
  }
  else if (showNRC && showRedun) {
    // Relative Redundancy
    XRelRedun = x + get_point(maxHeight) + H;
    YTopRelRedun = y - (TITLE_SPACE + VERT_TUNE + 0.5*chromHeight/VERT_RATIO);
    YBottomRelRedun = y + 2*chromHeight + innerSpace + TITLE_SPACE + VERT_TUNE +
                      0.5*chromHeight/VERT_RATIO;
    // Top wing
    if (lastPos.size() == 2) {
      path->d = path->M(x+get_point(lastPos[0]), YTopRelRedun) + 
                path->H(XRelRedun) + 
                path->V((YTopRelRedun+YBottomRelRedun)/2 - 47);
      // path->plot_shadow(f);
      path->plot(f);
    }
    // Bottom wing
    path->d = path->M(x+get_point(lastPos[1]), YBottomRelRedun) +
              path->H(XRelRedun) + 
              path->V((YTopRelRedun+YBottomRelRedun)/2 + 47);
    // path->plot_shadow(f);
    path->plot(f);

    // Redundancy
    XRedun = x + get_point(maxHeight) + H + 31;
    YTopRedun = y - (TITLE_SPACE + 2*VERT_TUNE + 1.5*chromHeight/VERT_RATIO);
    YBottomRedun = y + 2*chromHeight + innerSpace + TITLE_SPACE +
                   2*VERT_TUNE + 1.5*chromHeight/VERT_RATIO;
    // Top wing
    if (lastPos.size() == 2) {
      path->d = path->M(x+get_point(lastPos[0]), YTopRedun) + 
                path->H(XRedun) + 
                path->V((YTopRedun+YBottomRedun)/2 - 32);
      // path->plot_shadow(f);
      path->plot(f);
    }
    // Bottom wing
    path->d = path->M(x+get_point(lastPos[1]), YBottomRedun) +
              path->H(XRedun) + 
              path->V((YTopRedun+YBottomRedun)/2 + 32);
    // path->plot_shadow(f);
    path->plot(f);
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
  double shiftY    = 4;
  if (par.showNRC && par.showRedun)
    shiftY += 2 * (VERT_TUNE + chromHeight/VERT_RATIO);
  else if (par.showNRC ^ par.showRedun)
    shiftY += VERT_TUNE + chromHeight/VERT_RATIO;
  double CY=y;
  type=="ref" ? CY-=shiftY : CY+=chromHeight+innerSpace+chromHeight+shiftY;

  const auto set_dominantBaseline = [&](char type) {
    switch (type) {
    case 'b':  text->dominant_baseline="text-before-edge";  break;
    case 'm':  text->dominant_baseline="middle";            break;
    case 'e':  text->dominant_baseline="text-after-edge";   break;
    default:   text->dominant_baseline="middle";            break;
    }
  };

  const auto print_pos_ref = 
    [&](ofstream& f, unique_ptr<Text>& text, char c='\0') {
    text->text_anchor = "end";
    text->y -= 5;
    switch (c) {
      case 'b':  text->dominant_baseline = "hanging";   break;  // begin
      case 'm':  text->dominant_baseline = "middle";    break;  // middle
      case 'e':  text->dominant_baseline = "baseline";  break;  // end
      default:                                          break;
    }
    text->font_size = 9;
    text->plot(f);
  };

  const auto print_pos_tar =
    [&](ofstream& f, unique_ptr<Text>& text, char c='\0') {
    text->text_anchor = "start";
    text->y += 5;
    switch (c) {
      case 'b':  text->dominant_baseline = "hanging";   break;  // begin
      case 'm':  text->dominant_baseline = "middle";    break;  // middle
      case 'e':  text->dominant_baseline = "baseline";  break;  // end
      default:                                          break;
    }
    text->font_size = 9;
    text->plot(f);
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

      // text->font_weight = "bold";
      set_dominantBaseline(printType);
      text->Label  = finalLine;
      text->x = x + get_point(printPos);
      text->y = CY;
      type=="ref" ? print_pos_ref(fPlot, text) : print_pos_tar(fPlot, text);

      line.clear();
      lastLine.clear();

      // Last
      if (it+2 == nodes.end()) {
        finalLine = tspan((it+1)->start, (it+1)->position);
        sort_merge(finalLine);
        printPos = (it+1)->position;

        text->dominant_baseline="text-after-edge";
        text->Label  = finalLine;
        text->x = x + get_point(printPos);
        text->y = CY;
        type=="ref" ? print_pos_ref(fPlot, text) : print_pos_tar(fPlot, text);
      }
    }
    
    if (it+2==nodes.end() && nOverlap!=0) {
      lastLine = tspan((it+1)->start, (it+1)->position);
      string finalLine {line+lastLine};
      sort_merge(finalLine);

      // text->font_weight = "bold";
      set_dominantBaseline(printType);
      text->Label  = finalLine;
      text->x = x + get_point(printPos);
      text->y = CY;
      type=="ref" ? print_pos_ref(fPlot, text) : print_pos_tar(fPlot, text);
      break;
    }
  } // for
}

inline void VizPaint::plot_pos_axes (ofstream& f, u64 n_bases, u8 n_majorTicks,
u8 n_minorTicks) {
  const auto  maxPos           = get_point(n_bases);
  const u8    n_ranges         = n_majorTicks - 1, 
              n_subranges      = n_minorTicks + 1;
  const u16   minorTickSize    = 7,
              majorTickSize    = 2*minorTickSize,
              tickLabelSkip    = 8,
              vertSkip         = 13;
  const float minorStrokeWidth = 0.8f,
              majorStrokeWidth = 2*minorStrokeWidth;

  auto line = make_unique<Line>();
  line->stroke_width = majorStrokeWidth;
  auto text = make_unique<Text>();
  text->font_size = 9;

  // line->x1 = x;
  // line->x2 = x + maxPos;
  line->y1 = line->y2 
           = y - TITLE_SPACE - 2*(VERT_TUNE+chromHeight/VERT_RATIO) - vertSkip;
  // line->plot(f);

  // Ticks
  line->y1 -= line->stroke_width/2;
  u16 count = 0;
  for (u64 pos=0; pos < 1 + n_bases - n_bases/(n_ranges*n_subranges); 
       pos += n_bases/(n_ranges*n_subranges)) {
    line->x1 = line->x2 = x + get_point(pos) + line->stroke_width/2;

    if (count++ % n_subranges == 0) {  // Major ticks
      line->stroke_width = majorStrokeWidth;
      line->y2 = line->y1 - majorTickSize;
      line->plot(f);

      text->x = line->x1;
      text->y = line->y2 - tickLabelSkip;
      text->Label = human_readable(pos);
      text->plot(f);
    }
    else {  // Minor ticks
      line->stroke_width = minorStrokeWidth;
      line->y2 = line->y1 - minorTickSize;
      line->plot(f);
    }
  }

  // Last tick (major)
  line->stroke_width = majorStrokeWidth;
  line->x1 = line->x2 = x + maxPos - line->stroke_width/2;
  line->y2 = line->y1 - majorTickSize;
  line->plot(f);

  text->x = line->x1;
  text->y = line->y2 - tickLabelSkip;
  text->Label = human_readable(n_bases);
  text->plot(f);
}