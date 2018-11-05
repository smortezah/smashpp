#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include "def.hpp"

namespace smashpp {
static const     string DEF_IMAGE       {"map.svg"};
static constexpr u32    DEF_LINK        {5};
static constexpr u32    MIN_LINK        {1};
static constexpr u32    MAX_LINK        {5};
static constexpr u32    DEF_WIDT        {30};
static constexpr u32    MIN_WIDT        {10};
static constexpr u32    MAX_WIDT        {200};
static constexpr u32    DEF_SPAC        {70};
static constexpr u32    MIN_SPAC        {0};
static constexpr u32    MAX_SPAC        {200};
static constexpr u32    DEF_MULT        {46};  // 5 colors without overlapping
static constexpr u32    MIN_MULT        {1};
static constexpr u32    MAX_MULT        {255};
static constexpr u32    DEF_BEGI        {0};
static constexpr u32    MIN_BEGI        {0};
static constexpr u32    MAX_BEGI        {255};
static constexpr u32    DEF_MINP        {0};
static constexpr u32    MIN_MINP        {1};
static constexpr u32    MAX_MINP        {99999999};
static constexpr double PAINT_CX        {140.0};
static constexpr double PAINT_CY        {100.0};
static const     string PAINT_BGCOLOR   {"white"};
static constexpr u32    PAINT_SCALE     {100};
static constexpr u32    PAINT_EXTRA     {110};
static constexpr double PAINT_SHORT     {0.015};
static constexpr u8     PAINT_LVL_SATUR {160};
static constexpr u8     PAINT_LVL_VAL   {160};
static constexpr double HORIZ_TUNE      {5};
static constexpr double HORIZ_RATIO     {3};
}

#endif