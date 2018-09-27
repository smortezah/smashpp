//
// Created by morteza on 02-02-2018.
//

#define __STDC_FORMAT_MACROS
#if defined(_MSC_VER)
#  include <io.h>
#else
#  include <unistd.h>
#endif

#include <iostream>
#include <chrono>
#include <iomanip>      // setw, setprecision
#include "par.hpp"
#include "fcm.hpp"
#include "filter.hpp"
#include "fn.hpp"


int main (int argc, char* argv[])
{
  try {
    Param p;
    p.parse(argc, argv);           // Parse the command
    auto m = make_shared<FCM>(p);  // Equiv to auto* m = new FCM(p);
    m->store(p);                   // Build models
    m->compress(p);                // Compress

    // Filter and segment
//    auto flt = make_shared<Filter>(p);
//    t0=now();        flt->smooth_seg(p);   t1=now();   hms(t1-t0);

    // Build models for segments
//    swap(p.tar, p.ref);
//    t0=now();        m->store(p);          t1=now();   hms(t1-t0);


    // Report
//    m->report(p); // Without "-R" does nothing

///    delete m;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}