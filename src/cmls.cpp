//
// Created by morteza on 27-02-2018.
//

#include <random>
#include "cmls.hpp"

CMLS::CMLS () {
  w = DEF_W;    // w=[e/eps].      0 < eps:   error factor      < 1
  d = DEF_D;    // d=[ln 1/delta]. 0 < delta: error probability < 1
  tot = 0;
  sk.resize(d, vector<u8>(w));
  uhashShift = static_cast<u8>(G - std::ceil(std::log2(w)));
  ab.reserve(d);
  setAB();
}

void CMLS::update (u64 ctx) {
  auto c = minLogCount(ctx);
  if (incDecision(c)) {
    for (u8 i=0; i!=d; ++i) {
      auto cellIdx = hash(i, ctx);
      if (sk[i][cellIdx] == c)    // Conservative update
//        sk[i][cellIdx] = INC[c]
;
    }
  }
}

inline u8 CMLS::minLogCount (u64 ctx) const {
  u8 min = std::numeric_limits<u8>::max();
  for (u8 i=0; i!=d && min!=0; i++) {
    auto lg = readCell(i, hash(i,ctx));
//    auto lg = sk[i][hash(i,ctx)];
    if (lg < min)
      min = lg;
  }
  return min;
}

inline u8 CMLS::readCell(u8 i, u64 cell_idx) const {
//  return logCounter[cell_idx % 4][sk[i][cell_idx >> 1]];
}

inline bool CMLS::incDecision (u8 c) {
  return !(tot++ % POW2[c]); //todo. base 2
}

inline u64 CMLS::hash (u8 i, u64 ctx) const {
  return (ab[i][0]*ctx + ab[i][1]) >> uhashShift;
//  return i*w + ((ab[i][0]*ctx + ab[i][1]) >> uhashShift);
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

u16 CMLS::query (u64 ctx) const {
  auto c = minLogCount(ctx);
  return static_cast<u16>(POW2[c]-1);  //todo. base 2. otherwise (b^c-1)/(b-1)
}

u64 CMLS::getTotal () const {
  return tot;
}

void CMLS::printSketch () const {
  for (auto i=0; i!=d; i++) {
    for (auto j=0; j!=w; j++)
      std::cerr << static_cast<u16>(sk[i][j]) << ' ';
    std::cerr << "\n-------------------------------------------\n";
  }

//  for (u32 i=0; i!=d; i++) {
//    for (u32 j=0; j!=w; j++)
//      std::cerr << sk[i][j] << ' ';
//    std::cerr << "\n";
//  }

//  for (u32 j=0; j!=w; j++)
//    std::cerr << sk[0][j] << ' ';
//  std::cerr << "\n";
}