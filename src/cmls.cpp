//
// Created by morteza on 27-02-2018.
//

//#include <iostream>
//#include <cmath>
//#include <cstdlib>
//#include <ctime>
//#include <limits>
//using namespace std;




#include <random>
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

/*
 * Update context count
 */
void CMLS::update (u64 ctx, u64 c)
{
  tot += c;
  for (u8 i=0; i!=d; ++i) {
    u64 hashVal = (ab[i][0]*ctx + ab[i][1]) %LONG_PRIME %w;
    sk[i][hashVal] += c;
  }
}

// CMLS estimate item count (int)
u64 CMLS::estimate(u64 ctx) {
  u64 min = std::numeric_limits<u64>::max();
  for (u8 i=0; i!=d; i++) {
    u64 hashVal = (ab[i][0]*ctx + ab[i][1]) %LONG_PRIME %w;
    if (sk[i][hashVal] < min)
      min = sk[i][hashVal];
  }
  return min;
}

// CMLS getTotal returns the
// tot count of all items in the sketch
u64 CMLS::getTotal () {
  return tot;
}

/*
 * Print sketch
 */
void CMLS::printSketch ()
{
  for (u64 i=0; i!=d; i++) {
    for (u64 j=0; j!=w; j++)
      std::cerr << sk[i][j] << ' ';
    std::cerr << "\n\n";
  }
}