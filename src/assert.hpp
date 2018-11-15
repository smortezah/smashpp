//
// Created by morteza on 03-05-2018.
//

#ifndef PROJECT_ASSERT_HPP
#define PROJECT_ASSERT_HPP

#include <stdexcept>
#include "fn.hpp"

namespace smashpp {
template <typename Container>
static void assert_empty_elem (const Container& cont, string&& msg) { //Empty el
  for (const auto& e : cont)
    if (e.size() == 0)
      error(std::move(msg));
}


template <typename Value>
class ValRange {
 public:
  ValRange () = default;
  ValRange (Value min_, Value max_, Value d_, string&& l_, string&& c_,
            string&& m_, Problem p_) 
            : min(min_), max(max_), def(d_), label(move(l_)),
              criterion(move(c_)), initMode(move(m_)), problem(p_),
              inRange(true) {}
  void assert (Value&);

 private:
  Value   min, max, def;
  string  label, criterion, initMode;
  Problem problem;
  bool    inRange;
  string  message;
};

template <typename Value>
inline void ValRange<Value>::assert (Value& val) {
  bool isFloat = false;
  if (std::is_floating_point<Value>::value)
    isFloat=true;

  const auto append_msg = [&] (string&& msg) {
    message = "\""+label+"\" not in valid range " + msg;
    if (initMode == "default") {
      message += "Default value \""+
        (isFloat ? string_format("%.1f",def) : to_string(def)) +"\" been set.";
    }
    else if (initMode == "auto")
      message += "Will be automatically modified.";
    message += "\n";
  };
  
  if (criterion=="[]" && (val>max || val<min)) {
    inRange = false;
    string s = "["+
      (isFloat ? (string_format("%.1f",min)+";"+string_format("%.1f",max))
               : (to_string(min)+";"+to_string(max))) +"]. ";
    append_msg(std::move(s));
  }
  else if (criterion=="[)" && (val>=max || val<min)) {
    inRange = false;
    string s = "["+
      (isFloat ? (string_format("%.1f",min)+";"+string_format("%.1f",max))
               : (to_string(min)+";"+to_string(max))) +"). ";
    append_msg(std::move(s));
  }
  else if (criterion=="(]" && (val>max || val<=min)) {
    inRange = false;
    string s = "("+
      (isFloat ? (string_format("%.1f",min)+";"+string_format("%.1f",max))
               : (to_string(min)+";"+to_string(max))) +"]. ";
    append_msg(std::move(s));
  }
  else if (criterion=="()" && (val>=max || val<=min)) {
    inRange = false;
    string s = "("+
      (isFloat ? (string_format("%.1f",min)+";"+string_format("%.1f",max))
               : (to_string(min)+";"+to_string(max))) +"). ";
    append_msg(std::move(s));
  }
  
  if (!inRange) {
    val = def;
    if      (problem==Problem::WARNING)  warning(std::move(message));
    else if (problem==Problem::ERROR)    error(std::move(message));
  }
}


template <typename Value>
class ValSet {
 public:
  ValSet () = default;
  ValSet (vector<Value> set_, Value d_, string&& l_, string&& m_, 
          Problem p_, Value c_, bool i)
          : set(set_), def(d_), label(move(l_)), initMode(move(m_)),
            problem(p_), cmd(c_), inRange(i) {}
  void assert (Value&);

 private:
  vector<Value> set;
  Value         cmd, def;
  string        label, initMode;
  Problem       problem;
  bool          inRange;
  string        message;
};

template <typename Value>
inline void ValSet<Value>::assert (Value& val) {
  if (inRange) { val=cmd;  return; }

  val = def;
  const auto append_msg = [&] (string&& msg) {
    message = "\""+label+"\" not in valid set " + msg;
    if (initMode == "default") 
      message += "Default value \""+conv_to_string(def)+"\" been set.";
    else if (initMode == "auto")
      message += "Will be automatically modified.";
    message += "\n";
  };

  string msg = "{";
  for (auto it=set.begin(); it!=set.end()-1; ++it)
    msg += conv_to_string(*it)+", ";
  msg += conv_to_string(set.back())+"}. ";

  append_msg(std::move(msg));
  if      (problem==Problem::WARNING)  warning(std::move(message));
  else if (problem==Problem::ERROR)    error(std::move(message));
}
}

#endif //PROJECT_ASSERT_HPP