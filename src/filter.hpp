// Smash++
// Morteza Hosseini    mhosayny@gmail.com

#ifndef SMASHPP_FILTER_HPP
#define SMASHPP_FILTER_HPP

#include <memory>
#include "par.hpp"

namespace smashpp {
static constexpr uint8_t PREC_FIL{3};  // Precisions - floats in filt. file

class Filter {
 public:
  uint64_t nSegs;

  Filter();
  explicit Filter(std::unique_ptr<Param>&);
  void smooth_seg(std::vector<PosRow>&, std::unique_ptr<Param>&, uint8_t,
                  uint64_t&);
  void extract_seg(std::vector<PosRow>&, uint8_t, uint8_t, std::string) const;

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

  void set_sample_step_size(std::unique_ptr<Param>&);
  void set_filt_size(std::unique_ptr<Param>&);
  void show_info(std::unique_ptr<Param>&) const;
  void make_window(uint32_t);
  void make_hamming(uint32_t);
  void make_hann(uint32_t);
  void make_blackman(uint32_t);
  void make_triangular(uint32_t);  // Bartlett window
  void make_welch(uint32_t);
  void make_sine(uint32_t);
  void make_nuttall(uint32_t);
  void smooth_seg_win1(std::vector<PosRow>&, std::unique_ptr<Param>&, uint8_t);
  template <bool SaveFilter>
  void smooth_seg_rect(std::vector<PosRow>&, std::unique_ptr<Param>&, uint8_t);
  template <bool SaveFilter>
  void smooth_seg_non_rect(std::vector<PosRow>&, std::unique_ptr<Param>&,
                           uint8_t);

#ifdef BENCH
  template <typename Iter, typename Value>
  void shift_left_insert(Iter, Value);
#endif
};
}  // namespace smashpp

#endif  // SMASHPP_FILTER_HPP