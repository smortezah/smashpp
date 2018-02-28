//
// Created by morteza on 27-02-2018.
//

#ifndef SMASHPP_CMLS_HPP
#define SMASHPP_CMLS_HPP

#include "def.hpp"
using std::vector;
using std::array;

class CMLS    // Count-min-log sketch
{
 public:
  CMLS             ();
  void update      (u64, u64);    // Update ctx by count c
  u64  estimate    (u64);         // Estimate count of ctx
  u64  getTotal    ();            // Total count
  void printSketch ();

 private:
  u32  w;                         // Width of sketch
  u8   d;                         // Depth of sketch
  vector<array<u64,2>> ab;        // Coefficients of hash functions
  u64  M;                         // Universal hash function: (a*x+b) >> (G-M)
  vector<vector<u64>>  sk;        // Sketch
  u64  tot;
  
  void setAB ();                  // Set a and b, coeffs of hash functions
};

#endif //SMASHPP_CMLS_HPP