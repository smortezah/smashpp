//
// Created by morteza on 03-05-2018.
//

#ifndef PROJECT_FN_HPP
#define PROJECT_FN_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
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
  
  cerr << h << ":" << m << ":" << s << " hour:min:sec.\n";
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

template <typename Digit>
u8 popcount (Digit d) {  // Number of ones in a digit
  u8 n = 0;
  for (; d; ++n)  d &= d-1;  // First impl. Kernighan. Clear the LSB set
//  do { if (d & 1) ++n; } while (d >>= 1); // Second impl. Negative nums?
  return n;
}

template <typename Iter>
u8 best_sym (Iter first) {
  return static_cast<u8>(std::max_element(first,first+CARDINALITY) - first);
}

template <typename Iter>
u8 best_sym_abs (Iter first) {
  auto last = first + CARDINALITY;
  const auto max_pos = std::max_element(first, last);
  while (last-- != first)
    if (last!=max_pos && *last==*max_pos)
      return 255;
  return static_cast<u8>(max_pos - first);
}

template <typename Iter>
void normalize (Iter first, Iter last) {
  for (const double sum=std::accumulate(first,last,0.0); first!=last; ++first)
    *first /= sum;    // *first = *first / sum;
}

template <typename Value>
bool is_odd (Value v) {
  if (v < 0)
    error("\"" + to_string(v) + "\" is a negative number.");
  return (v & 1ull);
}

inline float pow2 (float base) noexcept {  // Must be inline
  return static_cast<float>(std::pow(base, 2));
}

inline double pow2 (double base) noexcept {  // Must be inline
  return std::pow(base, 2);
}

inline void check_file (const string &s) {  // Must be inline
  ifstream f(s);
  if (!f) {
    f.close();
    error("the file \"" + s + "\" cannot be opened or is empty.");
  }
  else {
    bool foundChar {false};
    for (char c; f.get(c) && !foundChar;)
      if (c!=' ' && c!='\n' && c!='\t')
        foundChar = true;
    if (!foundChar)
      error("the file \"" + s + "\" is empty.");
    f.close();
  }
}

inline void extract_subseq (const string& fIn, const string& fOut,
                            u64 begPos, u64 endPos) {  // Must be inline
  ifstream fi(fIn);
  ofstream fo(fOut);
  char c;
  fi.seekg(begPos);
  for (u64 i=endPos-begPos+1; i-- && fi.get(c);)
    fo << c;
  fi.close();
  fo.close();
}

#endif //PROJECT_FN_HPP