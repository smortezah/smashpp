//
// Created by morteza on 13-03-2018.
//

#ifndef SMASHPP_TABLE32_HPP
#define SMASHPP_TABLE32_HPP

#include <vector>
#include "def.hpp"
using std::vector;
using std::ifstream;
using std::ofstream;

class Table32
{
public:
  Table32          () = default;
  explicit Table32 (u8);
  void config      (u8);
  void update      (u32);               // Update table
  u32  query       (u32)       const;   // Query count of ctx
  u64  getTotal    ()          const;   // Total count of all items in the table
  u64  countMty    ()          const;   // Number of empty cells in the table
  u32  maxTblVal   ()          const;
  void dump        (ofstream&) const;
  void load        (ifstream&) const;
  void print       ()          const;

private:
  vector<u32> tbl;                      // Table of 32 bit counters
  u8          k;                        // Ctx size
  u32         nRenorm;                  // Renormalization times
  u64         tot;                      // Total # elements so far
  
  void renormalize ();
};

#endif //SMASHPP_TABLE32_HPP