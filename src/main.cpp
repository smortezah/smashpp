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
//    const auto t0{now()};
//    Param p;
//    p.parse(argc, argv);           // Parse the command
//    auto m = make_shared<FCM>(p);  // Equiv to auto* m = new FCM(p);
//    m->store(p);                   // Build models
//    m->compress(p);                // Compress
//    auto flt = make_shared<Filter>(p);
//    flt->smooth_seg(p);            // Filter and segment


extract_subseq("t2","t2_seg0",0,10);//todo


    // Build models for segments
    // Extract segments from the target file
//    swap(p.tar, p.ref);
//    m->store(p);

    // Report
//    m->report(p); // Without "-R" does nothing

//    const auto t1{now()};
//    cerr << OUT_SEP << "Total time: ";
//    hms(t1-t0);

///    delete m;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}