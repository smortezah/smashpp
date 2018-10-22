//
// Created by morteza on 03-05-2018.
//

#ifndef PROJECT_ASSERT_HPP
#define PROJECT_ASSERT_HPP

#include <stdexcept>
#include "fn.hpp"

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

#endif //PROJECT_ASSERT_HPP