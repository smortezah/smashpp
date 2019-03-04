#ifndef PROJECT_CONTAINER_HPP
#define PROJECT_CONTAINER_HPP

#include <numeric>
// #include <algorithm>
#include "def.hpp"

namespace smashpp {
// Split a range by delim and insert the result into an std::vector
template <typename InIter, typename Vec>
inline static void split(InIter first, InIter last, char delim, Vec& vOut) {
  while (true) {
    InIter found = std::find(first, last, delim);
    vOut.emplace_back(std::string(first, found));
    if (found == last) break;
    first = ++found;
  }
}

template <typename Iter, typename Element>
inline static bool has(Iter first, Iter last, Element elem) {
  return std::find(first, last, elem) != last;
}

#ifdef ARRAY_HISTORY
// Number of ones in a bool array
template <typename VecIter>  // Number of ones in a bool array
static uint8_t pop_count(VecIter first, uint8_t len) {
  uint8_t n{0};
  for (auto last = first + len; last-- != first;) {
    if (*last == true)  // 1u instead of "true"
      ++n;
  }
  return n;
}
#else
template <typename Digit>
inline static uint8_t pop_count(Digit d) {  // Number of ones in a digit
  uint8_t n{0};
  for (; d; ++n) d &= d - 1;  // First impl. Kernighan. Clear the LSB set
  //  do { if (d & 1) ++n; } while (d >>= 1); // Second impl. Negative nums?
  return n;
}
#endif

template <typename Iter, typename Value>
inline static bool are_all(Iter first, Value val) {
  return std::all_of(first, first + CARDIN, [val](uint64_t i) {
    return i == static_cast<uint64_t>(val);
  });
}

template <typename Iter>
inline static bool has_multi_max(Iter first) {
  auto last{first + CARDIN};
  for (const auto max_pos = std::max_element(first, last); last-- != first;)
    if (*last == *max_pos && last != max_pos) return true;
  return false;
}

template <typename Iter, typename PosIter>
inline static bool has_multi_max(Iter first, PosIter maxPos) {
  for (auto last = first + CARDIN; last-- != first;)
    if (*last == *maxPos && last != maxPos) return true;
  return false;
}

template <typename Iter>
inline static uint8_t best_sym(Iter first) {
  return static_cast<uint8_t>(*std::max_element(first, first + CARDIN));
}

template <typename Iter>
inline static uint8_t best_sym_abs(Iter first) {
  const auto max_pos = std::max_element(first, first + CARDIN);
  return static_cast<uint8_t>(has_multi_max(first, max_pos) ? 255
                                                            : max_pos - first);
}

template <typename OutIter, typename InIter>
inline static void normalize(OutIter oFirst, InIter iFirst, InIter iLast) {
  const auto sumInv = prc_t(1) / std::accumulate(iFirst, iLast, prc_t(0));
  for (; iFirst != iLast; ++iFirst, ++oFirst) *oFirst = *iFirst * sumInv;
}
}  // namespace smashpp

#endif  // PROJECT_CONTAINER_HPP