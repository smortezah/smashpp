#ifndef MEM_H_INCLUDED
#define MEM_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include "../def.hpp"

namespace smashpp {
void* Malloc       (size_t);
void* Calloc       (size_t, size_t);
void* Realloc      (void*, size_t);
void  Free         (void*);
u64   TotalMemory  ();
void  PrintHRBytes (i64);
}

#endif