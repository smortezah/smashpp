#include "mem.h"
#include "defs.h"
#include <math.h>
#include <stdlib.h>
#include "../fn.hpp"
using namespace smashpp;

void* smashpp::Malloc (size_t size) {
  void* pointer = malloc(size);
  if (pointer==NULL)  error("failed allocating "+to_string(size)+" bytes.");
  return pointer;
}

void* smashpp::Calloc (size_t nmemb, size_t size) {
  void* pointer = calloc(nmemb, size);
  if (pointer==NULL)  error("failed allocating "+to_string(size)+" bytes.");
  return pointer;
}

void smashpp::Free (void* ptr) {
  free(ptr);
}