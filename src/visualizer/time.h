#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include <time.h>

typedef struct{
  clock_t cpu_start;
  clock_t cpu_ndrm;
  clock_t cpu_total;
  uint8_t unity;
  }
TIME;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TIME    *CreateClock  (clock_t);
void    StopTimeNDRM  (TIME *, clock_t);
void    StopCalcAll   (TIME *, clock_t);
void    RemoveClock   (TIME *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

