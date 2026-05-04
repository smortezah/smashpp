// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_TIME_HPP
#define SMASHPP_TIME_HPP

#include <format>

#include "def.hpp"

namespace smashpp {
// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline static std::chrono::time_point<std::chrono::high_resolution_clock> now() noexcept {
  return std::chrono::high_resolution_clock::now();
}

template <typename Time>
inline static std::string hms(Time elapsed) {
  const auto durSec = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
  const auto h = durSec / 3600;
  const auto m = (durSec % 3600) / 60;
  const auto s = durSec % 60;

  if (m < 1) {
    return std::format("{} sec.\n", s);
  } else if (h < 1) {
    return std::format("{}:{} min:sec.\n", m, s);
  } else {
    return std::format("{}:{}:{} hour:min:sec.\n", h, m, s);
  }
}
}  // namespace smashpp

#endif  // SMASHPP_TIME_HPP
