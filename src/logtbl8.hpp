// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt

#ifndef SMASHPP_LOGTABLE8_HPP
#define SMASHPP_LOGTABLE8_HPP

#include <vector>

#include "def.hpp"

namespace smashpp {
class LogTable8 {
  using ctx_t = uint32_t;
  using val_t = uint32_t;

 private:
  std::vector<uint8_t> tbl;  // Table of 8 bit logarithmic counters
  uint8_t k;                 // Ctx size
  uint64_t tot;              // Total # elements so far

 public:
  LogTable8() : k(0), tot(0) {}
  explicit LogTable8(uint8_t);
  void update(ctx_t);                // Update table
  auto query(ctx_t) const -> val_t;  // Query count of ctx
  auto query_counters(ctx_t) const -> std::array<val_t, CARDIN>;

#ifdef DEBUG
  void dump(std::ofstream&) const;
  void load(std::ifstream&) const;
  auto get_total() const -> uint64_t;  // Total count of all items in the table
  auto count_empty() const -> uint64_t;  // Number of empty cells in the table
  auto max_tbl_val() const -> uint32_t;
  void print() const;
#endif
};
}  // namespace smashpp

#endif  // SMASHPP_LOGTABLE8_HPP