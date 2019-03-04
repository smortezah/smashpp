#ifndef SMASHPP_TABLE64_HPP
#define SMASHPP_TABLE64_HPP

#include <vector>
#include "def.hpp"

namespace smashpp {
class Table64 {
 public:
  Table64() : k(0) {}
  explicit Table64(uint8_t);
  void update(uint32_t);                   // Update table
  auto query(uint32_t) const -> uint64_t;  // Query count of ctx
  void dump(std::ofstream&) const;
  void load(std::ifstream&) const;
#ifdef DEBUG
  auto count_empty() const -> uint64_t;  // Number of empty cells in the table
  auto max_tbl_val() const -> uint64_t;
  void print() const;
#endif

 private:
  std::vector<uint64_t> tbl;  // Table of 64 bit counters
  uint8_t k;                  // Ctx size
};
}  // namespace smashpp

#endif  // SMASHPP_TABLE64_HPP