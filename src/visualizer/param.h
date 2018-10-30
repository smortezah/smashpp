#ifndef PARAM_H_INCLUDED
#define PARAM_H_INCLUDED

#include "defs.h"
#include "rmodel.h"

namespace smashpp {
struct FILEP {
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
  FILEP Ref, Tar;
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

#endif