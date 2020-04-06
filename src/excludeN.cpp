// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2020, IEETA, University of Aveiro, Portugal.

#include <fstream>
#include <iostream>

#include "par.hpp"
using namespace smashpp;

int main(int argc, char* argv[]) {
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