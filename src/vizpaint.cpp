#include "vizpaint.hpp"
#include "fn.hpp"
using namespace smashpp;
#define PREC  std::fixed << std::setprecision(2)

/*
 * class Text
 */
inline void Text::plot (ofstream& f) const {
  f << "<text sodipodi:role=\"line\" id=\"tspan3804\" "
    "x=\"" << PREC << origin.x  << "\" "
    "y=\"" << PREC << origin.y  << "\" "
    "dominant-baseline=\"" << dominantBaseline << "\" "
    "transform=\"" << transform << "\" "
    "style=\"font-size:" << to_string(fontSize) << "px;font-style:normal;"
    "font-variant:normal;font-weight:" << fontWeight << ";font-stretch:normal;"
    "fill:" << color << ";fill-opacity:1;text-align:start;line-height:125%%;"
    "writing-mode:lr-tb;text-anchor:" << textAnchor << ";font-family:Arial;"
    "-inkscape-font-specification:Arial\">" << label << "</text>\n";
}

inline void Text::plot_title (ofstream& f) {
  textAnchor = "middle";
  fontSize = 14;
  plot(f);
}

inline void Text::plot_pos_ref (ofstream& f, char c='\0') {
  textAnchor = "end";
  origin.x += -5;
  switch (c) {
    case 'b':  dominantBaseline = "hanging";   break;  // begin
    case 'm':  dominantBaseline = "middle";    break;  // middle
    case 'e':  dominantBaseline = "baseline";  break;  // end
    default:                                   break;
  }
  fontSize = 9;
  plot(f);
}

inline void Text::plot_pos_tar (ofstream& f, char c) {
  textAnchor = "start";
  origin.x += 5;
  switch (c) {
    case 'b':  dominantBaseline = "hanging";   break;  // begin
    case 'm':  dominantBaseline = "middle";    break;  // middle
    case 'e':  dominantBaseline = "baseline";  break;  // end
    default:                                   break;
  }
  fontSize = 9;
  plot(f);
}

/*
 * class Line
 */
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
inline void Rectangle::plot (ofstream& f) const {
  f << "<rect style=\"fill:" << color << ";stroke:" << color <<
    ";fill-opacity:" << opacity << ";stroke-width:1;stroke-miterlimit:4;"
    "stroke-dasharray:none\" id=\"rect3777\" "
    "width=\"" << PREC << width    << "\" height=\"" << PREC << height   <<"\" "
    "x=\""     << PREC << origin.x << "\" y=\""      << PREC << origin.y <<"\" "
    "ry=\"3\" />\n";
}

inline void Rectangle::plot_ir (ofstream& f) const {
  plot(f);
  f << "<rect style=\"fill-opacity:" << opacity << ";stroke-width:2;"
    "stroke-miterlimit:4;stroke-dasharray:none;fill:url(#Wavy);"
    "fill-rule:nonzero;opacity:1\" id=\"rect6217\" "
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
    ";fill-opacity:" << opacity << ";stroke-width:1;stroke-miterlimit:4;"
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

inline void Rectangle::plot_redun
(ofstream& f, u8 showNRC, char refTar = ' ') const {
  f << "<rect style=\"fill:" << color << ";stroke:" << color <<
    ";fill-opacity:" << opacity << ";stroke-width:1;stroke-miterlimit:4;"
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

inline void Rectangle::plot_redun_ref (ofstream& f, bool showNRC) const {
  showNRC ? plot_redun(f, 1, 'r') : plot_redun(f, 0, 'r');
}

inline void Rectangle::plot_redun_tar (ofstream& f, bool showNRC) const {
  showNRC ? plot_redun(f, 1, 't') : plot_redun(f, 0, 't');
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
inline void Circle::plot (ofstream& f) const {
  f << "<circle "
    "cx=\"" << PREC << origin.x << "\" cy=\"" << PREC << origin.y << "\" "
    "r=\""  << PREC << radius   << "\" "
    "fill=\"" << fillColor << "\"/>";
}

/*
 * class VizPaint
 */
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

  print_head(fPlot, PAINT_CX + width + space + width + PAINT_CX,
                    maxSize + PAINT_EXTRA);

  auto line   = make_unique<Line>();
  line->width = 2.0;
  auto rect   = make_unique<Rectangle>();
  rect->opacity = p.opacity;
  auto poly   = make_unique<Polygon>();
  auto text   = make_unique<Text>();

  rect->color  = backColor;
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

  text->origin = Point(cx + width/2, cy - 15);
  text->label  = ref;    //  text->label  = "REF";
  text->plot_title(fPlot);

  text->origin = Point(cx + width + space + width/2, cy - 15);
  text->label  = tar;    //  text->label  = "TAR";
  text->plot_title(fPlot);

  // IF MINIMUM IS SET DEFAULT, RESET TO BASE MAX PROPORTION
  if (p.min==0)  p.min=static_cast<u32>(maxSize / 100);
  p.mult = 256 / file_lines(p.posFile);

  auto customColor = [=] (u32 start) {
    return rgb_color(static_cast<u8>(start * p.mult));
  };

  vector<Pos> pos;
  {
  u64 start {p.start};
  i64 br, er, bt, et;
  for (double nr,nt,sr,st; fPos >> br>>er>>nr>>sr >> bt>>et>>nt>>st; ++start)
    pos.emplace_back(Pos(br, er, nr, sr, bt, et, nt, st, start));

  if (p.showPos) {
    struct Node {
      i64  position;
      char type;
      u64  start;
      Node (i64 p, char t, u64 s) : position(p), type(t), start(s) {}
    };
    
    vector<Node> nodes;    nodes.reserve(2*pos.size());
    for (u64 i=0; i!=pos.size(); ++i) 
      nodes.emplace_back(Node(pos[i].begRef, 'b', pos[i].start));
    for (u64 i=0; i!=pos.size(); ++i) 
      nodes.emplace_back(Node(pos[i].endRef, 'e', pos[i].start));
    std::sort(nodes.begin(), nodes.end(),
      [] (const Node &l, const Node &r) { return l.position < r.position; });

    double X = 0;
    if      (p.showNRC && p.showRedun) X = 2 * (HORIZ_TUNE + width/HORIZ_RATIO);
    else if (p.showNRC ^  p.showRedun) X = HORIZ_TUNE + width/HORIZ_RATIO;
    
    const auto tspan = [] (const string& fill, const string& pos) {
      return "<tspan style=\"fill:" + fill + "\">" + pos + ", </tspan>\n";
    };

    string line, lastLine;
    i64 printPos = 0;
    char printType = 'b';
    u64 nOverlap = 0;
    for (auto it=nodes.begin(); it!=nodes.end(); ++it) {
      if (it->type=='b' && (it+1)->type=='b') {
        if ((it+1)->position - it->position 
            < PAINT_SHORT * max(n_refBases,n_tarBases)) {
          if (++nOverlap==1) { printPos=it->position;  printType=it->type; }
          line += tspan(customColor(it->start), to_string(it->position));
          lastLine = 
            tspan(customColor((it+1)->start), to_string((it+1)->position));
        }
        else {
          if (nOverlap==0)  { printPos=it->position;  printType=it->type; }
          nOverlap = 0;
        }
      }
      else if (it->type=='b' && (it+1)->type=='e') {
        if ((it+1)->position - it->position 
            < PAINT_SHORT * max(n_refBases,n_tarBases)) {
          if (++nOverlap==1)  { printPos=it->position;  printType=it->type; }
          line += tspan(customColor(it->start), to_string(it->position));
          lastLine = 
            tspan(customColor((it+1)->start), to_string((it+1)->position));
        }
        else {
          if (nOverlap==0)  { printPos=it->position;  printType=it->type; }
          nOverlap = 0;
        }
      }
      else if (it->type=='e' && (it+1)->type=='b') {
        if (nOverlap==0)  { printPos=it->position;  printType=it->type; }
        nOverlap = 0;
      }
      else if (it->type=='e' && (it+1)->type=='e') {
        if ((it+1)->position - it->position 
            < PAINT_SHORT * max(n_refBases,n_tarBases)) {
          if (++nOverlap==1)  { printPos=it->position;  printType=it->type; }
          line += tspan(customColor(it->start), to_string(it->position));
          lastLine = 
            tspan(customColor((it+1)->start), to_string((it+1)->position));
        }
        else {
          if (nOverlap==0)  { printPos=it->position;  printType=it->type; }
          nOverlap = 0;
        }
      }

      if (nOverlap == 0) {
        lastLine = tspan(customColor(it->start), to_string(it->position));

        string finalLine {line+lastLine};
        if (!finalLine.empty()) {
          if (finalLine[finalLine.size()-11] == ',') 
            finalLine.erase(finalLine.size()-11, 1);
        }

        // text->fontWeight = "bold";
        if      (printType=='b')  text->dominantBaseline="text-before-edge";
        else if (printType=='e')  text->dominantBaseline="text-after-edge";
        text->origin = Point(cx - X, cy + get_point(printPos));
        text->label = finalLine;
        // cerr<<text->label<<'\n';
        text->plot_pos_ref(fPlot);

        line.clear();
        lastLine.clear();
      }

    }

  }
  } // End of if (p.showPos)

  u64 n_regular=0, n_inverse=0, n_ignored=0;
  for (auto e : pos) {
    if (abs(e.endRef-e.begRef)<p.min || abs(e.begTar-e.endTar)<p.min) {
      ++n_ignored;
      continue;
    }
    
    if (p.showPos) {
      double X = 0;
      if (p.showNRC && p.showRedun)
        X = 2 * (HORIZ_TUNE + width/HORIZ_RATIO);
      else if (p.showNRC ^ p.showRedun)
        X = HORIZ_TUNE + width/HORIZ_RATIO;

      // if (e.endRef-e.begRef < PAINT_SHORT*max(n_refBases,n_tarBases)) {
      //   text->fontWeight = "bold";
      //   text->origin = Point(cx - X,
      //     cy+get_point(e.begRef) + (get_point(e.endRef)-get_point(e.begRef))/2);
      //   text->label = to_string(e.begRef) + " - " + to_string(e.endRef);
      //   text->color = customColor(p.start);
        // text->plot_pos_ref(fPlot, 'm');//todo uncomment
      // }
      // else {
        // text->fontWeight = "bold";
        // text->origin = Point(cx - X, cy + get_point(e.begRef));
        // text->label  = to_string(e.begRef);
        // text->color  = customColor(p.start);
        // text->plot_pos_ref(fPlot, 'b');//todo uncomment
        // text->origin = Point(cx - X, cy + get_point(e.endRef));
        // text->label  = to_string(e.endRef);
        // text->color  = customColor(p.start);
        // text->plot_pos_ref(fPlot, 'e');//todo uncomment
      // }

      if (abs(e.endTar-e.begTar) < PAINT_SHORT*max(n_refBases,n_tarBases)) {
        text->origin = Point(cx + width + space + width + X,
                             cy + get_point(min(e.begTar,e.endTar)) +
                             abs(get_point(e.endTar)-get_point(e.begTar))/2);
        text->label = to_string(min(e.begTar,e.endTar)) + " - " +
                      to_string(max(e.begTar,e.endTar));
        text->color = customColor(p.start);
        text->plot_pos_tar(fPlot, 'm');
      }
      else {
        text->origin = Point(cx + width + space + width + X,
                             cy + get_point(e.begTar));
        text->label  = to_string(e.begTar);
        text->color  = customColor(p.start);
        text->plot_pos_tar(fPlot, (e.endTar>e.begTar ? 'b' : 'e'));
        text->origin = Point(cx + width + space + width + X,
                             cy + get_point(e.endTar));
        text->label  = to_string(e.endTar);
        text->color  = customColor(p.start);
        text->plot_pos_tar(fPlot, (e.endTar>e.begTar ? 'e' : 'b'));
      }
    }

    if (e.endTar > e.begTar) {
      if (p.regular) {
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

        rect->color  = customColor(p.start);
        rect->origin = Point(cx + width + space, cy + get_point(e.begTar));
        rect->height = get_point(e.endTar-e.begTar);
        rect->plot(fPlot);
        if (p.showNRC) {
          rect->color = nrc_color(e.entTar, p.color);
          rect->plot_nrc_tar(fPlot);
        }
        if (p.showRedun) {
          rect->color = redun_color(e.selfTar, p.color);
          rect->plot_redun_tar(fPlot, p.showNRC);
        }

        switch (p.link) {
        case 5:
          poly->lineColor = "grey";
          poly->fillColor = customColor(p.start);
          poly->one   = Point(cx + width,         cy + get_point(e.begRef));
          poly->two   = Point(cx + width,         cy + get_point(e.endRef));
          poly->three = Point(cx + width + space, cy + get_point(e.endTar));
          poly->four  = Point(cx + width + space, cy + get_point(e.begTar));
          poly->plot(fPlot);
          break;
        case 1:
          line->color = "black";
          line->beg = Point(cx + width,
                            cy + get_point(e.begRef+(e.endRef-e.begRef)/2.0));
          line->end = Point(cx + width + space,
                            cy + get_point(e.begTar+(e.endTar-e.begTar)/2.0));
          line->plot(fPlot);
          break;
        case 2:
          line->color = customColor(p.start);
          line->beg = Point(cx + width,
                            cy + get_point(e.begRef+(e.endRef-e.begRef)/2.0));
          line->end = Point(cx + width + space,
                            cy + get_point(e.begTar+(e.endTar-e.begTar)/2.0));
          line->plot(fPlot);
          break;
        case 3:
          line->color = "black";
          line->beg = Point(cx + width,         cy + get_point(e.begRef));
          line->end = Point(cx + width + space, cy + get_point(e.begTar));
          line->plot(fPlot);

          line->beg = Point(cx + width,         cy + get_point(e.endRef));
          line->end = Point(cx + width + space, cy + get_point(e.endTar));
          line->plot(fPlot);
          break;
        case 4:
          line->color = customColor(p.start);
          line->beg = Point(cx + width,         cy + get_point(e.begRef));
          line->end = Point(cx + width + space, cy + get_point(e.begTar));
          line->plot(fPlot);

          line->beg = Point(cx + width,         cy + get_point(e.endRef));
          line->end = Point(cx + width + space, cy + get_point(e.endTar));
          line->plot(fPlot);
          break;
        default:break;
        }
        ++n_regular;
      }
    }
    else {
      if (p.inverse) {
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

        rect->color  = customColor(p.start);
        rect->origin = Point(cx + width + space, cy + get_point(e.endTar));
        rect->height = get_point(e.begTar-e.endTar);
        rect->plot_ir(fPlot);
        if (p.showNRC) {
          rect->color = nrc_color(e.entTar, p.color);
          rect->plot_nrc_tar(fPlot);
        }
        if (p.showRedun) {
          rect->color = redun_color(e.selfTar, p.color);
          rect->plot_redun_tar(fPlot, p.showNRC);
        }

        switch (p.link) {
        case 5:
          poly->lineColor = "grey";
          poly->fillColor = customColor(p.start);
          poly->one   = Point(cx + width,         cy + get_point(e.begRef));
          poly->two   = Point(cx + width,         cy + get_point(e.endRef));
          poly->three = Point(cx + width + space, cy + get_point(e.endTar));
          poly->four  = Point(cx + width + space, cy + get_point(e.begTar));
          poly->plot(fPlot);
          break;
        case 1:
          line->color = "green";
          line->beg = Point(cx + width,
                            cy + get_point(e.begRef+(e.endRef-e.begRef)/2.0));
          line->end = Point(cx + width + space,
                            cy + get_point(e.endTar+(e.begTar-e.endTar)/2.0));
          line->plot(fPlot);
          break;
        case 2:
          line->color = customColor(p.start);
          line->beg = Point(cx + width,
                            cy + get_point(e.begRef+(e.endRef-e.begRef)/2.0));
          line->end = Point(cx + width + space,
                            cy + get_point(e.endTar+(e.begTar-e.endTar)/2.0));
          line->plot(fPlot);
          break;
        case 3:
          line->color = "green";
          line->beg = Point(cx + width,         cy + get_point(e.begRef));
          line->end = Point(cx + width + space, cy + get_point(e.begTar));
          line->plot(fPlot);

          line->beg = Point(cx + width,         cy + get_point(e.endRef));
          line->end = Point(cx + width + space, cy + get_point(e.endTar));
          line->plot(fPlot);
          break;
        case 4:
          line->color = customColor(p.start);
          line->beg = Point(cx + width,         cy + get_point(e.begRef));
          line->end = Point(cx + width + space, cy + get_point(e.begTar));
          line->plot(fPlot);

          line->beg = Point(cx + width,         cy + get_point(e.endRef));
          line->end = Point(cx + width + space, cy + get_point(e.endTar));
          line->plot(fPlot);
          break;
        default:break;
        }
        ++n_inverse;
      }
    }
    ++p.start;
  }
  fPos.seekg(ios::beg);

  rect->width  = width;
  rect->origin = Point(cx, cy);
  rect->height = refSize;
  rect->plot_chromosome(fPlot);

  rect->origin = Point(cx + width + space, cy);
  rect->height = tarSize;
  rect->plot_chromosome(fPlot);

  plot_legend(fPlot, p);

  print_tail(fPlot);

  cerr << "Plotting finished.\n";
  if (p.regular)    cerr << "Found "   << n_regular << " regular regions.\n";
  if (p.inverse)    cerr << "Found "   << n_inverse << " inverted regions.\n";
  // if (p.verbose)    cerr << "Ignored " << n_ignored << " regions.\n";
  cerr << '\n';

  fPos.close();
  fPlot.close();
}

inline void VizPaint::show_info (VizParam& p, const string& ref,
                                 const string& tar, u64 n_refBases,
                                 u64 n_tarBases) const {
  const u8 lblWidth=18, colWidth=8;
  u8 tblWidth=58;
  if (max(n_refBases,n_tarBases) > 999999)
    tblWidth = static_cast<u8>(lblWidth+4*colWidth);
  else
    tblWidth = static_cast<u8>(lblWidth+3*colWidth);

  const auto rule = [] (u8 n, const string& s) {
    for (auto i=n/s.size(); i--;) { cerr<<s; }    cerr<<'\n';
  };
  const auto toprule  = [&] () { rule(tblWidth, "~"); };
  const auto midrule  = [&] () { rule(tblWidth, "~"); };
  const auto botrule  = [&] () { rule(tblWidth, " "); };
  const auto label    = [&] (const string& s) {cerr<<setw(lblWidth)<<left<<s;};
  const auto header   = [&] (const string& s){cerr<<setw(2*colWidth)<<left<<s;};
  const auto design_vals = [&] (char c) {
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
  const auto file_vals = [&] (char c) {
    cerr << setw(2*colWidth) << left;
    switch (c) {
    case '1':  cerr.imbue(locale("en_US.UTF8"));  cerr<<n_refBases;  break;
    case 'r':  cerr<<ref;                                            break;
    case '2':  cerr.imbue(locale("en_US.UTF8"));  cerr<<n_tarBases;  break;
    case 't':  cerr<<tar;                                            break;
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

inline void VizPaint::config
(double width_, double space_, u64 refSize_, u64 tarSize_) {
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

inline string VizPaint::heatmap_color 
(double lambda, const HeatmapColor& heatmap) const {
  // CHANGE BEHAVIOUR [SENSITIVITY: NEAR LOW SIMILARITY // COMMENT 4 UNIFORM
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

inline string VizPaint::nrc_color (double entropy, u32 colorMode) const {
  keep_in_range(0.0, entropy, 2.0);
  // return heatmap_color(entropy/2 * (width+space+width));
  vector<string> colorSet {};
  switch (colorMode) {
  case 0:
    colorSet = {"#2c7bb6", "#00a6ca", "#00ccbc", "#90eb9d", "#ffff8c",
                "#f9d057", "#f29e2e", "#e76818", "#d7191c"};              break;
  case 1:
    colorSet = {"#FFFFDD", "#AAF191", "#80D385", "#61B385", "#3E9583",
                "#217681", "#285285", "#1F2D86", "#000086"};              break;
  case 2:
    colorSet = {"#5E4FA2", "#41799C", "#62A08D", "#9CB598", "#C8CEAD",
                "#E6E6BA", "#E8D499", "#E2B07F", "#E67F5F", "#C55562",
                "#A53A66"};                                               break;
  default:
    error("undefined color mode.");
  }
  return colorSet[entropy / 2 * (colorSet.size() - 1)];
}

inline string VizPaint::redun_color (double entropy, u32 colorMode) const {
  keep_in_range(0.0, entropy, 2.0);
  // return heatmap_color(entropy/2 * (width+space+width));
  vector<string> colorSet {};
  switch (colorMode) {
  case 0:
    colorSet = {"#2c7bb6", "#00a6ca", "#00ccbc", "#90eb9d", "#ffff8c",
                "#f9d057", "#f29e2e", "#e76818", "#d7191c"};              break;
  case 1:
    colorSet = {"#FFFFDD", "#AAF191", "#80D385", "#61B385", "#3E9583",
                "#217681", "#285285", "#1F2D86", "#000086"};              break;
  case 2:
    colorSet = {"#5E4FA2", "#41799C", "#62A08D", "#9CB598", "#C8CEAD",
                "#E6E6BA", "#E8D499", "#E2B07F", "#E67F5F", "#C55562",
                "#A53A66"};                                               break;
  default:
    error("undefined color mode.");
  }
  return colorSet[entropy / 2 * (colorSet.size() - 1)];
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
    "width=\""  << w << "\"\n"
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
  auto rect    = make_unique<Rectangle>();
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

  auto text    = make_unique<Text>();
  text->origin = Point(rect->origin.x+rect->width/2, rect->origin.y);
  text->textAnchor = "middle";
  text->fontWeight = "bold";
  text->dominantBaseline = "text-after-edge";
  text->label  = "RELATIVE REDUNDANCY";
  text->fontSize   = 9;
  text->plot(f);

  auto grad = make_unique<Gradient>();
  switch (p.color) {
  case 0:
    grad->offsetColor = {"#2c7bb6", "#00a6ca", "#00ccbc", "#90eb9d", "#ffff8c",
                "#f9d057", "#f29e2e", "#e76818", "#d7191c"};              break;
  case 1:
    grad->offsetColor = {"#FFFFDD", "#AAF191", "#80D385", "#61B385", "#3E9583",
                "#217681", "#285285", "#1F2D86", "#000086"};              break;
  case 2:
    grad->offsetColor = {"#5E4FA2", "#41799C", "#62A08D", "#9CB598", "#C8CEAD",
                "#E6E6BA", "#E8D499", "#E2B07F", "#E67F5F", "#C55562",
                "#A53A66"};                                               break;
  default:
    error("undefined color mode.");
  }

  const auto offset = [&] (u8 i) { 
    return to_string(i * 100 / (grad->offsetColor.size() - 1)) + "%";
  };
  auto id = to_string(rect->origin.x) + to_string(rect->origin.y);
  f << "<defs><linearGradient id=\"grad"+id+"\"  "
    "x1=\"0%\" y1=\"0%\" x2=\"100%\" y2=\"0%\"> ";
  for (u8 i=0; i!=grad->offsetColor.size(); ++i) {
    f << "<stop offset=\"" << offset(i) << "\" "
         "style=\"stop-color:" << grad->offsetColor[i] << ";stop-opacity:1\"/>";
  }
  f << "</linearGradient> </defs>"
    "<rect fill=\"url(#grad"+id+")\" "
    "width=\""  << PREC << rect->width    << "\" "
    "height=\"" << PREC << rect->height   << "\" "
    "x=\""      << PREC << rect->origin.x << "\" "
    "y=\""      << PREC << rect->origin.y << "\" ry=\"3\" />\n";

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
      // Point(rect->origin.x+(rect->width*i)/4, rect->origin.y+rect->height);
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
  // rect->origin = Point(cx-2*(HORIZ_TUNE+width/HORIZ_RATIO), 
  //                      vert+rect->height+12);
  // text->origin = Point(rect->origin.x+rect->width/2, 
  //                      rect->origin.y+rect->height);
  text->origin = Point(rect->origin.x+rect->width/2, 
                       rect->origin.y+rect->height);
  text->dominantBaseline = "text-before-edge";
  text->label  = "REDUNDANCY";
  text->fontSize   = 9;
  text->fontWeight = "bold";
  text->plot(f);

  // switch (colorMode) {
  // case 0:
  //   grad->offsetColor = {"#2c7bb6", "#00a6ca", "#00ccbc", "#90eb9d", "#ffff8c",
  //                        "#f9d057", "#f29e2e", "#e76818", "#d7191c"};     break;
  // case 1:
  //   grad->offsetColor = {"#FFFFDD", "#AAF191", "#80D385", "#61B385", "#3E9583",
  //                        "#217681", "#285285", "#1F2D86", "#000086"};     break;
  // case 2:
  //   grad->offsetColor = {"#5E4FA2", "#41799C", "#62A08D", "#9CB598", "#C8CEAD",
  //                        "#E6E6BA", "#E8D499", "#E2B07F", "#E67F5F", "#C55562",
  //                        "#A53A66"};                                      break;
  // default:
  //   error("undefined color mode.");
  // }
  // id = to_string(rect->origin.x) + to_string(rect->origin.y);
  // f << "<defs> <linearGradient id=\"grad"+id+"\" "
  //   "x1=\"0%\" y1=\"0%\" x2=\"100%\" y2=\"0%\"> ";
  // for (u8 i=0; i!=grad->offsetColor.size(); ++i) {
  //   f << "<stop offset=\"" << offset(i) << "\" "
  //        "style=\"stop-color:" << grad->offsetColor[i] << ";stop-opacity:1\"/>";
  // }
  // f << "</linearGradient> </defs>"
  //   "<rect fill=\"url(#grad"+id+")\" "
  //   "width=\""  << PREC << rect->width    << "\" "
  //   "height=\"" << PREC << rect->height   << "\" "
  //   "x=\""      << PREC << rect->origin.x << "\" "
  //   "y=\""      << PREC << rect->origin.y << "\" ry=\"3\" />\n";
}