#ifndef PROJECT_FILTER_HPP
#define PROJECT_FILTER_HPP

#include <memory>
#include "par.hpp"

namespace smashpp {
class Filter {
 public:
  u64 nSegs;

  explicit Filter (const Param&);
  void smooth_seg (const Param&);
  void merge_extract_seg (u32, const string&, const string&) const;
  void aggregate_mid_pos (u32, const string&, const string&) const;
  void aggregate_final_pos (const string&, const string&) const;

 private:
  WType  wtype;
  u32    wsize;
  string message;
  vector<float> window;
  struct Position {
    u64 beg, end;
    Position (u64 b, u64 e) : beg(b), end(e) {}
  };

  void set_wsize (const Param&);
  void show_info (const Param&) const;
  void make_window ();
  void hamming ();
  void hann ();
  void blackman ();
  void triangular ();  // Bartlett window
  void welch ();
  void sine ();
  void nuttall ();
  template <bool SaveFilter>
  void smooth_seg_rect (const Param&);
  template <bool SaveFilter>
  void smooth_seg_non_rect (const Param&);
  // bool is_mergable (const Position&, const Position&) const;
#ifdef BENCH
  template <typename Iter, typename Value>
  void shift_left_insert (Iter, Value);
#endif
};
}

#endif //PROJECT_FILTER_HPP