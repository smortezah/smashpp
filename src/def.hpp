// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2020, IEETA, University of Aveiro, Portugal.

//#define DEBUG 1

#ifndef SMASHPP_DEF_HPP
#define SMASHPP_DEF_HPP

#include <iostream>
#include <unordered_map>
#include <chrono>
#include <iomanip>  // setw, setprecision
#include <vector>
// #include <stdint.h>
// #define __STDC_FORMAT_MACROS

namespace smashpp {
// Version
static const std::string VERSION{"21.12"};
static const std::string DEV_YEARS{"2018-2021"};

// Typedef
using dur_t = std::chrono::duration<double>;
using prc_t = double;  // Precision type -- MUST be double

// Constant
static constexpr uint8_t CARDIN{4};  // CARDINALITY = Alphabet size
static constexpr double DBLANK{-2.0};

// #ifdef __unix__
// // const auto lacale = "en_US.UTF8";
const auto lacale = "en_US.UTF-8";
// #elif _WIN32
// const auto lacale = "en-US";
// const auto lacale = "en_US";
// #endif

// Enum
enum class Container {  // Data structure
  table_64,
  table_32,
  log_table_8,
  sketch_8
};
enum class FilterType {  // Types of windowing function
  rectangular,
  hamming,
  hann,
  blackman,
  triangular,
  welch,
  sine,
  nuttall
};
enum class Format { profile, filter, position, midposition, segment, self };
enum class FileType { seq, fasta, fastq };
enum class FilterScale { s, m, l };
enum class Problem { warning, error };
enum class Align { left, right, internal };
enum class Interval { open, closed, open_closed, closed_open };

const std::vector<FilterType> SET_WTYPE{
    FilterType::rectangular, FilterType::hamming,    FilterType::hann,
    FilterType::blackman,    FilterType::triangular, FilterType::welch,
    FilterType::sine,        FilterType::nuttall};
const std::vector<FilterScale> SET_FSCALE{FilterScale::s, FilterScale::m,
                                          FilterScale::l};

struct PosRow {
  uint64_t beg_pos;
  uint64_t end_pos;
  prc_t ent;
  prc_t self_ent;
  uint8_t run_num;  // 0=reg, 1=ir
  std::string ref;
  std::string tar;
  uint64_t seg_num;
  uint8_t round;

  PosRow(uint64_t beg_pos_ = 0, uint64_t end_pos_ = 0, prc_t ent_ = 0.0,
         prc_t self_ent_ = 0.0, uint8_t run_num_ = 0, std::string ref_ = "",
         std::string tar_ = "", uint64_t seg_num_ = 0, uint8_t round_ = 1)
      : beg_pos(beg_pos_),
        end_pos(end_pos_),
        ent(ent_),
        self_ent(self_ent_),
        run_num(run_num_),
        ref(ref_),
        tar(tar_),
        seg_num(seg_num_),
        round(round_) {}

  PosRow(const PosRow& row)
      : beg_pos(row.beg_pos),
        end_pos(row.end_pos),
        ent(row.ent),
        self_ent(row.self_ent),
        run_num(row.run_num),
        ref(row.ref),
        tar(row.tar),
        seg_num(row.seg_num),
        round(row.round) {}

  // Test
  void show() const {
    std::cerr << int(round) << ' ' << int(run_num) << ' ' << seg_num << ' '
              << ref << ' ' << tar << ' ' << beg_pos << ' ' << end_pos << ' '
              << ent << ' ' << self_ent;
  }

  void print() const {
    std::cerr << beg_pos << '\t' << end_pos << '\t' << ent << '\t' << self_ent;
  }
};

// Function
static auto base_code(char c) -> uint8_t {
  switch (c) {
    case 'C':
    case 'c':
      return 1;
      break;
    case 'G':
    case 'g':
      return 2;
      break;
    case 'T':
    case 't':
      return 3;
      break;
  }
  return 0;
}

// Metaprogram
// Power a^n
constexpr uint64_t sqr(uint64_t a) { return a * a; }
constexpr uint64_t power(uint64_t a, uint64_t n) {
  return n == 0 ? 1 : sqr(power(a, n >> 1u)) * ((n & 1ull) ? a : 1);
}

constexpr uint64_t pow2(uint64_t n) noexcept {
  return 1ull << n;  // ull is MANDATORY
}

// Lookup table
static constexpr uint64_t POW2minus1[44]{
  0x0000000000, 0x00000000001, 0x00000000003, 0x00000000007,
  0x000000000F, 0x0000000001F, 0x0000000003F, 0x0000000007F,
  0x00000000FF, 0x000000001FF, 0x000000003FF, 0x000000007FF,
  0x0000000FFF, 0x00000001FFF, 0x00000003FFF, 0x00000007FFF,
  0x000000FFFF, 0x0000001FFFF, 0x0000003FFFF, 0x0000007FFFF,
  0x00000FFFFF, 0x000001FFFFF, 0x000003FFFFF, 0x000007FFFFF,
  0x0000FFFFFF, 0x00001FFFFFF, 0x00003FFFFFF, 0x00007FFFFFF,
  0x000FFFFFFF, 0x0001FFFFFFF, 0x0003FFFFFFF, 0x0007FFFFFFF,
  0x00FFFFFFFF, 0x001FFFFFFFF, 0x003FFFFFFFF, 0x007FFFFFFFF,
  0x0FFFFFFFFF, 0x01FFFFFFFFF, 0x03FFFFFFFFF, 0x07FFFFFFFFF,
  0xFFFFFFFFFF, 0x1FFFFFFFFFF, 0x3FFFFFFFFFF, 0x7FFFFFFFFFF
};
static constexpr uint16_t FREQ2[16] { // 2^n - 1
    0,    1,     3,     7,   15,   31,    63,   127,
  255,  511,  1023,  2047, 4095, 8191, 16383, 32767
};
static constexpr uint64_t POW10[13] {
              1,       10,       100,       1000,       10000,       100000,//#6
        1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000, 
  1000000000000
};

// k,[w,d,]ir,alpha,gamma/thr,ir,alpha,gamma
static const std::vector<std::string> LEVEL{
    "14,0,0.005,0.95",                                            // Level 0
    "20,0,0.002,0.95",                                            // Level 1
    "14,0,0.005,0.95:6,0,0.1,0.95",                               // Level 2
    "14,0,0.001,0.95/5,0,0.001,0.95",                             // Level 3
    "18,0,0.002,0.95/3,0,0.02,0.95:13,0,0.01,0.95:6,0,0.1,0.95",  // Level 4
    "16,0,0.002,0.95/8,0,0.1,0.95:14,0,0.005,0.95/5,0,1,0.95:"
    "8,0,0.1,0.95:4,0,1,0.95",  // Level 5
    "20,0,0.002,0.95/5,0,0.05,0.95:14,0,0.005,0.95/3,0,1,0.95:"
    "8,0,0.1,0.95:4,0,1,0.95"  // Level 6
};
static const std::vector<std::string> REFFREE_LEVEL{LEVEL};

static const std::vector<std::vector<std::string>> COLORSET{
    // #0
    {"#F8766D", "#D89000", "#A3A500", "#39B600", "#00BF7D", "#00BFC4",
     "#00B0F6", "#9590FF"},
    // {"#F8766D", "#D89000", "#A3A500", "#39B600", "#00BF7D", "#00BFC4",
    //  "#00B0F6", "#9590FF", "#E76BF3", "#FF62BC"},
    // {"#0000ff", "#00aaff", "#00ffff", "#00ffaa", "#00ff55", "#55ff00",
    //  "#aaff00", "#ffff00", "#ffaa00", "#ff0000"},
    // {"#0000ff", "#0055ff", "#00aaff", "#00ffff", "#00ffaa", "#00ff55",
    //  "#00ff00", "#55ff00", "#aaff00", "#ffff00", "#ffaa00", "#ff5500",
    //  "#ff0000"},
    // #1
    {"#90ee90", "#70de94", "#61d59b", "#53cda4", "#45c5b0", "#2d9eb4",
     "#237eac", "#195ea4", "#103e9b", "#00008b"}
    // {"#90ee90", "#7fe690", "#70de94", "#61d59b", "#53cda4", "#45c5b0",
    //  "#39bdbd", "#2d9eb4", "#237eac", "#195ea4", "#103e9b", "#071f93",
    //  "#00008b"}
};
}  // namespace smashpp

#endif  //SMASHPP_DEF_HPP