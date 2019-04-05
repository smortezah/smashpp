// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_NUMBER_HPP
#define SMASHPP_NUMBER_HPP

#include <cmath>
#include "def.hpp"
// #include "exception.hpp"
// #include "string.hpp"

namespace smashpp {
template <typename Input>
inline static bool is_uint8_t(const Input& in) {
  return typeid(in) == typeid(uint8_t);
}

template <typename Value>
inline static bool is_odd(Value val) {
  // std::string message = "Error: " + std::move(msg);
  // wrap_text(message);
  // throw std::runtime_error(bold_red(message.substr(0, 6)) + message.substr(6)
  // +
  //                          "\n");
  if (val < 0) {
    // std::string message =
        // "Error: \"" + std::to_string(val) + "\" is a negative number.";
    // wrap_text(message);
    // std::string message = "\033[1m\033[38;5;1m" + "Error:" + "\033[0m" + " \"" +
    //                       std::to_string(val) + "\" is a negative number.\n";
    std::string message = std::string("\033[1m") +
                          "Error:" + std::string("\033[0m") + " \"" +
                          std::to_string(val) + "\" is a negative number.\n";
    throw std::runtime_error(message);
  }

  // if (val < 0) error("\"" + std::to_string(val) + "\" is a negative
  // number.");

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

template <typename T>
inline static std::string precision(T value) {
  std::ostringstream oss;
  oss << std::setprecision(value);
  return oss.str();
}

template <typename T>
inline static std::string fixed_precision(T value) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(value);
  return oss.str();
}

inline static uint8_t num_digits(uint64_t number) {
  return number == 0 ? 1 : std::log10(static_cast<double>(number)) + 1;
}

template <typename T>
inline static std::string thousands_sep(T number) {
  std::ostringstream ss;
  ss.imbue(std::locale("en_US.UTF-8"));
  ss << number;
  return ss.str();
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
}  // namespace smashpp

#endif  // SMASHPP_NUMBER_HPP