#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include "defs.h"

namespace smashpp {
#define DEF_BUF_GUARD 32
#define DEF_BUF_SIZE  65535

struct CBUF {
  u8* buf;
  i32 idx;
  u32 size;
  u32 guard;
};

struct BUF {
  u8* buf;
  i32 idx;
  u32 size;
};

CBUF* CreateCBuffer (u32, u32);
BUF*  CreateBuffer  (u32);
void  UpdateCBuffer (CBUF*);
void  UpdateBuffer  (BUF*);
void  RemoveCBuffer (CBUF*);
void  RemoveBuffer  (BUF*);
}

#endif