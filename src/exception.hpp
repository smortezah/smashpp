// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef PROJECT_EXCEPTION_HPP
#define PROJECT_EXCEPTION_HPP

#include "def.hpp"
#include "string.hpp"

namespace smashpp {
// "inline" is a MUST -- not to get "multiple definition of ..." error
inline static void error(std::string&& msg) {
  std::string message = "Error: " + std::move(msg);
  wrap_text(message);
  throw std::runtime_error(bold_red(message.substr(0, 6)) + message.substr(6) +
                           "\n");
}

inline static void err(std::string&& msg) {
  std::string message = "Error: " + std::move(msg);
  wrap_text(message);
  std::cerr << bold_red(message.substr(0, 6)) << message.substr(6) << '\n';
}

inline static void warning(std::string&& msg) {
  std::string message = "Warning: " + std::move(msg);
  wrap_text(message);
  std::cerr << bold(message.substr(0, 8)) << message.substr(8) << '\n';
}
}  // namespace smashpp

#endif  // PROJECT_EXCEPTION_HPP