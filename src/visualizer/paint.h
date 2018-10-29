#ifndef PAINT_H_INCLUDED
#define PAINT_H_INCLUDED

#include "defs.h"

#define DEFAULT_CX             70.0
#define DEFAULT_CY             75.0
#define DEFAULT_TX             50.0
#define DEFAULT_TY             82.0
#define LEVEL_SATURATION       160
#define LEVEL_VALUE            160
#define EXTRA                  150 
#define DEF_WIDT               40
#define MIN_WIDT               10
#define MAX_WIDT               5000
#define DEF_SPAC               160.0
#define MIN_SPAC               0
#define MAX_SPAC               5000
#define DEF_MULT               46  // 5 colors without overlapping 
#define MIN_MULT               1
#define MAX_MULT               255
#define DEF_BEGI               0
#define MIN_BEGI               0
#define MAX_BEGI               255
#define DEF_MINP               0
#define MIN_MINP               1
#define MAX_MINP               99999999

typedef struct
  {
  char    *backColor;
  double  width;
  double  space;
  double  cx; 
  double  cy;
  double  tx;
  double  ty;
  double  refSize;
  double  tarSize;
  double  maxSize;  
  }
Painter;

typedef struct
  {
  u8  r;
  u8  g;
  u8  b;
  } 
RgbColor;

typedef struct
  {
  u8  h;
  u8  s;
  u8  v;
  } 
HsvColor;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Painter   *CreatePainter (double, double, double, double, char *);
RgbColor  HsvToRgb       (HsvColor);
HsvColor  RgbToHsv       (RgbColor);
char      *GetRgbColor   (u8);
void      PrintFinal     (FILE *);
void      PrintHead      (FILE *, double, double);
void      Polygon        (FILE *, double, double, double, double, double,
                         double, double, double, char *, char *);
void      Line           (FILE *, double, double, double, double, double, 
                         char *);
void      Circle         (FILE *, double, double, double, char *);
void      RectOval       (FILE *, double, double, double, double, char *);
void      RectOvalIR     (FILE *, double, double, double, double, char *);
void      Rect           (FILE *, double, double, double, double, char *);
void      RectIR         (FILE *, double, double, double, double, char *);
void      Chromosome     (FILE *, double, double, double, double);
void      Text           (FILE *, double, double, char *);
void      Textfloatoat   (FILE *, double, double, double);
double    GetPoint       (u64);
void      SetRatio       (u32);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
