// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef PROJECT_FILTER_HPP
#define PROJECT_FILTER_HPP

#include <memory>
#include "par.hpp"

namespace smashpp {
class Filter {
 public:
  uint64_t nSegs;

  explicit Filter(std::unique_ptr<Param>&);
  void smooth_seg(std::unique_ptr<Param>&);
  void merge_extract_seg(uint32_t, std::string, std::string) const;
  void aggregate_mid_pos(uint32_t, std::string, std::string) const;
  void aggregate_final_pos(std::string, std::string) const;

 private:
  WType wtype;
  uint32_t wsize;
  std::string message;
  std::vector<float> window;
  struct Position {
    uint64_t beg;
    uint64_t end;
    Position(uint64_t b, uint64_t e) : beg(b), end(e) {}
  };

  void set_wsize(std::unique_ptr<Param>&);
  void show_info(std::unique_ptr<Param>&) const;
  void make_window();
  void make_hamming();
  void make_hann();
  void make_blackman();
  void make_triangular();  // Bartlett window
  void make_welch();
  void make_sine();
  void make_nuttall();
  template <bool SaveFilter>
  void smooth_seg_rect(std::unique_ptr<Param>&);
  template <bool SaveFilter>
  void smooth_seg_non_rect(std::unique_ptr<Param>&);
  // bool is_mergable (const Position&, const Position&) const;
  void aggregate_final_pos_single(std::string, std::string, std::string) const;
  void aggregate_final_pos_double(std::string, std::string, std::string,
                                  std::string) const;
  void move_mid_to_pos_file(std::string, std::string) const;
  
#ifdef BENCH
  template <typename Iter, typename Value>
  void shift_left_insert(Iter, Value);
#endif
};
}  // namespace smashpp

#endif  // PROJECT_FILTER_HPP