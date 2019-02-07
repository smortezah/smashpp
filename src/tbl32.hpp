#ifndef SMASHPP_TABLE32_HPP
#define SMASHPP_TABLE32_HPP

#include <vector>
#include "def.hpp"

namespace smashpp {
class Table32 {
 public:
  Table32 () : k(0), nRenorm(0), tot(0) {}
  explicit Table32 (uint8_t);
  void update (uint32_t);                   // Update table
  auto query (uint32_t) const -> uint32_t;  // Query count of ctx
  void dump (std::ofstream&) const;
  void load (std::ifstream&) const;
#ifdef DEBUG
  auto get_total () const -> uint64_t;  // Total count of all items in the table
  auto count_empty () const -> uint64_t;// Number of empty cells in the table
  auto max_tbl_val () const -> uint32_t;
  void print () const;
#endif

 private:
  std::vector<uint32_t> tbl;              // Table of 32 bit counters
  uint8_t k;                              // Ctx size
  uint32_t nRenorm;                       // Renormalization times
  uint64_t tot;                           // Total # elements so far
  
  void renormalize ();
};
}

#endif //SMASHPP_TABLE32_HPP