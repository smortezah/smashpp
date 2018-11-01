#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include "def.hpp"

namespace smashpp {
static const     string RELEASE_viz     {"1"};
static const     string VERSION_viz     {"1"};
static constexpr bool   DEF_HELP        {false};
static constexpr bool   DEF_VERSION     {false};
static constexpr bool   DEF_VERBOSE     {false};
static constexpr bool   DEF_FORCE       {false};
static constexpr bool   DEF_INVE        {true};
static constexpr bool   DEF_REGU        {true};
static const     string DEF_IMAGE       {"map.svg"};
static constexpr u32    DEF_LINK        {5};
static constexpr u32    MIN_LINK        {0};
static constexpr u32    MAX_LINK        {5};
static constexpr u32    DEF_WIDT        {40};
static constexpr u32    MIN_WIDT        {10};
static constexpr u32    MAX_WIDT        {5000};
static constexpr u32    DEF_SPAC        {160};
static constexpr u32    MIN_SPAC        {0};
static constexpr u32    MAX_SPAC        {5000};
static constexpr u32    DEF_MULT        {46};  // 5 colors without overlapping
static constexpr u32    MIN_MULT        {1};
static constexpr u32    MAX_MULT        {255};
static constexpr u32    DEF_BEGI        {0};
static constexpr u32    MIN_BEGI        {0};
static constexpr u32    MAX_BEGI        {255};
static constexpr u32    DEF_MINP        {0};
static constexpr u32    MIN_MINP        {1};
static constexpr u32    MAX_MINP        {99999999};
static constexpr double PAINT_CX        {70.0};
static constexpr double PAINT_CY        {75.0};
static constexpr double PAINT_TX        {50.0};
static constexpr double PAINT_TY        {82.0};
static const     string PAINT_BGCOLOR   {"#ffffff"};
static constexpr u32    PAINT_SCALE     {150};
static constexpr u32    PAINT_EXTRA     {150};
static const     string PAINT_REF       {"REF"};
static const     string PAINT_TAR       {"TAR"};
static constexpr u8     PAINT_LVL_SATUR {160};
static constexpr u8     PAINT_LVL_VAL   {160};
}

#endif