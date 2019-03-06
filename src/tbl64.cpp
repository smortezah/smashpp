// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#include <algorithm>
#include <fstream>
#include "tbl64.hpp"
#include "exception.hpp"
using namespace smashpp;

Table64::Table64(uint8_t k_) : k(k_) {
  try {  // 4<<2k = 4*2^2k = 4*4^k = 4^(k+1)
    tbl.resize(4ull << (k << 1u));
  } catch (std::bad_alloc& b) {
    error("failed memory allocation.");
  }
}

void Table64::update(uint32_t ctx) { ++tbl[ctx]; }

uint64_t Table64::query(uint32_t ctx) const { return tbl[ctx]; }

void Table64::dump(std::ofstream& ofs) const {
  ofs.write((const char*)&tbl[0], tbl.size());
  //  ofs.close();
}

void Table64::load(std::ifstream& ifs) const {
  ifs.read((char*)&tbl[0], tbl.size());
}

#ifdef DEBUG
uint64_t Table64::count_empty() const {
  return static_cast<uint64_t>(std::count(std::begin(tbl), std::end(tbl), 0));
}

uint64_t Table64::max_tbl_val() const {
  return *std::max_element(std::begin(tbl), std::end(tbl));
}

void Table64::print() const {
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