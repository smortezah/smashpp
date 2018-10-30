#ifndef PAINT_H_INCLUDED
#define PAINT_H_INCLUDED

#include "defs.h"

namespace smashpp {
#define DEFAULT_CX        70.0
#define DEFAULT_CY        75.0
#define DEFAULT_TX        50.0
#define DEFAULT_TY        82.0
#define LEVEL_SATURATION  160
#define LEVEL_VALUE       160
#define EXTRA             150
#define DEF_WIDT          40
#define MIN_WIDT          10
#define MAX_WIDT          5000
#define DEF_SPAC          160
#define MIN_SPAC          0
#define MAX_SPAC          5000
#define DEF_MULT          46  // 5 colors without overlapping
#define MIN_MULT          1
#define MAX_MULT          255
#define DEF_BEGI          0
#define MIN_BEGI          0
#define MAX_BEGI          255
#define DEF_MINP          0
#define MIN_MINP          1
#define MAX_MINP          99999999

struct Painter {
  string  backColor;
  double  width;
  double  space;
  double  cx, cy;
  double  tx, ty;
  double  refSize, tarSize, maxSize;
};

struct RgbColor {
  u8 r, g, b;
};

struct HsvColor {
  u8 h, s, v;
};

Painter* CreatePainter (double, double, double, double, string);
RgbColor HsvToRgb      (HsvColor);
string   GetRgbColor   (u8);
void     PrintFinal    (ofstream&);
void     PrintHead     (ofstream&, double, double);
void     Polygon       (ofstream&, double, double, double, double, double,
                                   double, double, double, string, string);
void     Line          (ofstream&, double, double, double, double, double,
                                   string);
void     RectOval      (ofstream&, double, double, double, double, string);
void     Rect          (ofstream&, double, double, double, double, string);
void     RectIR        (ofstream&, double, double, double, double, string);
void     Chromosome    (ofstream&, double, double, double, double);
void     Text          (ofstream&, double, double, string);
double   GetPoint      (u64);
void     SetRatio      (u32);
}

#endif