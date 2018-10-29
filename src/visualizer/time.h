#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include <time.h>

namespace smashpp {
struct TIME {
  clock_t cpu_start, cpu_ndrm, cpu_total;
  u8 unity;
};

TIME* CreateClock  (clock_t);
void  StopTimeNDRM (TIME*, clock_t);
void  StopCalcAll  (TIME*, clock_t);
void  RemoveClock  (TIME*);
}

#endif