// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_LOGTABLE8_HPP
#define SMASHPP_LOGTABLE8_HPP

#include <vector>
#include "def.hpp"

namespace smashpp {
class LogTable8 {
 public:
  LogTable8() : k(0), tot(0) {}
  explicit LogTable8(uint8_t);
  void update(uint32_t);                   // Update table
  auto query(uint32_t) const -> uint64_t;  // Query count of ctx
  void dump(std::ofstream&) const;
  void load(std::ifstream&) const;
#ifdef DEBUG
  auto get_total() const -> uint64_t;  // Total count of all items in the table
  auto count_empty() const -> uint64_t;  // Number of empty cells in the table
  auto max_tbl_val() const -> uint32_t;
  void print() const;
#endif

 private:
  std::vector<uint8_t> tbl;  // Table of 8 bit logarithmic counters
  uint8_t k;                 // Ctx size
  uint64_t tot;              // Total # elements so far
};
}  // namespace smashpp

#endif  // SMASHPP_LOGTABLE8_HPP