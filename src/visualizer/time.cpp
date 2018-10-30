#include <string>
#include "time.h"
#include "mem.h"
using namespace smashpp;

static void print_calc (char* text, clock_t t) {
  u32 seconds = t / CLOCKS_PER_SEC;
  if (seconds <= 60)
    cout << text << " cpu time: " << seconds << " second(s).\n";
  else if (seconds <= 3600)
    cout << text << " cpu time: " << seconds/60  << " minute(s) and "
         << seconds%60 << " second(s).\n";
  else
    cout << text << " cpu time: " << seconds/3600 << " hour(s) and "
         << seconds%3600 << " minute(s).\n";
}

TIME* smashpp::create_clock (clock_t t) {
  TIME* Time = (TIME*) Calloc(1, sizeof(Time));
  Time->cpu_start = t;
  return Time;
}

void smashpp::stop_time_ndrm (TIME* Time, clock_t t) {
  Time->cpu_ndrm = t - Time->cpu_start;
}

void smashpp::stop_calc_all (TIME* Time, clock_t t) {
  Time->cpu_total = t - Time->cpu_start;
//  print_calc("NDRM ", TIME->cpu_ndrm);
  print_calc((char*) "Total", Time->cpu_total);
}

void smashpp::remove_clock (TIME* Time) {
  Free(Time);
}