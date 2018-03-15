//
// Created by morteza on 13-03-2018.
//

#include <algorithm>
#include <fstream>
#include "tbl32.hpp"
using std::cerr;

Table32::Table32 (u8 k_) {
  config(k_);
}

void Table32::config (u8 k_) {
  k       = k_;
  nRenorm = 0;
  tot     = 0;
  try { tbl.resize(4<<(k<<1)); }    // 4<<2k = 4*2^2k = 4*4^k = 4^(k+1)
  catch (std::bad_alloc& b) {
    cerr << "Error: failed memory allocation.";
    throw EXIT_FAILURE;
  }
}

void Table32::update (u32 ctx) {
  if (tbl[ctx] == 0xFFFFFFFF) {    // 2^32-1
    renormalize();
    ++nRenorm;
  }
  ++tbl[ctx];
  ++tot;
}

inline void Table32::renormalize () {
  for (auto c : tbl)
    c >>=1;
}

u32 Table32::query (u32 ctx) const {
  return tbl[ctx];
}

u64 Table32::getTotal () const {
  return tot;
}

u64 Table32::countMty () const {
  return static_cast<u64>(std::count(tbl.begin(), tbl.end(), 0));
}

u32 Table32::maxTblVal () const {
  return *std::max_element(tbl.begin(), tbl.end());
}

void Table32::dump (ofstream& ofs) const {
  ofs.write((const char*) &tbl[0], tbl.size());
//  ofs.close();
}

void Table32::load (ifstream& ifs) const {
  ifs.read((char*) &tbl[0], tbl.size());
}

void Table32::printTbl () const {
  for (auto c : tbl)
    cerr << c << '\n';
}