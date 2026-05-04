// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#define __STDC_FORMAT_MACROS
#if defined(_MSC_VER)
#include <io.h>
#else
#include <unistd.h>
#endif

#include <format>

#include "application.hpp"
using namespace smashpp;

int main(int argc, char* argv[]) {
  try {
    auto const t0 = now();
    application{}.exe(argc, argv);
    auto const t1 = now();
    std::cerr << std::format("Total time: {}", hms(t1 - t0));
  } catch (int code) {
    return code;
  } catch (std::exception& e) {
    std::cerr << e.what();
    return EXIT_FAILURE;
  } catch (...) {
    return EXIT_FAILURE;
  }

  return 0;
}
