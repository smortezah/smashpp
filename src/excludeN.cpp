// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#include <iostream>
#include <vector>

#include "par.hpp"
using namespace smashpp;

int main(int, char*[]) {
  for (std::vector<char> buffer(FILE_READ_BUF, 0); std::cin;) {
    std::cin.read(buffer.data(), FILE_READ_BUF);
    std::string out;
    for (auto it = begin(buffer); it != begin(buffer) + std::cin.gcount();
         ++it) {
      if (*it != 'N') out += *it;
    }
    std::cout.write(out.data(), out.size());
  }

  return 0;
}
