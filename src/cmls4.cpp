//
// Created by morteza on 27-02-2018.
//

#include <random>
#include <fstream>
#include "cmls4.hpp"
#include "fn.hpp"

// W=[e/eps].      0 < eps:   error factor      < 1
// D=[ln 1/delta]. 0 < delta: error probability < 1
CMLS4::CMLS4 (u64 w_, u8 d_) {
  config(w_, d_);
}

void CMLS4::config (u64 w_, u8 d_) {
  w   = w_;
  d   = d_;
  tot = 0;
  try { sk.resize((d*w+1)>>1u); }
  catch (std::bad_alloc& b) {
    error("failed memory allocation.");
  }
  uhashShift = static_cast<u8>(G - std::ceil(std::log2(w)));
  ab.resize(d<<1u);
  set_a_b();
}

inline void CMLS4::set_a_b () {
  constexpr u64 seed {0};
  std::default_random_engine e(seed);
  std::uniform_int_distribution<u64> uDistA(0, (1ull<<63u)-1);    // k <= 2^63-1
  std::uniform_int_distribution<u64> uDistB(0, (1ull<<uhashShift)-1);
  for (u8 i=0; i!=d; ++i) {
    ab[i<<1u] = (uDistA(e)<<1u) + 1;// 1 <= a=2k+1 <= 2^64-1, rand odd posit.
    ab[(i<<1u)+1] = uDistB(e);      // 0 <= b <= 2^(G-M)-1,   rand posit.
  }                                 // Parenthesis in ab[(i<<1)+1] are MANDATORY
}

void CMLS4::update (u64 ctx) {
  const auto c {min_log_ctr(ctx)};
  if (!(tot++ & POW2minus1[c]))     // Increase decision.  x % 2^n = x & (2^n-1)
//    for (u8 i=0; i!=d; ++i) {
    for (u8 i=d; i--;) {
      const auto idx = hash(i, ctx);
      if (read_cell(idx) == c)       // Conservative update
        sk[idx>>1u] = INC_CTR[((idx&1ull)<<8u) + sk[idx>>1u]];
    }
}

inline u8 CMLS4::min_log_ctr (u64 ctx) const {
  u8 min {std::numeric_limits<u8>::max()};
//  for (u8 i=0; i!=d && min!=0; ++i) {
  for (u8 i=d; min!=0 && i--;) {
    const auto lg = read_cell(hash(i, ctx));
    if (lg < min)
      min = lg;
  }
  return min;
}

inline u8 CMLS4::read_cell (u64 idx) const {
  return CTR[((idx&1ull)<<8u) + sk[idx>>1u]];
//  return CTR[static_cast<u16>(((idx&1ull)<<8u) | sk[idx>>1u])];
}

inline u64 CMLS4::hash (u8 i, u64 ctx) const {    // Strong 2-universal
  return i*w + ((ab[i<<1u]*ctx + ab[(i<<1u)+1]) >> uhashShift);
}

u16 CMLS4::query (u64 ctx) const {
  return FREQ2[min_log_ctr(ctx)];  //Base 2. otherwise (b^c-1)/(b-1)
}

void CMLS4::dump (ofstream& ofs) const {
	ofs.write((const char*) &sk[0], sk.size());
//  ofs.close();
}

void CMLS4::load (ifstream& ifs) const {
	ifs.read((char*) &sk[0], sk.size());
}

#ifdef DEBUG
u64 CMLS4::get_total () const {
  return tot;
}

u64 CMLS4::count_empty () const {
  u64 n {0};
  for (auto i=w*d; i--;)
    if (readCell(i) == 0)
      ++n;
	return n;
}

u8 CMLS4::max_sk_val () const {
  u8 c {0};
  for (auto i=w*d; i--;)
    if (readCell(i) > c)
      c = readCell(i);
	return c;
}

void CMLS4::print () const {
  constexpr u8 cell_width {3};
  for (u8 i=0; i!=d; i++) {
    cerr << "d_" << static_cast<u16>(i) << ":  ";
    for (u64 j=0; j!=w; j++) {
      cerr.width(cell_width);
      cerr << std::left << static_cast<u16>(readCell(i*w+j));
    }
    cerr << '\n';
  }
}

inline void CMLS4::printAB () const {
  constexpr u8 w{23}, bl{3};
  cerr.width(w);  cerr<<std::left<<"a";
  cerr << "b\n";
  for (u8 i=0; i!=w-bl; ++i)  cerr<<"-";
  for (u8 i=0; i!=bl;   ++i)  cerr<<" ";
  for (u8 i=0; i!=w-bl; ++i)  cerr<<"-";
  cerr << '\n';
  for (u8 i=0; i!=ab.size(); ++i) {
    cerr.width(w);  cerr<<std::left<<ab[i];
    if (i & 1u)
      cerr << '\n';
  }
}
#endif