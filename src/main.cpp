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
#include "fn.hpp"


template <typename Iter>
inline u8 /*FCM::*/best_sym_abs (Iter first) /*const*/ {
  auto last = first + CARDINALITY;
  const auto max_pos = std::max_element(first, last);
  while (last-- != first)
    if (last!=max_pos && *last==*max_pos)
      return 255;
  return static_cast<u8>(max_pos - first);
}
int main (int argc, char* argv[])
{
  std::array<int,4> a{1000,100,99991,0};
  cerr<<int(best_sym_abs(a.begin()));
//  try {
//    Param p;
//    p.parse(argc, argv);
/////    auto* m = new FCM(p);
//    auto m = make_shared<FCM>(p);
//
//    // Build models  // Start time// Finish time// Elapsed time
//    auto t0{now()};  m->store(p);     auto t1{now()};  hms(t1-t0);
//    // Compress
//    t0=now();        m->compress(p);       t1=now();   hms(t1-t0);
//    // Result
//    cerr << "Average Entropy (H) = " << m->aveEnt << " bps\n";
//    // Report
////    m->report(p); // Without "-R" does nothing
//
/////    delete m;
//  }
//  catch (std::exception& e) { cout << e.what(); }
//  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}