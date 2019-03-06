// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include "def.hpp"

namespace smashpp {
static constexpr uint8_t PREC_VIZ{2};  // Precision
static const std::string IMAGE{"map.svg"};
static constexpr uint8_t LINK{1};
static constexpr uint8_t MIN_LINK{1};
static constexpr uint8_t MAX_LINK{6};
static constexpr uint8_t COLOR{0};
static constexpr uint8_t MIN_COLOR{0};
static constexpr uint8_t MAX_COLOR{2};
static constexpr float OPAC{0.9f};
static constexpr float MIN_OPAC{0.0f};
static constexpr float MAX_OPAC{1.0f};
static constexpr uint32_t WDTH{16};
static constexpr uint32_t MIN_WDTH{15};
static constexpr uint32_t MAX_WDTH{100};
static constexpr uint32_t SPC{62};
static constexpr uint32_t MIN_SPC{15};
static constexpr uint32_t MAX_SPC{200};
static constexpr uint32_t MULT{43};  // 5 colors without overlapping
static constexpr uint32_t MIN_MULT{1};
static constexpr uint32_t MAX_MULT{255};
static constexpr uint32_t BEGN{0};
static constexpr uint32_t MIN_BEGN{0};
static constexpr uint32_t MAX_BEGN{255};
static constexpr uint32_t MINP{1};
static constexpr uint32_t MIN_MINP{1};
static constexpr uint32_t MAX_MINP{0xffffffff};  // 2^32 - 1
static constexpr uint64_t TICK{100};             // Major tick
static constexpr uint64_t MIN_TICK{1};
static constexpr uint64_t MAX_TICK{0xffffffff};
static constexpr uint32_t PAINT_SCALE{100};
static constexpr double PAINT_SHORT{0.035};
static constexpr uint8_t PAINT_LVL_SATUR{160};
static constexpr uint8_t PAINT_LVL_VAL{160};
static constexpr float SPACE_TUNE{6.0f};
static constexpr float TITLE_SPACE{16.0f};
static constexpr double HEAT_START{0.35};
static constexpr double HEAT_ROT{1.5};
static constexpr double HEAT_HUE{1.92};
static constexpr double HEAT_GAMMA{0.5};
static constexpr float VERT_MIDDLE{0.35f};
static constexpr float VERT_BOTTOM{0.7f};

static const std::vector<std::vector<std::string>> COLORSET{
    {"#0000ff", "#0055ff", "#00aaff", "#00ffff", "#00ffaa", "#00ff55",
     "#00ff00", "#55ff00", "#aaff00", "#ffff00", "#ffaa00", "#ff5500",
     "#ff0000"},
    // {"#2c7bb6", "#00a6ca", "#00ccbc", "#90eb9d", "#ffff8c", "#f9d057",
    // "#f29e2e",
    //  "#e76818", "#d7191c"},
    {"#90ee90", "#7fe690", "#70de94", "#61d59b", "#53cda4", "#45c5b0",
     "#39bdbd", "#2d9eb4", "#237eac", "#195ea4", "#103e9b", "#071f93",
     "#00008b"},
    // {"#AAF191", "#80D385", "#61B385", "#3E9583", "#217681", "#285285",
    // "#1F2D86",
    //  "#000086"},
    {"#5E4FA2", "#41799C", "#62A08D", "#9CB598", "#C8CEAD", "#E6E6BA",
     "#E8D499", "#E2B07F", "#E67F5F", "#C55562", "#A53A66"}};
}  // namespace smashpp

#endif