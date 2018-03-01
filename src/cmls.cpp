//
// Created by morteza on 27-02-2018.
//

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
  M   = static_cast<u64>(std::ceil(std::log2(w)));
  ab.reserve(d);
  setAB();
//todo. u64 uhShift = G - M; universal hash shift. define in class, private
}

/*
 * Set a and b, coefficients of hash functions: (a*x+b) mod P mod w
 */
inline void CMLS::setAB ()
{
  std::random_device r;    // Seed with a real random value, if available
  std::default_random_engine e(r());
  std::uniform_int_distribution<u64> uDistA(0, (1ull<<63)-1);     // k <= 2^63-1
  std::uniform_int_distribution<u64> uDistB(0, (1ull<<(G-M))-1);
  for (u8 i=0; i!=d; ++i) {
    ab[i][0] = (uDistA(e)<<1) + 1;   // 1 <= a=2k+1 <= 2^64-1, rand odd positive
    ab[i][1] = uDistB(e);            // 0 <= b <= 2^(G-M)-1,   rand positive
  }
}

/*
 * Update context by count c
 */
void CMLS::update (u64 ctx, u64 c)
{
  tot += c;
  for (u8 i=0; i!=d; ++i) {
    u64 hashVal = (ab[i][0]*ctx + ab[i][1]) >> uhShift;
    //u64 hashVal = (ab[i][0]*ctx + ab[i][1]) >> (G-M);
    sk[i][hashVal] += c;
  }
}

/*
 * Estimate count of ctx
 */
u64 CMLS::estimate (u64 ctx)
{
  u64 min = std::numeric_limits<u64>::max();
  for (u8 i=0; i!=d; i++) {
    u64 hashVal = (ab[i][0]*ctx + ab[i][1]) >> (G-M);
    if (sk[i][hashVal] < min)
      min = sk[i][hashVal];
  }
  return min;
}

/*
 * Total count of all items in the sketch
 */
u64 CMLS::getTotal ()
{
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
