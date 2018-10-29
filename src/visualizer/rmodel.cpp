#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "defs.h"
#include "mem.h"
#include "common.h"
#include "rmodel.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CALCULATION OF CONTEXT MULTIPLICATOR FOR INDEX FUNCTION USAGE
//
u64 CalcMult(u32 c){
  u32 n;
  u64 x[c], p = 1;
  for(n = 0 ; n < c ; ++n){
    x[n] = p;
    p <<= 2;
    }
  return x[c-1];
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATES THE RCLASS BASIC STRUCTURE 
//
RCLASS *CreateRClass(u32 max, u32 min, u32 k, u8 ir){
  u32 n;

  RCLASS *C   = (RCLASS *)  Calloc(1,   sizeof(RCLASS));
  C->RM       = (RMODEL *)  Calloc(max, sizeof(RMODEL));
  C->active   = (u8 *) Calloc(max, sizeof(u8));
  C->nRM      = 0;
  C->mRM      = max;
  C->rev      = ir;
  C->idx      = 0;
  C->idxRev   = 0;
  C->kmer     = k;
  C->nBases   = 0;
  C->mult     = CalcMult(k);
  C->minSize  = min;
  for(n = 0 ; n < max ; ++n){
    C->RM[n].pos     = 0;
    C->RM[n].init    = 0;
    }

  return C;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REMOVE PERMANENTLY RCLASS 
//
void RemoveRClass(RCLASS *C){
  Free(C->RM);
  Free(C->active);
  Free(C);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REVERSE COMPLEMENT INDEX BASED ON PAST SYMBOLS FOR REPEATS
//
u64 GetIdxRevRM(u8 *p, RCLASS *C){
  return (C->idxRev = (C->idxRev>>2)+GetCompNum(*p)*C->mult);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// INDEX CALC BASED ON PAST SYMBOLS FOR REPEATS
//
u64 GetIdxRM(u8 *p, RCLASS *C){
  return (C->idx = ((C->idx-*(p-C->kmer)*C->mult)<<2)+*p);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET NON ACTIVE RMODEL ID
//
static int32_t GetFirstNonActiveRM(RCLASS *C){
  int32_t k;

  for(k = 0 ; k < C->mRM ; ++k)
    if(C->active[k] == 0)
      return k;

  fprintf(stderr, "Impossible state!\n");
  exit(1);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// START EACH REPEAT MODEL
//
void StartRMs(RCLASS *C, HASH *H, u64 relative, u64 absolute, u64
idx, u8 ir){
  u32 n = 0, k = 0;
  ENTRY *E;

  if((E = GetHEnt(H, idx)) == NULL)
    return; // NEVER SEEN IN THE HASH TABLE, SO LETS QUIT

  while(C->nRM < C->mRM && n < E->nPos){
    k = GetFirstNonActiveRM(C);

    if(E->pos[n] >= C->nBases - C->kmer || E->pos[n] < C->kmer){
      ++n;
      continue;
      }

    if(ir == 0)
      C->RM[k].init = C->RM[k].pos = E->pos[n];
    else
      C->RM[k].init = C->RM[k].pos = E->pos[n]-C->kmer-1;

    // RESET TO DEFAULTS
    C->RM[k].stop    = 0;
    C->RM[k].write   = 0;
    C->RM[k].rev     = ir;
    C->RM[k].initRel = relative;
    C->RM[k].initAbs = absolute;

    C->active[k] = 1;  // SET IT ACTIVE
    C->nRM++;          // INCREASE NUMBER OF REPEATS
    ++n;
    }

  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET INDEX POSITION
//
static u64 GetIPoint(HEADERS *Head, u64 init){
  u64 id;
  for(id = 0 ; id < Head->iPos ; ++id)
    if(Head->Pos[id].init <= init && Head->Pos[id].end >= init)
      return id;
  return id;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// PROTECT NAMES
//
void ProtectVoidName(u8 *name, u8 type){
  if(name[0] == '\0'){
    name[0] = type == 0 ? 't' : 'r';
    name[1] = type == 0 ? 'a' : 'e';
    name[2] = type == 0 ? 'r' : 'f';
    name[3] = '\0';
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE REPEAT MODEL
//
void UpdateRMs(RCLASS *C, u8 *b, u64 relative, u8 sym){
  u32 n;

  for(n = 0 ; n < C->mRM ; ++n){
    if(C->active[n] == 1){

      C->RM[n].size = labs(relative - C->RM[n].initRel) + C->kmer;

      // PROTECT EXTRA SYMBOLS
      if(b[C->RM[n].pos] == 4){
        C->RM[n].stop = 1; // SEE AFTER: DISCARDING POLITICS
        continue;
        }
      
      if(C->RM[n].rev == 0){ // REGULAR REPEAT
        if(b[C->RM[n].pos] != sym){
          C->RM[n].stop = 1;
          continue;
          }
        // STOP IF POS <= KMER
        if(C->RM[n].pos < C->nBases-1) C->RM[n].pos++;
        else                           C->RM[n].stop = 1;
        }
      else{ // INVERTED REPEAT
        if(GetCompNum(b[C->RM[n].pos]) != sym){
          C->RM[n].stop = 1;
          continue;
          }
        // STOP IF POS <= KMER
        if(C->RM[n].pos > 0) C->RM[n].pos--;
        else                 C->RM[n].stop = 1;
        }
      }
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// PRINT BLOCK
//
void PrintBlock(RCLASS *C, HEADERS *Head, u64 relative, u64 absolute,
u32 n, u8 *cName, FILE *W){
  u64 idxPos = 0;

  if(C->RM[n].rev == 0){ // REGULAR REPEAT
    idxPos = GetIPoint(Head, C->RM[n].init-C->kmer);
    ProtectVoidName(cName, 0);
    ProtectVoidName(Head->Pos[idxPos].name, 1);

    fprintf(W, "%s\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\t"
               "%s\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\n",

    cName,                                               // CONTIG NAME
    C->RM[n].initRel - C->kmer + 2,                      // CONTIG RELAT INIT
    relative + 1,                                        // CONTIG RELAT END
    C->RM[n].initAbs - C->kmer + 2,                      // CONTIG ABSOL INIT
    absolute + 1,                                        // CONTIG ABSOL END
    Head->Pos[idxPos].name,                              // REF NAME
    C->RM[n].init - C->kmer - Head->Pos[idxPos].init,    // REF RELAT INIT
    C->RM[n].pos  - Head->Pos[idxPos].init,              // REF RELAT END
    C->RM[n].init - C->kmer,                             // REF ABSOL INIT
    C->RM[n].pos);                                       // REF ABSOL END
    }

  else{ // REVERSE REPEAT
    idxPos = GetIPoint(Head, C->RM[n].pos);
    ProtectVoidName(cName, 0);
    ProtectVoidName(Head->Pos[idxPos].name, 1);
 
    fprintf(W, "%s\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\t"
               "%s\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\t%"PRIi64"\n",

    cName,                                               // CONTIG NAME
    C->RM[n].initRel - C->kmer + 1,                      // CONTIG RELAT INIT
    relative,                                            // CONTIG RELAT END
    C->RM[n].initAbs - C->kmer + 1,                      // CONTIG ABSOL INIT
    absolute,                                            // CONTIG ABSOL END
    Head->Pos[idxPos].name,                              // REF NAME
    C->RM[n].init + C->kmer - Head->Pos[idxPos].init,    // REF RELAT INIT
    C->RM[n].pos - Head->Pos[idxPos].init,               // REF RELAT END
    C->RM[n].init + C->kmer,                             // REF ABSOL INIT
    C->RM[n].pos);                                       // REF ABSOL END
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// STOP USELESS REPEAT MODELS
//
static void ResetRM(RCLASS *C, u32 id){
  C->RM[id].write = 0;
  C->RM[id].stop  = 0;
  C->active[id]   = 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// STOP USELESS REPEAT MODELS
//
void StopRMs(RCLASS *C, HEADERS *Head, u64 position, u64 absolute,
u8 *buf, FILE *Writter){
  int32_t id, largerRM = -1, largerRMIR = -1;
  u64 size = 0, sizeIR = 0;

  if(C->nRM > 0){
    for(id = 0 ; id < C->mRM ; ++id){
      if(C->active[id] == 1 && C->RM[id].stop == 1){
        if(C->RM[id].size >= C->minSize){
          if(C->RM[id].rev == 0 && size < C->RM[id].size){
            size = C->RM[id].size;
            largerRM = id;
            }
          if(C->RM[id].rev == 1 && sizeIR < C->RM[id].size){
            sizeIR = C->RM[id].size;
            largerRMIR = id;
            }
          C->RM[id].write = 2;
          continue;
          }

        ResetRM(C, id);
        --C->nRM;
        }
      }

    if(largerRM != -1)
      PrintBlock(C, Head, position, absolute, largerRM, buf, Writter);

    if(C->rev == 1 && largerRMIR != -1)
      PrintBlock(C, Head, position, absolute, largerRMIR, buf, Writter);

    for(id = 0 ; id < C->mRM ; ++id){
      if(C->RM[id].write == 2){ // IT WAS SMALLER
        if(C->RM[id].rev == 0){ // REGULAR
          if(C->RM[largerRM].init > C->RM[id].init && 
             C->RM[largerRM].pos  < C->RM[id].pos){
            PrintBlock(C, Head, position, absolute, id, buf, Writter);
            }
          }
        else{ // REVERSE
          if(C->RM[largerRMIR].init < C->RM[id].init && 
             C->RM[largerRMIR].pos  > C->RM[id].pos){
            PrintBlock(C, Head, position, absolute, id, buf, Writter);
            }
          }
        ResetRM(C, id);
        --C->nRM;
        }
      }
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// FORCE STOP REPEAT MODELS DURING END OF READ OR 'N'
//
void ResetAllRMs(RCLASS *C, HEADERS *Head, u64 relative, u64
absolute, u8 *cName, FILE *Writter){
  int32_t id, largerRM = -1, largerRMIR = -1;
  u64 size = 0, sizeIR = 0;

  for(id = 0 ; id < C->mRM ; ++id){
    if(C->active[id] == 1){
      if(C->RM[id].size >= C->minSize){
        if(C->RM[id].rev == 0 && size < C->RM[id].size){
          size = C->RM[id].size;
          largerRM = id;
          }
        if(C->RM[id].rev == 1 && sizeIR < C->RM[id].size){
          sizeIR = C->RM[id].size;
          largerRMIR = id;
          }
        }
      ResetRM(C, id);
      --C->nRM;
      }
    }
    
  if(largerRM != -1)
    PrintBlock(C, Head, relative, absolute, largerRM, cName, Writter);

  if(largerRMIR != -1)
    PrintBlock(C, Head, relative, absolute, largerRMIR, cName, Writter);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// START NEW REPEAT MODELS IF THERE IS STILL SPACE
//                         
void StartMultipleRMs(RCLASS *C, HASH *H, u64 relative, u64 absolute){
  if(C->nRM < C->mRM)
    StartRMs(C, H, relative, absolute, C->idx, 0);
  if(C->rev == 1 && C->nRM < C->mRM) 
    StartRMs(C, H, relative, absolute, C->idxRev, 1);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


