#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include "def.hpp"

namespace smashpp {
static constexpr u8   PREC_VIZ        {2};  // Precision
static const     auto IMAGE           {"map.svg"};
static constexpr u8   LINK            {1};
static constexpr u8   MIN_LINK        {1};
static constexpr u8   MAX_LINK        {6};
static constexpr u8   COLOR           {0};
static constexpr u8   MIN_COLOR       {0};
static constexpr u8   MAX_COLOR       {2};
static constexpr auto OPAC            {0.9f};
static constexpr auto MIN_OPAC        {0.0f};
static constexpr auto MAX_OPAC        {1.0f};
static constexpr u32  WDTH            {16};
static constexpr u32  MIN_WDTH        {15};
static constexpr u32  MAX_WDTH        {100};
static constexpr u32  SPC             {62};
static constexpr u32  MIN_SPC         {15};
static constexpr u32  MAX_SPC         {200};
static constexpr u32  MULT            {43};  // 5 colors without overlapping
static constexpr u32  MIN_MULT        {1};
static constexpr u32  MAX_MULT        {255};
static constexpr u32  BEGN            {0};
static constexpr u32  MIN_BEGN        {0};
static constexpr u32  MAX_BEGN        {255};
static constexpr u32  MINP            {1};
static constexpr u32  MIN_MINP        {1};
static constexpr u32  MAX_MINP        {0xffffffff};  // 2^32 - 1
static constexpr u32  PAINT_SCALE     {100};
static constexpr u32  PAINT_EXTRA     {75};
static constexpr auto PAINT_SHORT     {0.035};
static constexpr u8   PAINT_LVL_SATUR {160};
static constexpr u8   PAINT_LVL_VAL   {160};
static constexpr auto VERT_TUNE       {6.0f};
static constexpr auto VERT_RATIO      {3.0f};
static constexpr auto TITLE_SPACE     {16.0f};
static constexpr auto HEAT_START      {0.35};
static constexpr auto HEAT_ROT        {1.5};
static constexpr auto HEAT_HUE        {1.92};
static constexpr auto HEAT_GAMMA      {0.5};

static const vector<vector<string>> COLORSET {
  {"#0000ff", "#0055ff", "#00aaff", "#00ffff", "#00ffaa", "#00ff55", "#00ff00",
   "#55ff00", "#aaff00", "#ffff00", "#ffaa00", "#ff5500", "#ff0000"},
  // {"#2c7bb6", "#00a6ca", "#00ccbc", "#90eb9d", "#ffff8c", "#f9d057", "#f29e2e",
  //  "#e76818", "#d7191c"},
  {"#90ee90", "#7fe690", "#70de94", "#61d59b", "#53cda4", "#45c5b0", "#39bdbd",
   "#2d9eb4", "#237eac", "#195ea4", "#103e9b", "#071f93", "#00008b"},
  // {"#AAF191", "#80D385", "#61B385", "#3E9583", "#217681", "#285285", "#1F2D86", 
  //  "#000086"},
  {"#5E4FA2", "#41799C", "#62A08D", "#9CB598", "#C8CEAD", "#E6E6BA", "#E8D499",
   "#E2B07F", "#E67F5F", "#C55562", "#A53A66"}
};
}

#endif