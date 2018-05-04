//
// Created by morteza on 03-05-2018.
//

#ifndef PROJECT_FN_HPP
#define PROJECT_FN_HPP

#include <iostream>
#include "def.hpp"

// Print variadic inputs
template <class Input>
void print (Input&& in) /*noexcept*/ {
  cerr << (typeid(in)==typeid(u8) ? static_cast<u32>(in) : in) << '\n';
}
template <class Input, class... Args>
void print (Input&& in, Args&&... args) /*noexcept*/ {
  cerr << (typeid(in)==typeid(u8) ? static_cast<u32>(in) : in) << '\t';
  print(args...);
}

// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline std::chrono::time_point<std::chrono::high_resolution_clock> now ()
noexcept {
  return std::chrono::high_resolution_clock::now();
}

template <class Time>
constexpr void hms (Time elapsed) {
//  std::chrono::duration<double, std::milli> ms = elapsed;
  const auto durSec =
    std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
  const auto h = durSec / 3600;
  const auto m = (durSec % 3600) / 60;
  const auto s = durSec % 60;
  
  cerr << " in " << h << ":" << m << ":" << s << " hour:min:sec.\n";
}

#endif //PROJECT_FN_HPP