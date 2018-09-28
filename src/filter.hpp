//
// Created by morteza on 19-09-2018.
//

#ifndef PROJECT_FILTER_HPP
#define PROJECT_FILTER_HPP

#include <memory>
#include "par.hpp"

class Filter {
 public:
  u64   nSegs;
  float thresh;

//  Filter           () = default;
  explicit Filter  (const Param&);
  void smooth_seg  (const Param &);
  void extract_seg (const string&, const string&) const;

 private:
  WType         wtype;
  u32           wsize;
  vector<float> window;

  void config              (const Param&);
  void config_wtype        (const string&);
  void make_window         ();
  void hamming             ();
  void hann                ();
  void blackman            ();
  void triangular          ();  // Bartlett window
  void welch               ();
  void sine                ();
  void nuttall             ();
  void smooth_seg_rect     (const Param &);
  void smooth_seg_non_rect (const Param &);
#ifdef BENCH
  template <typename Iter, typename Value>
  void shift_left_insert (Iter, Value);
#endif
};

#endif //PROJECT_FILTER_HPP