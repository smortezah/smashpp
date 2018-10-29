#ifndef POS_H_INCLUDED
#define POS_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  u64 init;
  u64 end;
  u8  name[MAX_CONTIG_NAME];
  }
POS;

typedef struct{
  POS      *Pos;
  u64 iPos;
  }
HEADERS;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HEADERS   *CreateHeaders        (void);
void      UpdateHeaders         (HEADERS *);
void      RemoveHeaders         (HEADERS *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
