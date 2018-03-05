//
// Created by morteza on 27-02-2018.
//

#ifndef SMASHPP_CMLS_HPP
#define SMASHPP_CMLS_HPP

#include "def.hpp"
using std::vector;
using std::ifstream;
using std::ofstream;

class CMLS    // Count-min-log sketch
{
 public:
  CMLS ();
  void update (u64);              // Update sketch
  u16  query (u64) const;         // Query count of ctx
  u64  getTotal () const;         // Total count of all items in the sketch
  u64  countEmptyCells () const;  // Number of empty cells in the sketch
  u8   maxSketchVal () const;  // todo
//	void dump (ofstream&) const;
//	void load (ifstream&) const;
  void printSketch () const;

 private:
  u64 w;                          // Width of sketch
  u8  d;                          // Depth of sketch
  vector<u64> ab;                 // Coefficients of hash functions
  u8  uhashShift;                 // Universal hash shift. G-M in (a*x+b)>>(G-M)
  vector<u8>  sk;                 // Sketch
  u64 tot;                        // Total # elements so far
  
  u8   readCell (u64) const;      // Read each cell of the sketch
  bool incDecision (u8);          // Increase decision
  void setAB ();                  // Set coeffs a, b of hash funcs (a*x+b) %P %w
  u64  hash (u8, u64) const;
  u8   minLogCount (u64) const;   // Find min log value in the sketch
};

#endif //SMASHPP_CMLS_HPP