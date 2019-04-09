// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_PRINT_HPP
#define SMASHPP_PRINT_HPP

#include <iostream>
#include <string>
#include "string.hpp"

namespace smashpp {
void print_title(std::string str) { std::cerr << bold(str) << '\n'; }

void print_line(const std::string& str) { std::cerr << "  " << str << '\n'; }

// Print column 1: left-aligned + column 2: left-aligned
void print_aligned(std::string arg, std::string type,
                   std::string delim_description,
                   const std::string& description,
                   std::string delim_default = "",
                   std::string default_val = "") {
  std::cerr << "  ";
  std::cerr << std::left << std::setw(12) << bold(arg);
  std::cerr << std::left << std::setw(15)
            << ((type.empty() || type[0] == '[') ? type : "<" + type + ">");
  std::cerr << std::left << std::setw(2) << delim_description;
  std::cerr << std::left << std::setw(38) << description;
  std::cerr << std::left << std::setw(3) << delim_default;
  std::cerr << default_val << '\n';
}

void print_aligned(std::string arg, const std::string& description) {
  std::cerr << "  ";
  std::cerr << std::left << std::setw(19) << arg;
  std::cerr << std::left << std::setw(38) << description << '\n';
}

void print_aligned_right_left(std::string arg, std::string type,
                              std::string delim_description,
                              const std::string& description,
                              std::string delim_default = "",
                              std::string default_val = "") {
  std::cerr << "  ";
  std::cerr << std::left << std::setw(12) << bold(arg);
  std::cerr << std::right << std::setw(15)
            << ((type.empty() || type[0] == '[') ? type : "<" + type + ">");
  std::cerr << std::left << std::setw(2) << delim_description;
  std::cerr << std::left << std::setw(38) << description;
  std::cerr << std::left << std::setw(3) << delim_default;
  std::cerr << default_val << '\n';
}
}  // namespace smashpp

#endif