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
//#include "def.hpp"
#include "par.hpp"
#include "fcm.hpp"
using std::cout;
using std::chrono::high_resolution_clock;
using std::setprecision;


int main (int argc, char* argv[])
{
  try {
    Param p;
    p.parse(argc, argv);
    
//    auto* m = new FCM(p);
//    vector<FCM> m;
    
    //todo. multithr: arrThreadSize=std::min(nThr, nModel)
    
//for (u8 i=0; i!=p.nMdl; ++i) {
//    m.emplace_back(FCM(p));
////    // Build models
////    auto t0 = high_resolution_clock::now();    // Start time
////    m[0].buildModel(p);
////    auto t1 = high_resolution_clock::now();    // Finish time
////    dur_t e = t1-t0;                           // Elapsed time
////    cerr << "in " << std::fixed << setprecision(3)<< e.count() << " seconds.\n";
//}

//    m->mode=='t' ? m->printTbl(p) : m->printHashTbl();


//    // Compress
//    t0=high_resolution_clock::now();
//    m->compress(p);
//    t1=high_resolution_clock::now();
//    e=t1-t0;
//    cerr << "in " << std::fixed << setprecision(3)<< e.count() << " seconds.\n";

//    delete m;
  }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}