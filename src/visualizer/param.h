#ifndef PARAM_H_INCLUDED
#define PARAM_H_INCLUDED

#include "defs.h"
#include "rmodel.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  char *name;
  U64  nReads;
  U64  nBases;
  U32  nSym;
  U64  length;
  }
FILEP;

typedef struct{
  U8       help;
  U8       verbose;
  U8       force;
  U8       inversion;
  U8       regular;
  U8       level;
  U8       link;
  U32      kmer;
  U32      minimum;
  U32      repeats;
  U32      threshold;
  U32      nThreads;
  FILEP    Ref;
  FILEP    Tar;
  char     *positions;
  char     *image;
  }
Parameters;

typedef struct{
  u32 id;
  u32 tar;
  u32 ref;
  u64 min;
  }
Threads;

//Parameters *P;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
