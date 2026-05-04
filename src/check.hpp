// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_CHECKHPP
#define SMASHPP_CHECKHPP

#include <format>
#include <stdexcept>
#include <type_traits>

#include "exception.hpp"

namespace smashpp {
template <typename Value>
class ValRange {
 public:
  ValRange() = default;
  ValRange(Value min_, Value max_, Value d_, std::string&& l_, Interval i_, std::string&& m_,
           Problem p_)
      : min(min_),
        max(max_),
        def(d_),
        label(std::move(l_)),
        criterion(i_),
        initMode(std::move(m_)),
        problem(p_),
        inRange(true) {}

  void check(Value&);

 private:
  Value min;
  Value max;
  Value def;
  std::string label;
  Interval criterion;
  std::string initMode;
  Problem problem;
  bool inRange;
  std::string message;
};

template <typename Value>
void ValRange<Value>::check(Value& val) {
  const auto format_value = [](Value value) {
    if constexpr (std::is_floating_point_v<Value>) {
      return std::format("{:.1f}", value);
    } else {
      return std::format("{}", value);
    }
  };

  const auto append_msg = [&](std::string&& msg) {
    message = std::format("\"{}\" not in valid range {}", label, msg);
    if (initMode == "default") {
      message += std::format("Default value {} been set.", format_value(def));
    } else if (initMode == "auto") {
      message += "Will be automatically modified.";
    }
    message += "\n";
  };
  const auto create_message = [this, &append_msg, &format_value](char open, char close) {
    inRange = false;
    auto s = std::format("{}{},{}{}. ", open, format_value(min), format_value(max), close);
    append_msg(std::move(s));
  };

  if (criterion == Interval::closed && (val > max || val < min)) {
    create_message('[', ']');
  } else if (criterion == Interval::closed_open && (val >= max || val < min)) {
    create_message('[', ')');
  } else if (criterion == Interval::open_closed && (val > max || val <= min)) {
    create_message('(', ']');
  } else if (criterion == Interval::open && (val >= max || val <= min)) {
    create_message('(', ')');
  }

  if (!inRange) {
    val = def;
    if (problem == Problem::warning) {
      warning(std::move(message));
    } else if (problem == Problem::error) {
      error(std::move(message));
    }
  }
}

template <typename Value>
class ValSet {
 public:
  ValSet() = default;
  ValSet(const std::vector<Value>& set_, Value d_, std::string&& l_, std::string&& m_, Problem p_,
         Value c_, bool i)
      : set(set_),
        cmd(c_),
        def(d_),
        label(std::move(l_)),
        initMode(std::move(m_)),
        problem(p_),
        inRange(i) {}

  void check(Value&);

 private:
  std::vector<Value> set;
  Value cmd;
  Value def;
  std::string label;
  std::string initMode;
  Problem problem;
  bool inRange;
  std::string message;
};

template <typename Value>
void ValSet<Value>::check(Value& val) {
  if (inRange) {
    val = cmd;
    return;
  }

  val = def;
  const auto append_msg = [&](std::string&& msg) {
    message = std::format("\"{}\" not in valid set {}", label, msg);
    if (initMode == "default") {
      message += std::format("Default value {} been set.", conv_to_string(def));
    } else if (initMode == "auto") {
      message += "Will be automatically modified.";
    }
    message += "\n";
  };

  std::string msg = "{";
  for (auto it = begin(set); it != end(set) - 1; ++it) {
    msg += std::format("{}, ", conv_to_string(*it));
  }
  msg += std::format("{}}}. ", conv_to_string(set.back()));

  append_msg(std::move(msg));
  if (problem == Problem::warning) {
    warning(std::move(message));
  } else if (problem == Problem::error) {
    error(std::move(message));
  }
}
}  // namespace smashpp

#endif  // SMASHPP_CHECKHPP
