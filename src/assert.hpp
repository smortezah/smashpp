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
  void assert ();

 private:
  Value   val, min, max, def, valToComp;
  string  label, criterion, initMode;
  Problem problem;
};

template <typename Value>
inline void ValRange<Value>::assert () {
  if (problem == Problem::WARNING) {
    if (criterion == "[]") {
      if (val <= min || val >= max) {
        const auto msg = "\""+variable+"\" not in valid range "
            "["+to_string(min)+";"+to_string(max)+"]. Default value "
            "\""+to_string(def)+"\" been set.";
      }
    }
  }
}
}

#endif //PROJECT_ASSERT_HPP