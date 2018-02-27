//
// Created by morteza on 27-02-2018.
//

#ifndef SMASHPP_CMLS_HPP
#define SMASHPP_CMLS_HPP

#include <vector>
#include "def.hpp"
using std::vector;

#define LONG_PRIME 4294967311l
#define MIN(a,b)  (a < b ? a : b)

class CMLS    // Count-min-log sketch
{
public:
  CMLS  ();
  
  
  ~CMLS ();
  void update (int item, int c);// update item (int) by count c
  void update (const char*item, int c);// update item (string) by count c
  u32 estimate (int item);// estimate count of item i and return count
  u32 estimate (const char*item);// estimate count of item i
  u32 totalcount ();// tot count
  u32 hashstr (const char*str);// generates a hash value for a string. same as djb2 hash function

private:
  u32 w;           // Width of sketch
  u8  d;           // Depth of sketch
  vector<u64> sk;  // Sketch
  
  
  u32 a, b;// a, b in Z_p. hash generation function
  u32 tot;// tot count so far
  i32 **C;// array of arrays of counters
  i32 **hashes;// array {a,b} of hash values for a particular item
  void genajbj(int **hashes, int i);// generate "new" a,b
};

#endif //SMASHPP_CMLS_HPP