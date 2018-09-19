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
    p.parse(argc, argv);

//    auto m = make_shared<FCM>(p);  // auto* m = new FCM(p);
//    // Build models  // Start time// Finish time// Elapsed time
//    auto t0{now()};  m->store(p);     auto t1{now()};  hms(t1-t0);
//    // Compress
//    t0=now();        m->compress(p);       t1=now();   hms(t1-t0);
//    // Result
//    cerr << "Average Entropy (H) = " << m->aveEnt << " bps\n";

    // Filter
    auto flt = make_shared<Filter>(p);
    auto t0=now();      flt->smooth();     auto t1=now();   hms(t1-t0);
//    t0=now();        m->filter(p);       t1=now();   hms(t1-t0);

    // Report
//    m->report(p); // Without "-R" does nothing

///    delete m;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}