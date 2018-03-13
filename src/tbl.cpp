//
// Created by morteza on 13-03-2018.
//

#include "tbl.hpp"
using std::cerr;

Table::Table () : w(DEF_W), d(DEF_D) {
  tot = 0;
  try { tbl.resize((d*w+1)>>1); }
  catch (std::bad_alloc& b) {
    cerr << "Error: failed memory allocation.";
    throw EXIT_FAILURE;
  }
}