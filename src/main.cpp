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

std::string exec(const char* cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

#include "tbl64.hpp"
#include "tbl32.hpp"
#include "logtbl8.hpp"
#include "cmls4.hpp"
int main (int argc, char* argv[])
{
//  std::system("ls -lh");
  std::cerr<< exec("ls");
  
  
//  try {
//    Param p;
//    p.parse(argc, argv);
//    auto* m = new FCM(p);
//
//    // Build models
//    auto t0 {high_resolution_clock::now()};    // Start time
//    m->buildModel(p);
//    auto t1 {high_resolution_clock::now()};    // Finish time
//    dur_t e = t1 - t0;                         // Elapsed time
//    cerr << "in " << std::fixed << setprecision(3)<< e.count() << " seconds.\n";
//
//    // Compress
//    t0 = high_resolution_clock::now();
//    m->compress(p);
//    t1 = high_resolution_clock::now();
//    e  = t1 - t0;
//    cerr << "in " << std::fixed << setprecision(3)<< e.count() << " seconds.\n";
//
//    delete m;
//  }
//  catch (...) { return EXIT_FAILURE; }
  
  return 0;
}