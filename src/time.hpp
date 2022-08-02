// Smash++
// Morteza Hosseini    mhosayny@gmail.com

#ifndef SMASHPP_TIME_HPP
#define SMASHPP_TIME_HPP

#include "def.hpp"

namespace smashpp {
// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline static std::chrono::time_point<std::chrono::high_resolution_clock>
now() noexcept {
  return std::chrono::high_resolution_clock::now();
}

template <typename Time>
inline static std::string hms(Time elapsed) {
  const auto durSec =
      std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
  const auto h = durSec / 3600;
  const auto m = (durSec % 3600) / 60;
  const auto s = durSec % 60;

  if (m < 1)
    return std::to_string(s) + " sec.\n";
  else if (h < 1)
    return std::to_string(m) + ":" + std::to_string(s) + " min:sec.\n";
  else
    return std::to_string(h) + ":" + std::to_string(m) + ":" +
           std::to_string(s) + " hour:min:sec.\n";
}
}  // namespace smashpp

#endif  // SMASHPP_TIME_HPP