// Smash++
// Morteza Hosseini    mhosayny@gmail.com

#include "logtbl8.hpp"

#include <algorithm>
#include <fstream>
#include <array>

#include "exception.hpp"
using namespace smashpp;

LogTable8::LogTable8(uint8_t k_) : k(k_), tot(0) {
  try {  // 4<<2k = 4*2^2k = 4*4^k = 4^(k+1)
    tbl.resize(4ull << (k << 1u));
  } catch (std::bad_alloc& b) {
    error("failed memory allocation.");
  }
}

void LogTable8::update(LogTable8::ctx_t ctx) {
  if ((tot++ & POW2minus1[tbl[ctx]]) == 0)  // x % 2^n = x & (2^n-1)
    ++tbl[ctx];
}

auto LogTable8::query(LogTable8::ctx_t ctx) const -> LogTable8::val_t {
  return POW2minus1[tbl[ctx]];  // POW2[tbl[ctx]] - 1
}

auto LogTable8::query_counters(LogTable8::ctx_t l) const
    -> std::array<LogTable8::val_t, CARDIN> {
  auto row_address = &tbl[l];
  return {static_cast<LogTable8::val_t>((1ul << *row_address) - 1ul),
          static_cast<LogTable8::val_t>((1ul << *(row_address + 1ul)) - 1ul),
          static_cast<LogTable8::val_t>((1ul << *(row_address + 2ul)) - 1ul),
          static_cast<LogTable8::val_t>((1ul << *(row_address + 3ul)) - 1ul)};
}

#ifdef DEBUG
void LogTable8::dump(std::ofstream& ofs) const {
  ofs.write((const char*)&tbl[0], tbl.size());
  //  ofs.close();
}

void LogTable8::load(std::ifstream& ifs) const {
  ifs.read((char*)&tbl[0], tbl.size());
}

uint64_t LogTable8::get_total() const { return tot; }

uint64_t LogTable8::count_empty() const {
  return static_cast<uint64_t>(std::count(std::begin(tbl), std::end(tbl), 0));
}

uint32_t LogTable8::max_tbl_val() const {
  return *std::max_element(std::begin(tbl), std::end(tbl));
}

void LogTable8::print() const {
  constexpr uint8_t context_width{12};
  std::cerr.width(context_width);
  std::cerr << std::left << "Context";
  std::cerr << "Count\n";
  std::cerr << "-------------------\n";
  uint32_t i{0};
  for (const auto& c : tbl) {
    std::cerr.width(context_width);
    std::cerr << std::left << i++;
    std::cerr << static_cast<uint16_t>(c) << '\n';
  }
}
#endif