// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_PRINT_HPP
#define SMASHPP_PRINT_HPP

#include <iostream>
#include <string>
#include "string.hpp"

namespace smashpp {
void title(std::string str) { std::cerr << bold(str) << '\n'; }

void line(const std::string& str) { std::cerr << "  " << str << '\n'; }

// Print column 1: left-aligned + column 2: left-aligned
void line_left_left(const std::string& strL, uint8_t n,
                    const std::string& strR) {
  std::cerr << "  " << std::left << std::setw(27 + n * 8) << strL;
  std::cerr.clear();
  std::cerr << strR << '\n';
}

// Print column 1: right-aligned + column 2: left-aligned
void line_right_left(const std::string& strL, uint8_t n,
                     const std::string& strR) {
  std::cerr << "  " << std::right << std::setw(27 + n * 8) << strL;
  std::cerr.clear();
  std::cerr << strR << '\n';
}
}  // namespace smashpp

#endif