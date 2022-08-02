// Smash++
// Morteza Hosseini    mhosayny@gmail.com

#ifndef SMASHPP_TABLE64_HPP
#define SMASHPP_TABLE64_HPP

#include <vector>

#include "def.hpp"

namespace smashpp {
class Table64 {
  using ctx_t = uint32_t;
  using val_t = uint64_t;

 private:
  std::vector<uint64_t> tbl;  // Table of 64 bit counters
  uint8_t k;                  // Ctx size

 public:
  Table64() : k(0) {}
  explicit Table64(uint8_t);
  void update(ctx_t);                // Update table
  auto query(ctx_t) const -> val_t;  // Query count of ctx
  auto query_counters(ctx_t) const -> std::array<val_t, CARDIN>;

#ifdef DEBUG
  void dump(std::ofstream&) const;
  void load(std::ifstream&) const;
  auto count_empty() const -> uint64_t;  // Number of empty cells in the table
  auto max_tbl_val() const -> uint64_t;
  void print() const;
  void print(std::string) const;
#endif
};
}  // namespace smashpp

#endif  // SMASHPP_TABLE64_HPP