#ifndef PARAM_H_INCLUDED
#define PARAM_H_INCLUDED

#include "defs.h"
#include "rmodel.h"

namespace smashpp {
struct FILEP {
  char* name;
  U64  nReads;
  U64  nBases;
  U32  nSym;
  U64  length;
};

struct Parameters {
  U8    help;
  U8    verbose;
  U8    force;
  U8    inversion;
  U8    regular;
  U8    level;
  U8    link;
  U32   kmer;
  U32   minimum;
  U32   repeats;
  U32   threshold;
  U32   nThreads;
  FILEP Ref, Tar;
  char* positions;
  char* image;
};

struct Threads {
  u32 id;
  u32 tar, ref;
  u64 min;
};

//Parameters *P;
}

#endif