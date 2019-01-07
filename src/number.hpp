#ifndef PROJECT_NUMBER_HPP
#define PROJECT_NUMBER_HPP

#include <cmath>
#include "def.hpp"
#include "exception.hpp"

namespace smashpp {
template <typename Input>
inline static bool is_u8 (const Input& in) {
  return typeid(in)==typeid(u8);
}

template <typename Value>
inline static bool is_odd (Value val) {
  if (val < 0)
    error("\"" + to_string(val) + "\" is a negative number.");
  return (val & 1ull);
}

template <typename T>
inline static auto pow2 (T base) {  // Must be inline
  return std::pow(base, static_cast<T>(2));
}

// From http://martin.ankerl.com/2007/10/04/
// optimized-pow-approximation-for-java-and-c-c/
inline static double Power (double base, double exponent) {
  union {
    double d;
    int x[2];
  } u = { base };
  u.x[1] = int(exponent * (u.x[1]-1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
}
// inline static auto Power (double base, double exponent) {
//   int tmp  = (*(1 + (int*) &base)),
//       tmp2 = int(exponent * (tmp-1072632447) + 1072632447);
//   double p = 0.0;
//   *(1 + (int*) &p) = tmp2;
//   return p;
// }
// inline double Power(const double a, const double b) {
//   union {
//     double d;
//     struct {
//       int a;
//       int b;
//     } s;
//   } u = { a };
//   u.s.b = (int)(b * (u.s.b - 1072632447) + 1072632447);
//   u.s.a = 0;
//   return u.d;
// }

template <typename Val, typename MinVal, typename MaxVal>
inline static void keep_in_range (MinVal min, Val& val, MaxVal max) {
  if      (val < min)    val = min;
  else if (val > max)    val = max;
}

template <typename T>
inline static string precision (T value) {
  ostringstream oss;
  oss << std::setprecision(value);
  return oss.str();
}

template <typename T>
inline static string fixed_precision (T value) {
  ostringstream oss;
  oss << std::fixed << std::setprecision(value);
  return oss.str();
}

inline static u8 num_digits (u64 number) {
  return number==0 ? 1 : log10(static_cast<double>(number)) + 1;
}

template <typename T>
inline static string thousands_sep (T number) {
  ostringstream ss;
  ss.imbue(std::locale("en_US.UTF-8"));
  ss << number;
  return ss.str();
}
}

#endif //PROJECT_NUMBER_HPP