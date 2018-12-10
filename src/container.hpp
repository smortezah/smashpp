#ifndef PROJECT_CONTAINER_HPP
#define PROJECT_CONTAINER_HPP

#include "def.hpp"

namespace smashpp {
// Split a range by delim and insert the result into an std::vector
template <typename InIter, typename Vec>
inline static void split (InIter first, InIter last, char delim, Vec& vOut) {
  while (true) {
    InIter found = find(first, last, delim);
    vOut.emplace_back(string(first,found));
    if (found == last)
      break;
    first = ++found;
  }
}

template <typename Iter, typename Element>
inline static bool has (Iter first, Iter last, Element elem) {
  return find(first, last, elem) != last;
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
inline static u8 pop_count (Digit d) {  // Number of ones in a digit
  u8 n = 0;
  for (; d; ++n)  d &= d-1;  // First impl. Kernighan. Clear the LSB set
//  do { if (d & 1) ++n; } while (d >>= 1); // Second impl. Negative nums?
  return n;
}
#endif

template <typename Iter, typename Value>
inline static bool are_all (Iter first, Value val) {
  return all_of(first, first+CARDIN, 
    [val](u64 i) { return i==static_cast<u64>(val); });
}

template <typename Iter>
inline static bool has_multi_max (Iter first) {
  auto last = first + CARDIN;
  for (const auto max_pos=std::max_element(first,last); last-- != first;)
    if (*last==*max_pos && last!=max_pos)
      return true;
  return false;
}

template <typename Iter, typename PosIter>
inline static bool has_multi_max (Iter first, PosIter maxPos) {
  for (auto last=first+CARDIN; last-- != first;)
    if (*last==*maxPos && last!=maxPos)
      return true;
  return false;
}

template <typename Iter>
inline static u8 best_sym (Iter first) {
  return static_cast<u8>(*std::max_element(first, first+CARDIN));
}

template <typename Iter>
inline static u8 best_sym_abs (Iter first) {
  const auto max_pos = std::max_element(first, first+CARDIN);
  return static_cast<u8>(has_multi_max(first, max_pos) ? 255 : max_pos-first);
}

template <typename OutIter, typename InIter>
inline static void normalize (OutIter oFirst, InIter iFirst, InIter iLast) {
  const auto sumInv = prc_t(1) / accumulate(iFirst, iLast, prc_t(0));
  for (; iFirst!=iLast; ++iFirst,++oFirst)
    *oFirst = *iFirst * sumInv;
}
}

#endif //PROJECT_CONTAINER_HPP