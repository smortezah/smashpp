// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2020, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_NUMBER_HPP
#define SMASHPP_NUMBER_HPP

#include <cmath>
#include <sstream>

#include "def.hpp"
#include "file.hpp"

namespace smashpp {
void error(std::string&&);  // To avoid circular dependency

template <typename Input>
inline static bool is_uint8_t(const Input& in) {
  return typeid(in) == typeid(uint8_t);
}

template <typename Value>
inline static bool is_odd(Value val) {
  if (val < 0) error("\"" + std::to_string(val) + "\" is a negative number.");

  return (val & 1ull);
}

template <typename T>
inline static auto pow2(T base) {  // Must be inline
  return std::pow(base, static_cast<T>(2));
}

// From http://martin.ankerl.com/2007/10/04/
// optimized-pow-approximation-for-java-and-c-c/
inline static double Power(double base, double exponent) {
  union {
    double d;
    int x[2];
  } u = {base};
  u.x[1] = static_cast<int>(exponent * (u.x[1] - 1072632447) + 1072632447);
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
inline static void keep_in_range(MinVal min, Val& val, MaxVal max) {
  if (val < min)
    val = min;
  else if (val > max)
    val = max;
}

template <class T, class U>
inline static std::string precision(T prec, U value) {
  std::ostringstream oss;
  oss << std::setprecision(prec) << value;
  return oss.str();
}

template <class T, class U>
inline static std::string fixed_precision(T prec, U value) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(prec) << value;
  return oss.str();
}

inline static uint8_t num_digits(uint64_t number) {
  return number == 0 ? 1 : std::log10(static_cast<double>(number)) + 1;
}

template <typename T>
inline static std::string thousands_sep(T number) {
  std::ostringstream ss;
  // std::setlocale(LC_ALL, "en_US.UTF-8");
  // const auto lacale = "en_US.UTF8";
  // ss.imbue(std::locale(lacale));
  ss << number;
  return ss.str();
}

inline static double round_to_prec(double value, double precision = 1.0) {
  const auto reciprocal = 1.0 / precision;
  return std::ceil(value * reciprocal) / reciprocal;
}

inline static float tick_round(float lowerBound, float upperBound,
                               uint8_t n_ranges) {
  // Round fraction in range [0.1, 1)
  const auto round_frac = [=](float value) -> float {
    if (value >= 0.100 && value <= 0.125) return 0.1;
    // else if (value <= 0.175)  return 0.15;
    else if (value <= 0.225)
      return 0.2;
    else if (value <= 0.275)
      return 0.25;
    else if (value <= 0.325)
      return 0.3;
    // else if (value <= 0.375)  return 0.35;
    else if (value <= 0.425)
      return 0.4;
    // else if (value <= 0.475)  return 0.45;
    else if (value <= 0.525)
      return 0.5;
    // else if (value <= 0.575)  return 0.55;
    else if (value <= 0.625)
      return 0.6;
    // else if (value <= 0.675)  return 0.65;
    else if (value <= 0.725)
      return 0.7;
    else if (value <= 0.775)
      return 0.75;
    else if (value <= 0.825)
      return 0.8;
    // else if (value <= 0.875)  return 0.85;
    else if (value <= 0.925)
      return 0.9;
    // else if (value <= 0.975)  return 0.95;

    return 1.0;
  };

  float tick{(upperBound - lowerBound) / n_ranges};
  auto div{POW10[num_digits(tick)]};
  float frac{tick / div};
  return round_frac(frac) * div;
}

template <typename Int>
inline static Int map_interval(Int in_first, Int in_last, Int out_first,
                               Int out_last, Int value) {
  if (in_last == in_first)
    return out_first;
  else if (in_last == out_last && in_first == out_first)
    return value;

  const auto slope{static_cast<double>(out_last - out_first) /
                   (in_last - in_first)};
  return std::round(out_first + slope * (value - in_first));
}
}  // namespace smashpp

#endif  // SMASHPP_NUMBER_HPP