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
using std::cout;
using std::setprecision;


int main (int argc, char* argv[])
{
  try {
    Param p;
    p.parse(argc, argv);
    auto* m = new FCM(p);
    
    // Build models  // Start time// Finish time// Elapsed time
    auto t0{now()};  m->buildModel(p);  auto t1{now()};  hms(t1-t0);
    // Compress
    t0=now();        m->compress(p);         t1=now();   hms(t1-t0);
    // Result
    cerr << "Average Entropy (H) = " << m->aveEnt << " bps\n";
    // Report
    m->report(p.rep);
    
    delete m;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}