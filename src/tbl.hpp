//
// Created by morteza on 13-03-2018.
//

#ifndef SMASHPP_TABLE_HPP
#define SMASHPP_TABLE_HPP

#include <vector>
#include "def.hpp"
using std::vector;

class Table
{
 public:
  Table          ();
//  Table           (u64, u8);
//  void update    (u64);             // Update table
//  u16  query     (u64)       const; // Query count of ctx
//  u64  getTotal  ()          const; // Total count of all items in the table
//  u64  countMty  ()          const; // Number of empty cells in the table
//  u32   maxTblVal  ()          const;
//  void dump      (ofstream&) const;
//  void load      (ifstream&) const;
//  void printTbl   ()          const;

 private:
  vector<u32> tbl;                   // Table
  u64         tot;                  // Total # elements so far
};

#endif //SMASHPP_TABLE_HPP