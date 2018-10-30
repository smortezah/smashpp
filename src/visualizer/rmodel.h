#ifndef RMODEL_H_INCLUDED
#define RMODEL_H_INCLUDED

#include "defs.h"
#include "hash.h"
#include "pos.h"

namespace smashpp {
// REPEAT MODELS TO HANDLE LONG SEGMENTS. DATA SUBSTITUTIONS DO NOT AFFECT THE
// PERFORMANCE SO MUCH AS IN CONTEXT MODELS.
struct RMODEL {
//  i64 initRel;   // FIRST POSITION OF THE REPEAT IN THE CONTIG
//  i64 initAbs;   // FIRST POSITION OF THE REPEAT IN THE ABSOLUTE FILE
  i64 size;      // CURRENT SIZE OF THE RMODEL IN THE CONTIG
//  i64 init;      // FIRST POSITION OF THE FIRST PREDICTED K-MER
//  i64 pos;       // CURRENT POSITION OF THE FIRST PREDICTED K-MER
//  u8  rev;       // INVERTED REPEAT MODEL. IF REV=1 THEN IS ON
//  u8  stop;      // STOP THIS MODEL = 1
//  u8  write;     // WRITE TO DISK THIS MODEL = 1
};

//struct RCLASS {
//  RMODEL* RM;       // POINTER FOR EACH OF THE MULTIPLE REPEAT MODELS
//  u8*     active;   // THE REPEAT MODEL IS ACTIVE OR NOT
//  u32     nRM;      // CURRENT NUMBER OF REPEAT MODELS
//  u32     mRM;      // MAXIMUM NUMBER OF REPEAT MODELS
//  u32     kmer;     // CONTEXT TEMPLATE SIZE FOR REPEAT MODEL
//  u32     minSize;  // MINIMUM BLOCK SIZE
//  u64     mult;     // INDEX MULTIPLIER
//  u64     idx;      // CURRENT CONTEXT INDEX
//  u64     idxRev;   // CURRENT INVERTED REPEAT INDEX
//  u8      rev;      // INVERTED REPETAT MODEL. IF REV='Y' THEN IS TRUE
//  u64     nBases;   // NUMBER OF ACGTN's
//};
//
//u64     CalcMult         (u32);
//RCLASS* CreateRClass     (u32, u32, u32, u8);
//void    RemoveRClass     (RCLASS*);
//u64     GetIdxRevRM      (u8*, RCLASS*);
//u64     GetIdxRM         (u8*, RCLASS*);
//void    PrintBlock       (RCLASS*, HEADERS*, u64, u64, u32, u8*, FILE*);
//void    StartRMs         (RCLASS*, HASH*, u64, u64, u64, u8);
//void    UpdateRMs        (RCLASS*, u8*, u64, u8);
//void    StopRMs          (RCLASS*, HEADERS*, u64, u64, u8*, FILE*);
//void    ResetAllRMs      (RCLASS*, HEADERS*, u64, u64, u8*, FILE*);
//void    StartMultipleRMs (RCLASS*, HASH*, u64, u64);
}

#endif