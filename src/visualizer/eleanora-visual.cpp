#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include "mem.h"
#include "time.h"
#include "defs.h"
#include "param.h"
#include "msg.h"
#include "common.h"
#include "paint.h"
#include <iostream>
#include <fstream>
#include "../fn.hpp"
using namespace std;
using namespace smashpp;
Parameters* P;

// - - - - - - - - - - - - - - - - - - P L O T - - - - - - - - - - - - - - - -
void PrintPlot
(string posFile, u32 width, u32 space, u32 mult, u32 start, u64 minimum) {
  string backColor = "#ffffff";
  i64 tarNBases=0, refNBases=0;
  string watermark;
  Painter* Paint;
  check_file(posFile);
  check_file(P->image);
  ifstream POS(posFile);
  ofstream PLOT(P->image);

  POS >> watermark >> refNBases >> tarNBases;
  if (watermark != "#SCF")
    error("unknown positions file format!");

  if (P->verbose) {
    cerr <<
      "==[ CONFIGURATION ]================="                              <<"\n"
      "Verbose mode ....................... yes"                          <<"\n"
      "Reference number of bases .......... " << refNBases                <<"\n"
      "Target number of bases ............. " << tarNBases                <<"\n"
      "Link type .......................... " << P->link                  <<"\n"
      "Chromosomes design characteristics:"                               <<"\n"
      "  [+] Width ........................ " << width                    <<"\n"
      "  [+] Space ........................ " << space                    <<"\n"
      "  [+] Multiplication factor ........ " << mult                     <<"\n"
      "  [+] Begin ........................ " << start                    <<"\n"
      "  [+] Minimum ...................... " << minimum                  <<"\n"
      "  [+] Show regular ................. " << (P->regular  ?"yes":"no")<<"\n"
      "  [+] Show inversions .............. " << (P->inversion?"yes":"no")<<"\n"
      "Output map filename ................ " << P->image                 <<"\n"
                                                                        << endl;
  }

  cerr << "==[ PROCESSING ]====================\n"
          "Printing plot ...\n";

  set_ratio(MAX(refNBases, tarNBases) / DEFAULT_SCALE);
  Paint = create_painter(get_point(refNBases), get_point(tarNBases),
                         (double) width, (double) space, backColor);

  print_head(PLOT, (2 * DEFAULT_CX) + (((Paint->width + Paint->space) * 2) -
                                       Paint->space), Paint->maxSize + EXTRA);
  rect(PLOT, (2 * DEFAULT_CX) + (((Paint->width + Paint->space) * 2) -
                                 Paint->space), Paint->maxSize + EXTRA, 0, 0,
       backColor);
  rect_oval(PLOT, Paint->width, Paint->refSize, Paint->cx, Paint->cy, backColor);
  rect_oval(PLOT, Paint->width, Paint->tarSize, Paint->cx, Paint->cy, backColor);
  text(PLOT, Paint->cx, Paint->cy - 15, "REF");
  text(PLOT, Paint->cx + Paint->width + Paint->space, Paint->cy - 15, "TAR");

  // IF MINIMUM IS SET DEFAULT, RESET TO BASE MAX PROPORTION
  if (minimum == 0)
    minimum = MAX(refNBases,tarNBases) / 100;

  i64 begPosTar, endPosTar, begPosRef, endPosRef;
  u64 regular=0, inverse=0, ignored=0;
  string entTar, entRef;
  while (POS >> begPosTar>>endPosTar>>entTar>>begPosRef>>endPosRef>>entRef) {
//    if (fscanf(POS, "%s\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\t%s\t"
//        "%"PRIi64"\t""%"PRIi64"\t%"PRIi64"\t%"PRIi64"\n",
//        tmp1, &ci, &cf, &begPosTar, &endPosTar, tmp2, &ri, &rf, &begPosRef,
// &endPosRef) != 10)
//      break;

    if (labs(endPosRef-begPosRef) < minimum ||
        labs(begPosTar-endPosTar) < minimum) {
      ++ignored;
      continue;
    }

    if (endPosRef > begPosRef) {
      if (P->regular) {
        switch (P->link) {
        case 1:
          line(PLOT, 2, Paint->cx + Paint->width, Paint->cy +
                 get_point(begPosRef +
                           ((endPosRef -
                             begPosRef) / 2.0)),
               Paint->cx + Paint->space + Paint->width, Paint->cy +
                 get_point(begPosTar +
                           ((endPosTar -
                             begPosTar) /
                            2.0)),
               (char*) "black");
          break;
        case 2:
          line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy +
                 get_point(begPosRef + ((endPosRef - begPosRef) / 2.0)),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy +
                 get_point(begPosTar + ((endPosTar - begPosTar) / 2.0)),
               get_rgb_color(start * mult));
          break;
        case 3:
          line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + get_point(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + get_point(begPosTar), (char*) "black");
          line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + get_point(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + get_point(endPosTar), (char*) "black");
          break;
        case 4:
          line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + get_point(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + get_point(begPosTar),
               get_rgb_color(start * mult));
          line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + get_point(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + get_point(endPosTar),
               get_rgb_color(start * mult));
          break;
        case 5:
          polygon(PLOT,
                  Paint->cx + Paint->width,
                  Paint->cy + get_point(begPosRef),
                  Paint->cx + Paint->width,
                  Paint->cy + get_point(endPosRef),
                  Paint->cx + Paint->space + Paint->width,
                  Paint->cy + get_point(endPosTar),
                  Paint->cx + Paint->space + Paint->width,
                  Paint->cy + get_point(begPosTar),
                  get_rgb_color(start * mult), "grey");
          break;
        default:break;
        }

        rect(PLOT, Paint->width, get_point(endPosRef - begPosRef), Paint->cx,
             Paint->cy + get_point(begPosRef), get_rgb_color(start * mult));

        rect(PLOT, Paint->width, get_point(endPosTar - begPosTar),
             Paint->cx + Paint->space + Paint->width,
             Paint->cy + get_point(begPosTar), get_rgb_color(start * mult));

        ++regular;
      }
    }
    else {
      if (P->inversion) {
        switch (P->link) {
        case 1:
          line(PLOT, 2, Paint->cx + Paint->width, Paint->cy +
                 get_point(endPosRef +
                           ((begPosRef -
                             endPosRef) / 2.0)),
               Paint->cx + Paint->space + Paint->width, Paint->cy +
                 get_point(endPosTar +
                           ((begPosTar -
                             endPosTar) /
                            2.0)),
               (char*) "green");
          break;
        case 2:
          line(PLOT, 2, Paint->cx + Paint->width, Paint->cy +
                 get_point(endPosRef +
                           ((begPosRef -
                             endPosRef) / 2.0)),
               Paint->cx + Paint->space + Paint->width, Paint->cy +
                 get_point(endPosTar +
                           ((begPosTar -
                             endPosTar) /
                            2.0)),
               get_rgb_color(start * mult));
          break;
        case 3:
          line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + get_point(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + get_point(endPosTar), (char*) "green");
          line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + get_point(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + get_point(begPosTar), (char*) "green");
          break;
        case 4:
          line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + get_point(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + get_point(endPosTar),
               get_rgb_color(start * mult));
          line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + get_point(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + get_point(begPosTar),
               get_rgb_color(start * mult));
          break;
        case 5:
          polygon(PLOT,
                  Paint->cx + Paint->width, Paint->cy + get_point(endPosRef),
                  Paint->cx + Paint->width, Paint->cy + get_point(begPosRef),
                  Paint->cx + Paint->space + Paint->width,
                  Paint->cy + get_point(begPosTar),
                  Paint->cx + Paint->space + Paint->width,
                  Paint->cy + get_point(endPosTar),
                  get_rgb_color(start * mult), "grey");
          break;
        default:break;
        }

        rect(PLOT, Paint->width, get_point(begPosRef - endPosRef), Paint->cx,
             Paint->cy + get_point(endPosRef), get_rgb_color(start * mult));

        rect_ir(PLOT, Paint->width, get_point(endPosTar - begPosTar),
                Paint->cx + Paint->space + Paint->width,
                Paint->cy + get_point(begPosTar), get_rgb_color(start * mult));

        ++inverse;
      }
    }

    ++start;
  }
//  rewind(POS);
  POS.seekg(ios::beg);

  if (P->regular)    cerr << "Found "   << regular << " regular regions.\n";
  if (P->inversion)  cerr << "Found "   << inverse << " inverted regions.\n";
  if (P->verbose)    cerr << "Ignored " << ignored << " regions.\n";

  chromosome(PLOT, Paint->width, Paint->refSize, Paint->cx, Paint->cy);
  chromosome(PLOT, Paint->width, Paint->tarSize, Paint->cx + Paint->space +
                                                 Paint->width, Paint->cy);
  print_final(PLOT);
  POS.close();

  cerr << "Done!                       \n";
}

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int main (int argc, char* argv[]) {
  char** p = *&argv;
  u32 width, space, mult, start, minimum;

  P = (Parameters*) Malloc(1 * sizeof(Parameters));
  if ((P->help = args_state(DEF_HELP, p, argc, "-h")) || argc < 2) {
    print_menu_visual();
    return EXIT_SUCCESS;
  }

  if (args_state(DEF_VERSION, p, argc, "-V")) {
    print_version();
    return EXIT_SUCCESS;
  }

  P->verbose   = args_state(DEF_VERBOSE, p, argc, "-v");
  P->force     = args_state(DEF_FORCE, p, argc, "-F");
  P->link      = args_num(DEF_LINK, p, argc, "-l", MIN_LINK, MAX_LINK);
  width        = args_num(DEF_WIDT, p, argc, "-w", MIN_WIDT, MAX_WIDT);
  space        = args_num(DEF_SPAC, p, argc, "-s", MIN_SPAC, MAX_SPAC);
  mult         = args_num(DEF_MULT, p, argc, "-m", MIN_MULT, MAX_MULT);
  start        = args_num(DEF_BEGI, p, argc, "-b", MIN_BEGI, MAX_BEGI);
  minimum      = args_num(DEF_MINP, p, argc, "-c", MIN_MINP, MAX_MINP);
  P->inversion = args_state(DEF_INVE, p, argc, "-i");
  P->regular   = args_state(DEF_REGU, p, argc, "-r");
  P->image     = args_files_img(p, argc, "-o");

  cerr << "\n";
  TIME* Time = create_clock(clock());
  PrintPlot(string(argv[argc-1]), width, space, mult, start, minimum);
  stop_time_ndrm(Time, clock());
  cerr << "\n";

  cerr << "==[ STATISTICS ]====================\n";
  stop_calc_all(Time, clock());
  cerr << "\n";
  remove_clock(Time);

  return EXIT_SUCCESS;
}