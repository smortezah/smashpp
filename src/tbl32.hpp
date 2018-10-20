//
// Created by morteza on 13-03-2018.
//

#ifndef SMASHPP_TABLE32_HPP
#define SMASHPP_TABLE32_HPP

#include <vector>
#include "def.hpp"

class Table32
{
public:
  Table32          () : k(0), nRenorm(0), tot(0) {}
  explicit Table32 (u8);
  void config      (u8);
  void update      (u32);               // Update table
  u32  query       (u32)       const;   // Query count of ctx
  void dump        (ofstream&) const;
  void load        (ifstream&) const;
#ifdef DEBUG
  u64  get_total   ()          const;   // Total count of all items in the table
  u64  count_empty ()          const;   // Number of empty cells in the table
  u32  max_tbl_val ()          const;
  void print       ()          const;
#endif

private:
  vector<u32> tbl;                      // Table of 32 bit counters
  u8          k /*:BIT_K*/;             // Ctx size
  u32         nRenorm;                  // Renormalization times
  u64         tot;                      // Total # elements so far
  
  void renormalize ();
};

#endif //SMASHPP_TABLE32_HPP