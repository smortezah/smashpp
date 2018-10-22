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
static bool is_u8 (Input&& in) {
  return typeid(in)==typeid(u8);
}

#ifdef DEBUG
// Print variadic inputs
template <typename Integral>
static void print (Integral&& in) /*noexcept*/ {
  cerr << (is_u8(in) ? static_cast<u32>(in) : in) << '\n';
}
template <typename Integral, typename... Args>
static void print (Integral&& in, Args&&... args) /*noexcept*/ {
  cerr << (is_u8(in) ? static_cast<u32>(in) : in) << '\t';
  print(args...);
}
#endif

// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline static chrono::time_point<std::chrono::high_resolution_clock> now ()
noexcept {
  return chrono::high_resolution_clock::now();
}

template <typename Time>
static string hms (Time elapsed) {
//  std::chrono::duration<double, std::milli> ms = elapsed;
  const auto durSec =
    chrono::duration_cast<std::chrono::seconds>(elapsed).count();
  const auto h = durSec / 3600;
  const auto m = (durSec % 3600) / 60;
  const auto s = durSec % 60;

  return to_string(h)+":"+to_string(m)+":"+to_string(s)+" hour:min:sec.\n";
}

// Split a range by delim and insert the result into an std::vector
template <typename InIter, typename Vec>
static void split (InIter first, InIter last, char delim, Vec& vOut) {
  while (true) {
    InIter found = std::find(first, last, delim);
    vOut.emplace_back(string(first,found));
    if (found == last)
      break;
    first = ++found;
  }
}

// "inline" is a MUST -- not to get "multiple definition of `now()'" error
inline static void error (const string& msg) {
  throw std::runtime_error ("Error: " + msg + "\n");
}
inline static void error (string&& msg) {
  throw std::runtime_error ("Error: " + msg + "\n");
}

inline static void err (const string& msg) {
  cerr << "Error: " << msg << '\n';
}

template <typename Iter, typename Element>
static bool has (Iter first, Iter last, Element elem) {
  return std::find(first, last, elem) != last;
}

#ifdef ARRAY_HISTORY
template <typename VecIter>
static u8 pop_count (VecIter first, u8 len) {  // Number of ones in a bool array
  u8 n = 0;
  for (auto last=first+len; last--!=first;) {
    if (*last == true)  // 1u instead of "true"
      ++n;
  }
  return n;
}
#else
template <typename Digit>
static u8 pop_count (Digit d) {  // Number of ones in a digit
  u8 n = 0;
  for (; d; ++n)  d &= d-1;  // First impl. Kernighan. Clear the LSB set
//  do { if (d & 1) ++n; } while (d >>= 1); // Second impl. Negative nums?
  return n;
}
#endif

template <typename Iter, typename Value>
static bool are_all (Iter first, Value val) {
  return std::all_of(first, first+CARDIN,
                     [val](u64 i){ return i==static_cast<u64>(val); });
}

template <typename Iter, typename Value>
static bool is_any (Iter first, Value val) {
  return std::any_of(first, first+CARDIN,
                     [val](u64 i){ return i==static_cast<u64>(val); });
}

template <typename Iter>
static bool are_all_zero (Iter first) {
  return std::all_of(first, first+CARDIN, [](u64 i){ return i==0; });
}

template <typename Iter>
static bool has_multi_max (Iter first) {
  auto last = first + CARDIN;
  for (const auto max_pos=std::max_element(first,last); last-- != first;)
    if (*last==*max_pos && last!=max_pos)
      return true;
  return false;
}

template <typename Iter, typename PosIter>
static bool has_multi_max (Iter first, PosIter maxPos) {
  for (auto last=first+CARDIN; last-- != first;)
    if (*last==*maxPos && last!=maxPos)
      return true;
  return false;
}

template <typename Iter>
static u8 best_sym (Iter first) {
  return static_cast<u8>(*std::max_element(first, first+CARDIN));
}

template <typename Iter>
static u8 best_sym_abs (Iter first) {
  const auto max_pos = std::max_element(first, first+CARDIN);
  return static_cast<u8>(has_multi_max(first, max_pos) ? 255 : max_pos-first);
}

//template <typename Iter>
//void normalize (Iter first, Iter last) {
//  for (const auto sum=std::accumulate(first,last,static_cast<prec_t>(0));
//       first!=last; ++first)
//    *first /= sum;    // *first = *first / sum;
//}
template <typename OutIter, typename InIter>
static void normalize (OutIter oFirst, InIter iFirst, InIter iLast) {
  const auto sumInv =
    static_cast<prec_t>(1)/std::accumulate(iFirst,iLast,static_cast<prec_t>(0));
  for (; iFirst!=iLast; ++iFirst,++oFirst)
    *oFirst = *iFirst * sumInv;
}

template <typename Value>
static bool is_odd (Value val) {
  if (val < 0)
    error("\"" + to_string(val) + "\" is a negative number.");
  return (val & 1ull);
}

template <typename T>
inline static auto pow2 (T base) {  // Must be inline
  return std::pow(base, static_cast<T>(2));
}

inline static void check_file (const string& s) {  // Must be inline
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

inline static u64 file_size (const string& s) {
  check_file(s);
  ifstream f(s, ifstream::ate | ifstream::binary);
  return static_cast<u64>(f.tellg());
}

template <typename Position>
inline static void extract_subseq    // Must be inline//todo improve performance
(const string& fIn, const string& fOut, Position begPos, Position endPos) {
  ifstream fi(fIn);
  ofstream fo(fOut);
  char c;
  fi.seekg(begPos);
  for (Position i=endPos-begPos+1; i-- && fi.get(c);)
    fo << c;
  fi.close();
  fo.close();
}

#endif //PROJECT_FN_HPP