// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_EXCEPTION_HPP
#define SMASHPP_EXCEPTION_HPP

#include <format>

#include "def.hpp"
#include "string.hpp"

namespace smashpp {
extern void wrap_text(std::string&);
extern std::string bold(const std::string&);
extern std::string bold_red(const std::string&);

// "inline" is a MUST -- not to get "multiple definition of ..." error
inline void error(std::string&& msg) {
  std::string message = std::format("Error: {}", msg);
  wrap_text(message);
  throw std::runtime_error(
      std::format("{}{}\n", bold_red(message.substr(0, 6)), message.substr(6)));
}

inline void err(std::string&& msg) {
  std::string message = std::format("Error: {}", msg);
  wrap_text(message);
  std::cerr << std::format("{}{}\n", bold_red(message.substr(0, 6)), message.substr(6));
}

inline void warning(std::string&& msg) {
  std::string message = std::format("Warning: {}", msg);
  wrap_text(message);
  std::cerr << std::format("{}{}\n", bold(message.substr(0, 8)), message.substr(8));
}

inline void exit(std::string&& msg) {
  std::string message = std::move(msg);
  wrap_text(message);
  throw std::runtime_error(std::format("{}\n", message));
}
}  // namespace smashpp

#endif  // SMASHPP_EXCEPTION_HPP
