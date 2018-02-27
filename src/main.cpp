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
#include <random>
//#include "def.hpp"
#include "par.hpp"
#include "fcm.hpp"
using std::cout;
using std::chrono::high_resolution_clock;
using std::setprecision;


std::minstd_rand0 &randomEngine () {
  static std::minstd_rand0 e{};
  return e;
}
void newSrand (u16 s) {
  randomEngine().seed(s);
}
u16 newRand () {
  return (u16) (randomEngine()() - randomEngine().min());
}

# include <iostream>
# include <map>
# include <cstdlib>
# include <cmath>
# include "cmls.hpp"
using namespace std;

int main (int argc, char* argv[])
{
  CMLS q;
  
  
  
  // Count for ar_str[i] is i1+i2+...
  // where i's are the positions where ar_str[i] occurs
  const char *ar_str[] = {
    "hello", "some", "one", "hello", "alice",
    "one", "lady", "let", "us", "lady",
    "alice", "in", "wonderland", "us", "lady",
    "lady", "some", "hello", "none", "pie"
  };
  
//  CMLS c(0.01, 0.1);
//  unsigned int i, total = 0;
//  map<const char *, int> mapitems;
//  map<const char *, int>::const_iterator it;

//  for (i = 0; i < 15; i++) {
//    if ((it = mapitems.find(ar_str[i]))!=mapitems.end()) {
//      mapitems[ar_str[i]] += i;
//    } else {
//      mapitems[ar_str[i]] = i;
//    }
//    c.update(ar_str[i], i);
//    tot += i;
//  }

//  // 1. test for items in ar_str
//  // note: since probablistic (and not deterministic)
//  // might not be accurate sometimes
//  for (it = mapitems.begin(); it != mapitems.end(); it++) {
//    if (c.estimate(it->first) != mapitems[it->first]) {
//      cout << "Incorrect count for " << it->first <<
//	";error: " << abs(int(c.estimate(it->first)-mapitems[it->first]))
//	   << endl;
//    } else {
//      cout << "Correct count for '" << it->first <<
//	"' ;count: " << c.estimate(it->first) << endl;
//    }
//  }
//  cout << "c.totalcount()==tot? "
//       << (c.totalcount() == tot ? "True" : "False")
//       << "Sketch Total: " << c.totalcount() << endl;
//
//  // 2. test for items not in ar_str
//  cout << "Testing for strings not in ar_str..." << endl;
//  cout << c.estimate("blabla") << endl;
//  cout << c.estimate("yoyo!") << endl;

  
  







  std::random_device r;// Seed with a real random value, if available
  std::default_random_engine e1(r());// Choose a random mean between 1 and 6
  std::uniform_int_distribution<u32> uniform_dist(0, std::numeric_limits<u32>::max());
  std::cout << uniform_dist(e1) << '\n';
  std::cout << uniform_dist(e1) << '\n';
//
////  newSrand(681493);
////  cerr<<(randomEngine()());
//






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