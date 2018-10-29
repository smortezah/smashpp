#ifndef POS_H_INCLUDED
#define POS_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  uint64_t init;
  uint64_t end;
  uint8_t  name[MAX_CONTIG_NAME];
  }
POS;

typedef struct{
  POS      *Pos;
  uint64_t iPos;
  }
HEADERS;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HEADERS   *CreateHeaders        (void);
void      UpdateHeaders         (HEADERS *);
void      RemoveHeaders         (HEADERS *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
