//
// Created by morteza on 27-02-2018.
//

#include <random>
#include <fstream>
#include "cmls.hpp"
using std::cerr;

CMLS::CMLS () {
  w = DEF_W;    // w=[e/eps].      0 < eps:   error factor      < 1
  d = DEF_D;    // d=[ln 1/delta]. 0 < delta: error probability < 1
  tot = 0;
  try { sk.resize((d*w+1)>>1); }
  catch (std::bad_alloc& b) {
    cerr << "Error: failed memory allocation.";
    throw EXIT_FAILURE;
  }
  uhashShift = static_cast<u8>(G - std::ceil(std::log2(w)));
  ab.reserve(d<<1);
  setAB();
}

void CMLS::update (u64 ctx) {
  auto c = minLogCtr(ctx);
  if (incDecide(c)) {
    for (u8 i=0; i!=d; ++i) {
      auto cellIdx = hash(i, ctx);
      if (readCell(cellIdx) == c)    // Conservative update
        sk[cellIdx>>1] = INC_CTR[cellIdx&1][sk[cellIdx>>1]];
    }
  }
}

inline u8 CMLS::minLogCtr (u64 ctx) const {
  u8 min = std::numeric_limits<u8>::max();
  for (u8 i=0; i!=d && min!=0; i++) {
    auto lg = readCell(hash(i,ctx));
    if (lg < min)
      min = lg;
  }
  return min;
}

inline u8 CMLS::readCell(u64 idx) const {
  return CTR[idx&1][sk[idx>>1]];
}

inline bool CMLS::incDecide (u8 c) {
  return !(tot++ % POW2[c]); //todo. base 2
}

inline u64 CMLS::hash (u8 i, u64 ctx) const {
  return i*w + ((ab[i<<1]*ctx + ab[i<<1+1]) >> uhashShift);
}

inline void CMLS::setAB () {
  std::random_device r;              // Seed with a real random value, if avail.
  std::default_random_engine e(r());
  std::uniform_int_distribution<u64> uDistA(0, (1ull<<63)-1);     // k <= 2^63-1
  std::uniform_int_distribution<u64> uDistB(0, (1ull<<uhashShift)-1);
  for (u8 i=0; i!=d; ++i) {
    ab[i<<1]   = (uDistA(e)<<1) + 1; // 1 <= a=2k+1 <= 2^64-1, rand odd positive
    ab[i<<1+1] = uDistB(e);          // 0 <= b <= 2^(G-M)-1,   rand positive
  }
}

u16 CMLS::query (u64 ctx) const {
  auto c = minLogCtr(ctx);
  return static_cast<u16>(POW2[c]-1);  //todo. base 2. otherwise (b^c-1)/(b-1)
}

u64 CMLS::getTotal () const {
  return tot;
}

u64 CMLS::countMty () const {
  u64 n = 0;
  for (auto i=w*d; i--;) {
    if (readCell(i) == 0)
      ++n;
	}
	return n;
}

u8 CMLS::maxSkVal () const {
  u8 c = 0;
  for (auto i=w*d; i--;) {
    if (readCell(i) > c)
      c = readCell(i);
	}
	return c;
}

void CMLS::dump (ofstream& ofs) const {
	ofs.write((const char*) &sk[0], sk.size());
//  ofs.close();
}

void CMLS::load (ifstream& ifs) const {
	ifs.read((char*) &sk[0], sk.size());
}

void CMLS::printSk () const {
  for (auto i=0; i!=d; i++) {
    for (auto j=0; j!=w; j++)
      cerr << static_cast<u16>(readCell(i*w+j)) << ' ';
    cerr << "\n------------------------------------\n";
  }
}