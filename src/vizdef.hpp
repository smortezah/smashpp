#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include "def.hpp"
#define PREC  std::fixed << std::setprecision(2)

namespace smashpp {
static const     string IMAGE           {"map.svg"};
static constexpr u8     LINK            {2};
static constexpr u8     MIN_LINK        {1};
static constexpr u8     MAX_LINK        {5};
static constexpr u8     COLOR           {0};
static constexpr u8     MIN_COLOR       {0};
static constexpr u8     MAX_COLOR       {2};
static constexpr float  OPAC            {0.75};
static constexpr float  MIN_OPAC        {0};
static constexpr float  MAX_OPAC        {1};
static constexpr u32    WDTH            {23};
static constexpr u32    MIN_WDTH        {10};
static constexpr u32    MAX_WDTH        {200};
static constexpr u32    SPC             {65};
static constexpr u32    MIN_SPC         {0};
static constexpr u32    MAX_SPC         {200};
static constexpr u32    MULT            {46};  // 5 colors without overlapping
static constexpr u32    MIN_MULT        {1};
static constexpr u32    MAX_MULT        {255};
static constexpr u32    BEGN            {0};
static constexpr u32    MIN_BEGN        {0};
static constexpr u32    MAX_BEGN        {255};
static constexpr u32    MINP            {1};
static constexpr u32    MIN_MINP        {1};
static constexpr u32    MAX_MINP        {99999999};
static constexpr double PAINT_CX        {180.0};
static constexpr double PAINT_CY        {100.0};
static const     string PAINT_BGCOLOR   {"white"};
static constexpr u32    PAINT_SCALE     {100};
static constexpr u32    PAINT_EXTRA     {110};
static constexpr double PAINT_SHORT     {0.035};
static constexpr u8     PAINT_LVL_SATUR {160};
static constexpr u8     PAINT_LVL_VAL   {160};
static constexpr double HORIZ_TUNE      {5};
static constexpr double HORIZ_RATIO     {3};
static constexpr double HEAT_START      {0.35};
static constexpr double HEAT_ROT        {1.5};
static constexpr double HEAT_HUE        {1.92};
static constexpr double HEAT_GAMMA      {0.5};
}

#endif