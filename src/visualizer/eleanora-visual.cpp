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
  char backColor[] = "#ffffff";
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

  SetRatio(MAX(refNBases,tarNBases) / DEFAULT_SCALE);
  Paint = CreatePainter(GetPoint(refNBases), GetPoint(tarNBases),
                        (double) width, (double) space, backColor);

  PrintHead(PLOT, (2*DEFAULT_CX) + (((Paint->width+Paint->space) * 2) -
            Paint->space), Paint->maxSize + EXTRA);
  Rect(PLOT, (2*DEFAULT_CX) + (((Paint->width+Paint->space) * 2) -
       Paint->space), Paint->maxSize + EXTRA, 0, 0, backColor);
  RectOval(PLOT, Paint->width, Paint->refSize, Paint->cx, Paint->cy, backColor);
  RectOval(PLOT, Paint->width, Paint->tarSize, Paint->cx, Paint->cy, backColor);
  Text(PLOT, Paint->cx, Paint->cy - 15, (char*) "REF");
  Text(PLOT, Paint->cx+Paint->width+Paint->space, Paint->cy-15, (char*) "TAR");

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
          Line(PLOT, 2, Paint->cx + Paint->width, Paint->cy +
               GetPoint(begPosRef + ((endPosRef-begPosRef)/2.0)),
               Paint->cx + Paint->space + Paint->width, Paint->cy +
               GetPoint(begPosTar + ((endPosTar-begPosTar)/2.0)),
               (char*) "black");
          break;
        case 2:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy +
               GetPoint(begPosRef + ((endPosRef - begPosRef) / 2.0)),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy +
               GetPoint(begPosTar + ((endPosTar - begPosTar) / 2.0)),
               GetRgbColor(start * mult));
          break;
        case 3:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(begPosTar), (char*) "black");
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(endPosTar), (char*) "black");
          break;
        case 4:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(begPosTar),
               GetRgbColor(start * mult));
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(endPosTar),
               GetRgbColor(start * mult));
          break;
        case 5:
          Polygon(PLOT,
                  Paint->cx + Paint->width,
                  Paint->cy + GetPoint(begPosRef),
                  Paint->cx + Paint->width,
                  Paint->cy + GetPoint(endPosRef),
                  Paint->cx + Paint->space + Paint->width,
                  Paint->cy + GetPoint(endPosTar),
                  Paint->cx + Paint->space + Paint->width,
                  Paint->cy + GetPoint(begPosTar),
                  GetRgbColor(start * mult), "grey");
          break;
        default:break;
        }

        Rect(PLOT, Paint->width, GetPoint(endPosRef-begPosRef), Paint->cx,
             Paint->cy + GetPoint(begPosRef), GetRgbColor(start*mult));

        Rect(PLOT, Paint->width, GetPoint(endPosTar-begPosTar),
             Paint->cx + Paint->space + Paint->width,
             Paint->cy + GetPoint(begPosTar), GetRgbColor(start*mult));

        ++regular;
      }
    }
    else {
      if (P->inversion) {
        switch (P->link) {
        case 1:
          Line(PLOT, 2, Paint->cx + Paint->width, Paint->cy +
               GetPoint(endPosRef + ((begPosRef-endPosRef)/2.0)),
               Paint->cx + Paint->space + Paint->width, Paint->cy +
               GetPoint(endPosTar + ((begPosTar-endPosTar)/2.0)),
               (char*) "green");
          break;
        case 2:
          Line(PLOT, 2, Paint->cx + Paint->width, Paint->cy +
               GetPoint(endPosRef + ((begPosRef-endPosRef)/2.0)),
               Paint->cx + Paint->space + Paint->width, Paint->cy +
               GetPoint(endPosTar + ((begPosTar-endPosTar)/2.0)),
               GetRgbColor(start*mult));
          break;
        case 3:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(endPosTar), (char*) "green");
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(begPosTar), (char*) "green");
          break;
        case 4:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(endPosTar),
               GetRgbColor(start*mult));
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(begPosTar),
               GetRgbColor(start*mult));
          break;
        case 5:
          Polygon(PLOT,
                  Paint->cx + Paint->width, Paint->cy + GetPoint(endPosRef),
                  Paint->cx + Paint->width, Paint->cy + GetPoint(begPosRef),
                  Paint->cx + Paint->space + Paint->width,
                  Paint->cy + GetPoint(begPosTar),
                  Paint->cx + Paint->space + Paint->width,
                  Paint->cy + GetPoint(endPosTar),
                  GetRgbColor(start*mult), "grey");
          break;
        default:break;
        }

        Rect(PLOT, Paint->width, GetPoint(begPosRef-endPosRef), Paint->cx,
             Paint->cy + GetPoint(endPosRef), GetRgbColor(start*mult));

        RectIR(PLOT, Paint->width, GetPoint(endPosTar-begPosTar),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(begPosTar), GetRgbColor(start*mult));

        ++inverse;
      }
    }

    ++start;
  }
//  rewind(POS);
  POS.seekg(ios::beg);

  if (P->regular)    cerr << "Found " << regular << " regular regions.\n";
  if (P->inversion)  cerr << "Found " << inverse << " inverted regions.\n";
  if (P->verbose)    cerr << "Ignored " << ignored << " regions.\n";

  Chromosome(PLOT, Paint->width, Paint->refSize, Paint->cx, Paint->cy);
  Chromosome(PLOT, Paint->width, Paint->tarSize, Paint->cx + Paint->space +
                                                 Paint->width, Paint->cy);
  PrintFinal(PLOT);
  POS.close();

  cerr << "Done!                       \n";
}

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
i32 main (int argc, char* argv[]) {
  char** p = *&argv;
  u32 width, space, mult, start, minimum;

  P = (Parameters*) Malloc(1 * sizeof(Parameters));
  if ((P->help = ArgsState(DEF_HELP, p, argc, "-h")) || argc < 2) {
    PrintMenuVisual();
    return EXIT_SUCCESS;
  }

  if (ArgsState(DEF_VERSION, p, argc, "-V")) {
    PrintVersion();
    return EXIT_SUCCESS;
  }

  P->verbose   = ArgsState(DEF_VERBOSE, p, argc, "-v");
  P->force     = ArgsState(DEF_FORCE,   p, argc, "-F");
  P->link      = ArgsNum  (DEF_LINK,    p, argc, "-l", MIN_LINK, MAX_LINK);
  width        = ArgsNum  (DEF_WIDT,    p, argc, "-w", MIN_WIDT, MAX_WIDT);
  space        = ArgsNum  (DEF_SPAC,    p, argc, "-s", MIN_SPAC, MAX_SPAC);
  mult         = ArgsNum  (DEF_MULT,    p, argc, "-m", MIN_MULT, MAX_MULT);
  start        = ArgsNum  (DEF_BEGI,    p, argc, "-b", MIN_BEGI, MAX_BEGI);
  minimum      = ArgsNum  (DEF_MINP,    p, argc, "-c", MIN_MINP, MAX_MINP);
  P->inversion = ArgsState(DEF_INVE,    p, argc, "-i");
  P->regular   = ArgsState(DEF_REGU,    p, argc, "-r");
  P->image     = ArgsFilesImg          (p, argc, "-o");

  cerr << "\n";
  TIME* Time = CreateClock(clock());
  PrintPlot(string(argv[argc-1]), width, space, mult, start, minimum);
  StopTimeNDRM(Time, clock());
  cerr << "\n";

  cerr << "==[ STATISTICS ]====================\n";
  StopCalcAll(Time, clock());
  cerr << "\n";
  RemoveClock(Time);

  return EXIT_SUCCESS;
}