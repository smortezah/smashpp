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
    Param par;
    par.parse(argc, argv);              // Parse the command
    auto models = make_shared<FCM>(par);// Equiv to auto* models = new FCM(par);
    models->store(par);                 // Build models
    models->compress(par);              // Compress
    auto filter = make_shared<Filter>(par);
    filter->smooth_seg(par);            // Filter and segment
    filter->extract_seg(par.tar, par.ref);  // Extract segs from the target file

    // Consider the ref as new tar and the tar segments as new refs
    const auto newTar=par.ref, segName=par.ref+"_"+par.tar+SEG_LBL;
    par.tar = newTar;
    const auto nSegs = filter->nSegs;
    for (auto i=0; i!=nSegs; ++i) {
      par.ref = segName+to_string(i);
      par.thresh = 2.0;
      models = make_shared<FCM>(par);
      models->store(par);
      models->compress(par);
      filter = make_shared<Filter>(par);
      filter->smooth_seg(par);
    }


    // Report
//    models->report(par); // Without "-R" does nothing

    const auto t1{now()};
    cerr << OUT_SEP << "Total time: " << hms(t1-t0);
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}