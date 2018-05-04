//
// Created by morteza on 03-05-2018.
//

#ifndef PROJECT_ASSERT_HPP
#define PROJECT_ASSERT_HPP

#include <stdexcept>
using std::string;

template <class Container>
void assert_empty_elem (Container cont, string&& msg) {  // Empty element
  for (const auto& e : cont)
    if (e.size() == 0)
      throw std::runtime_error(msg + "\n");
}

#endif //PROJECT_ASSERT_HPP