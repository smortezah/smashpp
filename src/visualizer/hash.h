#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include "defs.h"

namespace smashpp {
// HASH TABLE TO STORE REPEATING POSITIONS AND INDEXES ALONG THE SEQUENCE
// DO NOT CHANGE THESE MACRO VALUES UNLESS YOU REALLY KNOW WHAT YOU ARE DOING!
#define HSIZE        16777259   // NEXT PRIME AFTER 16777216 (24 BITS)
#define MAX_CTX      20         // ((HASH_SIZE (24 B) + KEY (16 B))>>1) = 20 
#define LOWP

typedef u32 PPR;  // PRECISION OF THE POSITION POINTER FOR REPEATS

struct ENTRY {
  #ifdef LOWP
  u16  key;      // THE KEY (INDEX / HASHSIZE) STORED IN THIS ENTRY
  #else
  u64  key;      // THE KEY STORED IN THIS ENTRY
  #endif
  u16  nPos;
  PPR* pos;     // LIST WITH THE REPEATING POSITIONS
};

struct HASH {
  u16*    size;    // NUMBER OF KEYS FOR EACH ENTRY
  ENTRY** ent;    // ENTRIES VECTORS POINTERS
};

u64    XHASH         (u64);
HASH*  CreateHash    ();
ENTRY* GetHEnt       (HASH*, u64);
void   InsertKmerPos (HASH*, u64, u32);
}

#endif