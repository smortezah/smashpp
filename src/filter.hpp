// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_FILTER_HPP
#define SMASHPP_FILTER_HPP

#include <span>

#include "par.hpp"

namespace smashpp {
static constexpr uint8_t PREC_FIL{3};  // Precisions - floats in filt. file

class Filter {
 public:
  uint64_t nSegs;

  Filter();
  explicit Filter(const Param&);
  void smooth_seg(std::vector<PosRow>&, std::span<const prc_t>, Param&, uint8_t, uint64_t&);
  auto extract_seg(std::vector<PosRow>&, uint8_t, uint8_t, const std::string&, bool) const
      -> std::vector<SegmentView>;

 private:
  FilterType filt_type;
  uint32_t filt_size;
  std::string message;
  std::vector<float> window;
  struct Position {
    uint64_t beg;
    uint64_t end;
    Position(uint64_t b, uint64_t e) : beg(b), end(e) {}
  };

  void set_filt_size(const Param&);
  void show_info(const Param&) const;
  void make_window(uint32_t);
  void make_hamming(uint32_t);
  void make_hann(uint32_t);
  void make_blackman(uint32_t);
  void make_triangular(uint32_t);  // Bartlett window
  void make_welch(uint32_t);
  void make_sine(uint32_t);
  void make_nuttall(uint32_t);
  void smooth_seg_win1(std::vector<PosRow>&, std::span<const prc_t>, const Param&, uint8_t);
  template <bool SaveFilter>
  void smooth_seg_rect(std::vector<PosRow>&, std::span<const prc_t>, const Param&, uint8_t);
  template <bool SaveFilter>
  void smooth_seg_non_rect(std::vector<PosRow>&, std::span<const prc_t>, const Param&, uint8_t);

#ifdef BENCH
  template <typename Iter, typename Value>
  void shift_left_insert(Iter, Value);
#endif
};
}  // namespace smashpp

#endif  // SMASHPP_FILTER_HPP
