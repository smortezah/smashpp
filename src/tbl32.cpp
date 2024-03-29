// Smash++
// Morteza Hosseini    mhosayny@gmail.com

#include "tbl32.hpp"

#include <algorithm>
#include <fstream>
#include <array>

#include "exception.hpp"
using namespace smashpp;

Table32::Table32(uint8_t k_) : k(k_), nRenorm(0), tot(0) {
  try {  // 4<<2k = 4*2^2k = 4*4^k = 4^(k+1)
    tbl.resize(4ull << (k << 1u));
  } catch (std::bad_alloc& b) {
    error("failed memory allocation.");
  }
}

void Table32::update(Table32::ctx_t ctx) {
  if (tbl[ctx] == 0xFFFFFFFF)  // 2^32-1
    renormalize();
  ++tbl[ctx];
  ++tot;
}

inline void Table32::renormalize() {
  for (auto c : tbl) c >>= 1;
  ++nRenorm;
}

auto Table32::query(Table32::ctx_t ctx) const -> Table32::val_t {
  return tbl[ctx];
}

auto Table32::query_counters(Table32::ctx_t l) const
    -> std::array<Table32::val_t, CARDIN> {
  auto row_address = &tbl[l];
  return {*row_address, *(row_address + 1), *(row_address + 2),
          *(row_address + 3)};
}

#ifdef DEBUG
void Table32::dump(std::ofstream& ofs) const {
  ofs.write((const char*)&tbl[0], tbl.size());
}

void Table32::load(std::ifstream& ifs) const {
  ifs.read((char*)&tbl[0], tbl.size());
}

uint64_t Table32::get_total() const { return tot; }

uint64_t Table32::count_empty() const {
  return static_cast<uint64_t>(std::count(begin(tbl), end(tbl), 0));
}

uint32_t Table32::max_tbl_val() const {
  return *std::max_element(std::begin(tbl), std::end(tbl));
}

void Table32::print() const {
  constexpr uint8_t context_width{12};
  std::cerr.width(context_width);
  std::cerr << std::left << "Context";
  std::cerr << "Count\n";
  std::cerr << "-------------------\n";
  uint32_t i{0};
  for (const auto& c : tbl) {
    std::cerr.width(context_width);
    std::cerr << std::left << i++;
    std::cerr << c << '\n';
  }
}
#endif