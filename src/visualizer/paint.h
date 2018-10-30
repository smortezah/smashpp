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

Painter* create_painter (double, double, double, double, string);
RgbColor hsv_to_rgb     (HsvColor);
string   get_rgb_color  (u8);
void     print_final    (ofstream&);
void     print_head     (ofstream&, double, double);
void     polygon        (ofstream&, double, double, double, double, double,
                                    double, double, double, string, string);
void     line           (ofstream&, double, double, double, double, double,
                                    string);
void     rect_oval      (ofstream&, double, double, double, double, string);
void     rect           (ofstream&, double, double, double, double, string);
void     rect_ir        (ofstream&, double, double, double, double, string);
void     chromosome     (ofstream&, double, double, double, double);
void     text           (ofstream&, double, double, string);
double   get_point      (u64);
void     set_ratio      (u32);
}

#endif