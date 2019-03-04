//#define DEBUG 1

#ifndef SMASHPP_DEF_HPP
#define SMASHPP_DEF_HPP

#include <iostream>
#include <unordered_map>
#include <chrono>
#include <iomanip>  // setw, setprecision
#include <vector>

namespace smashpp {
// Version
static const std::string SMASHPP{"SMASHPP"};
static const std::string VERSION{"19.03"};
static const std::string DEV_YEARS{"2018-2019"};

// Typedef
using dur_t = std::chrono::duration<double>;
using prc_t = double;  // Precision type -- MUST be double

// Enum
enum class Container {  // Data struct
  TABLE_64,
  TABLE_32,
  LOG_TABLE_8,
  SKETCH_8
};
enum class WType {  // Types of windowing function
  RECTANGULAR,
  HAMMING,
  HANN,
  BLACKMAN,
  TRIANGULAR,
  WELCH,
  SINE,
  NUTTALL
};
enum class Format { PROFILE, FILTER, POSITION, SEGMENT, SELF };
enum class FileType { SEQ, FASTA, FASTQ };
enum class FilterScale { S, M, L };
enum class Problem { WARNING, ERROR };

const std::vector<WType> SET_WTYPE{
    WType::RECTANGULAR, WType::HAMMING, WType::HANN, WType::BLACKMAN,
    WType::TRIANGULAR,  WType::WELCH,   WType::SINE, WType::NUTTALL};
const std::vector<FilterScale> SET_FSCALE{FilterScale::S, FilterScale::M,
                                          FilterScale::L};

// Metaprogram
// Power a^n
constexpr uint64_t sqr(uint64_t a) { return a * a; }
constexpr uint64_t power(uint64_t a, uint64_t n) {
  return n == 0 ? 1 : sqr(power(a, n >> 1u)) * ((n & 1ull) ? a : 1);
}

constexpr uint64_t pow2(uint64_t n) noexcept {
  return 1ull << n;  // ull is MANDATORY
}

// Constant
static constexpr uint8_t THRD{4};           // No. threads
static constexpr uint8_t MIN_THRD{1};
static constexpr uint8_t MAX_THRD{8};
static constexpr uint8_t LVL{0};            // Level
static constexpr uint8_t MIN_LVL{0};
static constexpr uint8_t MAX_LVL{5};
static constexpr uint32_t SSIZE{1};         // Min segment size
static constexpr uint32_t MIN_SSIZE{1};
static constexpr uint32_t MAX_SSIZE{0xffffffff};  // 2^32 - 1
static constexpr prc_t ENTR_N{2.0};         // Entropy of Ns
static constexpr prc_t MIN_ENTR_N{0.0};
static constexpr prc_t MAX_ENTR_N{100.0};
static constexpr char TAR_ALT_N{'T'};       // Alter. to Ns in target file
static constexpr uint8_t PART_GUARD{10};    // Expand beg & end pos -- filter
static constexpr uint32_t WS{256};          // Window size -- filter
static constexpr uint32_t MIN_WS{1};
static constexpr uint32_t MAX_WS{0xffffffff};  // 2^32 - 1
static constexpr auto WT{WType::HANN};      // Window type -- filter
static constexpr auto FS{FilterScale::L};   // Filt. scale -- filter
static constexpr float THRSH{1.5};          // Threshold   -- filter
static constexpr float MIN_THRSH{0};
static constexpr float MAX_THRSH{20};
static constexpr uint8_t CARDIN{4};         // CARDINALITY = Alphabet size
static constexpr uint8_t K_MAX_TBL64{11};   // Max ctx table 64     (128 MB mem)
static constexpr uint8_t K_MAX_TBL32{13};   // Max ctx table 32     (1   GB mem)
static constexpr uint8_t K_MAX_LGTBL8{14};  // Max ctx log table 8  (1   GB mem)
static constexpr uint32_t BLK_SZ{8192};     // 8K
static constexpr uint64_t W{2 << 29ull};    // Width of CML sketch
static constexpr uint8_t D{5};              // Depth of CML sketch
static constexpr uint32_t G{64};            // Machine word size-univers hash fn
static constexpr uint8_t LOG_BASE{2};       // Logarithmic counting
static constexpr uint8_t PREC_PRF{3};       // Precisions - floats in Inf. prof
static constexpr uint8_t PREC_FIL{3};       // Precisions - floats in filt. file
static const std::string FMT_PRF{"prf"};    // Format - inf. profile
static const std::string FMT_POS{"pos"};    // Format - positions file
static const std::string FMT_FIL{"fil"};    // Format - filtered files
static const std::string FMT_N{"n"};        // Format - position of N files
static const std::string LBL_SEG{"s"};      // Label  - segment files
static const std::string LBL_BAK{"_bk"};    // Label  - backup files
static const std::string LBL_MID{"mid"};    // Label  - mid position files
static const std::string POS_HDR{"#" + SMASHPP};  // Hdr of pos file
static const std::string POS_NRC{"N"};      // Hdr of pos file
static const std::string POS_REDUN{"R"};    // Hdr of pos file
static constexpr float PI{3.14159265f};
static constexpr int FILE_BUF{8 * 1024};    // 8K
static constexpr uint8_t TEXTWIDTH{65};
static constexpr double DBLANK{-2.0};
static const std::string TERM_SEP{
    ". . . . . . . . . . . . . . . . . . . "
    ". . . . . . . . . .\n"};

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

////template<uint32_t N>    // Up to 262144=2^18 elements
////struct LogInt      // 0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,...
////{
////  constexpr LogInt() : lg() {
////    for (uint32_t i=0; i!=LOG_BASE; ++i)
////      lg[i] = 0;
////    for (uint32_t i=LOG_BASE; i!=N; ++i)
////      lg[i] = static_cast<uint8_t>(1 + lg[i/LOG_BASE]);
////  }
////  uint8_t lg[N];
////};
//// Inside function definition
////constexpr auto a = LogInt<256>();
////cerr << (int) a.lg[3];
}  // namespace smashpp

#endif  // SMASHPP_DEF_HPP