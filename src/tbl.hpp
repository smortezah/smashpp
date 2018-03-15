//
// Created by morteza on 13-03-2018.
//

#ifndef SMASHPP_TABLE_HPP
#define SMASHPP_TABLE_HPP

#include <vector>
#include "def.hpp"
using std::vector;
using std::ifstream;
using std::ofstream;

class Table
{
 public:
  Table          () = default;
  explicit Table (u8);
  void config    (u8);
  void update    (u64);             // Update table
  u32  query     (u64)       const; // Query count of ctx
  u64  getTotal  ()          const; // Total count of all items in the table
  u64  countMty  ()          const; // Number of empty cells in the table
  u32  maxTblVal ()          const;
  void dump      (ofstream&) const;
  void load      (ifstream&) const;
  void printTbl  ()          const;
  
 private:
  vector<u32> tbl;                  // Table
  u8          k;                    // Ctx size
  u32         nRenorm;              // Renormalization times
  u64         tot;                  // Total # elements so far
  
  void renormalize ();
};

#endif //SMASHPP_TABLE_HPP