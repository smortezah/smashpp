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

constexpr std::array<int,2> f (int i){ return std::array<int,2>{i, 2*i}; }
int main (int argc, char* argv[])
{
//  for(auto i: f(2))
//    cerr<<i<<' ';
  
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
//std::initializer_list<int> a{2,4};cerr<<*(a.begin()+1);
    delete m;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}