// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2020, IEETA, University of Aveiro, Portugal.

#include "vizpaint.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "number.hpp"
using namespace smashpp;

void VizPaint::plot(std::unique_ptr<VizParam>& p) {
  check_file(p->posFile);
  std::ifstream fPos(p->posFile);
  std::ofstream fPlot(p->image);

  read_matadata(fPos, p);
  if (p->verbose) show_info(p);

  std::cerr << "Plotting ...\r";

  config(p->width, p->space, p->mult);
  set_page(p->vertical);
  svg->print_header(fPlot);
  plot_background(fPlot);

  // If min is set to default, reset to base max proportion
  if (p->min == 0) p->min = static_cast<uint32_t>(maxSize / 100);

  // Read positions from file and Print them
  std::vector<Position> pos;
  plot_pos(fPlot, fPos, pos, p);

  // Set color number
  set_n_color(pos);

  // Set total number of colors to be used
  if (!p->man_tot_color) {
    for (const auto& pos_line : pos)
      if (pos_line.n_color > p->tot_color) p->tot_color = pos_line.n_color;
  }

  // Plot
  uint64_t n_regular{0};
  uint64_t n_regularSolo{0};
  uint64_t n_inverse{0};
  uint64_t n_inverseSolo{0};
  uint64_t n_ignored{0};
  std::sort(
      std::begin(pos), std::end(pos),
      [](const Position& l, const Position& r) { return l.begRef > r.begRef; });

  for (auto e = std::begin(pos); e != std::end(pos); ++e) {
    if (std::abs(e->endTar - e->begTar) <= p->min) {
      ++n_ignored;
      continue;
    // } else if (e->begRef != DBLANK && e->endRef - e->begRef <= p->min) {
    //   ++n_ignored;
    //   continue;
    }

    if (e->begRef == DBLANK)
      e->endTar > e->begTar ? ++n_regularSolo : ++n_inverseSolo;

    if (e->endTar > e->begTar) {
      if (p->regular) {
        plot_seq_ref(fPlot, e, p);
        plot_seq_tar(fPlot, e, p, false /*ir*/);

        if (e->begRef != DBLANK) {
          plot_connector(fPlot, e, p, false /*ir*/);
          ++n_regular;
        }
      }
    } else {
      if (p->inverse) {
        plot_seq_ref(fPlot, e, p);
        plot_seq_tar(fPlot, e, p, true /*ir*/);

        if (e->begRef != DBLANK) {
          plot_connector(fPlot, e, p, true /*ir*/);
          ++n_inverse;
        }
      }
    }
  }
  fPos.seekg(std::ios::beg);

  if (p->showN) plot_Ns(fPlot, p->opacity, p->vertical);
  plot_seq_borders(fPlot, p->vertical);
  plot_title(fPlot, ref, tar, p->vertical);
  plot_legend(fPlot, p, std::max(n_refBases, n_tarBases));
  print_log(p->stat, p->image, n_regular, n_regularSolo, n_inverse,
            n_inverseSolo, n_ignored);

  svg->print_tailer(fPlot);
  fPos.close();
  fPlot.close();

  if (n_regular + n_regularSolo + n_inverse + n_inverseSolo == 0)
    remove((p->image).c_str());
}

inline void VizPaint::read_matadata(std::ifstream& fPos,
                                    std::unique_ptr<VizParam>& p) {
  std::string watermark;
  std::string param;
  std::string info_ref, info_ref_size, info_tar, info_tar_size;
  std::string titles;

  auto split = [&](auto pos, char delim = ',') -> std::string {
    std::string str;
    std::getline(fPos, str, delim);
    return str.substr(pos);
  };

  std::getline(fPos, watermark);
  std::getline(fPos, param);
  ref = split(12);
  n_refBases = std::stoull(split(8));
  tar = split(4);
  auto info_n_tarBases = split(8, '\n');
  info_n_tarBases.pop_back();
  n_tarBases = std::stoull(info_n_tarBases);
  std::getline(fPos, titles);

  if (!p->refName.empty()) ref = p->refName;
  if (!p->tarName.empty()) tar = p->tarName;

  if (watermark != POS_WATERMARK) error("unknown file format for positions.");
}

inline void VizPaint::show_info(std::unique_ptr<VizParam>& p) const {
  const uint8_t lblWidth{18};
  const uint8_t colWidth{8};
  uint8_t tblWidth{58};
  if (std::max(n_refBases, n_tarBases) > 999999)
    tblWidth = static_cast<uint8_t>(lblWidth + 4 * colWidth);
  else
    tblWidth = static_cast<uint8_t>(lblWidth + 3 * colWidth);

  const auto rule = [](uint8_t n, std::string s) {
    for (auto i = n / s.size(); i--;) std::cerr << s;
    std::cerr << '\n';
  };
  const auto toprule = [&]() { rule(tblWidth, "~"); };
  const auto midrule = [&]() { rule(tblWidth, "~"); };
  const auto botrule = [&]() { rule(tblWidth, " "); };
  const auto label = [=](std::string s) {
    std::cerr << std::setw(lblWidth) << std::left << s;
  };
  const auto header = [=](std::string s) {
    std::cerr << std::setw(2 * colWidth) << std::left << s;
  };
  const auto design_vals = [&](char c) {
    std::cerr << std::setw(colWidth) << std::left;
    switch (c) {
      case 'w':
        std::cerr << p->width;
        break;
      case 's':
        std::cerr << p->space;
        break;
      case 'f':
        std::cerr << p->mult;
        break;
      case 'b':
        std::cerr << p->start;
        break;
      case 'm':
        std::cerr << p->min;
        break;
      case 'r':
        std::cerr << (p->regular ? "yes" : "no");
        break;
      case 'i':
        std::cerr << (p->inverse ? "yes" : "no");
        break;
      case 'l':
        std::cerr << static_cast<uint16_t>(p->link);
        break;
      default:
        break;
    }
    std::cerr << '\n';
  };
  const auto file_vals = [&](char c) {
    std::cerr << std::setw(2 * colWidth) << std::left;
    // const auto lacale = "en_US.UTF8";
    // std::setlocale(LC_ALL, "en_US.UTF-8");
    switch (c) {
      case '1':
        // std::cerr.imbue(std::locale(lacale));
        std::cerr << n_refBases;
        break;
      // case 'r':  cerr<<ref;  break;
      case 'r':
        std::cerr << file_name(ref);
        break;
      case '2':
        // std::cerr.imbue(std::locale(lacale));
        std::cerr << n_tarBases;
        break;
      // case 't':  cerr<<tar;  break;
      case 't':
        std::cerr << file_name(tar);
        break;
      case 'i':
        std::cerr << p->image;
        break;
      default:
        std::cerr << '-';
        break;
    }
  };

  toprule();
  label("Sequence image");
  std::cerr << '\n';
  midrule();
  label("Width");
  design_vals('w');
  label("Space");
  design_vals('s');
  label("Multiplication");
  design_vals('f');
  label("Begin");
  design_vals('b');
  label("Minimum");
  design_vals('m');
  label("Show regular");
  design_vals('r');
  label("Show inversions");
  design_vals('i');
  label("Link type");
  design_vals('l');
  botrule();  // cerr << '\n';

  toprule();
  label("Files");
  header("Name");
  header("Size (B)");
  std::cerr << '\n';
  midrule();
  label("Reference");
  file_vals('r');
  file_vals('1');
  std::cerr << '\n';
  label("Target");
  file_vals('t');
  file_vals('2');
  std::cerr << '\n';
  label("Image");
  file_vals('i');
  file_vals('-');
  std::cerr << '\n';
  botrule();
}

inline void VizPaint::config(double width_, double space_, uint32_t mult_) {
  ratio = static_cast<uint32_t>(std::max(n_refBases, n_tarBases) / PAINT_SCALE);
  seqWidth = width_;
  periphWidth = seqWidth / 3;
  innerSpace = space_;
  mult = mult_;
  refSize = get_point(n_refBases);
  tarSize = get_point(n_tarBases);
  maxSize = std::max(refSize, tarSize);
}

inline void VizPaint::set_page(bool vertical) {
  if (vertical) {
    x = 100.0f;
    // const auto max_n_digits =
    //   max(num_digits(n_refBases), num_digits(n_tarBases));
    // if (max_n_digits > 4)   x += 17.0f;
    // if (max_n_digits > 7)   x += 8.5f;
    // if (max_n_digits > 10)  x += 8.5f;
    y = 30.0f;
    svg->width = 2 * x + 2 * seqWidth + innerSpace;
    svg->height = maxSize + 105;
  } else {
    x = 20.0f;
    y = 100.0f;
    svg->width = maxSize + 105;
    svg->height = 2 * y + 2 * seqWidth + innerSpace;
  }
}

std::string VizPaint::rgb_color(uint32_t start) const {
  const auto hue{static_cast<uint8_t>(start * mult)};
  return to_hex(HSV(hue));
}

std::string VizPaint::make_color(uint8_t n_color, uint8_t tot_color) const {
  const auto out_idx =
      (n_color > tot_color)
          ? map_interval<uint8_t>(1, n_color, 1, tot_color, n_color)
          : n_color;
  const auto hue{static_cast<uint8_t>(out_idx * 360 / (tot_color + 1) + 15)};
  return to_hex(HSV(hue));
}

inline std::string VizPaint::nrc_color(double entropy,
                                       uint32_t colorMode) const {
  keep_in_range(0.0, entropy, 2.0);
#ifdef EXTEND
  // return heatmap_color(entropy/2 * (width+space+width));
#endif
  switch (colorMode) {
    case 0:
      return COLORSET[0][entropy / 2 * (COLORSET[0].size() - 1)];
    case 1:
      return COLORSET[1][entropy / 2 * (COLORSET[1].size() - 1)];
    case 2:
      return COLORSET[2][entropy / 2 * (COLORSET[2].size() - 1)];
    default:
      error("undefined color mode.");
  }
  return "";
}

inline std::string VizPaint::redun_color(double entropy,
                                         uint32_t colorMode) const {
  keep_in_range(0.0, entropy, 2.0);
#ifdef EXTEND
  // return heatmap_color(entropy/2 * (width+space+width));
#endif
  return nrc_color(entropy, colorMode);
}

inline std::string VizPaint::seq_gradient(std::ofstream& fPlot,
                                          std::string color,
                                          std::string id) const {
  auto grad = std::make_unique<LinearGradient>();
  grad->id = "grad" + id;
  grad->add_stop("0%", color);
  grad->add_stop("50%", tint(color, 0.5));
  grad->add_stop("100%", color);
  grad->plot(fPlot);

  return "url(#" + grad->id + ")";
}

inline std::string VizPaint::periph_gradient(std::ofstream& fPlot,
                                             std::string color,
                                             std::string id) const {
  auto grad = std::make_unique<LinearGradient>();
  grad->id = "grad" + id;
  // grad->add_stop("0%", color);
  // grad->add_stop("50%", shade(color, 0.5));
  grad->add_stop("100%", color);
  grad->plot(fPlot);

  return "url(#" + grad->id + ")";
}

template <typename Value>
inline double VizPaint::get_point(Value index) const {
  return 5.0 * index / ratio;
}

inline uint64_t VizPaint::get_index(double point) const {
  return static_cast<uint64_t>(point * ratio / 5.0);
}

inline void VizPaint::plot_background(std::ofstream& f) const {
  auto rect = std::make_unique<Rectangle>();
  rect->fill = rect->stroke = "white";
  rect->x = 0;
  rect->y = 0;
  rect->width = svg->width;
  rect->height = svg->height;
  rect->plot(f);
}

inline void VizPaint::plot_seq_ref(std::ofstream& fPlot,
                                   const std::vector<Position>::iterator& e,
                                   std::unique_ptr<VizParam>& p) const {
  if (e->begRef != DBLANK) {
    auto rect = std::make_unique<Rectangle>();
    rect->width = seqWidth;
    rect->stroke_width = 0.75;
    rect->fill_opacity = rect->stroke_opacity = p->opacity;
    rect->height = get_point(e->endRef - e->begRef);
    rect->stroke = rect->fill;
    // rect->stroke = shade(rgb_color(e->start));
    if (p->vertical) {
      rect->x = x;
      rect->y = y + get_point(e->begRef);
    } else {
      rect->x = x + get_point(e->begRef);
      rect->y = y + seqWidth;
      rect->transform = "rotate(-90 " + std::to_string(rect->x) + " " +
                        std::to_string(rect->y) + ")";
    }
    rect->id = std::to_string(rect->x) + std::to_string(rect->y);
    rect->fill =
        seq_gradient(fPlot, make_color(e->n_color, p->tot_color), rect->id);
    // rect->fill = seq_gradient(fPlot, rgb_color(e->start), rect->id);
    rect->plot(fPlot);

    if (p->showRelRedun) {
      rect->id += "NRC";
      rect->fill =
          periph_gradient(fPlot, nrc_color(e->entRef, p->colorMode), rect->id);
      rect->stroke = rect->fill;
      // rect->stroke = shade(nrc_color(e->entRef, p->colorMode), 0.96);
      plot_periph(fPlot, rect, p->vertical, 'r', 0);
    }
    if (p->showRedun) {
      rect->id += "Redun";
      rect->fill = periph_gradient(fPlot, redun_color(e->selfRef, p->colorMode),
                                   rect->id);
      rect->stroke = rect->fill;
      // rect->stroke = shade(redun_color(e->selfRef, p->colorMode), 0.95);
      plot_periph(fPlot, rect, p->vertical, 'r', uint8_t(p->showRelRedun));
    }
  }
}

// // With Cylinder
// inline void VizPaint::plot_seq_ref(std::ofstream& fPlot,
//                                    const std::vector<Position>::iterator& e,
//                                    std::unique_ptr<VizParam>& p) const {
//   if (e->begRef != DBLANK) {
//     auto cylinder = std::make_unique<Cylinder>();
//     cylinder->width = seqWidth;
//     // cylinder->stroke_width = 3;//todo
//     cylinder->stroke_width = 0.75;
//     cylinder->fill_opacity = cylinder->stroke_opacity = p->opacity;
//     cylinder->height = get_point(e->endRef - e->begRef);
//     cylinder->stroke = shade(rgb_color(e->start));
//     if (p->vertical) {
//       cylinder->x = x;
//       cylinder->y = y + get_point(e->begRef);
//     } else {
//       cylinder->x = x + get_point(e->begRef);
//       cylinder->y = y + seqWidth;
//       cylinder->transform = "rotate(-90 " + std::to_string(cylinder->x) + " "
//       +
//                             std::to_string(cylinder->y) + ")";
//     }
//     cylinder->id = std::to_string(cylinder->x) + std::to_string(cylinder->y);
//     cylinder->fill = seq_gradient(fPlot, rgb_color(e->start), cylinder->id);
//     cylinder->plot(fPlot);

//     cylinder->stroke_width = 0.7;
//     if (p->showRelRedun) {
//       cylinder->id += "NRC";
//       cylinder->fill = periph_gradient(
//           fPlot, nrc_color(e->entRef, p->colorMode), cylinder->id);
//       cylinder->stroke = shade(nrc_color(e->entRef, p->colorMode), 0.96);
//       plot_periph(fPlot, cylinder, p->vertical, 'r', 0);
//     }
//     if (p->showRedun) {
//       cylinder->id += "Redun";
//       cylinder->fill = periph_gradient(
//           fPlot, redun_color(e->selfRef, p->colorMode), cylinder->id);
//       cylinder->stroke = shade(redun_color(e->selfRef, p->colorMode), 0.95);
//       plot_periph(fPlot, cylinder, p->vertical, 'r', uint8_t(p->showRelRedun));
//     }
//   }
// }

inline void VizPaint::plot_seq_tar(std::ofstream& fPlot,
                                   const std::vector<Position>::iterator& e,
                                   std::unique_ptr<VizParam>& p,
                                   bool inverted) const {
  auto rect = std::make_unique<Rectangle>();
  rect->width = seqWidth;
  rect->stroke_width = 0.75;
  rect->fill_opacity = rect->stroke_opacity = p->opacity;
  rect->height = get_point(std::abs(e->begTar - e->endTar));
  if (p->vertical) {
    rect->x = x + seqWidth + innerSpace;
    rect->y = !inverted ? y + get_point(e->begTar) : y + get_point(e->endTar);
  } else {
    rect->x = !inverted ? x + get_point(e->begTar) : x + get_point(e->endTar);
    rect->y = y + 2 * seqWidth + innerSpace;
    rect->transform = "rotate(-90 " + std::to_string(rect->x) + " " +
                      std::to_string(rect->y) + ")";
  }
  rect->id = std::to_string(rect->x) + std::to_string(rect->y);
  if (e->begRef == DBLANK) {
    rect->fill = "black";
    rect->stroke = "white";
  } else {
    rect->fill =
        seq_gradient(fPlot, make_color(e->n_color, p->tot_color), rect->id);
    rect->stroke = rect->fill;
  }

  if (!inverted) {
    rect->plot(fPlot);
  } else {
    if (e->begRef == DBLANK)
      rect->plot_ir(fPlot, "WavyWhite");
    else
      rect->plot_ir(fPlot, "Wavy",
                    shade(make_color(e->n_color, p->tot_color), 0.33));
  }

  // rect->stroke_width = 0.7;
  if (p->showRelRedun) {
    rect->id += "NRC";
    rect->fill =
        periph_gradient(fPlot, nrc_color(e->entTar, p->colorMode), rect->id);
    rect->stroke = rect->fill;
    // rect->stroke = shade(nrc_color(e->entTar, p->colorMode), 0.96);
    plot_periph(fPlot, rect, p->vertical, 't', 0);
  }
  if (p->showRedun) {
    rect->id += "Redun";
    rect->fill =
        periph_gradient(fPlot, redun_color(e->selfTar, p->colorMode), rect->id);
    rect->stroke = rect->fill;
    // rect->stroke = shade(redun_color(e->selfTar, p->colorMode), 0.95);
    plot_periph(fPlot, rect, p->vertical, 't', uint8_t(p->showRelRedun));
  }
}

// // With Cylinder
// inline void VizPaint::plot_seq_tar(std::ofstream& fPlot,
//                                    const std::vector<Position>::iterator& e,
//                                    std::unique_ptr<VizParam>& p,
//                                    bool inverted) const {
//   auto cylinder = std::make_unique<Cylinder>();
//   cylinder->width = seqWidth;
//   cylinder->stroke_width = 0.75;
//   cylinder->fill_opacity = cylinder->stroke_opacity = p->opacity;
//   cylinder->height = get_point(abs(e->begTar - e->endTar));
//   if (p->vertical) {
//     cylinder->x = x + seqWidth + innerSpace;
//     cylinder->y =
//         !inverted ? y + get_point(e->begTar) : y + get_point(e->endTar);
//   } else {
//     cylinder->x =
//         !inverted ? x + get_point(e->begTar) : x + get_point(e->endTar);
//     cylinder->y = y + 2 * seqWidth + innerSpace;
//     cylinder->transform = "rotate(-90 " + std::to_string(cylinder->x) + " " +
//                           std::to_string(cylinder->y) + ")";
//   }
//   cylinder->id = std::to_string(cylinder->x) + std::to_string(cylinder->y);
//   if (e->begRef == DBLANK) {
//     cylinder->fill = "black";
//     cylinder->stroke = "white";
//   } else {
//     cylinder->fill = seq_gradient(fPlot, rgb_color(e->start), cylinder->id);
//     cylinder->stroke = shade(rgb_color(e->start));
//   }

//   if (!inverted) {
//     cylinder->plot(fPlot);
//   } else {
//     if (e->begRef == DBLANK)
//       cylinder->plot_ir(fPlot, "WavyWhite");
//     else
//       cylinder->plot_ir(fPlot);
//   }

//   cylinder->stroke_width = 0.7;
//   if (p->showRelRedun) {
//     cylinder->id += "NRC";
//     cylinder->fill = periph_gradient(fPlot, nrc_color(e->entTar,
//     p->colorMode),
//                                      cylinder->id);
//     cylinder->stroke = shade(nrc_color(e->entTar, p->colorMode), 0.96);
//     plot_periph(fPlot, cylinder, p->vertical, 't', 0);
//   }
//   if (p->showRedun) {
//     cylinder->id += "Redun";
//     cylinder->fill = periph_gradient(
//         fPlot, redun_color(e->selfTar, p->colorMode), cylinder->id);
//     cylinder->stroke = shade(redun_color(e->selfTar, p->colorMode), 0.95);
//     plot_periph(fPlot, cylinder, p->vertical, 't', uint8_t(p->showRelRedun));
//   }
// }

inline void VizPaint::plot_periph(std::ofstream& f,
                                  std::unique_ptr<Rectangle>& rect,
                                  bool vertical, char RefTar,
                                  uint8_t showRelRedun) const {
  const auto mainOriginX{rect->x};
  const auto mainWidth{rect->width};
  const auto mainStrokeWidth{rect->stroke_width};
  const auto mainRy{rect->ry};

  if (RefTar == 'r') {
    if (vertical)
      rect->x = rect->x - TITLE_SPACE / 2 -
                (1 + showRelRedun) * (periphWidth + SPACE_TUNE);
    else
      rect->x = rect->x + rect->width + TITLE_SPACE + SPACE_TUNE +
                showRelRedun * (periphWidth + SPACE_TUNE);
  } else {
    if (vertical)
      rect->x = rect->x + rect->width + TITLE_SPACE / 2 + SPACE_TUNE +
                showRelRedun * (SPACE_TUNE + periphWidth);
    else
      rect->x = rect->x - (periphWidth + TITLE_SPACE + SPACE_TUNE +
                           showRelRedun * (SPACE_TUNE + periphWidth));
  }
  rect->width = periphWidth;
  rect->stroke_width *= 1.5;
  // rect->ry /= 4;
  rect->ry = 0.5;
  rect->plot(f);

  rect->x = mainOriginX;
  rect->width = mainWidth;
  rect->stroke_width = mainStrokeWidth;
  rect->ry = mainRy;
}

inline void VizPaint::plot_periph(std::ofstream& f,
                                  std::unique_ptr<Cylinder>& cylinder,
                                  bool vertical, char RefTar,
                                  uint8_t showRelRedun) const {
  const auto mainOriginX{cylinder->x};
  const auto mainWidth{cylinder->width};
  const auto mainStrokeWidth{cylinder->stroke_width};
  const auto mainRy{cylinder->ry};

  if (RefTar == 'r') {
    if (vertical)
      cylinder->x = cylinder->x - TITLE_SPACE / 2 -
                    (1 + showRelRedun) * (periphWidth + SPACE_TUNE);
    else
      cylinder->x = cylinder->x + cylinder->width + TITLE_SPACE + SPACE_TUNE +
                    showRelRedun * (periphWidth + SPACE_TUNE);
  } else {
    if (vertical)
      cylinder->x = cylinder->x + cylinder->width + TITLE_SPACE / 2 +
                    SPACE_TUNE + showRelRedun * (SPACE_TUNE + periphWidth);
    else
      cylinder->x = cylinder->x - (periphWidth + TITLE_SPACE + SPACE_TUNE +
                                   showRelRedun * (SPACE_TUNE + periphWidth));
  }
  cylinder->width = periphWidth;
  cylinder->stroke_width *= 2;
  cylinder->ry /= 2;
  cylinder->plot(f);

  cylinder->x = mainOriginX;
  cylinder->width = mainWidth;
  cylinder->stroke_width = mainStrokeWidth;
  cylinder->ry = mainRy;
}

inline void VizPaint::plot_connector(std::ofstream& fPlot,
                                     const std::vector<Position>::iterator& e,
                                     std::unique_ptr<VizParam>& par,
                                     bool ir) const {
  auto poly = std::make_unique<Polygon>();
  auto line = std::make_unique<Line>();
  poly->stroke_width = 0.3;
  line->stroke_width = 1.0;

  switch (par->link) {
    case 1:
      if (par->vertical)
        poly->points =
            poly->point(x + seqWidth, y + get_point(e->begRef)) +
            poly->point(x + seqWidth, y + get_point(e->endRef)) +
            poly->point(x + seqWidth + innerSpace, y + get_point(e->endTar)) +
            poly->point(x + seqWidth + innerSpace, y + get_point(e->begTar));
      else
        poly->points =
            poly->point(x + get_point(e->begRef), y + seqWidth) +
            poly->point(x + get_point(e->endRef), y + seqWidth) +
            poly->point(x + get_point(e->endTar), y + seqWidth + innerSpace) +
            poly->point(x + get_point(e->begTar), y + seqWidth + innerSpace);
      poly->stroke = poly->fill = make_color(e->n_color, par->tot_color);
      // poly->stroke = poly->fill = rgb_color(e->start);
      poly->stroke_opacity = 
      poly->fill_opacity = 0.2 * par->opacity;
      poly->plot(fPlot);
      break;
    case 2:
      if (par->vertical) {
        line->x1 = x + seqWidth;
        line->y1 = y + get_point(e->begRef + (e->endRef - e->begRef) / 2.0);
        line->x2 = x + seqWidth + innerSpace;
        line->y2 =
            ir ? y + get_point(e->endTar + (e->begTar - e->endTar) / 2.0)
               : y + get_point(e->begTar + (e->endTar - e->begTar) / 2.0);
      } else {
        line->x1 = x + get_point(e->begRef + (e->endRef - e->begRef) / 2.0);
        line->y1 = y + seqWidth;
        line->x2 =
            ir ? x + get_point(e->endTar + (e->begTar - e->endTar) / 2.0)
               : x + get_point(e->begTar + (e->endTar - e->begTar) / 2.0);
        line->y2 = y + seqWidth + innerSpace;
      }
      line->stroke = make_color(e->n_color, par->tot_color);
      // line->stroke = rgb_color(e->start);
      line->plot(fPlot);
      break;
    case 3:
      if (par->vertical) {
        line->x1 = x + seqWidth;
        line->y1 = y + get_point(e->begRef + (e->endRef - e->begRef) / 2.0);
        line->x2 = x + seqWidth + innerSpace;
        line->y2 =
            ir ? y + get_point(e->endTar + (e->begTar - e->endTar) / 2.0)
               : y + get_point(e->begTar + (e->endTar - e->begTar) / 2.0);
      } else {
        line->x1 = x + get_point(e->begRef + (e->endRef - e->begRef) / 2.0);
        line->y1 = y + seqWidth;
        line->x2 =
            ir ? x + get_point(e->endTar + (e->begTar - e->endTar) / 2.0)
               : x + get_point(e->begTar + (e->endTar - e->begTar) / 2.0);
        line->y2 = y + seqWidth + innerSpace;
      }
      line->stroke = ir ? "green" : "black";
      line->plot(fPlot);
      break;
    case 4:
      line->stroke = make_color(e->n_color, par->tot_color);
      // line->stroke = rgb_color(e->start);
      if (par->vertical) {
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
      line->stroke = ir ? "green" : "black";
      if (par->vertical) {
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
    default:
      break;
  }
}

inline void VizPaint::plot_title(std::ofstream& f, std::string ref,
                                 std::string tar, bool vertical) const {
  auto text = std::make_unique<Text>();
  text->font_weight = "bold";
  text->font_size = 9;

  if (vertical) {
    text->text_anchor = "middle";
    // text->dominant_baseline = "middle";
    text->y = y - 0.6 * TITLE_SPACE;

    const auto charSpace{5};
    const bool tooClose{(innerSpace -
                             std::abs(ref.size() * charSpace - seqWidth) / 2 -
                             std::abs(tar.size() * charSpace - seqWidth) / 2 <
                         15)};

    text->x = x + seqWidth / 2;
    if (tooClose) {
      text->x += seqWidth / 2;
      text->text_anchor = "end";
    }
    text->Label = ref;
    text->plot(f);

    text->x = x + 1.5 * seqWidth + innerSpace;
    if (tooClose) {
      text->x -= seqWidth / 2;
      text->text_anchor = "start";
    }
    text->Label = tar;
    text->plot(f);
  } else {
    text->text_anchor = "start";
    text->x = x;
    text->y = y - TITLE_SPACE + VERT_BOTTOM * text->font_size;
    // text->dominant_baseline = "text-before-edge";
    text->Label = ref;
    text->plot(f);

    text->y = y + 2 * seqWidth + innerSpace + TITLE_SPACE;
    // text->dominant_baseline = "text-after-edge";
    text->Label = tar;
    text->plot(f);
  }
}

inline void VizPaint::plot_legend(std::ofstream& f,
                                  std::unique_ptr<VizParam>& p,
                                  int64_t maxWidth) const {
  if (!p->showRelRedun && !p->showRedun) return;

  auto legend = std::make_unique<LegendPlot>();
  legend->maxWidth = maxWidth;
  legend->showRelRedun = p->showRelRedun;
  legend->showRedun = p->showRedun;
  legend->vertical = p->vertical;
  legend->colorMode = p->colorMode;

  legend->text.push_back(std::make_unique<Text>());  // Numbers
  if (legend->showRelRedun && !legend->showRedun)
    legend->text.push_back(std::make_unique<Text>());  // NRC
  else if (!legend->showRelRedun && legend->showRedun)
    legend->text.push_back(std::make_unique<Text>());  // Redun
  else if (legend->showRelRedun && legend->showRedun)
    for (uint8_t i = 0; i != 2; ++i)
      legend->text.push_back(std::make_unique<Text>());  // NRC + Redun

  if (legend->vertical) {
    set_legend_rect(f, legend, 'h');
    plot_legend_gradient(f, legend);
    plot_legend_text_horiz(f, legend);
    plot_legend_path_horiz(f, legend);
  } else {
    set_legend_rect(f, legend, 'v');
    plot_legend_gradient(f, legend);
    plot_legend_text_vert(f, legend);
    plot_legend_path_vert(f, legend);
  }
}

inline void VizPaint::set_legend_rect(std::ofstream& f,
                                      std::unique_ptr<LegendPlot>& legend,
                                      char direction) const {
  if (direction == 'h' || direction == 'H') {
    legend->rect->height = 11;
    legend->rect->x = x - TITLE_SPACE / 2;
    legend->rect->y = y + get_point(legend->maxWidth) + 40;
    legend->rect->width = TITLE_SPACE + 2 * seqWidth + innerSpace;
  } else if (direction == 'v' || direction == 'V') {
    legend->rect->width = 15;
    legend->rect->x = x + get_point(legend->maxWidth) + 40;
    legend->rect->y = y - TITLE_SPACE - SPACE_TUNE;
    legend->rect->height =
        2 * (TITLE_SPACE + SPACE_TUNE) + 2 * seqWidth + innerSpace;
  }
}

inline void VizPaint::plot_legend_gradient(
    std::ofstream& f, std::unique_ptr<LegendPlot>& legend) const {
  std::vector<std::string> colorset;
  switch (legend->colorMode) {
    case 0:
      colorset = COLORSET[0];
      break;
    case 1:
      colorset = COLORSET[1];
      break;
    case 2:
      colorset = COLORSET[2];
      break;
    default:
      error("undefined color mode.");
  }
  auto id{std::to_string(legend->rect->x) + std::to_string(legend->rect->y)};

  auto grad = std::make_unique<LinearGradient>();
  if (legend->vertical) {
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
  grad->id = "grad" + id;
  for (uint8_t i = 0; i != colorset.size(); ++i)
    grad->add_stop(std::to_string(i * 100 / (colorset.size() - 1)) + "%",
                   colorset[i]);
  grad->plot(f);

  legend->rect->stroke = "black";
  legend->rect->stroke_width = 0.5;
  legend->rect->rx = legend->rect->ry = 2;
  legend->rect->fill = "url(#grad" + id + ")";
  legend->rect->plot(f);

  // auto cylinder = make_unique<Cylinder>();
  // cylinder->width = legend->rect->height;
  // cylinder->height = legend->rect->width;
  // cylinder->x = legend->rect->x;
  // cylinder->y = legend->rect->y + legend->rect->height;
  // cylinder->transform = "rotate(-90 " + to_string(cylinder->x) + " " +
  //   to_string(cylinder->y) + ")";
  // cylinder->fill = "url(#grad" + id + ")";
  // cylinder->ry = 0;
  // cylinder->stroke_width = 0.35;
  // cylinder->stroke = "black";
  // cylinder->plot(f);
}

inline void VizPaint::plot_legend_text_horiz(
    std::ofstream& f, std::unique_ptr<LegendPlot>& legend) const {
  for (auto& e : legend->text) {
    e->text_anchor = "middle";
    // e->dominant_baseline = "middle";
    e->font_size = 8;
  }

  if (legend->showRelRedun && !legend->showRedun) {
    legend->text[1]->x = legend->rect->x + legend->rect->width / 2;
    if (innerSpace > 55) {
      legend->text[1]->y = legend->rect->y - legend->labelShift +
                           VERT_MIDDLE * legend->text[1]->font_size;
      legend->text[1]->Label = "Relative Redundancy";
      // legend->text[1]->plot_shadow(f);
      legend->text[1]->plot(f);
    } else {
      legend->text[1]->y = legend->rect->y - legend->labelShift - 10 +
                           VERT_MIDDLE * legend->text[1]->font_size;
      legend->text[1]->Label = "Relative";
      // legend->text[1]->plot_shadow(f);
      legend->text[1]->plot(f);
      legend->text[1]->y = legend->rect->y - legend->labelShift +
                           VERT_MIDDLE * legend->text[1]->font_size;
      legend->text[1]->Label = "Redundancy";
      // legend->text[1]->plot_shadow(f);
      legend->text[1]->plot(f);
    }
  } else if (!legend->showRelRedun && legend->showRedun) {
    legend->text[1]->x = legend->rect->x + legend->rect->width / 2;
    legend->text[1]->y = legend->rect->y - legend->labelShift +
                         VERT_MIDDLE * legend->text[1]->font_size;
    legend->text[1]->Label = "Redundancy";
    // legend->text[1]->plot_shadow(f);
    legend->text[1]->plot(f);
  } else if (legend->showRelRedun && legend->showRedun) {
    legend->text[1]->x = legend->rect->x + legend->rect->width / 2;
    if (innerSpace > 55) {
      legend->text[1]->y = legend->rect->y - legend->labelShift +
                           VERT_MIDDLE * legend->text[1]->font_size;
      legend->text[1]->Label = "Relative Redundancy";
      // legend->text[1]->plot_shadow(f);
      legend->text[1]->plot(f);
    } else {
      legend->text[1]->y = legend->rect->y - legend->labelShift - 10 +
                           VERT_MIDDLE * legend->text[1]->font_size;
      legend->text[1]->Label = "Relative";
      // legend->text[1]->plot_shadow(f);
      legend->text[1]->plot(f);
      legend->text[1]->y = legend->rect->y - legend->labelShift +
                           VERT_MIDDLE * legend->text[1]->font_size;
      legend->text[1]->Label = "Redundancy";
      // legend->text[1]->plot_shadow(f);
      legend->text[1]->plot(f);
    }

    // Redundancy
    legend->text[2]->x = legend->rect->x + legend->rect->width / 2;
    legend->text[2]->y = legend->rect->y + legend->rect->height +
                         legend->labelShift +
                         VERT_MIDDLE * legend->text[2]->font_size;
    legend->text[2]->Label = "Redundancy";
    // legend->text[2]->plot_shadow(f);
    legend->text[2]->plot(f);
  }

  // Numbers (measures)
  // 0.0
  // legend->text[0]->font_weight = "bold";
  legend->text[0]->font_size = 7;
  legend->text[0]->fill = "black";
  legend->text[0]->dy = VERT_MIDDLE * legend->text[0]->font_size;
  legend->text[0]->x = legend->rect->x - 4;
  legend->text[0]->y = legend->rect->y + legend->rect->height / 2;
  legend->text[0]->text_anchor = "end";
  legend->text[0]->Label = "0.0";
  legend->text[0]->plot(f);
  // 2.0
  legend->text[0]->x = legend->rect->x + legend->rect->width + 4;
  legend->text[0]->y = legend->rect->y + legend->rect->height / 2;
  legend->text[0]->text_anchor = "start";
  legend->text[0]->Label = "2.0";
  legend->text[0]->plot(f);
  // [0.5  1.0  1.5] or [  1.0  ]
  if (innerSpace < 25) {
    legend->text[0]->text_anchor = "middle";
    legend->text[0]->y = legend->rect->y + legend->rect->height / 2;
    legend->text[0]->x = legend->rect->x + legend->rect->width / 2;
    legend->text[0]->Label = "1.0";
    legend->text[0]->plot(f);
  } else {
    for (uint8_t i = 1; i != 4; ++i) {
      legend->text[0]->text_anchor = "middle";
      legend->text[0]->y = legend->rect->y + legend->rect->height / 2;
      legend->text[0]->x = legend->rect->x + legend->rect->width / 4 * i;
      if (legend->colorMode == 1 && i == 3) legend->text[0]->fill = "white";
      legend->text[0]->Label = string_format("%.1f", i * 0.5);
      legend->text[0]->plot(f);
    }
  }

  // for (auto& e : legend->text)
  //   e->transform.clear();
}

inline void VizPaint::plot_legend_path_horiz(
    std::ofstream& f, std::unique_ptr<LegendPlot>& legend) const {
  legend->path->stroke = "black";
  legend->path->stroke_width = 0.5;
  legend->path->stroke_dasharray = "8 4";

  if (legend->showRelRedun && !legend->showRedun) {
    float X1RelRedun = x - (TITLE_SPACE / 2 + SPACE_TUNE + 0.5 * periphWidth);
    float X2RelRedun = x + 2 * seqWidth + innerSpace + TITLE_SPACE / 2 +
                       SPACE_TUNE + 0.5 * periphWidth;
    // Left wing
    if (lastPos.size() == 2) {
      if (innerSpace < 20)
        legend->path->d =
            legend->path->M(X1RelRedun, y + get_point(lastPos[0])) +
            legend->path->V(legend->text[1]->y - 8 -
                            VERT_MIDDLE * legend->text[1]->font_size) +
            legend->path->H((X1RelRedun + X2RelRedun) / 2 - 25);
      else if (innerSpace > 55)
        legend->path->d =
            legend->path->M(X1RelRedun, y + get_point(lastPos[0])) +
            legend->path->V(legend->text[1]->y -
                            VERT_MIDDLE * legend->text[1]->font_size) +
            legend->path->H((X1RelRedun + X2RelRedun) / 2 - 50);
      else
        legend->path->d =
            legend->path->M(X1RelRedun, y + get_point(lastPos[0])) +
            legend->path->V(legend->text[1]->y - 5 -
                            VERT_MIDDLE * legend->text[1]->font_size) +
            legend->path->H((X1RelRedun + X2RelRedun) / 2 - 32);
      // legend->path->plot_shadow(f);
      legend->path->plot(f);
    }
    // Right wing
    if (innerSpace < 20)
      legend->path->d =
          legend->path->M(X2RelRedun, y + get_point(lastPos[1])) +
          legend->path->V(legend->text[1]->y - 8 -
                          VERT_MIDDLE * legend->text[1]->font_size) +
          legend->path->H((X1RelRedun + X2RelRedun) / 2 + 25);
    else if (innerSpace > 55)
      legend->path->d =
          legend->path->M(X2RelRedun, y + get_point(lastPos[1])) +
          legend->path->V(legend->text[1]->y -
                          VERT_MIDDLE * legend->text[1]->font_size) +
          legend->path->H((X1RelRedun + X2RelRedun) / 2 + 50);
    else
      legend->path->d =
          legend->path->M(X2RelRedun, y + get_point(lastPos[1])) +
          legend->path->V(legend->text[1]->y - 5 -
                          VERT_MIDDLE * legend->text[1]->font_size) +
          legend->path->H((X1RelRedun + X2RelRedun) / 2 + 32);
    // legend->path->plot_shadow(f);
    legend->path->plot(f);
  } else if (!legend->showRelRedun && legend->showRedun) {
    float X1Redun = x - (TITLE_SPACE / 2 + SPACE_TUNE + 0.5 * periphWidth);
    float X2Redun = x + 2 * seqWidth + innerSpace + TITLE_SPACE / 2 +
                    SPACE_TUNE + 0.5 * periphWidth;
    // Left wing
    if (lastPos.size() == 2) {
      legend->path->d =
          legend->path->M(X1Redun, y + get_point(lastPos[0])) +
          legend->path->V(legend->text[1]->y -
                          VERT_MIDDLE * legend->text[1]->font_size) +
          legend->path->H((X1Redun + X2Redun) / 2 - 32);
      // legend->path->plot_shadow(f);
      legend->path->plot(f);
    }
    // Right wing
    legend->path->d =
        legend->path->M(X2Redun, y + get_point(lastPos[1])) +
        legend->path->V(legend->text[1]->y -
                        VERT_MIDDLE * legend->text[1]->font_size) +
        legend->path->H((X1Redun + X2Redun) / 2 + 32);
    // legend->path->plot_shadow(f);
    legend->path->plot(f);
  } else if (legend->showRelRedun && legend->showRedun) {
    float X1RelRedun = x - (TITLE_SPACE / 2 + SPACE_TUNE + 0.5 * periphWidth);
    float X2RelRedun = x + 2 * seqWidth + innerSpace + TITLE_SPACE / 2 +
                       SPACE_TUNE + 0.5 * periphWidth;
    // Left wing
    if (lastPos.size() == 2) {
      if (innerSpace < 20)
        legend->path->d =
            legend->path->M(X1RelRedun, y + get_point(lastPos[0])) +
            legend->path->V(legend->text[1]->y - 8 -
                            VERT_MIDDLE * legend->text[1]->font_size) +
            legend->path->H((X1RelRedun + X2RelRedun) / 2 - 25);
      else if (innerSpace > 55)
        legend->path->d =
            legend->path->M(X1RelRedun, y + get_point(lastPos[0])) +
            legend->path->V(legend->text[1]->y -
                            VERT_MIDDLE * legend->text[1]->font_size) +
            legend->path->H((X1RelRedun + X2RelRedun) / 2 - 50);
      else
        legend->path->d =
            legend->path->M(X1RelRedun, y + get_point(lastPos[0])) +
            legend->path->V(legend->text[1]->y - 5 -
                            VERT_MIDDLE * legend->text[1]->font_size) +
            legend->path->H((X1RelRedun + X2RelRedun) / 2 - 32);
      // legend->path->plot_shadow(f);
      legend->path->plot(f);
    }
    // Right wing
    if (innerSpace < 20)
      legend->path->d =
          legend->path->M(X2RelRedun, y + get_point(lastPos[1])) +
          legend->path->V(legend->text[1]->y - 8 -
                          VERT_MIDDLE * legend->text[1]->font_size) +
          legend->path->H((X1RelRedun + X2RelRedun) / 2 + 25);
    else if (innerSpace > 55)
      legend->path->d =
          legend->path->M(X2RelRedun, y + get_point(lastPos[1])) +
          legend->path->V(legend->text[1]->y -
                          VERT_MIDDLE * legend->text[1]->font_size) +
          legend->path->H((X1RelRedun + X2RelRedun) / 2 + 50);
    else
      legend->path->d =
          legend->path->M(X2RelRedun, y + get_point(lastPos[1])) +
          legend->path->V(legend->text[1]->y - 5 -
                          VERT_MIDDLE * legend->text[1]->font_size) +
          legend->path->H((X1RelRedun + X2RelRedun) / 2 + 32);
    // legend->path->plot_shadow(f);
    legend->path->plot(f);

    // Redundancy
    float X1Redun = x - (TITLE_SPACE / 2 + 2 * SPACE_TUNE + 1.5 * periphWidth);
    float X2Redun = x + 2 * seqWidth + innerSpace + TITLE_SPACE / 2 +
                    2 * SPACE_TUNE + 1.5 * periphWidth;
    // Left wing
    if (lastPos.size() == 2) {
      legend->path->d =
          legend->path->M(X1Redun, y + get_point(lastPos[0])) +
          legend->path->V(legend->text[2]->y -
                          VERT_MIDDLE * legend->text[2]->font_size) +
          legend->path->H((X1Redun + X2Redun) / 2 - 32);
      // legend->path->plot_shadow(f);
      legend->path->plot(f);
    }
    // Right wing
    legend->path->d =
        legend->path->M(X2Redun, y + get_point(lastPos[1])) +
        legend->path->V(legend->text[2]->y -
                        VERT_MIDDLE * legend->text[2]->font_size) +
        legend->path->H((X1Redun + X2Redun) / 2 + 32);
    // legend->path->plot_shadow(f);
    legend->path->plot(f);
  }
}

inline void VizPaint::plot_legend_text_vert(
    std::ofstream& f, std::unique_ptr<LegendPlot>& legend) const {
  for (auto& e : legend->text) {
    e->text_anchor = "middle";
    e->font_size = 8;
  }

  if (legend->showRelRedun && !legend->showRedun) {
    legend->text[1]->x = legend->rect->x - legend->labelShift -
                         VERT_MIDDLE * legend->text[1]->font_size;
    legend->text[1]->y = legend->rect->y + legend->rect->height / 2;
    legend->text[1]->transform = "rotate(90 " +
                                 std::to_string(legend->text[1]->x) + " " +
                                 std::to_string(legend->text[1]->y) + ")";
    legend->text[1]->Label = "Relative Redundancy";
    // legend->text[1]->plot_shadow(f);
    legend->text[1]->plot(f);
  } else if (!legend->showRelRedun && legend->showRedun) {
    legend->text[1]->x = legend->rect->x - legend->labelShift -
                         VERT_MIDDLE * legend->text[1]->font_size;
    legend->text[1]->y = legend->rect->y + legend->rect->height / 2;
    legend->text[1]->transform = "rotate(90 " +
                                 std::to_string(legend->text[1]->x) + " " +
                                 std::to_string(legend->text[1]->y) + ")";
    legend->text[1]->Label = "Redundancy";
    // legend->text[1]->plot_shadow(f);
    legend->text[1]->plot(f);
  } else if (legend->showRelRedun && legend->showRedun) {
    legend->text[1]->x = legend->rect->x - legend->labelShift -
                         VERT_MIDDLE * legend->text[1]->font_size;
    legend->text[1]->y = legend->rect->y + legend->rect->height / 2;
    legend->text[1]->transform = "rotate(90 " +
                                 std::to_string(legend->text[1]->x) + " " +
                                 std::to_string(legend->text[1]->y) + ")";
    legend->text[1]->Label = "Relative Redundancy";
    // legend->text[1]->plot_shadow(f);
    legend->text[1]->plot(f);

    // Redundancy
    legend->text[2]->x = legend->rect->x + legend->rect->width +
                         legend->labelShift -
                         VERT_MIDDLE * legend->text[1]->font_size;
    legend->text[2]->y = legend->rect->y + legend->rect->height / 2;
    legend->text[2]->transform = "rotate(90 " +
                                 std::to_string(legend->text[2]->x) + " " +
                                 std::to_string(legend->text[2]->y) + ")";
    legend->text[2]->Label = "Redundancy";
    // legend->text[2]->plot_shadow(f);
    legend->text[2]->plot(f);
  }

  // Numbers (measures)
  // 0.0
  // legend->text[0]->font_weight = "bold";
  legend->text[0]->font_size = 7;
  legend->text[0]->fill = "black";
  legend->text[0]->text_anchor = "middle";
  // legend->text[0]->dominant_baseline = "middle";
  legend->text[0]->dy = VERT_MIDDLE * legend->text[0]->font_size;
  legend->text[0]->x = legend->rect->x + legend->rect->width / 2;
  legend->text[0]->y = legend->rect->y + legend->rect->height + 6;
  legend->text[0]->Label = "0.0";
  legend->text[0]->plot(f);
  // 2.0
  legend->text[0]->text_anchor = "middle";
  legend->text[0]->x = legend->rect->x + legend->rect->width / 2;
  legend->text[0]->y = legend->rect->y - 6;
  legend->text[0]->Label = "2.0";
  legend->text[0]->plot(f);
  // 0.5  1.0  1.5
  for (uint8_t i = 1; i != 4; ++i) {
    legend->text[0]->text_anchor = "middle";
    legend->text[0]->x = legend->rect->x + legend->rect->width / 2;
    legend->text[0]->y =
        legend->rect->y + legend->rect->height - legend->rect->height * i / 4;
    if (legend->colorMode == 1 && i == 3) legend->text[0]->fill = "white";
    legend->text[0]->Label = string_format("%.1f", i * 0.5);
    legend->text[0]->plot(f);
  }

  // for (auto& e : legend->text)
  //   e->transform.clear();
}

inline void VizPaint::plot_legend_path_vert(
    std::ofstream& f, std::unique_ptr<LegendPlot>& legend) const {
  legend->path->stroke = "black";
  legend->path->stroke_width = 0.5;
  legend->path->stroke_dasharray = "8 4";

  if (legend->showRelRedun && !legend->showRedun) {
    float Y1RelRedun = y - (TITLE_SPACE + SPACE_TUNE + 0.5 * periphWidth);
    float Y2RelRedun = y + 2 * seqWidth + innerSpace + TITLE_SPACE +
                       SPACE_TUNE + 0.5 * periphWidth;
    // Top wing
    if (lastPos.size() == 2) {
      if (innerSpace < 31) {
        legend->path->d =
            legend->path->M(x + get_point(lastPos[0]), Y1RelRedun) +
            legend->path->H(legend->text[1]->x -
                            1.5 * legend->text[1]->font_size) +
            legend->path->V(Y1RelRedun + TITLE_SPACE) +
            legend->path->h(legend->text[1]->font_size);
      } else {
        legend->path->d =
            legend->path->M(x + get_point(lastPos[0]), Y1RelRedun) +
            legend->path->H(legend->text[1]->x +
                            VERT_MIDDLE * legend->text[1]->font_size) +
            legend->path->V((Y1RelRedun + Y2RelRedun) / 2 - 47);
        // legend->path->plot_shadow(f);
      }
      legend->path->plot(f);
    }
    // Bottom wing
    if (innerSpace < 31) {
      legend->path->d = legend->path->M(x + get_point(lastPos[1]), Y2RelRedun) +
                        legend->path->H(legend->text[1]->x -
                                        1.5 * legend->text[1]->font_size) +
                        legend->path->V(Y2RelRedun - TITLE_SPACE) +
                        legend->path->h(legend->text[1]->font_size);
    } else {
      legend->path->d =
          legend->path->M(x + get_point(lastPos[1]), Y2RelRedun) +
          legend->path->H(legend->text[1]->x +
                          VERT_MIDDLE * legend->text[1]->font_size) +
          legend->path->V((Y1RelRedun + Y2RelRedun) / 2 + 47);
      // legend->path->plot_shadow(f);
    }
    legend->path->plot(f);
  } else if (!legend->showRelRedun && legend->showRedun) {
    float Y1Redun = y - (TITLE_SPACE + SPACE_TUNE + 0.5 * periphWidth);
    float Y2Redun = y + 2 * seqWidth + innerSpace + TITLE_SPACE + SPACE_TUNE +
                    0.5 * periphWidth;
    // Top wing
    if (lastPos.size() == 2) {
      legend->path->d =
          legend->path->M(x + get_point(lastPos[0]), Y1Redun) +
          legend->path->H(legend->text[1]->x +
                          VERT_MIDDLE * legend->text[1]->font_size) +
          legend->path->V((Y1Redun + Y2Redun) / 2 - 30);
      // legend->path->plot_shadow(f);
      legend->path->plot(f);
    }
    // Bottom wing
    legend->path->d =
        legend->path->M(x + get_point(lastPos[1]), Y2Redun) +
        legend->path->H(legend->text[1]->x +
                        VERT_MIDDLE * legend->text[1]->font_size) +
        legend->path->V((Y1Redun + Y2Redun) / 2 + 30);
    // legend->path->plot_shadow(f);
    legend->path->plot(f);
  } else if (legend->showRelRedun && legend->showRedun) {
    float Y1RelRedun = y - (TITLE_SPACE + SPACE_TUNE + 0.5 * periphWidth);
    float Y2RelRedun = y + 2 * seqWidth + innerSpace + TITLE_SPACE +
                       SPACE_TUNE + 0.5 * periphWidth;
    // Top wing
    if (lastPos.size() == 2) {
      if (innerSpace < 31) {
        legend->path->d =
            legend->path->M(x + get_point(lastPos[0]), Y1RelRedun) +
            legend->path->H(legend->text[1]->x -
                            1.5 * legend->text[1]->font_size) +
            legend->path->V(Y1RelRedun + TITLE_SPACE) +
            legend->path->h(legend->text[1]->font_size);
      } else {
        legend->path->d =
            legend->path->M(x + get_point(lastPos[0]), Y1RelRedun) +
            legend->path->H(legend->text[1]->x +
                            VERT_MIDDLE * legend->text[1]->font_size) +
            legend->path->V((Y1RelRedun + Y2RelRedun) / 2 - 47);
        // legend->path->plot_shadow(f);
      }
      legend->path->plot(f);
    }
    // Bottom wing
    if (innerSpace < 31) {
      legend->path->d = legend->path->M(x + get_point(lastPos[1]), Y2RelRedun) +
                        legend->path->H(legend->text[1]->x -
                                        1.5 * legend->text[1]->font_size) +
                        legend->path->V(Y2RelRedun - TITLE_SPACE) +
                        legend->path->h(legend->text[1]->font_size);
    } else {
      legend->path->d =
          legend->path->M(x + get_point(lastPos[1]), Y2RelRedun) +
          legend->path->H(legend->text[1]->x +
                          VERT_MIDDLE * legend->text[1]->font_size) +
          legend->path->V((Y1RelRedun + Y2RelRedun) / 2 + 47);
      // legend->path->plot_shadow(f);
    }
    legend->path->plot(f);

    // Redundancy
    float Y1Redun = y - (TITLE_SPACE + 2 * SPACE_TUNE + 1.5 * periphWidth);
    float Y2Redun = y + 2 * seqWidth + innerSpace + TITLE_SPACE +
                    2 * SPACE_TUNE + 1.5 * periphWidth;
    // Top wing
    if (lastPos.size() == 2) {
      legend->path->d =
          legend->path->M(x + get_point(lastPos[0]), Y1Redun) +
          legend->path->H(legend->text[2]->x +
                          VERT_MIDDLE * legend->text[2]->font_size) +
          legend->path->V((Y1Redun + Y2Redun) / 2 - 30);
      // legend->path->plot_shadow(f);
      legend->path->plot(f);
    }
    // Bottom wing
    legend->path->d =
        legend->path->M(x + get_point(lastPos[1]), Y2Redun) +
        legend->path->H(legend->text[2]->x +
                        VERT_MIDDLE * legend->text[2]->font_size) +
        legend->path->V((Y1Redun + Y2Redun) / 2 + 30);
    // legend->path->plot_shadow(f);
    legend->path->plot(f);
  }
}

inline std::string VizPaint::tspan(uint32_t start, int64_t pos) const {
  return "<tspan id=\"" + std::to_string(start) +
         "\" style=\"fill:" + rgb_color(start) + "\">" + std::to_string(pos) +
         ", </tspan>\n";
}

inline std::string VizPaint::tspan(uint32_t start, std::string pos) const {
  return "<tspan id=\"" + std::to_string(start) +
         "\" style=\"fill:" + rgb_color(start) + "\">" + pos + ", </tspan>\n";
}

inline void VizPaint::sort_merge(std::string& s) const {
  std::istringstream stream(s);
  std::vector<std::string> vLine;
  for (std::string gl; std::getline(stream, gl);) vLine.push_back(gl);

  if (vLine.size() == 1) {
    s.erase(s.find(", "), 2);
    return;
  }

  std::vector<uint64_t> vID;
  for (const auto& l : vLine) {
    auto foundBeg{l.find("id=", 0) + 4};
    auto foundEnd{l.find("\"", foundBeg + 1)};
    vID.push_back(std::stoull(l.substr(foundBeg, foundEnd)));
  }

  std::vector<int64_t> vPos;
  for (const auto& l : vLine) {
    auto foundBeg{l.find(">", 0) + 1};
    auto foundEnd{l.find("<", foundBeg + 1)};
    auto strPos{l.substr(foundBeg, foundEnd - foundBeg)};
    strPos.erase(strPos.find_last_of(", ") - 1, 2);
    vPos.push_back(std::stoull(strPos));
  }

  struct Env {
    uint64_t id;
    std::string line;
    int64_t pos;
  };

  std::vector<Env> vEnv;
  vEnv.resize(vLine.size());
  for (size_t i = 0; i != vLine.size(); ++i) {
    vEnv[i].id = vID[i];
    vEnv[i].line = vLine[i];
    vEnv[i].pos = vPos[i];
  }
  std::sort(std::begin(vEnv), std::end(vEnv),
            [](const Env& l, const Env& r) { return l.id < r.id; });

  s.clear();
  uint64_t leftOver = vEnv.size();
  for (auto it = std::begin(vEnv); it < std::end(vEnv) - 1;) {
    if (it->id == (it + 1)->id) {
      s += tspan(it->id, std::to_string(it->pos) + "-" +
                             std::to_string((it + 1)->pos)) +
           "\n";
      leftOver -= 2;
      it += 2;
    } else {
      s += it->line + "\n";
      leftOver -= 1;
      it += 1;
    }
  }
  if (leftOver == 1) s += vEnv.back().line + "\n";

  s.erase(s.find_last_of(", <") - 2, 2);
}

inline void VizPaint::save_n_pos(std::string filePath) const {
  std::ifstream inFile(filePath);
  std::ofstream NFile(file_name(filePath) + "." + FMT_N);
  uint64_t pos{0};
  uint64_t beg{0};
  uint64_t num{0};
  bool begun{false};

  for (char c; inFile.get(c); ++pos) {
    if (c == 'N' || c == 'n') {
      if (!begun) {
        begun = true;
        beg = pos;
      }
      ++num;
    } else {
      begun = false;
      if (num != 0) NFile << beg << '\t' << beg + num - 1 << '\n';
      num = 0;
      beg = 0;
    }
  }

  inFile.close();
  NFile.close();
}

inline void VizPaint::read_pos(std::ifstream& fPos, std::vector<Position>& pos,
                               std::unique_ptr<VizParam>& par) const {
  double nr, nt, sr, st;
  char inv;

  for (int64_t br, er, bt, et;
       fPos >> br >> er >> nr >> sr >> bt >> et >> nt >> st >> inv;
       ++par->start)
    pos.push_back(Position(br, er, nr, sr, bt, et, nt, st, inv, par->start));

  if (sr == DBLANK && st == DBLANK) par->showRedun = false;

  // std::sort(begin(pos), end(pos),
  //   [](const Position& l, const Position& r) { return l.begRef < r.begRef;
  //   });
  // const auto last = unique(begin(pos), end(pos),
  //   [](const Position &l, const Position &r) {
  //     return l.begRef==r.begRef && l.endRef==r.endRef;
  //   });
  // pos.erase(last, end(pos));
}

inline void VizPaint::make_posNode(const std::vector<Position>& pos,
                                   std::unique_ptr<VizParam>& par,
                                   std::string&& type) {
  nodes.clear();
  nodes.reserve(2 * pos.size());

  for (auto e : pos) {
    // if (abs(e.endTar - e.begTar) > par->min) {
      if (e.begRef == DBLANK || e.endRef - e.begRef > par->min) {
        if (type == "ref") {
          nodes.push_back(PosNode(e.begRef, 'b', e.start));
          nodes.push_back(PosNode(e.endRef, 'e', e.start));
        } else if (type == "tar") {
          nodes.push_back(
              PosNode(e.begTar, e.endTar > e.begTar ? 'b' : 'e', e.start));
          nodes.push_back(
              PosNode(e.endTar, e.endTar > e.begTar ? 'e' : 'b', e.start));
        }
      }
    // }
  }

  std::sort(std::begin(nodes), std::end(nodes),
            [](const PosNode& l, const PosNode& r) {
              return l.position < r.position;
            });

  plottable = static_cast<bool>(nodes.size());
  if (!plottable) return;

  // mult = par.mult;

  lastPos.push_back(nodes.back().position);
}

// inline void VizPaint::print_pos (ofstream& fPlot, VizParam& par,
// const vector<Position>& pos, uint64_t maxBases, string&& type) {
//   auto   text = make_unique<Text>();
//   string line, lastLine;
//   int64_t    printPos  = 0;
//   char   printType = 'b';
//   uint64_t    nOverlap  = 0;
//   double shiftY    = 4;
//   if (par.showRelRedun && par.showRedun)
//     shiftY += 2 * (SPACE_TUNE + periphWidth);
//   else if (par.showRelRedun ^ par.showRedun)
//     shiftY += SPACE_TUNE + periphWidth;
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

//   for (auto it=begin(nodes); it<end(nodes)-1; ++it) {
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
//       if (it+2 == end(nodes)) {
//         finalLine = tspan((it+1)->start, (it+1)->position);
//         sort_merge(finalLine);
//         printPos = (it+1)->position;

//         text->dominant_baseline="text-after-edge";
//         text->Label  = finalLine;
//         text->x = x + get_point(printPos);
//         text->y = CY;
//         type=="ref" ? print_pos_ref(fPlot, text) : print_pos_tar(fPlot,
//         text);
//       }
//     }

//     if (it+2==end(nodes) && nOverlap!=0) {
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

inline void VizPaint::plot_pos(std::ofstream& fPlot, std::ifstream& fPos,
                               std::vector<Position>& pos,
                               std::unique_ptr<VizParam>& p) {
  read_pos(fPos, pos, p);
  auto posPlot = std::make_unique<PosPlot>();
  posPlot->vertical = p->vertical;
  posPlot->showRelRedun = p->showRelRedun;
  posPlot->showRedun = p->showRedun;
  posPlot->refTick = p->refTick;
  posPlot->tarTick = p->tarTick;
  posPlot->tickHumanRead = p->tickHumanRead;

  make_posNode(pos, p, "ref");
  posPlot->n_bases = n_refBases;
  posPlot->plotRef = true;
  posPlot->vertical ? plot_pos_vertical(fPlot, posPlot)
                    : plot_pos_horizontal(fPlot, posPlot);
  // print_pos(fPlot, p, pos, max(n_refBases,n_tarBases), "ref");

  make_posNode(pos, p, "tar");
  posPlot->n_bases = n_tarBases;
  posPlot->plotRef = false;
  posPlot->vertical ? plot_pos_vertical(fPlot, posPlot)
                    : plot_pos_horizontal(fPlot, posPlot);
  // print_pos(fPlot, p, pos, max(n_refBases,n_tarBases), "tar");

  if (!plottable) exit("No plottable position.\n");
}

inline void VizPaint::plot_pos_horizontal(
    std::ofstream& f, std::unique_ptr<PosPlot>& posPlot) const {
  posPlot->tickLabelSkip = 7;

  auto line = std::make_unique<Line>();
  line->stroke_width = posPlot->majorStrokeWidth;
  line->x1 = x;
  line->x2 = x + get_point(posPlot->n_bases) + 0.5 * line->stroke_width;
  if (posPlot->plotRef)
    line->y1 = line->y2 =
        y - TITLE_SPACE - posPlot->vertSkip -
        (uint8_t(posPlot->showRelRedun) + uint8_t(posPlot->showRedun)) *
            (SPACE_TUNE + periphWidth) -
        posPlot->majorTickSize;
  else
    line->y1 = line->y2 =
        y + 2 * seqWidth + innerSpace + TITLE_SPACE + posPlot->vertSkip +
        (uint8_t(posPlot->showRelRedun) + uint8_t(posPlot->showRedun)) *
            (SPACE_TUNE + periphWidth) +
        posPlot->majorTickSize;
  // line->plot(f);

  auto text = std::make_unique<Text>();
  // Print 0 b
  text->font_size = 7;
  text->text_anchor = "start";
  // text->dominant_baseline = posPlot->plotRef ? "baseline" : "hanging";
  text->x = line->x1 - 1.5;
  if (posPlot->plotRef)
    text->y = line->y1 - posPlot->tickLabelSkip;
  else
    text->y = line->y1 + posPlot->tickLabelSkip +
              VERT_BOTTOM * text->font_size /*font-size of rest*/;
  text->Label = "0 b";
  text->plot(f);

  float majorTick{tick_round(0, posPlot->n_bases, posPlot->n_ranges)};
  if (posPlot->plotRef) {
    if (posPlot->refTick != 0) majorTick = posPlot->refTick;
  } else {
    if (posPlot->tarTick != 0) majorTick = posPlot->tarTick;
  }
  float minorTick{majorTick / posPlot->n_subranges};

  for (float pos = 0; pos <= posPlot->n_bases; pos += minorTick) {
    line->x1 = line->x2 = x + get_point((uint64_t)pos);
    if (pos == 0.0f) {
      line->x1 += 0.5 * line->stroke_width;
      line->x2 = line->x1;
    }

    // Major ticks
    if (static_cast<uint64_t>(std::round(pos)) %
            static_cast<uint64_t>(majorTick) ==
        0) {
      // Line
      line->y2 = posPlot->plotRef ? line->y1 + posPlot->majorTickSize
                                  : line->y1 - posPlot->majorTickSize;
      line->stroke_width = posPlot->majorStrokeWidth;
      line->plot(f);

      // Text
      text->x = line->x1;
      if (posPlot->plotRef)
        text->y = line->y1 - posPlot->tickLabelSkip;
      else
        text->y =
            line->y1 + posPlot->tickLabelSkip + VERT_BOTTOM * text->font_size;
      text->font_weight = "normal";
      text->text_anchor = "middle";
      // text->dominant_baseline = posPlot->plotRef ? "baseline" : "hanging";
      if (posPlot->tickHumanRead)
        text->Label = human_readable_non_cs(uint64_t(std::round(pos)), 1);
      else
        text->Label = std::to_string(uint64_t(std::round(pos)));
      // text->Label = thousands_sep(uint64_t(round(pos)));
      if (pos != 0.0f) text->plot(f);
    } else {  // Minor ticks
      // line->y2 = posPlot->plotRef ? line->y1 + posPlot->minorTickSize
      //                             : line->y1 - posPlot->minorTickSize;
      const auto temp = line->y1;
      line->y1 = posPlot->plotRef ? line->y2 - posPlot->minorTickSize
                                  : line->y2 + posPlot->minorTickSize;
      line->stroke_width = posPlot->minorStrokeWidth;
      line->plot(f);
      line->y1 = temp;
    }
  }
}

inline void VizPaint::plot_pos_vertical(
    std::ofstream& f, std::unique_ptr<PosPlot>& posPlot) const {
  posPlot->tickLabelSkip = 5;

  auto line = std::make_unique<Line>();
  line->stroke_width = posPlot->majorStrokeWidth;
  line->y1 = y;
  line->y2 = y + get_point(posPlot->n_bases) + 0.5 * line->stroke_width;
  if (posPlot->plotRef)
    line->x1 = line->x2 =
        x -
        static_cast<uint8_t>(posPlot->showRelRedun | posPlot->showRedun) *
            TITLE_SPACE / 2 -
        (static_cast<uint8_t>(posPlot->showRelRedun) +
         static_cast<uint8_t>(posPlot->showRedun)) *
            (SPACE_TUNE + periphWidth) -
        posPlot->vertSkip - posPlot->majorTickSize;
  else
    line->x1 = line->x2 =
        x + 2 * seqWidth + innerSpace +
        static_cast<uint8_t>(posPlot->showRelRedun | posPlot->showRedun) *
            TITLE_SPACE / 2 +
        posPlot->vertSkip +
        (static_cast<uint8_t>(posPlot->showRelRedun) +
         static_cast<uint8_t>(posPlot->showRedun)) *
            (SPACE_TUNE + periphWidth) +
        posPlot->majorTickSize;
  // line->plot(f);//todo

  auto text = std::make_unique<Text>();
  text->font_size = 7;

  // // Print bp
  // // text->font_weight = "bold";
  // if (posPlot->plotRef) {
  //   text->text_anchor = "end";
  //   text->x = line->x1 - posPlot->tickLabelSkip;
  // } else {
  //   text->text_anchor = "start";
  //   text->x = line->x1 + posPlot->tickLabelSkip;
  // }
  // text->y = line->y1 + VERT_BOTTOM * 7 /*font size of pos labels*/;
  // // text->dominant_baseline = "middle";
  // text->Label = "0 b";
  // // text->Label = "bp";
  // text->plot(f);

  float majorTick{tick_round(0, posPlot->n_bases, posPlot->n_ranges)};
  if (posPlot->plotRef) {
    if (posPlot->refTick != 0) majorTick = posPlot->refTick;
  } else {
    if (posPlot->tarTick != 0) majorTick = posPlot->tarTick;
  }
  float minorTick{majorTick / posPlot->n_subranges};

  for (float pos = 0; pos <= posPlot->n_bases; pos += minorTick) {
    line->y1 = line->y2 = y + get_point((uint64_t)pos);
    if (pos == 0.0f) {
      line->y1 += 0.5 * line->stroke_width;
      line->y2 = line->y1;
    }

    // Major ticks
    if (static_cast<uint64_t>(round(pos)) % static_cast<uint64_t>(majorTick) ==
        0) {
      // Line
      line->x2 = posPlot->plotRef ? line->x1 + posPlot->majorTickSize
                                  : line->x1 - posPlot->majorTickSize;
      line->stroke_width = posPlot->majorStrokeWidth;
      line->plot(f);

      // Text
      text->x = posPlot->plotRef ? line->x1 - posPlot->tickLabelSkip
                                 : line->x1 + posPlot->tickLabelSkip;
      text->y = line->y1 + VERT_MIDDLE * text->font_size;
      // text->font_size = 7;
      text->font_weight = "normal";
      text->text_anchor = posPlot->plotRef ? "end" : "start";
      // text->dominant_baseline = "middle";
      if (posPlot->tickHumanRead)
        text->Label = human_readable_non_cs(uint64_t(std::round(pos)), 1);
      else
        text->Label = std::to_string(uint64_t(std::round(pos)));
      // text->Label = thousands_sep(uint64_t(round(pos)));
      // if (pos != 0.0f) text->plot(f);
      text->plot(f);
    } else {  // Minor ticks
      // line->x2 = posPlot->plotRef ? line->x1 + posPlot->minorTickSize
      //                             : line->x1 - posPlot->minorTickSize;
      const auto temp = line->x1;
      line->x1 = posPlot->plotRef ? line->x2 - posPlot->minorTickSize
                                  : line->x2 + posPlot->minorTickSize;
      line->stroke_width = posPlot->minorStrokeWidth;
      line->plot(f);
      line->x1 = temp;
    }
  }
}

//todo. modify
inline void VizPaint::plot_Ns(std::ofstream& fPlot, float opacity,
                              bool vertical) const {
  save_n_pos(ref);
  std::ifstream refFile(file_name(ref) + "." + FMT_N);

  for (int64_t beg, end; refFile >> beg >> end;) {
    auto cylinder = std::make_unique<Cylinder>();
    cylinder->stroke_width = 0.75;
    cylinder->fill = cylinder->stroke = "grey";
    cylinder->fill_opacity = cylinder->stroke_opacity = opacity;
    cylinder->width = seqWidth;
    cylinder->height = get_point(end - beg + 1);
    if (vertical) {
      cylinder->x = x;
      cylinder->y = y + get_point(beg);
    } else {
      cylinder->x = x + get_point(beg);
      cylinder->y = y + seqWidth;
      cylinder->transform = "rotate(-90 " + std::to_string(cylinder->x) + " " +
                            std::to_string(cylinder->y) + ")";
    }
    cylinder->id = std::to_string(cylinder->x) + std::to_string(cylinder->y);
    cylinder->plot(fPlot);
  }

  refFile.close();
  remove((file_name(ref) + "." + FMT_N).c_str());

  save_n_pos(tar);
  std::ifstream tarFile(file_name(tar) + "." + FMT_N);

  for (int64_t beg, end; tarFile >> beg >> end;) {
    auto cylinder = std::make_unique<Cylinder>();
    cylinder->stroke_width = 0.75;
    cylinder->fill = cylinder->stroke = "grey";
    cylinder->fill_opacity = cylinder->stroke_opacity = opacity;
    cylinder->width = seqWidth;
    cylinder->height = get_point(end - beg + 1);
    if (vertical) {
      cylinder->x = x + seqWidth + innerSpace;
      cylinder->y = y + get_point(beg);
    } else {
      cylinder->x = x + get_point(beg);
      cylinder->y = y + 2 * seqWidth + innerSpace;
      cylinder->transform = "rotate(-90 " + std::to_string(cylinder->x) + " " +
                            std::to_string(cylinder->y) + ")";
    }
    cylinder->id = std::to_string(cylinder->x) + std::to_string(cylinder->y);
    cylinder->plot(fPlot);
  }

  tarFile.close();
  remove((file_name(tar) + "." + FMT_N).c_str());
}

inline void VizPaint::plot_seq_borders(std::ofstream& f, bool vertical) const {
  auto cylinder = std::make_unique<Cylinder>();
  cylinder->width = seqWidth;
  cylinder->stroke_width = 1;

  auto path = std::make_unique<Path>();
  path->id = "rhs" + std::to_string(x) + std::to_string(y);
  path->fill = path->stroke = "white";
  path->fill_opacity = path->stroke_opacity = 1.0;
  path->stroke_width = cylinder->stroke_width;

  // Reference
  cylinder->height = refSize - cylinder->stroke_width / 2;
  if (vertical) {
    cylinder->x = x;
    cylinder->y = y;
  } else {
    cylinder->x = x;
    cylinder->y = y + seqWidth;
    cylinder->transform = "rotate(-90 " + std::to_string(cylinder->x) + " " +
                          std::to_string(cylinder->y) + ")";
  }
  // cylinder->stroke_dasharray = "8 3";
  path->stroke_dasharray = cylinder->stroke_dasharray;
  path->transform = cylinder->transform;
  path->d =
      path->M(cylinder->x, cylinder->y + cylinder->height - cylinder->ry) +
      path->a(cylinder->width / 2, cylinder->ry, 0, 0, 0, cylinder->width, 0) +
      path->v(cylinder->ry) + path->h(-cylinder->width) +
      path->v(-cylinder->ry) + path->z();
  path->plot(f);
  path->d =
      path->M(cylinder->x + cylinder->width, cylinder->y + cylinder->ry) +
      path->a(cylinder->width / 2, cylinder->ry, 0, 0, 0, -cylinder->width, 0) +
      path->v(-cylinder->ry) + path->h(cylinder->width) +
      path->v(cylinder->ry) + path->z();
  path->plot(f);
  cylinder->plot(f);

  // Target
  cylinder->height = tarSize;
  if (vertical) {
    cylinder->x = x + seqWidth + innerSpace;
    cylinder->y = y;
  } else {
    cylinder->x = x;
    cylinder->y = y + 2 * seqWidth + innerSpace;
    cylinder->transform = "rotate(-90 " + std::to_string(cylinder->x) + " " +
                          std::to_string(cylinder->y) + ")";
  }
  path->stroke_dasharray = cylinder->stroke_dasharray;
  path->transform = cylinder->transform;
  path->d =
      path->M(cylinder->x, cylinder->y + cylinder->height - cylinder->ry) +
      path->a(cylinder->width / 2, cylinder->ry, 0, 0, 0, cylinder->width, 0) +
      path->v(cylinder->ry) + path->h(-cylinder->width) +
      path->v(-cylinder->ry) + path->z();
  path->plot(f);
  path->d =
      path->M(cylinder->x + cylinder->width, cylinder->y + cylinder->ry) +
      path->a(cylinder->width / 2, cylinder->ry, 0, 0, 0, -cylinder->width, 0) +
      path->v(-cylinder->ry) + path->h(cylinder->width) +
      path->v(cylinder->ry) + path->z();
  path->plot(f);
  cylinder->plot(f);
}

inline void VizPaint::print_log(bool stat, std::string image,
                                uint64_t n_regular, uint64_t n_regularSolo,
                                uint64_t n_inverse, uint64_t n_inverseSolo,
                                uint64_t n_ignored) const {
  if (stat) {
    std::string stat_file_name = STAT_NAME;
    if (image != IMAGE) {
      std::string temp = image;
      temp.erase(std::end(temp) - 4, std::end(temp));
      stat_file_name = temp + ".csv";
    }

    std::ofstream stat_file(stat_file_name);
    stat_file << "Regular,RegularSolo,Inverted,InvertedSolo,Ignored\n";
    stat_file << n_regular << ',' << n_regularSolo << ',' << n_inverse << ','
              << n_inverseSolo << ',' << n_ignored << '\n';
    stat_file.close();
  }

  // Count '+' signs needed
  uint8_t n_pluses{0};
  if (n_regular != 0) ++n_pluses;
  if (n_regularSolo != 0) ++n_pluses;
  if (n_inverse != 0) ++n_pluses;
  if (n_inverseSolo != 0) ++n_pluses;

  if (n_pluses != 0) {
    --n_pluses;
    std::cerr << "Plotting finished.\n";
    std::cerr << "Found ";

    if (n_regular != 0) {
      std::cerr << n_regular << " regular";
    }
    if (n_pluses != 0) {
      std::cerr << " + ";
      --n_pluses;
    }
    if (n_regularSolo != 0) {
      std::cerr << n_regularSolo << " solo regular";
    }
    if (n_pluses != 0) {
      std::cerr << " + ";
      --n_pluses;
    }
    if (n_inverse != 0) {
      std::cerr << n_inverse << " inverted";
    }
    if (n_pluses != 0) {
      std::cerr << " + ";
      // --n_pluses;  // No need for the last one
    }
    if (n_inverseSolo != 0) {
      std::cerr << n_inverseSolo << " solo inverted";
    }

    std::cerr << " region"
              << (n_regular + n_regularSolo + n_inverse + n_inverseSolo > 1
                      ? "s"
                      : "")
              << ".\n";

    if (n_ignored != 0)
      std::cerr << "Ignored " << n_ignored << " region"
                << (n_ignored > 1 ? "s" : "") << ".\n";
    std::cerr << '\n';
  } else {
    exit("No plottable position.\n");
  }
}

void VizPaint::set_n_color(std::vector<Position>& pos) {
  std::vector<std::pair<int64_t, int64_t>> history_tar_pos{};
  std::vector<uint8_t> history_n_color{};
  uint8_t color_number = 0;

  for (auto& p : pos) {
    const auto tar_pos = std::make_pair(p.begTar, p.endTar);
    const auto found = std::find(std::begin(history_tar_pos),
                                 std::end(history_tar_pos), tar_pos);
    if (found == std::end(history_tar_pos)) {
      p.n_color = ++color_number;
      history_tar_pos.push_back(std::make_pair(p.begTar, p.endTar));
      history_n_color.push_back(p.n_color);
    } else {
      const auto n_color_idx = static_cast<int64_t>(
          std::distance(std::begin(history_tar_pos), found));
      p.n_color = history_n_color[n_color_idx];
    }
  }
}