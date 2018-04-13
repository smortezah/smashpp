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


void f (std::vector<std::vector<u8>>& level, const std::vector<u8>& vir,
        const std::vector<u8>& vk, const std::vector<u8>& valpha) {
  for (const auto& ir : vir)
    for (const auto& k : vk)
      for (const auto& a : valpha)
        level.emplace_back(std::vector<u8>{ir, k, a});
  for (const auto& lv : level) {
    for (const auto& l : lv)
      cerr << static_cast<u16>(l) << ' ';
    cerr << '\n';
  }
}
int main (int argc, char* argv[])
{
//  for(auto i: f(2))
//    cerr<<i<<' ';
//  static std::vector<std::vector<u8>> level;
//  std::vector<u8> vir    {0, 1};
//  std::vector<u8> valpha {100, 10, 1};
//  std::vector<u8> vk     {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
//  f(level, vir, vk, valpha);
  
  
  
  try {
    Param p;
    p.parse(argc, argv);
    auto* m = new FCM(p);
    
    // Build models  // Start time// Finish time// Elapsed time
//    auto t0{now()};  m->buildModel(p);  auto t1{now()};  hms(t1-t0);
    // Compress
//    t0=now();        m->compress(p);         t1=now();   hms(t1-t0);
    // Result
//    cerr << "Average Entropy (H) = " << m->aveEnt << " bps\n";
    delete m;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}