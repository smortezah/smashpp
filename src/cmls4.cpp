#include <random>
#include <fstream>
#include "cmls4.hpp"
#include "exception.hpp"
using namespace smashpp;

// W=[e/eps].      0 < eps:   error factor      < 1
// D=[ln 1/delta]. 0 < delta: error probability < 1
CMLS4::CMLS4(uint64_t w_, uint8_t d_) : w(w_), d(d_), tot(0) {
  try {
    sk.resize((d * w + 1) >> 1u);
  } catch (std::bad_alloc& b) {
    error("failed memory allocation.");
  }
  uhashShift = static_cast<uint8_t>(G - std::floor(std::log2(w)));
  ab.resize(d << 1u);
  set_a_b();
}

void CMLS4::set_a_b() {
  constexpr uint64_t seed{0};
  std::default_random_engine e(seed);
  std::uniform_int_distribution<uint64_t> uDistA(
      0, (1ull << 63u) - 1);  // k <= 2^63-1
  std::uniform_int_distribution<uint64_t> uDistB(0, (1ull << uhashShift) - 1);
  for (uint8_t i = 0; i != d; ++i) {
    ab[i << 1u] =
        (uDistA(e) << 1u) + 1;      // 1 <= a=2k+1 <= 2^64-1, rand odd posit.
    ab[(i << 1u) + 1] = uDistB(e);  // 0 <= b <= 2^(G-M)-1,   rand posit.
  }                                 // Parenthesis in ab[(i<<1)+1] are MANDATORY
}

void CMLS4::update(uint64_t ctx) {
  const auto c{min_log_ctr(ctx)};
  if (!(tot++ & POW2minus1[c]))     // Increase decision.  x % 2^n = x & (2^n-1)
  //    for (uint8_t i=0; i!=d; ++i) {
    for (uint8_t i = d; i--;) {
      const auto idx = hash(i, ctx);
      if (read_cell(idx) == c)      // Conservative update
        sk[idx >> 1u] = INC_CTR[((idx & 1ull) << 8u) + sk[idx >> 1u]];
    }
}

uint8_t CMLS4::min_log_ctr(uint64_t ctx) const {
  uint8_t min{15};  // 15 = max val in CTR[]
  //  for (uint8_t i=0; i!=d && min!=0; ++i) {
  for (uint8_t i = d; min != 0 && i--;) {
    const auto lg = read_cell(hash(i, ctx));
    if (lg < min) min = lg;
  }
  return min;
}

uint8_t CMLS4::read_cell(uint64_t idx) const {
  return CTR[((idx & 1ull) << 8u) + sk[idx >> 1u]];
  ////  return CTR[static_cast<uint16_t>(((idx&1ull)<<8u) | sk[idx>>1u])];
}

// Strong 2-universal
uint64_t CMLS4::hash(uint8_t i, uint64_t ctx) const {
  return i * w + ((ab[i << 1u] * ctx + ab[(i << 1u) + 1]) >> uhashShift);
}

uint16_t CMLS4::query(uint64_t ctx) const {
  return FREQ2[min_log_ctr(ctx)];  // Base 2. otherwise (b^c-1)/(b-1)
}

void CMLS4::dump(std::ofstream& ofs) const {
  ofs.write((const char*)&sk[0], sk.size());
  //  ofs.close();
}

void CMLS4::load(std::ifstream& ifs) const {
  ifs.read((char*)&sk[0], sk.size());
}

#ifdef DEBUG
uint64_t CMLS4::get_total() const { return tot; }

uint64_t CMLS4::count_empty() const {
  uint64_t n{0};
  for (auto i = w * d; i--;)
    if (readCell(i) == 0) ++n;
  return n;
}

uint8_t CMLS4::max_sk_val() const {
  uint8_t c{0};
  for (auto i = w * d; i--;)
    if (readCell(i) > c) c = readCell(i);
  return c;
}

void CMLS4::print() const {
  constexpr uint8_t cell_width{3};
  for (uint8_t i = 0; i != d; ++i) {
    std::cerr << "d_" << static_cast<uint16_t>(i) << ":  ";
    for (uint64_t j = 0; j != w; ++j) {
      std::cerr.width(cell_width);
      std::cerr << std::left << static_cast<uint16_t>(readCell(i * w + j));
    }
    std::cerr << '\n';
  }
}

void CMLS4::printAB() const {
  constexpr uint8_t w{23}, bl{3};
  std::cerr.width(w);
  cerr << std::left << "a";
  std::cerr << "b\n";
  for (uint8_t i = 0; i != w - bl; ++i) std::cerr << "-";
  for (uint8_t i = 0; i != bl; ++i) std::cerr << " ";
  for (uint8_t i = 0; i != w - bl; ++i) std::cerr << "-";
  std::cerr << '\n';
  for (uint8_t i = 0; i != ab.size(); ++i) {
    std::cerr.width(w);
    std::cerr << std::left << ab[i];
    if (i & 1u) std::cerr << '\n';
  }
}
#endif