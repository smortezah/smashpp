// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_NUMBER_HPP
#define SMASHPP_NUMBER_HPP

#include <cmath>
#include <format>
#include <type_traits>
#include <typeinfo>

#include "def.hpp"
#include "file.hpp"

namespace smashpp {
void error(std::string&&);  // To avoid circular dependency

template <typename Input>
[[nodiscard]] inline static bool is_uint8_t(const Input& in) {
  return typeid(in) == typeid(uint8_t);
}

template <typename Value>
[[nodiscard]] inline static bool is_odd(Value val) {
  if (val < 0) {
    error(std::format("\"{}\" is a negative number.", val));
  }

  return (val & 1ull);
}

template <typename T>
[[nodiscard]] inline static auto pow2(T base) {  // Must be inline
  return std::pow(base, static_cast<T>(2));
}

// From http://martin.ankerl.com/2007/10/04/
// optimized-pow-approximation-for-java-and-c-c/
[[nodiscard]] inline static double Power(double base, double exponent) {
  union {
    double d;
    int x[2];
  } u = {base};
  u.x[1] = static_cast<int>(exponent * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
}

template <typename Val, typename MinVal, typename MaxVal>
inline static void keep_in_range(MinVal min, Val& val, MaxVal max) {
  if (val < min) {
    val = min;
  } else if (val > max) {
    val = max;
  }
}

template <class T, class U>
[[nodiscard]] inline static std::string precision(T prec, U value) {
  if constexpr (std::is_floating_point_v<U>) {
    return std::format("{:.{}g}", value, static_cast<int>(prec));
  } else {
    return std::format("{}", value);
  }
}

template <class T, class U>
[[nodiscard]] inline static std::string fixed_precision(T prec, U value) {
  if constexpr (std::is_floating_point_v<U>) {
    return std::format("{:.{}f}", value, static_cast<int>(prec));
  } else {
    return std::format("{}", value);
  }
}

[[nodiscard]] inline static uint8_t num_digits(uint64_t number) {
  uint8_t digits{1};
  while (number >= 10) {
    number /= 10;
    ++digits;
  }
  return digits;
}

template <typename T>
[[nodiscard]] inline static std::string thousands_sep(T number) {
  return std::format("{}", number);
}

[[nodiscard]] inline static double round_to_prec(double value, double precision = 1.0) {
  const auto reciprocal = 1.0 / precision;
  return std::ceil(value * reciprocal) / reciprocal;
}

[[nodiscard]] inline static float tick_round(float lowerBound, float upperBound, uint8_t n_ranges) {
  // Round fraction in range [0.1, 1)
  const auto round_frac = [=](float value) -> float {
    if (value >= 0.100 && value <= 0.125) {
      return 0.1f;
    } else if (value <= 0.225) {
      return 0.2f;
    } else if (value <= 0.275) {
      return 0.25f;
    } else if (value <= 0.325) {
      return 0.3f;
    } else if (value <= 0.425) {
      return 0.4f;
    } else if (value <= 0.525) {
      return 0.5f;
    } else if (value <= 0.625) {
      return 0.6f;
    } else if (value <= 0.725) {
      return 0.7f;
    } else if (value <= 0.775) {
      return 0.75f;
    } else if (value <= 0.825) {
      return 0.8f;
    } else if (value <= 0.925) {
      return 0.9f;
    }

    return 1.0f;
  };

  float tick{(upperBound - lowerBound) / n_ranges};
  auto div{POW10[num_digits(static_cast<uint64_t>(tick))]};
  float frac{tick / div};
  return round_frac(frac) * static_cast<float>(div);
}

template <typename Int>
[[nodiscard]] inline static Int map_interval(Int in_first, Int in_last, Int out_first, Int out_last,
                                             Int value) {
  if (in_last == in_first) {
    return out_first;
  } else if (in_last == out_last && in_first == out_first) {
    return value;
  }

  const auto slope{static_cast<double>(out_last - out_first) / (in_last - in_first)};
  return static_cast<Int>(std::round(out_first + slope * (value - in_first)));
}
}  // namespace smashpp

#endif  // SMASHPP_NUMBER_HPP
