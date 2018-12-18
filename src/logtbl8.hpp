#ifndef SMASHPP_LOGTABLE8_HPP
#define SMASHPP_LOGTABLE8_HPP

#include <vector>
#include "def.hpp"

namespace smashpp {
class LogTable8 {
 public:
  LogTable8 () : k(0), tot(0) {}
  explicit LogTable8 (u8);
  void update (u32);                 // Update table
  auto query (u32) const -> u64;     // Query count of ctx
  void dump (ofstream&) const;
  void load (ifstream&) const;
#ifdef DEBUG
  auto get_total () const -> u64;    // Total count of all items in the table
  auto count_empty () const -> u64;  // Number of empty cells in the table
  auto max_tbl_val () const -> u32;
  void print () const;
#endif

 private:
  vector<u8> tbl;                    // Table of 8 bit logarithmic counters
  u8  k;                             // Ctx size
  u64 tot;                           // Total # elements so far
};
}

#endif //SMASHPP_LOGTABLE8_HPP