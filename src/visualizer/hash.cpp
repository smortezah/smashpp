#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "defs.h"
#include "mem.h"
#include "hash.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// SILVERS HASH
//
uint64_t XHASH(uint64_t x){
  return (x * 786433 + 196613) % 68719476735;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATES THE HASH STRUCTURE 
//
HASH *CreateHash(void){
  HASH *H = (HASH *) Calloc(1, sizeof(HASH));
  H->ent  = (ENTRY   **) Calloc(HSIZE, sizeof(ENTRY  *));
  H->size = (uint16_t *) Calloc(HSIZE, sizeof(uint16_t));
  return H;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET REPEAT MODEL HASH ENTRY
//
ENTRY *GetHEnt(HASH *H, uint64_t key){
  uint32_t n, h = key % HSIZE;
  #ifdef LOWP
  uint16_t b = key & 0xffff;
  #endif
  for(n = 0 ; n < H->size[h] ; ++n)
    #ifdef LOWP
    if(H->ent[h][n].key == b){
    #else
    if(H->ent[h][n].key == key){
    #endif
      return &H->ent[h][n];
      }

  return NULL; // NOT FOUND!
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// INSERT KMER POSITION INTO HASH TABLE 
//
void InsertKmerPos(HASH *H, uint64_t key, uint32_t pos){
  uint32_t n, h = key % HSIZE;
  #ifdef LOWP
  uint16_t b = key & 0xffff;
  #endif

  for(n = 0 ; n < H->size[h] ; ++n)
    #ifdef LOWP
    if(H->ent[h][n].key == b){
    #else
    if(H->ent[h][n].key == key){
    #endif
      H->ent[h][n].pos = (PPR *) Realloc(H->ent[h][n].pos, 
      (H->ent[h][n].nPos + 1) * sizeof(PPR));
      H->ent[h][n].pos[H->ent[h][n].nPos++] = pos;           
      return;
      }

  // CREATE A NEW ENTRY
  H->ent[h] = (ENTRY *) Realloc(H->ent[h], (H->size[h]+1) * sizeof(ENTRY));

  // CREATE A NEW POSITION
  H->ent[h][H->size[h]].pos = (PPR *) Malloc(sizeof(PPR));
  H->ent[h][H->size[h]].pos[0] = pos;
  H->ent[h][H->size[h]].nPos = 1;
  #ifdef LOWP
  H->ent[h][H->size[h]].key = b;
  #else
  H->ent[h][H->size[h]].key = key;
  #endif
  H->size[h]++;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

