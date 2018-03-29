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
using std::chrono::high_resolution_clock;
using std::setprecision;


#include "tbl64.hpp"
#include "tbl32.hpp"
#include "logtbl8.hpp"
#include "cmls4.hpp"
int main (int argc, char* argv[])
{
//  int n=0;
//  for (int i = 0; i<30; ++i) {
//    if(!(i % POW2[2]))
//      cerr<<"1 ";
//    else
//      cerr<<"0 ";
//  }



  
  try {
    Param p;
    p.parse(argc, argv);
    auto* m = new FCM(p);
    
    // Build models
    auto t0 = high_resolution_clock::now();    // Start time
    m->buildModel(p);
    auto t1 = high_resolution_clock::now();    // Finish time
    dur_t e = t1 - t0;                         // Elapsed time
    cerr << "in " << std::fixed << setprecision(3)<< e.count() << " seconds.\n";
    
    // Compress
    t0 = high_resolution_clock::now();
    m->compress(p);
    t1 = high_resolution_clock::now();
    e  = t1 - t0;
    cerr << "in " << std::fixed << setprecision(3)<< e.count() << " seconds.\n";
    
    delete m;
  }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}