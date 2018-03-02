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
  void update      (u64);         // Update sketch
  u64  estimate    (u64);         // Estimate count of ctx
  u64  getTotal    ();            // Total count of all items in the sketch
  void printSketch ();

 private:
  u64 w;                          // Width of sketch
  u8  d;                          // Depth of sketch
  vector<array<u64,2>> ab;        // Coefficients of hash functions
  u64 uhashShift;                 // Universal hash shift. G-M in (a*x+b)>>(G-M)
  vector<vector<u32>>  sk;        // Sketch
  u64 tot;                        // Total # elements so far
  u32 minLog;                     // Min log value for current ctx
  
  bool incDecision (u64);         // Increase decision
  void setAB ();                  // Set coeffs a, b of hash funcs (a*x+b) %P %w
  u64  hash  (u8, u64)   const;
  u32  minLogCount (u64) const;   // Find min log value in the sketch
};

#endif //SMASHPP_CMLS_HPP