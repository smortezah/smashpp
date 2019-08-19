// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_EXCEPTION_HPP
#define SMASHPP_EXCEPTION_HPP

#include "def.hpp"
#include "string.hpp"

namespace smashpp {
extern void wrap_text(std::string&);
extern std::string bold(const std::string&);
extern std::string bold_red(const std::string&);

// "inline" is a MUST -- not to get "multiple definition of ..." error
inline void error(std::string&& msg) {
  std::string message = "Error: " + std::move(msg);
  wrap_text(message);
  throw std::runtime_error(bold_red(message.substr(0, 6)) + message.substr(6) +
                           "\n");
}

inline void err(std::string&& msg) {
  std::string message = "Error: " + std::move(msg);
  wrap_text(message);
  std::cerr << bold_red(message.substr(0, 6)) << message.substr(6) << '\n';
}

inline void warning(std::string&& msg) {
  std::string message = "Warning: " + std::move(msg);
  wrap_text(message);
  std::cerr << bold(message.substr(0, 8)) << message.substr(8) << '\n';
}

inline void exit(std::string&& msg) {
  std::string message = std::move(msg);
  wrap_text(message);
  throw std::runtime_error(message + "\n");
}
}  // namespace smashpp

#endif  // SMASHPP_EXCEPTION_HPP