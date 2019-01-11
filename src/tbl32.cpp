#include <algorithm>
#include <fstream>
#include "tbl32.hpp"
#include "exception.hpp"
using namespace smashpp;

Table32::Table32 (u8 k_) : k(k_), nRenorm(0), tot(0) {
  try { tbl.resize(4ull<<(k<<1u)); }    // 4<<2k = 4*2^2k = 4*4^k = 4^(k+1)
  catch (std::bad_alloc& b) { error("failed memory allocation."); }
}

void Table32::update (u32 ctx) {
  if (tbl[ctx] == 0xFFFFFFFF)    // 2^32-1
    renormalize();
  ++tbl[ctx];
  ++tot;
}

inline void Table32::renormalize () {
  for (auto c : tbl)
    c >>=1;
  ++nRenorm;
}

u32 Table32::query (u32 ctx) const {
  return tbl[ctx];
}

void Table32::dump (ofstream& ofs) const {
  ofs.write((const char*) &tbl[0], tbl.size());
//  ofs.close();
}

void Table32::load (ifstream& ifs) const {
  ifs.read((char*) &tbl[0], tbl.size());
}

#ifdef DEBUG
u64 Table32::get_total () const {
  return tot;
}

u64 Table32::count_empty () const {
  return static_cast<u64>(std::count(begin(tbl), end(tbl), 0));
}

u32 Table32::max_tbl_val () const {
  return *std::max_element(begin(tbl), end(tbl));
}

void Table32::print () const {
  constexpr u8 context_width {12};
  cerr.width(context_width);  cerr<<std::left<<"Context";
  cerr << "Count\n";
  cerr << "-------------------\n";
  u32 i {0};
  for (const auto& c : tbl) {
    cerr.width(context_width);  cerr<<std::left<<i++;
    cerr << c << '\n';
  }
}
#endif