#ifndef POS_H_INCLUDED
#define POS_H_INCLUDED

#include "defs.h"

namespace smashpp {
struct POS {
  u64 init;
  u64 end;
  u8  name[MAX_CONTIG_NAME];
};

struct HEADERS {
  POS* Pos;
  u64  iPos;
};

HEADERS* CreateHeaders ();
void     UpdateHeaders (HEADERS*);
void     RemoveHeaders (HEADERS*);
}

#endif