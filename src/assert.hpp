// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_ASSERT_HPP
#define SMASHPP_ASSERT_HPP

#include <stdexcept>
#include "exception.hpp"

namespace smashpp {
template <typename Value>
class ValRange {
 public:
  ValRange() = default;
  ValRange(Value min_, Value max_, Value d_, std::string&& l_, std::string&& c_,
           std::string&& m_, Problem p_)
      : min(min_),
        max(max_),
        def(d_),
        label(std::move(l_)),
        criterion(std::move(c_)),
        initMode(std::move(m_)),
        problem(p_),
        inRange(true) {}

  void assert(Value&);

 private:
  Value min;
  Value max;
  Value def;
  std::string label;
  std::string criterion;
  std::string initMode;
  Problem problem;
  bool inRange;
  std::string message;
};

template <typename Value>
void ValRange<Value>::assert(Value& val) {
  bool isFloat{false};
  if (std::is_floating_point<Value>::value) isFloat = true;

  const auto append_msg = [&](std::string&& msg) {
    message = "\"" + label + "\" not in valid range " + msg;
    if (initMode == "default")
      message += "Default value " +
                 (isFloat ? string_format("%.1f", def) : std::to_string(def)) +
                 " been set.";
    else if (initMode == "auto")
      message += "Will be automatically modified.";
    message += "\n";
  };
  const auto create_message = [=](char open, char close) {
    inRange = false;
    auto s =
        std::string(1, open) +
        (isFloat
             ? (string_format("%.1f", min) + "," + string_format("%.1f", max))
             : (std::to_string(min) + "," + std::to_string(max))) +
        std::string(1, close) + ". ";
    append_msg(std::move(s));
  };

  if (criterion == "[]" && (val > max || val < min))
    create_message('[', ']');
  else if (criterion == "[)" && (val >= max || val < min))
    create_message('[', ')');
  else if (criterion == "(]" && (val > max || val <= min))
    create_message('(', ']');
  else if (criterion == "()" && (val >= max || val <= min))
    create_message('(', ')');

  if (!inRange) {
    val = def;
    if (problem == Problem::warning)
      warning(std::move(message));
    else if (problem == Problem::error)
      error(std::move(message));
  }
}

template <typename Value>
class ValSet {
 public:
  ValSet() = default;
  ValSet(const std::vector<Value>& set_, Value d_, std::string&& l_,
         std::string&& m_, Problem p_, Value c_, bool i)
      : set(set_),
        cmd(c_),
        def(d_),
        label(std::move(l_)),
        initMode(std::move(m_)),
        problem(p_),
        inRange(i) {}

  void assert(Value&);

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
void ValSet<Value>::assert(Value& val) {
  if (inRange) {
    val = cmd;
    return;
  }

  val = def;
  const auto append_msg = [&](std::string&& msg) {
    message = "\"" + label + "\" not in valid set " + msg;
    if (initMode == "default")
      message += "Default value " + conv_to_string(def) + " been set.";
    else if (initMode == "auto")
      message += "Will be automatically modified.";
    message += "\n";
  };

  std::string msg = "{";
  for (auto it = begin(set); it != end(set) - 1; ++it)
    msg += conv_to_string(*it) + ", ";
  msg += conv_to_string(set.back()) + "}. ";

  append_msg(std::move(msg));
  if (problem == Problem::warning)
    warning(std::move(message));
  else if (problem == Problem::error)
    error(std::move(message));
}
}  // namespace smashpp

#endif  // SMASHPP_ASSERT_HPP