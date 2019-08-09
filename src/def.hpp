// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

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
static const std::string SMASHPP{"SMASHPP"};
static const std::string VERSION{"19.07"};
static const std::string DEV_YEARS{"2018-2019"};

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
static constexpr uint64_t POW2[44] {
  0x00000000001, 0x00000000002, 0x00000000004, 0x00000000008,
  0x00000000010, 0x00000000020, 0x00000000040, 0x00000000080,
  0x00000000100, 0x00000000200, 0x00000000400, 0x00000000800,
  0x00000001000, 0x00000002000, 0x00000004000, 0x00000008000,
  0x00000010000, 0x00000020000, 0x00000040000, 0x00000080000,
  0x00000100000, 0x00000200000, 0x00000400000, 0x00000800000,
  0x00001000000, 0x00002000000, 0x00004000000, 0x00008000000,
  0x00010000000, 0x00020000000, 0x00040000000, 0x00080000000,
  0x00100000000, 0x00200000000, 0x00400000000, 0x00800000000,
  0x01000000000, 0x02000000000, 0x04000000000, 0x08000000000,
  0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000
};
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
// static constexpr uint64_t POW2[36] {
//            1,          2,           4,           8,
//           16,         32,          64,         128,
//          256,        512,        1024,        2048,
//         4096,       8192,       16384,       32768,
//        65536,     131072,      262144,      524288,
//      1048576,    2097152,     4194304,     8388608,
//     16777216,   33554432,    67108864,   134217728,
//    268435456,  536870912,  1073741824,  2147483648,
//   4294967296, 8589934592, 17179869184, 34359738368
// };
// static constexpr uint64_t POW2minus1[36] {
//            0,          1,           3,           7,
//           15,         31,          63,         127,
//          255,        511,        1023,        2047,
//         4095,       8191,       16383,       32767,
//        65535,     131071,      262143,      524287,
//      1048575,    2097151,     4194303,     8388607,
//     16777215,   33554431,    67108863,   134217727,
//    268435455,  536870911,  1073741823,  2147483647,
//   4294967295, 8589934591, 17179869183, 34359738367
// };
// static constexpr uint64_t POW4[32] {    // Max 4^31 contained in uint64_t
//                     1,                   4,                 16,
//                    64,                 256,               1024,
//                  4096,               16384,              65536,
//                262144,             1048576,            4194304,
//              16777216,            67108864,          268435456,
//            1073741824,          4294967296,        17179869184,
//           68719476736,        274877906944,      1099511627776,
//         4398046511104,      17592186044416,     70368744177664,
//       281474976710656,    1125899906842624,   4503599627370496,
//     18014398509481984,   72057594037927936, 288230376151711744,
//   1152921504606846976, 4611686018427387904
// };
static constexpr uint16_t FREQ2[16] { // 2^n - 1
    0,    1,     3,     7,   15,   31,    63,   127,
  255,  511,  1023,  2047, 4095, 8191, 16383, 32767
};
static constexpr uint32_t POW3[17] {
         1,     3,     9,     27,     81,     243,     729,     2187,      // #8
      6561, 19683, 59049, 177147, 531441, 1594323, 4782969, 14348907,
  43046721
};
static constexpr uint64_t POW5[23] {
               1,               5,               25,            125,
             625,            3125,            15625,          78125,
          390625,         1953125,          9765625,       48828125,
       244140625,      1220703125,       6103515625,    30517578125,
    152587890625,    762939453125,    3814697265625, 19073486328125,
  95367431640625, 476837158203125, 2384185791015625
};
static constexpr uint64_t POW10[13] {
              1,       10,       100,       1000,       10000,       100000,//#6
        1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000, 
  1000000000000
};
static constexpr uint8_t NUM[123] {    // a,A=0  c,C=1  g,G=2  t,T=3  n,N=0
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,             // #20
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
  0, 0, 0
};
//static constexpr uint8_t REVNUM[123] {    // a,A->3  c,C->2  g,G->1  t,T->0
//  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // #20
//  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//  0, 0, 0, 0, 0, 3, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
//  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2,
//  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//  0, 0, 0
//};
//static constexpr uint8_t REV[123] {//a,A->84(T)  c,C->71(G)  g,G->67(C)  t,T->65(A)
//  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,   // #20
//  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,
//  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,
//  0, 0, 0,  0,  0, 84, 0, 71, 0, 0, 0, 67, 0, 0, 0, 0,  0,  0, 0,  0,
//  0, 0, 0,  0, 65,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 84, 0, 71,
//  0, 0, 0, 67,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 65,  0, 0,  0,
//  0, 0, 0
//};

// k,[w,d,]ir,alpha,gamma/thr,ir,alpha,gamma
static const std::vector<std::string> LEVEL{
    "14,0,0.005,0.95",                                            // Level 0
    "20,0,0.002,0.95",                                            // Level 1
    "14,0,0.005,0.95:6,0,0.1,0.95",                               // Level 2
    "18,0,0.002,0.95/3,0,0.02,0.95:13,0,0.01,0.95:6,0,0.1,0.95",  // Level 3
    "20,0,0.002,0.95/5,0,0.05,0.95:14,0,0.005,0.95/3,0,1,0.95:"
    "8,0,0.1,0.95:4,0,1,0.95",  // Level 4
    "16,0,0.002,0.95/8,0,0.1,0.95:14,0,0.005,0.95/5,0,1,0.95:"
    "8,0,0.1,0.95:4,0,1,0.95"  // Level 5
};
static const std::vector<std::string> REFFREE_LEVEL{
    "14,0,0.05,0.95",                                        // Level 0
    "20,0,0.02,0.95",                                        // Level 1
    "14,0,0.05,0.95:6,0,1,0.95",                             // Level 2
    "18,0,0.05,0.95/3,0,0.1,0.95:13,0,0.5,0.95:6,0,1,0.95",  // Level 3
    "20,0,0.05,0.95/5,0,1,0.95:14,0,0.05,0.95/3,0,0.1,0.95:"
    "8,0,1,0.95:4,0,1,0.95",  // Level 4
    "20,0,0.05,0.95/10,0,1,0.95:14,0,0.05,0.95/5,0,1,0.95:"
    "8,0,1,0.92:4,0,1,0.9"  // Level 5
};

static const std::vector<std::vector<std::string>> COLORSET{
    // #0
    {"#F8766D", "#EA8331", "#D89000", "#C09B00", "#A3A500",
     "#7CAE00", "#39B600", "#00BB4E", "#00BF7D", "#00C1A3",
     "#00BFC4", "#00BAE0", "#00B0F6", "#35A2FF", "#9590FF",
     "#C77CFF"},
    ///  "#C77CFF", "#E76BF3", "#FA62DB", "#FF62BC", "#FF6A98"},
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