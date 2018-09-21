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
    auto t0=now();      flt->smooth(p);     auto t1=now();   hms(t1-t0);
//    t0=now();        m->filter(p);       t1=now();   hms(t1-t0);

//    auto t0=now();
//vector<int> v{3,2,1,4,5};
//for (int i=0;i<70000000;++i){
////  v.emplace_back(7);
////  v.erase(v.begin());
//copy(v.begin()+1, v.end(), v.begin());
//v.back()=7;
////move(v.begin()+1, v.end(), v.begin());
////v.back()=7;
//}
//    auto t1=now();   hms(t1-t0);

    // Report
//    m->report(p); // Without "-R" does nothing

///    delete m;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}