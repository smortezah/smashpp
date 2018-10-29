#ifndef MEM_H_INCLUDED
#define MEM_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

namespace smashpp {
void* Malloc       (size_t);
void* Calloc       (size_t, size_t);
void* Realloc      (void*, size_t);
void  Free         (void*);
u64   TotalMemory  (void);
void  PrintHRBytes (int64_t);
}

#endif