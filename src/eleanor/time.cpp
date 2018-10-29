#include <string>
#include "time.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void PrintCalc(char *text, clock_t t){
  uint32_t seconds = t / CLOCKS_PER_SEC;
  if(seconds <= 60)
    fprintf(stdout, "%s cpu time: %u second(s).\n", text, seconds);
  else if(seconds <= 3600)
    fprintf(stdout, "%s cpu time: %u minute(s) and %u second(s).\n", text, 
    seconds / 60, seconds % 60);
  else
    fprintf(stdout, "%s cpu time: %u hour(s) and %u minute(s).\n", text, 
    seconds / 3600, seconds % 3600);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TIME *CreateClock(clock_t t){
  TIME *Time = (TIME *) Calloc(1, sizeof(TIME));
  Time->cpu_start = t;
  return Time;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void StopTimeNDRM(TIME *Time, clock_t t){
  Time->cpu_ndrm = t - Time->cpu_start;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void StopCalcAll(TIME *Time, clock_t t){
  Time->cpu_total = t - Time->cpu_start;
//  PrintCalc("NDRM ", Time->cpu_ndrm);
  PrintCalc((char*)"Total", Time->cpu_total);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveClock(TIME *Time){
  Free(Time);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
