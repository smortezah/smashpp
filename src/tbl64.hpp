//
// Created by morteza on 15-03-2018.
//

#ifndef SMASHPP_TABLE64_HPP
#define SMASHPP_TABLE64_HPP

#include <vector>
#include "def.hpp"

namespace smashpp {
class Table64 {
 public:
  Table64          () : k(0)/*, tot(0)*/ {}
  explicit Table64 (u8);
  void config      (u8);
  void update      (u32);               // Update table
  u64  query       (u32)       const;   // Query count of ctx
  void dump        (ofstream&) const;
  void load        (ifstream&) const;
#ifdef DEBUG
  u64  get_total   ()          const;   // Total count of all items in the table
  u64  count_empty ()          const;   // Number of empty cells in the table
  u64  max_tbl_val ()          const;
  void print       ()          const;
#endif
  
 private:
  vector<u64> tbl;                      // Table of 64 bit counters
  u8          k;             // Ctx size
//  u64         tot;                      // Total # elements so far
};
}

#endif //SMASHPP_TABLE64_HPP