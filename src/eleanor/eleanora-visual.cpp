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
Parameters *P;


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - P L O T - - - - - - - - - - - - - - - -
void PrintPlot (char *posFile, uint32_t width, uint32_t space, uint32_t mult,
                uint32_t start, uint64_t minimum) {
//  FILE *PLOT = NULL, *POS = NULL;
  FILE *PLOT = NULL;
  char backColor[] = "#ffffff";
  int64_t conNBases = 0, refNBases = 0;
//  char watermark[MAX_FILENAME];
  string watermark;
  Painter *Paint;
  check_file((string)posFile);
  ifstream POS(posFile);
  PLOT = Fopen(P->image, "w");

//  if (fscanf(POS, "%s\t%"PRIi64"\t%"PRIi64"\n", watermark, &conNBases,
//             &refNBases) != 3 || watermark[0] != '#' || watermark[1] != 'S' ||
//      watermark[2] != 'C' || watermark[3] != 'F') {
  POS >> watermark >> refNBases >> conNBases;
  if (watermark != "#SCF") {
    fprintf(stderr, "[x] Error: unknown positions file format!\n");
    exit(1);
  }

  if (P->verbose) {
    fprintf(stderr, "==[ CONFIGURATION ]=================\n");
    fprintf(stderr, "Verbose mode ....................... yes\n");
    fprintf(stderr, "Reference number of bases .......... %"PRIu64"\n",
            refNBases);
    fprintf(stderr, "Target number of bases ............. %"PRIu64"\n",
            conNBases);
    fprintf(stderr, "Link type .......................... %u\n", P->link);
    fprintf(stderr, "Chromosomes design characteristics:\n");
    fprintf(stderr, "  [+] Width ........................ %u\n", width);
    fprintf(stderr, "  [+] Space ........................ %u\n", space);
    fprintf(stderr, "  [+] Multiplication factor ........ %u\n", mult);
    fprintf(stderr, "  [+] Begin ........................ %u\n", start);
    fprintf(stderr, "  [+] Minimum ...................... %"PRIi64"\n",
            minimum);
    fprintf(stderr, "  [+] Show regular ................. %s\n", P->regular ?
                                                                 "yes" : "no");
    fprintf(stderr, "  [+] Show inversions .............. %s\n", P->inversion ?
                                                                 "yes" : "no");
    fprintf(stderr, "Output map filename ................ %s\n", P->image);
    fprintf(stderr, "\n");
  }

  fprintf(stderr, "==[ PROCESSING ]====================\n");
  fprintf(stderr, "Printing plot ...\n");

  SetRatio(MAX(refNBases, conNBases) / DEFAULT_SCALE);
  Paint = CreatePainter(GetPoint(refNBases), GetPoint(conNBases),
                        (double) width, (double) space, backColor);

  PrintHead(PLOT, (2 * DEFAULT_CX) + (((Paint->width + Paint->space) * 2) -
                                      Paint->space), Paint->maxSize + EXTRA);
  Rect(PLOT, (2 * DEFAULT_CX) + (((Paint->width + Paint->space) * 2) -
  Paint->space), Paint->maxSize + EXTRA, 0, 0, backColor);
  RectOval(PLOT, Paint->width, Paint->refSize, Paint->cx, Paint->cy,
           backColor);
  RectOval(PLOT, Paint->width, Paint->tarSize, Paint->cx, Paint->cy,
           backColor);
  Text(PLOT, Paint->cx, Paint->cy - 15, (char*)"REF");
  Text(PLOT, Paint->cx + Paint->width + Paint->space, Paint->cy - 15,
       (char *) "TAR");

  // IF MINIMUM IS SET DEFAULT, RESET TO BASE MAX PROPORTION
  if (minimum == 0)
    minimum = MAX(refNBases, conNBases) / 100;

  int64_t begPosTar, endPosTar, begPosRef, endPosRef;
  uint64_t regular = 0, inverse = 0, ignored = 0;
  string entTar, entRef;
//  char tmp1[MAX_STR] = {'\0'}, tmp2[MAX_STR] = {'\0'};
  while (POS >> begPosRef>>endPosRef>>entRef>>begPosTar>>endPosTar>>entTar) {
//    if (fscanf(POS, "%s\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\t%s\t"
//        "%"PRIi64"\t""%"PRIi64"\t%"PRIi64"\t%"PRIi64"\n",
//        tmp1, &ci, &cf, &begPosTar, &endPosTar, tmp2, &ri, &rf, &begPosRef,
// &endPosRef) != 10)
//      break;

    if (labs(endPosRef - begPosRef) < minimum || labs(begPosTar - endPosTar) < minimum) {
      ++ignored;
      continue;
    }

    if (endPosRef > begPosRef) {
      if (P->regular) {
        switch (P->link) {
        case 1:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(begPosRef + ((endPosRef - begPosRef) / 2.0)),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(begPosTar + ((endPosTar - begPosTar) / 2.0)), (char*)"black");
          break;
        case 2:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(begPosRef + ((endPosRef - begPosRef) / 2.0)),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(begPosTar + ((endPosTar - begPosTar) / 2.0)),
               GetRgbColor(start * mult));
          break;
        case 3:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(begPosTar), (char*)"black");
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(endPosTar), (char*)"black");
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
                  GetRgbColor(start * mult), (char*)"grey");
          break;
        default:break;
        }

        Rect(PLOT, Paint->width, GetPoint(endPosRef - begPosRef), Paint->cx, Paint->cy +
                                                               GetPoint(begPosRef),
             GetRgbColor(start * mult));

        Rect(PLOT, Paint->width, GetPoint(endPosTar - begPosTar), Paint->cx + Paint->space +
                                                    Paint->width,
             Paint->cy + GetPoint(begPosTar), GetRgbColor(start * mult));

        ++regular;
      }
    }
    else {
      if (P->inversion) {
        switch (P->link) {
        case 1:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(endPosRef + ((begPosRef - endPosRef) / 2.0)),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(endPosTar + ((begPosTar - endPosTar) / 2.0)), (char*)"green");
          break;
        case 2:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(endPosRef + ((begPosRef - endPosRef) / 2.0)),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(endPosTar + ((begPosTar - endPosTar) / 2.0)),
               GetRgbColor(start * mult));
          break;
        case 3:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(endPosTar), (char*)"green");
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(begPosTar), (char*)"green");
          break;
        case 4:
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(endPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(endPosTar),
               GetRgbColor(start * mult));
          Line(PLOT, 2, Paint->cx + Paint->width,
               Paint->cy + GetPoint(begPosRef),
               Paint->cx + Paint->space + Paint->width,
               Paint->cy + GetPoint(begPosTar),
               GetRgbColor(start * mult));
          break;
        case 5:
          Polygon(PLOT,
                  Paint->cx + Paint->width,
                  Paint->cy + GetPoint(endPosRef),
                  Paint->cx + Paint->width,
                  Paint->cy + GetPoint(begPosRef),
                  Paint->cx + Paint->space + Paint->width,
                  Paint->cy + GetPoint(begPosTar),
                  Paint->cx + Paint->space + Paint->width,
                  Paint->cy + GetPoint(endPosTar),
                  GetRgbColor(start * mult), (char*)"grey");
          break;
        default:break;
        }

        Rect(PLOT, Paint->width, GetPoint(begPosRef - endPosRef), Paint->cx, Paint->cy +
                                                               GetPoint(endPosRef),
             GetRgbColor(start * mult));

        RectIR(PLOT, Paint->width, GetPoint(endPosTar - begPosTar), Paint->cx + Paint->space +
                                                      Paint->width,
               Paint->cy + GetPoint(begPosTar), GetRgbColor(start * mult));

        ++inverse;
      }
    }

    ++start;
  }
//  rewind(POS);
  POS.seekg(ios::beg);

  if (P->regular)
    std::cerr << "Found " << regular << " regular regions.\n";
//    fprintf(stderr, "Found %"PRIu64" regular regions. \n", regular);
  if (P->inversion)
    std::cerr << "Found " << inverse << " inverted regions.\n";
//    fprintf(stderr, "Found %"PRIu64" inverted regions.\n", inverse);
  if (P->verbose)
    std::cerr << "Ignored " << ignored << " regions.\n";
//  fprintf(stderr, "Ignored %"PRIu64" regions.\n", ignored);

  Chromosome(PLOT, Paint->width, Paint->refSize, Paint->cx, Paint->cy);
  Chromosome(PLOT, Paint->width, Paint->tarSize, Paint->cx + Paint->space +
                                                 Paint->width, Paint->cy);
  PrintFinal(PLOT);
//  fclose(POS);
  POS.close();

  fprintf(stderr, "Done!                       \n");
}

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int32_t main (int argc, char *argv[]) {
  char **p = *&argv;
  uint32_t width, space, mult, start, minimum;

  P = (Parameters *) Malloc(1 * sizeof(Parameters));
  if ((P->help = ArgsState(DEF_HELP, p, argc, (char*)"-h")) == 1 || argc < 2) {
    PrintMenuVisual();
    return EXIT_SUCCESS;
  }

  if (ArgsState(DEF_VERSION, p, argc, (char*)"-V")) {
    PrintVersion();
    return EXIT_SUCCESS;
  }

  P->verbose  =ArgsState (DEF_VERBOSE, p, argc, (char*)"-v" );
  P->force    =ArgsState (DEF_FORCE,   p, argc, (char*)"-F" );
  P->link     =ArgsNum   (DEF_LINK,    p, argc, (char*)"-l",MIN_LINK, MAX_LINK);
  width       =ArgsNum   (DEF_WIDT,    p, argc, (char*)"-w",MIN_WIDT, MAX_WIDT);
  space       =ArgsNum   (DEF_SPAC,    p, argc, (char*)"-s",MIN_SPAC, MAX_SPAC);
  mult        =ArgsNum   (DEF_MULT,    p, argc, (char*)"-m",MIN_MULT, MAX_MULT);
  start       =ArgsNum   (DEF_BEGI,    p, argc, (char*)"-b",MIN_BEGI, MAX_BEGI);
  minimum     =ArgsNum   (DEF_MINP,    p, argc, (char*)"-c",MIN_MINP, MAX_MINP);
  P->inversion=ArgsState (DEF_INVE,    p, argc, (char*)"-i" );
  P->regular  =ArgsState (DEF_REGU,    p, argc, (char*)"-r" );
  P->image    =ArgsFilesImg           (p, argc, (char*)"-o");

  fprintf(stderr, "\n");
  TIME *Time = CreateClock(clock());
  PrintPlot(argv[argc - 1], width, space, mult, start, minimum);
  StopTimeNDRM(Time, clock());
  fprintf(stderr, "\n");

  fprintf(stderr, "==[ STATISTICS ]====================\n");
  StopCalcAll(Time, clock());
  fprintf(stderr, "\n");
  RemoveClock(Time);

  return EXIT_SUCCESS;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
