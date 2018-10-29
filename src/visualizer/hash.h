#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// HASH TABLE TO STORE REPEATING POSITIONS AND INDEXES ALONG THE SEQUENCE
// DO NOT CHANGE THESE MACRO VALUES UNLESS YOU REALLY KNOW WHAT YOU ARE DOING!
//
#define HSIZE        16777259   // NEXT PRIME AFTER 16777216 (24 BITS)
#define MAX_CTX      20         // ((HASH_SIZE (24 B) + KEY (16 B))>>1) = 20 
#define LOWP

typedef uint32_t PPR;  // PRECISION OF THE POSITION POINTER FOR REPEATS

typedef struct{
  #ifdef LOWP
  uint16_t key;      // THE KEY (INDEX / HASHSIZE) STORED IN THIS ENTRY
  #else
  uint64_t key;      // THE KEY STORED IN THIS ENTRY
  #endif
  uint16_t nPos;
  PPR      *pos;     // LIST WITH THE REPEATING POSITIONS
  }
ENTRY;

typedef struct{
  uint16_t *size;    // NUMBER OF KEYS FOR EACH ENTRY
  ENTRY    **ent;    // ENTRIES VECTORS POINTERS
  }
HASH;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t    XHASH             (uint64_t);
HASH        *CreateHash       (void);
ENTRY       *GetHEnt          (HASH   *, uint64_t);
void        InsertKmerPos     (HASH   *, uint64_t, uint32_t);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
