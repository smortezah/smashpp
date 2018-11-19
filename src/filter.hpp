//
// Created by morteza on 19-09-2018.
//

#ifndef PROJECT_FILTER_HPP
#define PROJECT_FILTER_HPP

#include <memory>
#include "par.hpp"

namespace smashpp {
class Filter {
 public:
  u64   nSegs;

//  Filter                   () = default;
  explicit Filter          (const Param&);
  void smooth_seg          (const Param&);
  void extract_seg         (u32, const string&, const string&) const;
  void aggregate_mid_pos   (u32, const string&, const string&) const;
  void aggregate_final_pos (u32, const string&, const string&) const;

 private:
  WType         wtype;
  u32           wsize;
  vector<float> window;
  string        message;

  void config              (const Param&);
  void config_wsize        (const Param&);
  void show_info           (const Param&) const;
  void make_window         ();
  void hamming             ();
  void hann                ();
  void blackman            ();
  void triangular          ();  // Bartlett window
  void welch               ();
  void sine                ();
  void nuttall             ();
  template <bool SaveFilter>
  void smooth_seg_rect     (const Param&);
  template <bool SaveFilter>
  void smooth_seg_non_rect (const Param&);
#ifdef BENCH
  template <typename Iter, typename Value>
  void shift_left_insert (Iter, Value);
#endif
};
}

#endif //PROJECT_FILTER_HPP