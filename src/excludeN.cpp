// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2020, IEETA, University of Aveiro, Portugal.

#include <iostream>
#include <fstream>
#include "def.hpp"

int main(int argc, char* argv[]) {
  for (std::vector<char> buffer(FILE_BUF, 0); cin;) {
    cin.read(buffer.data(), FILE_BUF);
    std::string out;
    for (auto it = begin(buffer); it != begin(buffer) + cin.gcount(); ++it) {
      if (*it != 'N') out += *it;
    }
    cout.write(out.data(), out.size());
  }

  return 0;
}