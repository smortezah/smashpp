//
// Created by morteza on 03-05-2018.
//

#ifndef PROJECT_FN_HPP
#define PROJECT_FN_HPP

#include <iostream>
#include "def.hpp"

template <typename Input>
bool is_u8 (Input&& in) {
  return typeid(in)==typeid(u8);
}

#ifdef DEBUG
// Print variadic inputs
template <typename Integral>
void print (Integral&& in) /*noexcept*/ {
  cerr << (is_u8(in) ? static_cast<u32>(in) : in) << '\n';
}
template <typename Integral, typename... Args>
void print (Integral&& in, Args&&... args) /*noexcept*/ {
  cerr << (is_u8(in) ? static_cast<u32>(in) : in) << '\t';
  print(args...);
}
#endif

// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline std::chrono::time_point<std::chrono::high_resolution_clock> now ()
noexcept {
  return std::chrono::high_resolution_clock::now();
}

template <typename Time>
void hms (Time elapsed) {
//  std::chrono::duration<double, std::milli> ms = elapsed;
  const auto durSec =
    std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
  const auto h = durSec / 3600;
  const auto m = (durSec % 3600) / 60;
  const auto s = durSec % 60;
  
  cerr << " in " << h << ":" << m << ":" << s << " hour:min:sec.\n";
}

// Split a range by delim and insert the result into an std::vector
template <typename InIter, typename Vec>
void split (InIter first, InIter last, char delim, Vec& vOut) {
  while (true) {
    InIter found = std::find(first, last, delim);
    vOut.emplace_back(string(first,found));
    if (found == last)
      break;
    first = ++found;
  }
}

// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline void error (const string &msg) {
  throw std::runtime_error ("Error: " + msg + "\n");
}

inline void err (const string &msg) {
  cerr << "Error: " << msg << '\n';
}

template <typename Iter, typename Element>
bool has (Iter first, Iter last, Element elem) {
  return std::find(first, last, elem) != last;
}

#endif //PROJECT_FN_HPP