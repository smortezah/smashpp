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


//#include "cstring"
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

    const auto newTar = par.ref;
    const auto segName = par.ref+"_"+par.tar+SEG_LBL;
    for (auto i=0; i!=filter->nSegs; ++i) {
////    for (auto i=0; i!=1; ++i) {
      cerr<<(par.ref = segName+to_string(i));//todo
      par.tar = newTar;
      models->store(par);
      models->compress(par);
      par.thresh = 1.5;
      filter->smooth_seg(par);
    }

//par.ref = segName + "1";
//par.tar = newTar;
//models->store(par);
//models->compress(par);
//filter->thresh = 1.5;
//filter->smooth_seg(par);
/////f->extract_seg(p.tar, p.ref);



    // Report
//    models->report(par); // Without "-R" does nothing


    const auto t1{now()};
    cerr << OUT_SEP << "Total time: " << hms(t1-t0);

///    delete m;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}