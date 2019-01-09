#include "vizpaint.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "number.hpp"
using namespace smashpp;

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

  if (p.vertical) {
    const auto max_n_digits = 
      max(num_digits(n_refBases), num_digits(n_tarBases));
    x = 100.0f;
    if (max_n_digits > 4)   x += 17.0f;
    if (max_n_digits > 7)   x += 17.0f;
    if (max_n_digits > 10)  x += 17.0f;
    y = 30.0f;
    svg->width = 2*x + 2*seqWidth + innerSpace;
    svg->height = maxSize + 105;
  } else {
    x = 20.0f;
    y = 100.0f;
    svg->width = maxSize + 105;
    svg->height = 2*y + 2*seqWidth + innerSpace;
  }
  svg->print_header(fPlot);

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
  rect->width = svg->width;
  rect->height = svg->height;
  rect->plot(fPlot);

  // If min is set to default, reset to base max proportion
  if (p.min==0)  p.min=static_cast<u32>(maxSize / 100);

  // Read positions from file and Print them
  vector<Position> pos;
  read_pos(fPos, pos, p);

  make_posNode(pos, p, "ref");
  plot_pos(fPlot, p, n_refBases, true);
  // print_pos(fPlot, p, pos, max(n_refBases,n_tarBases), "ref");

  make_posNode(pos, p, "tar");
  plot_pos(fPlot, p, n_tarBases, false);
  // print_pos(fPlot, p, pos, max(n_refBases,n_tarBases), "tar");

  if (!plottable)  error("not plottable positions.");

  // Plot
  u64 n_regular=0, n_regularSolo=0, n_inverse=0, n_inverseSolo=0, n_ignore=0;
  std::sort(pos.begin(), pos.end(),
    [](const Position& l, const Position& r) { return l.begRef > r.begRef; });

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
      cylinder->width = seqWidth;
      cylinder->stroke_width = 0.75;
      cylinder->fill_opacity = cylinder->stroke_opacity = p.opacity;
    };

    const auto plot_main_ref = [&]() {
      if (e->begRef != DBLANK) {
        auto cylinder = make_unique<Cylinder>();
        plot_main(cylinder);
        cylinder->height = get_point(e->endRef-e->begRef);
        cylinder->stroke = shade(rgb_color(e->start));
        if (p.vertical) {
          cylinder->x = x;
          cylinder->y = y + get_point(e->begRef);
        } else {
          cylinder->x = x + get_point(e->begRef);
          cylinder->y = y + seqWidth;
          cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " + 
                                to_string(cylinder->y) + ")";
        }
        cylinder->id = to_string(cylinder->x)+to_string(cylinder->y);
        cylinder->fill = make_gradient(rgb_color(e->start), 'r', cylinder->id);
        cylinder->plot(fPlot);

        cylinder->stroke_width = 0.7;
        if (p.showNRC) {
          cylinder->id += "NRC";
          cylinder->fill = make_gradient_periph(
            nrc_color(e->entRef, p.colorMode), 'r', cylinder->id);
          cylinder->stroke = shade(nrc_color(e->entRef, p.colorMode), 0.96);
          cylinder->plot_periph(fPlot, p.vertical, 'r');
        }
        if (p.showRedun) {
          cylinder->id += "Redun";
          cylinder->fill = make_gradient_periph(
            redun_color(e->selfRef, p.colorMode), 'r', cylinder->id);
          cylinder->stroke = shade(redun_color(e->selfRef, p.colorMode), 0.95);
          cylinder->plot_periph(fPlot, p.vertical, 'r', u8(p.showNRC));
        }
      }
    };

    const auto plot_main_tar = [&](bool inverted) {
      auto cylinder = make_unique<Cylinder>();
      plot_main(cylinder);
      cylinder->height = get_point(abs(e->begTar-e->endTar));
      if (p.vertical) {
        cylinder->x = x + seqWidth + innerSpace;
        cylinder->y = !inverted ? y+get_point(e->begTar) 
                                : y+get_point(e->endTar);
      } else {
        cylinder->x = !inverted ? x+get_point(e->begTar) 
                                : x+get_point(e->endTar);
        cylinder->y = y + 2*seqWidth + innerSpace;
        cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " + 
                              to_string(cylinder->y) + ")";
      }
      cylinder->id = to_string(cylinder->x) + to_string(cylinder->y);
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
        cylinder->stroke = shade(nrc_color(e->entTar, p.colorMode), 0.96);
        cylinder->plot_periph(fPlot, p.vertical, 't');
      }
      if (p.showRedun) {
        cylinder->id += "Redun";
        cylinder->fill = make_gradient_periph(
          redun_color(e->selfTar, p.colorMode), 'r', cylinder->id);
        cylinder->stroke = shade(redun_color(e->selfTar, p.colorMode), 0.95);
        cylinder->plot_periph(fPlot, p.vertical, 't', u8(p.showNRC));
      }
    };

    if (e->endTar > e->begTar) {
      if (p.regular) {
        plot_main_ref();
        plot_main_tar(false);

        if (e->begRef != DBLANK) {
          switch (p.link) {
          case 1:
            if (p.vertical)
              poly->points 
                = poly->point(x+seqWidth,            y+get_point(e->begRef)) +
                  poly->point(x+seqWidth,            y+get_point(e->endRef)) +
                  poly->point(x+seqWidth+innerSpace, y+get_point(e->endTar)) +
                  poly->point(x+seqWidth+innerSpace, y+get_point(e->begTar));
            else
              poly->points 
                = poly->point(x+get_point(e->begRef), y+seqWidth) +
                  poly->point(x+get_point(e->endRef), y+seqWidth) +
                  poly->point(x+get_point(e->endTar), y+seqWidth+innerSpace) +
                  poly->point(x+get_point(e->begTar), y+seqWidth+innerSpace);
            poly->stroke = poly->fill = rgb_color(e->start);
            poly->stroke_opacity = poly->fill_opacity = 0.5 * p.opacity;
            poly->plot(fPlot);
            break;
          case 2:
            if (p.vertical) {
              line->x1 = x + seqWidth;
              line->y1 = y + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
              line->x2 = x + seqWidth + innerSpace;
              line->y2 = y + get_point(e->begTar+(e->endTar-e->begTar)/2.0);
            } else {
              line->x1 = x + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->begTar+(e->endTar-e->begTar)/2.0);
              line->y2 = y + seqWidth + innerSpace;
            }
            line->stroke = rgb_color(e->start);
            line->plot(fPlot);
            break;
          case 3:
            if (p.vertical) {
              line->x1 = x + seqWidth;
              line->y1 = y + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
              line->x2 = x + seqWidth + innerSpace;
              line->y2 = y + get_point(e->begTar+(e->endTar-e->begTar)/2.0);
            } else {
              line->x1 = x + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->begTar+(e->endTar-e->begTar)/2.0);
              line->y2 = y + seqWidth + innerSpace;
            }
            line->stroke = "black";
            line->plot(fPlot);
            break;
          case 4:
            line->stroke = rgb_color(e->start);
            if (p.vertical) {
              line->x1 = x + seqWidth;
              line->y1 = y + get_point(e->begRef);
              line->x2 = x + seqWidth + innerSpace;
              line->y2 = y + get_point(e->begTar);
              line->plot(fPlot);
              line->x1 = x + seqWidth;
              line->y1 = y + get_point(e->endRef);
              line->x2 = x + seqWidth + innerSpace;
              line->y2 = y + get_point(e->endTar);
              line->plot(fPlot);
            } else {
              line->x1 = x + get_point(e->begRef);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->begTar);
              line->y2 = y + seqWidth + innerSpace;
              line->plot(fPlot);
              line->x1 = x + get_point(e->endRef);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->endTar);
              line->y2 = y + seqWidth + innerSpace;
              line->plot(fPlot);
            }
            break;
          case 5:
            line->stroke = "black";
            if (p.vertical) {
              line->x1 = x + seqWidth;
              line->y1 = y + get_point(e->begRef);
              line->x2 = x + seqWidth + innerSpace;
              line->y2 = y + get_point(e->begTar);
              line->plot(fPlot);
              line->x1 = x + seqWidth;
              line->y1 = y + get_point(e->endRef);
              line->x2 = x + seqWidth + innerSpace;
              line->y2 = y + get_point(e->endTar);
              line->plot(fPlot);
            } else {
              line->x1 = x + get_point(e->begRef);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->begTar);
              line->y2 = y + seqWidth + innerSpace;
              line->plot(fPlot);
              line->x1 = x + get_point(e->endRef);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->endTar);
              line->y2 = y + seqWidth + innerSpace;
              line->plot(fPlot);
            }
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
            if (p.vertical)
              poly->points
                = poly->point(x+seqWidth,            y+get_point(e->begRef)) +
                  poly->point(x+seqWidth,            y+get_point(e->endRef)) +
                  poly->point(x+seqWidth+innerSpace, y+get_point(e->endTar)) +
                  poly->point(x+seqWidth+innerSpace, y+get_point(e->begTar));
            else
              poly->points
                = poly->point(x+get_point(e->begRef), y+seqWidth) +
                  poly->point(x+get_point(e->endRef), y+seqWidth) +
                  poly->point(x+get_point(e->endTar), y+seqWidth+innerSpace) +
                  poly->point(x+get_point(e->begTar), y+seqWidth+innerSpace);
            poly->stroke = poly->fill = rgb_color(e->start);
            poly->stroke_opacity = poly->fill_opacity = 0.5 * p.opacity;
            poly->plot(fPlot);
            break;
          case 2:
            if (p.vertical) {
              line->x1 = x + seqWidth;
              line->y1 = y + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
              line->x2 = x + seqWidth + innerSpace;
              line->y2 = y + get_point(e->endTar+(e->begTar-e->endTar)/2.0);
            } else {
              line->x1 = x + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->endTar+(e->begTar-e->endTar)/2.0);
              line->y2 = y + seqWidth + innerSpace;
            }
            line->stroke = rgb_color(e->start);
            line->plot(fPlot);
            break;
          case 3:
            if (p.vertical) {
              line->x1 = x + seqWidth;
              line->y1 = y + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
              line->x2 = x + seqWidth + innerSpace;
              line->y2 = y + get_point(e->endTar+(e->begTar-e->endTar)/2.0);
            } else {
              line->x1 = x + get_point(e->begRef+(e->endRef-e->begRef)/2.0);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->endTar+(e->begTar-e->endTar)/2.0);
              line->y2 = y + seqWidth + innerSpace;
            }
            line->stroke = "green";
            line->plot(fPlot);
            break;
          case 4:
            line->stroke = rgb_color(e->start);
            if (p.vertical) {
              line->x1 = x + seqWidth;
              line->y1 = y + get_point(e->begRef);
              line->x2 = x + seqWidth + innerSpace;
              line->y2 = y + get_point(e->begTar);
              line->plot(fPlot);

              line->x1 = x + seqWidth;
              line->y1 = y + get_point(e->endRef);
              line->x2 = x + seqWidth + innerSpace;
              line->y2 = y + get_point(e->endTar);
              line->plot(fPlot);
            } else {
              line->x1 = x + get_point(e->begRef);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->begTar);
              line->y2 = y + seqWidth + innerSpace;
              line->plot(fPlot);

              line->x1 = x + get_point(e->endRef);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->endTar);
              line->y2 = y + seqWidth + innerSpace;
              line->plot(fPlot);
            }
            break;
          case 5:
            line->stroke = "green";
            if (p.vertical) {
              line->y1 = y + get_point(e->begRef);
              line->x1 = x + seqWidth;
              line->y2 = y + get_point(e->begTar);
              line->x2 = x + seqWidth + innerSpace;
              line->plot(fPlot);

              line->y1 = y + get_point(e->endRef);
              line->x1 = x + seqWidth;
              line->y2 = y + get_point(e->endTar);
              line->x2 = x + seqWidth + innerSpace;
              line->plot(fPlot);
            } else {
              line->x1 = x + get_point(e->begRef);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->begTar);
              line->y2 = y + seqWidth + innerSpace;
              line->plot(fPlot);

              line->x1 = x + get_point(e->endRef);
              line->y1 = y + seqWidth;
              line->x2 = x + get_point(e->endTar);
              line->y2 = y + seqWidth + innerSpace;
              line->plot(fPlot);
            }
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
    auto cylinder = make_unique<Cylinder>();
    cylinder->stroke_width = 0.75;
    cylinder->fill = cylinder->stroke = "grey";
    cylinder->fill_opacity = cylinder->stroke_opacity = p.opacity;
    cylinder->width = seqWidth;
    cylinder->height = get_point(end-beg+1);
    if (p.vertical) {
      cylinder->x = x;
      cylinder->y = y + get_point(beg);
    } else {
      cylinder->x = x + get_point(beg);
      cylinder->y = y + seqWidth;
      cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " + 
                            to_string(cylinder->y) + ")";
    }
    cylinder->id = to_string(cylinder->x)+to_string(cylinder->y);
    cylinder->plot(fPlot);
  }
  refFile.close();
  remove((file_name(ref)+"."+FMT_N).c_str());

  save_n_pos(tar);
  ifstream tarFile(file_name(tar)+"."+FMT_N);
  for (i64 beg,end; tarFile>>beg>>end;) {
    auto cylinder = make_unique<Cylinder>();
    cylinder->stroke_width = 0.75;
    cylinder->fill = cylinder->stroke = "grey";
    cylinder->fill_opacity = cylinder->stroke_opacity = p.opacity;
    cylinder->width = seqWidth;
    cylinder->height = get_point(end-beg+1);
    if (p.vertical) {
      cylinder->x = x + seqWidth + innerSpace;
      cylinder->y = y + get_point(beg);
    } else {
      cylinder->x = x + get_point(beg);
      cylinder->y = y + 2*seqWidth + innerSpace;
      cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " + 
                            to_string(cylinder->y) + ")";
    }
    cylinder->id = to_string(cylinder->x)+to_string(cylinder->y);
    cylinder->plot(fPlot);
  }
  tarFile.close();
  remove((file_name(tar)+"."+FMT_N).c_str());
  
  // Plot chromosomes
  auto cylinder = make_unique<Cylinder>();
  cylinder->width = seqWidth;
  cylinder->height = refSize;
  cylinder->stroke_width = 1.25;
  if (p.vertical) {
    cylinder->x = x;
    cylinder->y = y;
  } else {
    cylinder->x = x;
    cylinder->y = y + seqWidth;
    cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " + 
                          to_string(cylinder->y) + ")";
  }
  cylinder->plot(fPlot);

  cylinder->height = tarSize;
  if (p.vertical) {
    cylinder->x = x + seqWidth + innerSpace;
    cylinder->y = y;
  } else {
    cylinder->x = x;
    cylinder->y = y + 2*seqWidth + innerSpace;
    cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " + 
                          to_string(cylinder->y) + ")";
  }
  cylinder->plot(fPlot);

  // Plot title, legend and annotation
  plot_title(fPlot, ref, tar, p.vertical);
  plot_legend(fPlot, p, max(n_refBases,n_tarBases));

  svg->print_tailer(fPlot);

  // Log
  cerr << "Plotting finished.\n";
  cerr << "Found ";
  u8 n_pluses = 0;
  if (p.regular)         ++n_pluses;
  if (n_regularSolo!=0)  ++n_pluses;
  if (p.inverse)         ++n_pluses;
  if (n_inverseSolo!=0)  ++n_pluses;
  --n_pluses;
  
  if (p.regular)         cerr << n_regular     << " regular";
  if (n_pluses!=0) {     cerr << " + ";  --n_pluses; }
  if (n_regularSolo!=0)  cerr << n_regularSolo << " solo regular";
  if (n_pluses!=0) {     cerr << " + ";  --n_pluses; }
  if (p.inverse)         cerr << n_inverse     << " inverted";
  if (n_pluses!=0) {     cerr << " + ";  --n_pluses; }
  if (n_inverseSolo!=0)  cerr << n_inverseSolo << " solo inverted";

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
  seqWidth = width_;
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

template <typename Value>
inline double VizPaint::get_point (Value index) const {
  return 5.0 * index / ratio;
}

inline u64 VizPaint::get_index (double point) const {
  return static_cast<u64>(point * ratio / 5.0);
}

inline void VizPaint::plot_title (ofstream& f, const string& ref, 
const string& tar, bool vertical) const {
  auto text = make_unique<Text>();
  text->font_weight = "bold";
  text->font_size = 10;

  if (vertical) {
    text->text_anchor = "middle";
    text->dominant_baseline = "middle";
    text->y = y - 0.75*TITLE_SPACE;

    text->x = x + seqWidth/2;
    if (innerSpace - (abs(ref.size()*1-seqWidth)/2 + abs(tar.size()*1-seqWidth)/2) < 15)
      // text->x -= 10;
      cerr<<"too close";

    text->Label = ref;
    text->plot(f);

    text->x = x + 1.5*seqWidth + innerSpace;
    text->Label = tar;
    text->plot(f);
  }
  else {
    text->text_anchor = "start";
    text->x = x;

    text->y = y - TITLE_SPACE;
    text->dominant_baseline = "text-before-edge";
    text->Label = ref;
    text->plot(f);

    text->y = y + 2*seqWidth + innerSpace + TITLE_SPACE;
    text->dominant_baseline = "text-after-edge";
    text->Label = tar;
    text->plot(f);
  }
}

inline void VizPaint::plot_legend (ofstream& f, const VizParam& p, 
i64 maxWidth) {
  if (!p.showNRC && !p.showRedun)  return;
  
  const auto originFixed = p.vertical ? y + get_point(maxWidth) + 40
                                      : x + get_point(maxWidth) + 40;
  const auto label_shift = 10;
  auto rect = make_shared<Rectangle>();

  auto text = make_unique<Text>();
  text->text_anchor = "middle";
  // text->font_weight = "bold";
  text->font_size = 9;

  auto path = make_unique<Path>();
  path->stroke = "black";
  path->stroke_width = 0.5;
  // path->stroke_dasharray = "8 3";

  float X1RelRedun=0.0f, X2RelRedun=0.0f, X1Redun=0.0f, X2Redun=0.0f,
        Y1RelRedun=0.0f, Y2RelRedun=0.0f, Y1Redun=0.0f, Y2Redun=0.0f;

  if (p.showNRC && !p.showRedun) {
    if (p.vertical) {
      rect->x = x - TITLE_SPACE/2;
      rect->y = originFixed;
      rect->width = TITLE_SPACE + 2*seqWidth + innerSpace;
      rect->height = 11;

      text->x = rect->x + rect->width/2;
      if (rect->width > 64) {
        text->y = rect->y - label_shift;
        text->Label = "Relative Redundancy";
        // text->plot_shadow(f);
        text->plot(f);
      } else {
        text->y = rect->y - label_shift - 8;
        text->Label = "Relative";
        // text->plot_shadow(f);
        text->plot(f);
        text->y = rect->y - label_shift + 2;
        text->Label = "Redundancy";
        // text->plot_shadow(f);
        text->plot(f);
      }

      X1RelRedun = x - (TITLE_SPACE/2 + VERT_TUNE + 0.5*seqWidth/VERT_RATIO);
      X2RelRedun = x + 2*seqWidth + innerSpace + TITLE_SPACE/2 + 
                   VERT_TUNE + 0.5*seqWidth/VERT_RATIO;
    } 
    else {
      rect->x = originFixed;
      rect->y = y - (TITLE_SPACE + VERT_TUNE);
      rect->width = 15;
      rect->height = 2*(TITLE_SPACE + VERT_TUNE) + 2*seqWidth + innerSpace;

      text->x = rect->x - label_shift;
      text->y = rect->y + rect->height/2;
      text->transform = "rotate(90 " + to_string(text->x) + " " +
                        to_string(text->y) + ")";
      text->Label = "Relative Redundancy";
      // text->plot_shadow(f);
      text->plot(f);

      Y1RelRedun = y - (TITLE_SPACE + VERT_TUNE + 0.5*seqWidth/VERT_RATIO);
      Y2RelRedun = y + 2*seqWidth + innerSpace + TITLE_SPACE + 
                        VERT_TUNE + 0.5*seqWidth/VERT_RATIO;
    }
    
    // Top/Left wing
    if (lastPos.size() == 2) {
      if (p.vertical) {
        if (rect->width > 64)
          path->d = path->M(X1RelRedun, y+get_point(lastPos[0])) + 
                    path->V(text->y) + 
                    path->H((X1RelRedun+X2RelRedun)/2 - 50);
        else
          path->d = path->M(X1RelRedun, y+get_point(lastPos[0])) + 
                    path->V(text->y-5) + 
                    path->H((X1RelRedun+X2RelRedun)/2 - 32);
      }
      else
        path->d = path->M(x+get_point(lastPos[0]), Y1RelRedun) + 
                  path->H(text->x) + 
                  path->V((Y1RelRedun+Y2RelRedun)/2 - 47);
      // path->plot_shadow(f);
      path->plot(f);
    }
    // Bottom/Right wing
    if (p.vertical) {
      if (rect->width > 64)
        path->d = path->M(X2RelRedun, y+get_point(lastPos[1])) +
                  path->V(text->y) + 
                  path->H((X1RelRedun+X2RelRedun)/2 + 50);
      else
        path->d = path->M(X2RelRedun, y+get_point(lastPos[1])) + 
                  path->V(text->y-5) + 
                  path->H((X1RelRedun+X2RelRedun)/2 + 32);
    }
    else
      path->d = path->M(x+get_point(lastPos[1]), Y2RelRedun) +
                path->H(text->x) + 
                path->V((Y1RelRedun+Y2RelRedun)/2 + 47);
    // path->plot_shadow(f);
    path->plot(f);

    text->transform.clear();
  }
  else if (!p.showNRC && p.showRedun) {
    if (p.vertical) {
      rect->x = x - TITLE_SPACE/2;
      rect->y = originFixed;
      rect->width = TITLE_SPACE + 2*seqWidth + innerSpace;
      rect->height = 11;

      text->x = rect->x + rect->width/2;
      text->y = rect->y - label_shift;
      text->Label = "Redundancy";
      // text->plot_shadow(f);
      text->plot(f);

      X1Redun = x - (TITLE_SPACE/2 + VERT_TUNE + 0.5*seqWidth/VERT_RATIO);
      X2Redun = x + 2*seqWidth + innerSpace + TITLE_SPACE/2 + VERT_TUNE +
                0.5*seqWidth/VERT_RATIO;
    } 
    else {
      rect->x = originFixed;
      rect->y = y - (TITLE_SPACE + VERT_TUNE);
      rect->width = 15;
      rect->height = 2*(TITLE_SPACE + VERT_TUNE) + 2*seqWidth + innerSpace;

      text->x = rect->x - label_shift;
      text->y = rect->y + rect->height/2;
      text->transform = "rotate(90 " + to_string(text->x) + " " +
                        to_string(text->y) + ")";
      text->Label = "Redundancy";
      // text->plot_shadow(f);
      text->plot(f);

      Y1Redun = y - (TITLE_SPACE + VERT_TUNE + 0.5*seqWidth/VERT_RATIO);
      Y2Redun = y + 2*seqWidth + innerSpace + TITLE_SPACE + VERT_TUNE +
                0.5*seqWidth/VERT_RATIO;
    }
    
    // Top/Left wing
    if (lastPos.size() == 2) {
      if (p.vertical)
        path->d = path->M(X1Redun, y+get_point(lastPos[0])) + 
                  path->V(text->y) + 
                  path->H((X1Redun+X2Redun)/2 - 32);
      else
        path->d = path->M(x+get_point(lastPos[0]), Y1Redun) + 
                  path->H(text->x) + 
                  path->V((Y1Redun+Y2Redun)/2 - 32);
      // path->plot_shadow(f);
      path->plot(f);
    }
    // Bottom/Right wing
    if (p.vertical)
      path->d = path->M(X2Redun, y+get_point(lastPos[1])) +
                path->V(text->y) + 
                path->H((X1Redun+X2Redun)/2 + 32);
    else
      path->d = path->M(x+get_point(lastPos[1]), Y2Redun) +
                path->H(text->x) + 
                path->V((Y1Redun+Y2Redun)/2 + 32);
    // path->plot_shadow(f);
    path->plot(f);

    text->transform.clear();
  }
  else if (p.showNRC && p.showRedun) {
    if (p.vertical) {
      rect->x = x - TITLE_SPACE/2;
      rect->y = originFixed;
      rect->width = TITLE_SPACE + 2*seqWidth + innerSpace;
      rect->height = 11;

      text->x = rect->x + rect->width/2;
      if (rect->width > 64) {
        text->y = rect->y - label_shift;
        text->Label = "Relative Redundancy";
        // text->plot_shadow(f);
        text->plot(f);
      } else {
        text->y = rect->y - label_shift - 8;
        text->Label = "Relative";
        // text->plot_shadow(f);
        text->plot(f);
        text->y = rect->y - label_shift + 2;
        text->Label = "Redundancy";
        // text->plot_shadow(f);
        text->plot(f);
      }

      X1RelRedun = x - (TITLE_SPACE/2 + VERT_TUNE + 0.5*seqWidth/VERT_RATIO);
      X2RelRedun = x + 2*seqWidth + innerSpace + TITLE_SPACE/2 + 
                   VERT_TUNE + 0.5*seqWidth/VERT_RATIO;
    } 
    else {
      rect->x = originFixed;
      rect->y = y - (TITLE_SPACE + VERT_TUNE);
      rect->width = 15;
      rect->height = 2*(TITLE_SPACE + VERT_TUNE) + 2*seqWidth + innerSpace;

      text->x = rect->x - label_shift;
      text->y = rect->y + rect->height/2;
      text->transform = "rotate(90 " + to_string(text->x) + " " +
                        to_string(text->y) + ")";
      text->Label = "Relative Redundancy";
      // text->plot_shadow(f);
      text->plot(f);

      Y1RelRedun = y - (TITLE_SPACE + VERT_TUNE + 0.5*seqWidth/VERT_RATIO);
      Y2RelRedun = y + 2*seqWidth + innerSpace + TITLE_SPACE + 
                        VERT_TUNE + 0.5*seqWidth/VERT_RATIO;
    }
    
    // Top/Left wing
    if (lastPos.size() == 2) {
      if (p.vertical) {
        if (rect->width > 64)
          path->d = path->M(X1RelRedun, y+get_point(lastPos[0])) + 
                    path->V(text->y) + 
                    path->H((X1RelRedun+X2RelRedun)/2 - 50);
        else
          path->d = path->M(X1RelRedun, y+get_point(lastPos[0])) + 
                    path->V(text->y-5) + 
                    path->H((X1RelRedun+X2RelRedun)/2 - 32);
      }
      else
        path->d = path->M(x+get_point(lastPos[0]), Y1RelRedun) + 
                  path->H(text->x) + 
                  path->V((Y1RelRedun+Y2RelRedun)/2 - 47);
      // path->plot_shadow(f);
      path->plot(f);
    }
    // Bottom/Right wing
    if (p.vertical) {
      if (rect->width > 64)
        path->d = path->M(X2RelRedun, y+get_point(lastPos[1])) +
                  path->V(text->y) + 
                  path->H((X1RelRedun+X2RelRedun)/2 + 50);
      else
        path->d = path->M(X2RelRedun, y+get_point(lastPos[1])) + 
                  path->V(text->y-5) + 
                  path->H((X1RelRedun+X2RelRedun)/2 + 32);
    }
    else
      path->d = path->M(x+get_point(lastPos[1]), Y2RelRedun) +
                path->H(text->x) + 
                path->V((Y1RelRedun+Y2RelRedun)/2 + 47);
    // path->plot_shadow(f);
    path->plot(f);

    // Redundancy
    if (p.vertical) {
      text->x = rect->x + rect->width/2;
      text->y = rect->y + rect->height + label_shift;
      text->Label = "Redundancy";
      // text->plot_shadow(f);
      text->plot(f);
  
      X1Redun = x - (TITLE_SPACE/2 + 2*VERT_TUNE + 1.5*seqWidth/VERT_RATIO);
      X2Redun = x + 2*seqWidth + innerSpace + TITLE_SPACE/2 +
                2*VERT_TUNE + 1.5*seqWidth/VERT_RATIO;
    }
    else {
      text->x = rect->x + rect->width + label_shift;
      text->y = rect->y + rect->height/2;
      text->transform = "rotate(90 " + to_string(text->x) + " " +
                        to_string(text->y) + ")";
      text->Label = "Redundancy";
      // text->plot_shadow(f);
      text->plot(f);

      Y1Redun = y - (TITLE_SPACE + 2*VERT_TUNE + 1.5*seqWidth/VERT_RATIO);
      Y2Redun = y + 2*seqWidth + innerSpace + TITLE_SPACE +
                     2*VERT_TUNE + 1.5*seqWidth/VERT_RATIO;
    }

    // Top/Left wing
    if (lastPos.size() == 2) {
      if (p.vertical)
        path->d = path->M(X1Redun, y+get_point(lastPos[0])) + 
                  path->V(text->y) + 
                  path->H((X1Redun+X2Redun)/2 - 32);
      else
        path->d = path->M(x+get_point(lastPos[0]), Y1Redun) + 
                  path->H(text->x) + 
                  path->V((Y1Redun+Y2Redun)/2 - 32);
      // path->plot_shadow(f);
      path->plot(f);
    }
    // Bottom/Right wing
    if (p.vertical)
      path->d = path->M(X2Redun, y+get_point(lastPos[1])) +
                path->V(text->y) + 
                path->H((X1Redun+X2Redun)/2 + 32);
    else
      path->d = path->M(x+get_point(lastPos[1]), Y2Redun) +
                path->H(text->x) + 
                path->V((Y1Redun+Y2Redun)/2 + 32);
    // path->plot_shadow(f);
    path->plot(f);

    text->transform.clear();
  }

  plot_legend_gradient(f, rect, p.colorMode, p.vertical);

  // Print numbers (measures)
  text->font_weight = "normal";
  text->dominant_baseline = "middle";
  text->font_size = 8;
  if (p.vertical) {
    text->text_align = "end";
    text->x = rect->x - 9;
    text->y = rect->y + rect->height/2;
  } else {
    text->text_align = "middle";
    text->x = rect->x + rect->width/2;
    text->y = rect->y + rect->height + 6;
  }
  text->Label = "0.0";
  text->plot(f);
  for (u8 i=1; i!=4; ++i) {
    text->text_align = "middle";
    if (p.vertical) {
      text->x = rect->x + rect->width*i/4;
      text->y = rect->y + rect->height/2;
    } else {
      text->x = rect->x + rect->width/2;
      text->y = rect->y + rect->height - rect->height*i/4;
    }
    if (p.colorMode==1 && i==3)  text->fill="white";
    text->Label = string_format("%.1f", i*0.5);
    text->plot(f);
  }
  if (p.vertical) {
    text->text_align = "start";
    text->x = rect->x + rect->width + 9;
    text->y = rect->y + rect->height/2;
  } else {
    text->text_align = "middle";
    text->x = rect->x + rect->width/2;
    text->y = rect->y - 6;
  }
  text->fill = "black";
  text->Label = "2.0";
  text->plot(f);
}

template <typename Rect>
inline void VizPaint::plot_legend_gradient (ofstream& f, const Rect& rect, 
u8 colorMode, bool vertical) {
  vector<string> colorset;
  switch (colorMode) {
  case 0:   colorset = COLORSET[0];  break;
  case 1:   colorset = COLORSET[1];  break;
  case 2:   colorset = COLORSET[2];  break;
  default:  error("undefined color mode.");
  }
  auto id = to_string(rect->x) + to_string(rect->y);

  auto grad = make_unique<LinearGradient>();
  if (vertical) {
    grad->x1 = "0%";
    grad->y1 = "0%";
    grad->x2 = "100%";
    grad->y2 = "0%";
  } else {
    grad->x1 = "0%";
    grad->y1 = "100%";
    grad->x2 = "0%";
    grad->y2 = "0%";
  }
  grad->id = "grad"+id;
  for (u8 i=0; i!=colorset.size(); ++i)
    grad->add_stop(to_string(i*100/(colorset.size()-1))+"%", colorset[i]);
  grad->plot(f);

  rect->stroke = "black";
  rect->stroke_width = 0.5;
  rect->rx = rect->ry = 2;
  rect->fill = "url(#grad" + id + ")";
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

// inline void VizPaint::print_pos (ofstream& fPlot, VizParam& par, 
// const vector<Position>& pos, u64 maxBases, string&& type) {
//   auto   text = make_unique<Text>();
//   string line, lastLine;
//   i64    printPos  = 0;
//   char   printType = 'b';
//   u64    nOverlap  = 0;
//   double shiftY    = 4;
//   if (par.showNRC && par.showRedun)
//     shiftY += 2 * (VERT_TUNE + seqWidth/VERT_RATIO);
//   else if (par.showNRC ^ par.showRedun)
//     shiftY += VERT_TUNE + seqWidth/VERT_RATIO;
//   double CY=y;
//   type=="ref" ? CY-=shiftY : CY+=seqWidth+innerSpace+seqWidth+shiftY;

//   const auto set_dominantBaseline = [&](char type) {
//     switch (type) {
//     case 'b':  text->dominant_baseline="text-before-edge";  break;
//     case 'm':  text->dominant_baseline="middle";            break;
//     case 'e':  text->dominant_baseline="text-after-edge";   break;
//     default:   text->dominant_baseline="middle";            break;
//     }
//   };

//   const auto print_pos_ref = 
//     [&](ofstream& f, unique_ptr<Text>& text, char c='\0') {
//     text->text_anchor = "end";
//     text->y -= 5;
//     switch (c) {
//       case 'b':  text->dominant_baseline = "hanging";   break;  // begin
//       case 'm':  text->dominant_baseline = "middle";    break;  // middle
//       case 'e':  text->dominant_baseline = "baseline";  break;  // end
//       default:                                          break;
//     }
//     text->font_size = 9;
//     text->plot(f);
//   };

//   const auto print_pos_tar =
//     [&](ofstream& f, unique_ptr<Text>& text, char c='\0') {
//     text->text_anchor = "start";
//     text->y += 5;
//     switch (c) {
//       case 'b':  text->dominant_baseline = "hanging";   break;  // begin
//       case 'm':  text->dominant_baseline = "middle";    break;  // middle
//       case 'e':  text->dominant_baseline = "baseline";  break;  // end
//       default:                                          break;
//     }
//     text->font_size = 9;
//     text->plot(f);
//   };

//   for (auto it=nodes.begin(); it<nodes.end()-1; ++it) {
//     if ((it->type=='b' && (it+1)->type=='b') ||
//         (it->type=='e' && (it+1)->type=='e')) {
//       if ((it+1)->position - it->position < PAINT_SHORT * maxBases) {
//         if (++nOverlap == 1) {
//           if (it->start == (it+1)->start) {
//             printPos  = (it->position + (it+1)->position) / 2;
//             printType = 'm';
//           } else {
//             printPos  = it->position;
//             printType = it->type;
//           }
//         }
//         line    += tspan(it->start, it->position);
//         lastLine = tspan((it+1)->start, (it+1)->position);
//       }
//       else {
//         if (nOverlap == 0) {
//           printPos  = it->position;
//           printType = it->type;
//         }
//         nOverlap = 0;
//       }
//     }
//     else if (it->type=='b' && (it+1)->type=='e') {
//       if ((it+1)->position - it->position < PAINT_SHORT * maxBases) {
//         if (++nOverlap == 1) {
//           if (it->start == (it+1)->start) {
//             printPos  = (it->position + (it+1)->position) / 2;
//             printType = 'm';
//           } else {
//             printPos  = (it->position + (it+1)->position) / 2;
//             printType = 'm';
//           }
//         }
//         line    += tspan(it->start, it->position);
//         lastLine = tspan((it+1)->start, (it+1)->position);
//       }
//       else {
//         if (nOverlap == 0) {
//           printPos  = it->position;
//           printType = it->type;
//         }
//         nOverlap = 0;
//       }
//     }
//     else if (it->type=='e' && (it+1)->type=='b') {
//       if (nOverlap == 0) {
//         printPos  = it->position;
//         printType = it->type;
//       }
//       nOverlap = 0;
//     }

//     if (nOverlap == 0) {
//       lastLine = tspan(it->start, it->position);
//       string finalLine {line+lastLine};
//       sort_merge(finalLine);

//       // text->font_weight = "bold";
//       set_dominantBaseline(printType);
//       text->Label  = finalLine;
//       text->x = x + get_point(printPos);
//       text->y = CY;
//       type=="ref" ? print_pos_ref(fPlot, text) : print_pos_tar(fPlot, text);

//       line.clear();
//       lastLine.clear();

//       // Last
//       if (it+2 == nodes.end()) {
//         finalLine = tspan((it+1)->start, (it+1)->position);
//         sort_merge(finalLine);
//         printPos = (it+1)->position;

//         text->dominant_baseline="text-after-edge";
//         text->Label  = finalLine;
//         text->x = x + get_point(printPos);
//         text->y = CY;
//         type=="ref" ? print_pos_ref(fPlot, text) : print_pos_tar(fPlot, text);
//       }
//     }
    
//     if (it+2==nodes.end() && nOverlap!=0) {
//       lastLine = tspan((it+1)->start, (it+1)->position);
//       string finalLine {line+lastLine};
//       sort_merge(finalLine);

//       // text->font_weight = "bold";
//       set_dominantBaseline(printType);
//       text->Label  = finalLine;
//       text->x = x + get_point(printPos);
//       text->y = CY;
//       type=="ref" ? print_pos_ref(fPlot, text) : print_pos_tar(fPlot, text);
//       break;
//     }
//   } // for
// }

inline void VizPaint::plot_pos (ofstream& f, VizParam& par, u64 n_bases, 
bool plotRef) {
  const auto  maxPos           = get_point(n_bases);
  const u8    n_subranges      = 2;
  const u16   minorTickSize    = 6,
              majorTickSize    = 1.6*minorTickSize,
              tickLabelSkip    = 8,
              vertSkip         = 13;
  const float minorStrokeWidth = 0.8f,
              majorStrokeWidth = 1.6*minorStrokeWidth;

  auto line = make_unique<Line>();
  line->stroke_width = minorStrokeWidth;
  auto text = make_unique<Text>();
  text->font_size = 9;

  if (par.vertical) {
    line->y1 = y;
    line->y2 = y + maxPos + 0.75*line->stroke_width;
    if (plotRef)
      line->x1 = line->x2 = x - TITLE_SPACE - vertSkip -
        (u8(par.showNRC)+u8(par.showRedun)) * (VERT_TUNE+seqWidth/VERT_RATIO) -
        majorTickSize;
    else
      line->x1 = line->x2 = x + 2*seqWidth + innerSpace + TITLE_SPACE +
        vertSkip + (u8(par.showNRC)+u8(par.showRedun)) * 
        (VERT_TUNE+seqWidth/VERT_RATIO) + majorTickSize;
  }
  else {
    line->x1 = x;
    line->x2 = x + maxPos + 0.75*line->stroke_width;
    if (plotRef)
      line->y1 = line->y2 = y - TITLE_SPACE - vertSkip -
        (u8(par.showNRC)+u8(par.showRedun)) * (VERT_TUNE+seqWidth/VERT_RATIO) -
        majorTickSize;
    else
      line->y1 = line->y2 = y + 2*seqWidth + innerSpace + TITLE_SPACE +
        vertSkip + (u8(par.showNRC)+u8(par.showRedun)) * 
        (VERT_TUNE+seqWidth/VERT_RATIO) + majorTickSize;
  }
  line->plot(f);

  // Print bp
  text->font_weight = "bold";
  text->font_size = 10;
  if (par.vertical) {
    if (plotRef) {
      text->text_anchor = "start";
      text->x = line->x1 + tickLabelSkip/2;
    } else {
      text->text_anchor = "end";
      text->x = line->x1 - tickLabelSkip/2;
    }
    text->dominant_baseline = "text-before-edge";
    text->y = line->y1;
  } else {
    text->text_anchor = "start";
    text->dominant_baseline = "middle";
    text->x = line->x1;
    text->y = plotRef ? line->y1 + tickLabelSkip : line->y1 - tickLabelSkip;
  }
  text->Label = "bp";
  text->plot(f);

  const u64 oneTenth = n_bases / 10,
            tens = POW10[num_digits(oneTenth) - 1],
            divInt = oneTenth / tens;
  double divDouble = double(oneTenth) / tens;
  if (double(divInt) != divDouble) {
    const double roundHalfUp = divInt + 0.5;
    divDouble = (divDouble<=roundHalfUp) ? roundHalfUp : roundHalfUp+0.5;
  }

  float majorHop = divDouble * tens,
        minorHop = majorHop / n_subranges;

  for (float pos=0; pos <= n_bases; pos+=minorHop) {
    if (par.vertical)  line->y1 = line->y2 = y + get_point((u64)pos);
    else               line->x1 = line->x2 = x + get_point((u64)pos);

    // Major ticks
    if ((u64)pos % (u64)majorHop == 0) {  
      line->stroke_width = majorStrokeWidth;
      if (par.vertical) {
        line->x2 = plotRef ? line->x1 + majorTickSize : line->x1 -majorTickSize;
        if (pos!=0.0f) {
          line->plot(f);
          text->dominant_baseline = "middle";
        } else {
          text->dominant_baseline = "text-before-edge";
        }
        text->text_anchor = plotRef ? "end" : "start";
      }
      else {
        line->y2 = plotRef ? line->y1 + majorTickSize : line->y1 -majorTickSize;
        if (pos != 0.0f) {
          line->plot(f);
          text->text_anchor = "middle";
        } else {
          text->text_anchor = "start";
        }
      }

      text->font_weight = "normal";
      text->font_size = (n_bases < POW10[7]) ? 9 : 8.5;
      if (par.vertical) {
        text->x = plotRef ? line->x1 - tickLabelSkip : line->x1 + tickLabelSkip;
        text->y = line->y1;
      } else {
        text->x = line->x1;
        text->y = plotRef ? line->y1 - tickLabelSkip : line->y1 + tickLabelSkip;
      }
      // text->Label = human_readable_non_cs(pos);
      text->Label = thousands_sep(u64(pos));
      text->plot(f);
    }
    else {  // Minor ticks
      line->stroke_width = minorStrokeWidth;
      if (par.vertical)
        line->x2 = plotRef ? line->x1+minorTickSize : line->x1-minorTickSize;
      else
        line->y2 = plotRef ? line->y1+minorTickSize : line->y1-minorTickSize;
      line->plot(f);
    }
  }
}