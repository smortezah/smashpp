#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include "defs.h"

#define DEF_BUF_GUARD 32
#define DEF_BUF_SIZE  65535

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  uint8_t  *buf;
  int32_t  idx;
  uint32_t size;
  uint32_t guard;
  }
CBUF;

typedef struct{
  uint8_t  *buf;
  int32_t  idx;
  uint32_t size;
  }
BUF;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CBUF     *CreateCBuffer (uint32_t, uint32_t);
BUF      *CreateBuffer  (uint32_t);
void     UpdateCBuffer  (CBUF *);
void     UpdateBuffer   (BUF *);
void     RemoveCBuffer  (CBUF *);
void     RemoveBuffer   (BUF *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
