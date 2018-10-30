#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "defs.h"
#include "param.h"

namespace smashpp {
u32    ArgsNum      (u32 , char*[], int, string, u32, u32);
bool   ArgsState    (bool, char*[], int, string);
string ArgsFilesImg (char*[], int, string);
}

#endif