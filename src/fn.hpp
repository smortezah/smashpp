//
// Created by morteza on 03-05-2018.
//

#ifndef PROJECT_FN_HPP
#define PROJECT_FN_HPP

#include <iostream>
#include "def.hpp"
using std::cerr;

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
inline std::chrono::time_point<std::chrono::high_resolution_clock>
  now () noexcept {
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

////template<u32 N>    // Up to 262144=2^18 elements
////struct LogInt      // 0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,...
////{
////  constexpr LogInt() : lg() {
////    for (u32 i=0; i!=LOG_BASE; ++i)
////      lg[i] = 0;
////    for (u32 i=LOG_BASE; i!=N; ++i)
////      lg[i] = static_cast<u8>(1 + lg[i/LOG_BASE]);
////  }
////  u8 lg[N];
////};
//// Inside function definition
////constexpr auto a = LogInt<256>();
////cerr << (int) a.lg[3];

#endif //PROJECT_FN_HPP