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
#include "segment.hpp"


int main (int argc, char* argv[])
{
  try {
    const auto t0{now()};
    Param p;
    p.parse(argc, argv);           // Parse the command
    auto m = make_shared<FCM>(p);  // Equiv to auto* m = new FCM(p);
    m->store(p);                   // Build models
    m->compress(p);                // Compress
    auto f = make_shared<Filter>(p);
    f->smooth_seg(p);              // Filter and segment
//    f->extract_seg(p.tar, p.ref);  // Extract segments from the target file
////    for (u64 i=0; i!=f->nSegs; ++i) {
////    }


//const auto newTar = p.ref;
//const string segName = p.ref + "_" + p.tar + SEG_LBL;
//p.ref = segName + "0";
//p.tar = newTar;
//m->store(p);
//m->compress(p);
//f->thresh = 1.5;
//f->smooth_seg(p);
/////f->extract_seg(p.tar, p.ref);
//
//p.ref = segName + "1";
//p.tar = newTar;
//m->store(p);
//m->compress(p);
//f->thresh = 1.5;
//f->smooth_seg(p);
/////f->extract_seg(p.tar, p.ref);



    // Report
//    m->report(p); // Without "-R" does nothing


    const auto t1{now()};
    cerr << OUT_SEP << "Total time: " << hms(t1-t0);

///    delete m;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}