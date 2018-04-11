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
    
    // Build models
//    auto t0 {high_resolution_clock::now()};    // Start time
    auto t0 {now()};    // Start time
    m->buildModel(p);
    auto t1 {now()};    // Finish time
//    auto t1 {high_resolution_clock::now()};    // Finish time
    //todo
    hms(t1-t0);
//    dur_t e = t1 - t0;                         // Elapsed time
//    cerr << "in " << std::fixed << setprecision(3)<< e.count() << " seconds.\n";

//    // Compress
//    t0 = high_resolution_clock::now();
//    m->compress(p);
//    t1 = high_resolution_clock::now();
//    //todo
//    hms(t1 - t0);
////    e  = t1 - t0;
////    cerr << "in " << std::fixed << setprecision(3)<< e.count() << " seconds.\n";

    delete m;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}