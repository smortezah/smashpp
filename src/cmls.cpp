//
// Created by morteza on 27-02-2018.
//

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
//using namespace std;




#include <random>
//#include <vector>
//using std::vector;
#include "cmls.hpp"

/*
 * Constructors
 * w=[e/eps].      0 < eps   = error factor      < 1
 * d=[ln 1/delta]. 0 < delta = error probability < 1
 */
CMLS::CMLS ()
{
  w   = DEF_W;
  d   = DEF_D;
  tot = 0;
  sk.resize(d, vector<u64>(w));
  ab.reserve(d);
  setAB();
  
//  //todo. print a, b
//  for (int i=0; i<d; i++)
//    for (int j=0; j<2; j++)
//      std::cerr << ab[i][j] << ' ';
}

/*
 * Set a and b: coefficients of hash functions
 */
inline void CMLS::setAB ()
{
  std::random_device r;// Seed with a real random value, if available
  std::default_random_engine e(r());// Choose a random mean between 1 and 6
  std::uniform_int_distribution<u32> uDist(0, std::numeric_limits<u32>::max());
  
  for (u8 i=0; i!=d; ++i) {
    ab[i][0] = uDist(e);
    ab[i][1] = uDist(e);
  }
}

//// generates a,b from field Z_p for use in hashing
//void CMLS::genajbj(int** hashes, int i) {
//  hashes[i][0] = int(float(rand())*float(LONG_PRIME)/float(RAND_MAX) + 1);
//  hashes[i][1] = int(float(rand())*float(LONG_PRIME)/float(RAND_MAX) + 1);
//}

/*
 * Update context count
 */
inline void CMLS::update (u64 ctx, u64 c)
{
  tot += c;
  for (u8 i=0; i!=d; ++i) {
    u64 hashVal = (ab[i][0]*ctx + ab[i][1]) %LONG_PRIME %w;
    sk[i][hashVal] += c;
  }
}

//// countMinSketch update item count (string)
//void CMLS::update(const char *str, int c) {
//  int hashval = hashstr(str);
//  update(hashval, c);
//}

// CMLS estimate item count (int)
inline u64 CMLS::estimate(u64 ctx) {
  u64 min = std::numeric_limits<u64>::max();
  for (u8 i=0; i!=d; i++) {
    u64 hashVal = (ab[i][0]*ctx + ab[i][1]) %LONG_PRIME %w;
    if (sk[i][hashVal] < min)
      min = sk[i][hashVal];
  }
  return min;
}

//// CMLS estimate item count (string)
//unsigned int CMLS::estimate(const char *str) {
//  int hashval = hashstr(str);
//  return estimate(hashval);
//}

// CMLS getTotal returns the
// tot count of all items in the sketch
inline u64 CMLS::getTotal () {
  return tot;
}

//// generates a hash value for a sting
//// same as djb2 hash function
//unsigned int CMLS::hashstr(const char *str) {
//  unsigned long hash = 5381;
//  int c;
//  while (c = *str++) {
//    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
//  }
//  return hash;
//}