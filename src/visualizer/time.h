#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "../def.hpp"

namespace smashpp {
struct TIME {
  clock_t cpu_start, cpu_ndrm, cpu_total;
  u8 unity;
};

TIME* create_clock   (clock_t);
void  stop_time_ndrm (TIME*, clock_t);
void  stop_calc_all  (TIME*, clock_t);
void  remove_clock   (TIME*);
}

#endif