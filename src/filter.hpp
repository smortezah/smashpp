//
// Created by morteza on 19-09-2018.
//

#ifndef PROJECT_FILTER_HPP
#define PROJECT_FILTER_HPP

#include "par.hpp"

class Filter {
 public:
  Filter          () = default;
  explicit Filter (const Param&);
  void smooth     ();

 private:
  u32           wsize;
  string        wtype;
  vector<float> window;

  void config      (const Param&);
  void make_window ();
  void hamming     ();
};

#endif //PROJECT_FILTER_HPP