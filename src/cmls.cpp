//
// Created by morteza on 27-02-2018.
//

#include <random>
#include "cmls.hpp"

CMLS::CMLS () {
  w      = DEF_W;            // w=[e/eps].      0 < eps:   error factor      < 1
  d      = DEF_D;            // d=[ln 1/delta]. 0 < delta: error probability < 1
  tot    = 0;
  minLog = 0;
  sk.resize(d, vector<u64>(w));
  uhashShift = G - static_cast<u64>(std::ceil(std::log2(w)));
  ab.reserve(d);
  setAB();
}

inline void CMLS::setAB () {
  std::random_device r;              // Seed with a real random value, if avail.
  std::default_random_engine e(r());
  std::uniform_int_distribution<u64> uDistA(0, (1ull<<63)-1);     // k <= 2^63-1
  std::uniform_int_distribution<u64> uDistB(0, (1ull<<uhashShift)-1);
  for (u8 i=0; i!=d; ++i) {
    ab[i][0] = (uDistA(e)<<1) + 1;   // 1 <= a=2k+1 <= 2^64-1, rand odd positive
    ab[i][1] = uDistB(e);            // 0 <= b <= 2^(G-M)-1,   rand positive
  }
}

inline u64 CMLS::hash (u8 i, u64 ctx) const {
  return (ab[i][0]*ctx + ab[i][1]) >> uhashShift;
}

inline u64 CMLS::minLogCount (u64 ctx) const {
  u64 min = std::numeric_limits<u64>::max();
  for (u8 i=0; i!=d && min!=0; i++) {
    u64 lg = sk[i][hash(i,ctx)];
    if (lg < min)
      min = lg;
  }
  return min;
}

inline bool CMLS::incDecision (u64 ctx) {
  auto lg = minLogCount(ctx);
  return !(tot++ % POW2[lg]); //todo. base 2
}

void CMLS::update (u64 ctx, u64 c) {
  tot += c;
  for (u8 i=0; i!=d; ++i) {
    u64 hashVal = (ab[i][0]*ctx + ab[i][1]) >> uhashShift;
    sk[i][hashVal] += c;
  }
}

u64 CMLS::estimate (u64 ctx) {
  u64 min = std::numeric_limits<u64>::max();
  for (u8 i=0; i!=d; i++) {
    u64 hashVal = (ab[i][0]*ctx + ab[i][1]) >> uhashShift;
    if (sk[i][hashVal] < min)
      min = sk[i][hashVal];
  }
  return min;
}

u64 CMLS::getTotal () {
  return tot;
}

void CMLS::printSketch () {
  for (u64 i=0; i!=d; i++) {
    for (u64 j=0; j!=w; j++)
      std::cerr << sk[i][j] << ' ';
    std::cerr << "\n\n";
  }
}

//size_t CMLS::read_cell(size_t cell_idx) {
//  constexpr auto logCounter = LogInt<1ull<<8>();
//  return logCounter.lg[cell_idx % 4][data[cell_idx >> 2]];
//}