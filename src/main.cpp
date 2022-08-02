// Smash++
// Morteza Hosseini    mhosayny@gmail.com

#define __STDC_FORMAT_MACROS
#if defined(_MSC_VER)
#include <io.h>
#else
#include <unistd.h>
#endif

#include "application.hpp"
using namespace smashpp;

int main(int argc, char* argv[]) {
  try {
    auto const t0 = now();
    application{}.exe(argc, argv);
    auto const t1 = now();
    std::cerr << "Total time: " << hms(t1 - t0);
  } catch (std::exception& e) {
    std::cerr << e.what();
  } catch (...) {
    return EXIT_FAILURE;
  }

  return 0;
}