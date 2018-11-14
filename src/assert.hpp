//
// Created by morteza on 03-05-2018.
//

#ifndef PROJECT_ASSERT_HPP
#define PROJECT_ASSERT_HPP

#include <stdexcept>
#include "fn.hpp"

namespace smashpp {
template <typename Container>
static void assert_empty_elem (const Container& cont, string&& msg) {//Empty el
  for (const auto& e : cont)
    if (e.size() == 0)
      error(msg);
}
template <typename Container>
static void assert_empty_elem (const Container& cont, const string& msg) {
  for (const auto &e : cont)
    if (e.size() == 0)
      error(msg);
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
  void assert (Value& value);

 private:
  Value   min, max, def;
  string  label, criterion, initMode;
  Problem problem;
  bool    inRange;
  string  message;
};

template <typename Value>
inline void ValRange<Value>::assert (Value& val) {
  const auto append_msg = [&] (string&& msg) {
    message = "\""+label+"\" not in valid range " + msg;
    if (initMode == "default") 
      message += "Default value \""+to_string(def)+"\" been set.";
    else if (initMode == "auto")
      message += "Will be automatically modified.";
  };
  
  if (criterion=="[]" && (val>max || val<min)) {
    inRange = false;
    append_msg(move("["+to_string(min)+";"+to_string(max)+"]. "));
  }
  else if (criterion=="[)" && (val>=max || val<min)) {
    inRange = false;
    append_msg(move("["+to_string(min)+";"+to_string(max)+"). "));
  }
  else if (criterion=="(]" && (val>max || val<=min)) {
    inRange = false;
    append_msg(move("("+to_string(min)+";"+to_string(max)+"]. "));
  }
  else if (criterion=="()" && (val>=max || val<=min)) {
    inRange = false;
    append_msg(move("("+to_string(min)+";"+to_string(max)+"). "));
  }
  
  if (!inRange) {
    val = def;
    if      (problem==Problem::WARNING)  warning(move(message));
    else if (problem==Problem::ERROR)    error(move(message));
  }
}
}

#endif //PROJECT_ASSERT_HPP