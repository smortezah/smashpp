#ifndef PROJECT_TIME_HPP
#define PROJECT_TIME_HPP

#include "def.hpp"

namespace smashpp {
// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline static chrono::time_point<chrono::high_resolution_clock> now () noexcept{
  return chrono::high_resolution_clock::now();
}

template <typename Time>
inline static string hms (Time elapsed) {
//  std::chrono::duration<double, std::milli> ms = elapsed;
  const auto durSec = chrono::duration_cast<chrono::seconds>(elapsed).count();
  const auto h = durSec / 3600;
  const auto m = (durSec % 3600) / 60;
  const auto s = durSec % 60;

  if (m < 1)
    return to_string(s)+" sec.\n";
  else if (h < 1)
    return to_string(m)+":"+to_string(s)+" min:sec.\n";
  else
    return to_string(h)+":"+to_string(m)+":"+to_string(s)+" hour:min:sec.\n";
}
}

#endif //PROJECT_TIME_HPP