#ifndef SMASHPP_TABLE64_HPP
#define SMASHPP_TABLE64_HPP

#include <vector>
#include "def.hpp"

namespace smashpp {
class Table64 {
 public:
  Table64 () : k(0)/*, tot(0)*/ {}
  explicit Table64 (u8);
  void config (u8);
  void update (u32);                 // Update table
  auto query (u32) const -> u64;     // Query count of ctx
  void dump (ofstream&) const;
  void load (ifstream&) const;
#ifdef DEBUG
  auto get_total () const -> u64;    // Total count of all items in the table
  auto count_empty () const -> u64;  // Number of empty cells in the table
  auto max_tbl_val () const -> u64;
  void print() const;
#endif
  
 private:
  vector<u64> tbl;                   // Table of 64 bit counters
  u8 k;                              // Ctx size
};
}

#endif //SMASHPP_TABLE64_HPP