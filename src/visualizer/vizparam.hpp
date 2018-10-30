//
// Created by morteza on 30-10-2018.
//

#ifndef PROJECT_VIZPARAM_HPP
#define PROJECT_VIZPARAM_HPP

#include "../def.hpp"

namespace smashpp {
class VizParam {
 public:

 private:
};

struct Filep {
  char* name;
  u64  nReads;
  u64  nBases;
  u32  nSym;
  u64  length;
};

struct Parameters {
  bool  help;
  bool  verbose, force;
  u32   link;
  bool  inversion, regular;
  u8    level;
  u32   kmer;
  u32   minimum;
  u32   repeats;
  u32   threshold;
  u32   nThreads;
  Filep Ref, Tar;
  char* positions;
  string image;
};

struct Threads {
  u32 id;
  u32 tar, ref;
  u64 min;
};

//Parameters *P;
}

#endif //PROJECT_VIZPARAM_HPP