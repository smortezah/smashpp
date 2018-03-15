//
// Created by morteza on 13-03-2018.
//

#include <algorithm>
#include <fstream>
#include "tbl.hpp"
using std::cerr;

Table::Table (u8 k_) {
  config(k_);
}

void Table::config (u8 k_) {
  k       = k_;
  nRenorm = 0;
  tot     = 0;
  try { tbl.resize(4<<(k<<1)); }    // 4<<2k = 4*2^2k = 4*4^k = 4^(k+1)
  catch (std::bad_alloc& b) {
    cerr << "Error: failed memory allocation.";
    throw EXIT_FAILURE;
  }
}

void Table::update (u64 ctx) {
  if (tbl[ctx] == 0xFFFFFFFF) {    // 2^32-1
    renormalize();
    ++nRenorm;
  }
  ++tbl[ctx];
}

inline void Table::renormalize () {
  for (auto c : tbl)
    c >>=1;
}

u32 Table::query (u64 ctx) const {
  //todo. return ba tavajoh be renomrmalization
}

u64 Table::getTotal () const {
  return tot;
}

u64 Table::countMty () const {
  return static_cast<u64>(std::count(tbl.begin(), tbl.end(), 0));
}

u32 Table::maxTblVal () const {
  return *std::max_element(tbl.begin(), tbl.end());
}

void Table::dump (ofstream& ofs) const {
  ofs.write((const char*) &tbl[0], tbl.size());
//  ofs.close();
}

void Table::load (ifstream& ifs) const {
  ifs.read((char*) &tbl[0], tbl.size());
}

void Table::printTbl () const {
  for (auto c : tbl)
    cerr << c << '\n';
}