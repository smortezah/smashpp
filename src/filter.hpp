//
// Created by morteza on 19-09-2018.
//

#ifndef PROJECT_FILTER_HPP
#define PROJECT_FILTER_HPP

#include "par.hpp"

class Filter {
 public:
//  Filter          () = default;
  explicit Filter (const Param&);
  void smooth     (const Param&);

 private:
  WType         wtype;
  u32           wsize;
  vector<float> window;
  float         thresh;

  void config          (const Param&);
  void config_wtype    (const string&);
  void make_window     ();
  void hamming         ();
  void hann            ();
  void blackman        ();
  void triangular      ();  // Bartlett window
  void welch           ();
  void sine            ();
  void nuttall         ();
  void smooth_rect     (const Param&);
  void smooth_non_rect (const Param&);

#ifdef BENCH
  template <typename Iter, typename Value>
  void shift_left_insert (Iter, Value);
#endif
};

#endif //PROJECT_FILTER_HPP