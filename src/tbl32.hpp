#ifndef SMASHPP_TABLE32_HPP
#define SMASHPP_TABLE32_HPP

#include <vector>
#include "def.hpp"

namespace smashpp {
class Table32 {
 public:
  Table32 () : k(0), nRenorm(0), tot(0) {}
  explicit Table32 (u8);
  void config (u8);
  void update (u32);                 // Update table
  auto query (u32) const -> u32;     // Query count of ctx
  void dump (ofstream&) const;
  void load (ifstream&) const;
#ifdef DEBUG
  auto get_total () const -> u64;    // Total count of all items in the table
  auto count_empty () const -> u64;  // Number of empty cells in the table
  auto max_tbl_val () const -> u32;
  void print () const;
#endif

 private:
  vector<u32> tbl;                   // Table of 32 bit counters
  u8  k;                             // Ctx size
  u32 nRenorm;                       // Renormalization times
  u64 tot;                           // Total # elements so far
  
  void renormalize ();
};
}

#endif //SMASHPP_TABLE32_HPP