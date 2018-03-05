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
#include "cmls.hpp"
using std::cout;
using std::chrono::high_resolution_clock;
using std::setprecision;


int main (int argc, char* argv[])
{
  CMLS m;
  
//  for (int i = 0; i<16385; ++i) {
//    m.update(3);
//  }
//  m.printSketch();
  
  
  
//  constexpr u8 data[1]={18};
//  constexpr u8 c[2][19]={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
//                         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}};
//  for (int i = 0; i<2100000000; ++i) {
//    int a = (int)c[1%2][data[1>>1]];
//  }
  
//  constexpr u8 msk=0xF;
//  for (int i = 0; i<2200000000; ++i) {
//    int a = (int) (data[1>>1]>>(4*1)) & msk;
//  }


//  for (int i = 0; i<100000000; i++)
//    m.update(i, 1);
//  for (int i = 0; i<500000000; i++)
//    m.update(i, 1);
//
//  m.printSketch();
////  std::cerr << m.estimate(200);
////  std::cerr << m.getTotal();

  
  


//  try {
//    Param p;
//    p.parse(argc, argv);
//
//    auto* m = new FCM();
////    vector<FCM> m;
//
//    //todo. multithr: arrThreadSize=std::min(nThr, nModel)
//
////for (u8 i=0; i!=p.nMdl; ++i) {
////    m.emplace_back(FCM());
//    // Build models
//    auto t0 = high_resolution_clock::now();    // Start time
////    m[0].buildModel(p);
//    m->buildModel(p);
//    auto t1 = high_resolution_clock::now();    // Finish time
//    dur_t e = t1-t0;                           // Elapsed time
//    cerr << "in " << std::fixed << setprecision(3)<< e.count() << " seconds.\n";
////}
//
////    p.mode=='t' ? m[0].printTbl(p) : m[0].printHashTbl();
////    m->mode=='t' ? m->printTbl(p) : m->printHashTbl();
//
//
////    // Compress
////    t0=high_resolution_clock::now();
////    m->compress(p);
////    t1=high_resolution_clock::now();
////    e=t1-t0;
////    cerr << "in " << std::fixed << setprecision(3)<< e.count() << " seconds.\n";
//
////    delete m;
//  }
//  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}
