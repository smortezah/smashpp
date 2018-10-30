#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "defs.h"
#include "param.h"

namespace smashpp {
u32    args_num       (u32, char*[], int, string, u32, u32);
bool   args_state     (bool, char*[], int, string);
string args_files_img (char*[], int, string);
}

#endif