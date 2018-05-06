//
// Created by morteza on 15-03-2018.
//

#include <algorithm>
#include <fstream>
#include "tbl64.hpp"

Table64::Table64 (u8 k_) {
  config(k_);
}

void Table64::config (u8 k_) {
  k   = k_;
  tot = 0;
  try { tbl.resize(4ull<<(k<<1u)); }    // 4<<2k = 4*2^2k = 4*4^k = 4^(k+1)
  catch (std::bad_alloc& b) {
    std::runtime_error("Error: failed memory allocation.\n");
  }
}

void Table64::update (u32 ctx) {
  ++tbl[ctx];
  ++tot;
}

u64 Table64::query (u32 ctx) const {
  return tbl[ctx];
}

#ifdef DEBUG
u64 Table64::getTotal () const {
  return tot;
}

u64 Table64::countMty () const {
  return static_cast<u64>(std::count(tbl.begin(), tbl.end(), 0));
}

u64 Table64::maxTblVal () const {
  return *std::max_element(tbl.begin(), tbl.end());
}
#endif

void Table64::dump (ofstream& ofs) const {
  ofs.write((const char*) &tbl[0], tbl.size());
//  ofs.close();
}

void Table64::load (ifstream& ifs) const {
  ifs.read((char*) &tbl[0], tbl.size());
}

#ifdef DEBUG
void Table64::print () const {
  constexpr u8 context_width {12};
  cerr.width(context_width);  cerr<<std::left<<"Context";
  cerr << "Count\n";
  cerr << "-------------------\n";
  u32 i {0};
  for (auto c : tbl) {
    cerr.width(context_width);  cerr<<std::left<<i++;
    cerr << c << '\n';
  }
}
#endif