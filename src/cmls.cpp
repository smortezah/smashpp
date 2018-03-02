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
  sk.resize(d, vector<u32>(w));
  uhashShift = G - static_cast<u32>(std::ceil(std::log2(w)));
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

inline u32 CMLS::minLogCount (u64 ctx) const {
  u32 min = std::numeric_limits<u32>::max();
  for (u8 i=0; i!=d && min!=0; i++) {
    u32 lg = sk[i][hash(i,ctx)];
    if (lg < min)
      min = lg;
  }
  return min;
}

inline bool CMLS::incDecision (u64 ctx) {
  minLog = minLogCount(ctx);
  return !(tot++ % POW2[minLog]); //todo. base 2
}

void CMLS::update (u64 ctx) {
//  std::cout<<hash(0, ctx)<<'\t'<<hash(1, ctx)<<'\t'<<hash(2, ctx)<<'\n';//todo.
  
  if (incDecision(ctx)) {
    for (u8 i=0; i!=d; ++i) {
      auto cellIdx = hash(i, ctx);
      if (sk[i][cellIdx] == minLog)    // Conservative update
        sk[i][cellIdx] = INC[minLog];
    }
  }
}

u64 CMLS::estimate (u64 ctx) {
  u32 min = std::numeric_limits<u32>::max();
  for (u8 i=0; i!=d; i++) {
    u32 lg = sk[i][hash(i,ctx)];
    if (lg < min)
      min = lg;
  }
  return min;
}

u64 CMLS::getTotal () {
  return tot;
}

void CMLS::printSketch () {
  for (u32 i=0; i!=d; i++) {
    for (u32 j=0; j!=w; j++)
      std::cerr << sk[i][j] << ' ';
    std::cerr << "\n\n";
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

//size_t CMLS::read_cell(size_t cell_idx) {
//  constexpr auto logCounter = LogInt<1ull<<8>();
//  return logCounter.lg[cell_idx % 4][data[cell_idx >> 2]];
//}