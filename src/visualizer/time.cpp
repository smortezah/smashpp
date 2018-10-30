#include <string>
#include "time.h"
#include "mem.h"
using namespace smashpp;

static void PrintCalc (char* text, clock_t t) {
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

TIME* smashpp::CreateClock (clock_t t) {
  TIME* Time = (TIME*) Calloc(1, sizeof(TIME));
  Time->cpu_start = t;
  return Time;
}

void smashpp::StopTimeNDRM (TIME* Time, clock_t t) {
  Time->cpu_ndrm = t - Time->cpu_start;
}

void smashpp::StopCalcAll (TIME* Time, clock_t t) {
  Time->cpu_total = t - Time->cpu_start;
//  PrintCalc("NDRM ", Time->cpu_ndrm);
  PrintCalc((char*) "Total", Time->cpu_total);
}

void smashpp::RemoveClock (TIME* Time) {
  Free(Time);
}