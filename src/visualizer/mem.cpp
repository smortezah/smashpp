#include "mem.h"
#include "defs.h"
#include <math.h>
#include <stdlib.h>
using namespace smashpp;

void* smashpp::Malloc (size_t size) {
  void* pointer = malloc(size);
  if (pointer == NULL) {
    fprintf(stderr, "Error allocating %zu bytes.\n", size);
    exit(1);
  }
  return pointer;
}

void* smashpp::Calloc (size_t nmemb, size_t size) {
  void* pointer = calloc(nmemb, size);
  if (pointer == NULL) {
    fprintf(stderr, "Error allocating %zu bytes.\n", size);
    exit(1);
  }
  return pointer;
}

void* smashpp::Realloc (void* ptr, size_t size) {
  void* pointer = realloc(ptr, size);
  if (pointer == NULL) {
    fprintf(stderr, "Error allocating %zu bytes.\n", size);
    exit(1);
  }
  return pointer;
}

void smashpp::Free (void* ptr) {
  free(ptr);
}

// HUMAN READABLE MEMORY PRINT
void smashpp::PrintHRBytes (i64 b) {
  i32 u=1024, e;
  double s;
  u8 t[] = {'K', 'M', 'G', 'T', 'E', 'Z', 'Y'};
  if (b<u)  cerr<<b<<" B";
  else {
    e = log(b) / log(u);
    s = (double) b / pow((double) u, (double) e);
    fprintf(stdout, "%.1lf %cB", s, t[e-1]);
  }
}