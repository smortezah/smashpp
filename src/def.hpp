//#define DEBUG 1

#ifndef SMASHPP_DEF_HPP
#define SMASHPP_DEF_HPP

#include <iostream>
#include <unordered_map>
#include <chrono>
#include <iomanip>      // setw, setprecision
#include <vector>
using namespace std;

namespace smashpp {
// Version
static const string SMASHPP   {"SMASHPP"};
static const string VERSION   {"18.12"};
static const string DEV_YEARS {"2018-2019"};

// Typedef
using i8    = signed char;   // === typedef signed char  i8;
using u8    = unsigned char;
using i16   = signed short;
using u16   = unsigned short;
using i32   = signed int;
using u32   = unsigned int;
using i64   = signed long long;
using u64   = unsigned long long;
using dur_t = std::chrono::duration<double>;
using prc_t = double;  // Precision type -- MUST be double

// Enum
enum class Container {TABLE_64, TABLE_32, LOG_TABLE_8, SKETCH_8};//Data struct
enum class WType {RECTANGULAR, HAMMING, HANN, BLACKMAN, TRIANGULAR, WELCH, SINE,
  NUTTALL};  // Types of windowing function
enum class Format {PROFILE, FILTER, POSITION, SEGMENT, SELF};
enum class FileType {SEQ, FASTA, FASTQ};
enum class FilterScale {S, M, L};
enum class Problem {WARNING, ERROR};

const vector<WType> SET_WTYPE {WType::RECTANGULAR, WType::HAMMING, WType::HANN,
  WType::BLACKMAN, WType::TRIANGULAR, WType::WELCH, WType::SINE,WType::NUTTALL};
const vector<FilterScale> SET_FSCALE {FilterScale::S, FilterScale::M,
  FilterScale::L};

// Metaprogram
// Power a^n
constexpr u64 sqr (u64 a) { return a * a; }
constexpr u64 power (u64 a, u64 n) {
  return n==0 ? 1 : sqr(power(a, n>>1u)) * ((n&1ull) ? a : 1);
}

constexpr u64 pow2 (u64 n) noexcept {
  return 1ull<<n;  // ull is MANDATORY
}

// Constant
static constexpr u8     THRD         {4};         // No. threads
static constexpr u8     MIN_THRD     {1};
static constexpr u8     MAX_THRD     {8};
static constexpr u8     LVL          {0};         // Level
static constexpr u8     MIN_LVL      {0};  
static constexpr u8     MAX_LVL      {4};  
static constexpr u32    SSIZE        {1};         // Min segment size
static constexpr u32    MIN_SSIZE    {1};
static constexpr u32    MAX_SSIZE    {0xffffffff};// 2^32 - 1
static constexpr prc_t  ENTR_N       {2.0};       // Entropy of Ns
static constexpr prc_t  MIN_ENTR_N   {0.0};
static constexpr prc_t  MAX_ENTR_N   {100.0};
static constexpr char   TAR_ALT_N    {'T'};       // Alter. to Ns in target file
static constexpr u32    WS           {256};             // Window size -- filter
static constexpr u32    MIN_WS       {1};
static constexpr u32    MAX_WS       {100000};
static constexpr auto   WT           {WType::HANN};     // Window type -- filter
static constexpr auto   FS           {FilterScale::L};  // Filt. scale -- filter
static constexpr float  THRSH        {1.5};             // Threshold   -- filter
static constexpr float  MIN_THRSH    {0};
static constexpr float  MAX_THRSH    {20};
static constexpr u8     CARDIN       {4};   // CARDINALITY = Alphabet size
static constexpr u8     K_MAX_TBL64  {11};  // Max ctx table 64     (128 MB mem)
static constexpr u8     K_MAX_TBL32  {13};  // Max ctx table 32     (1   GB mem)
static constexpr u8     K_MAX_LGTBL8 {14};  // Max ctx log table 8  (1   GB mem)
static constexpr u32    BLK_SZ       {8192};// 8K
static constexpr u64    W            {pow2(29ull)};       // Width of CML sketch
static constexpr u8     D            {5};                 // Depth of CML sketch
static constexpr u32    G            {64};  // Machine word size-univers hash fn
static constexpr u8     LOG_BASE     {2};   // Logarithmic counting
static constexpr u8     PRF_PREC     {3};   // Precisions - floats in Inf. prof
static constexpr u8     FIL_PREC     {3};   // Precisions - floats in filt. file
static const     string FMT_PRF      {"prf"};   // Format - inf. profile
static const     string FMT_POS      {"pos"};   // Format - positions file
static const     string FMT_FIL      {"fil"};   // Format - filtered files
static const     string FMT_N        {"n"};     // Format - position of N files
static const     string LBL_SEG      {"s"};     // Label  - segment files
static const     string LBL_BAK      {"_bk"};   // Label  - backup files
static const     string LBL_MID      {"mid"};   // Label  - mid position files
static const     string POS_HDR      {"#"+SMASHPP+VERSION};   // Hdr of pos file
static const     string POS_NRC      {"N"};     // Hdr of pos file
static const     string POS_REDUN    {"R"};     // Hdr of pos file
static constexpr auto   PI           {3.14159265f};
static constexpr int    FILE_BUF     {8*1024};  // 8K
static constexpr u8     TEXTWIDTH    {65};
static const     string TERM_SEP     {". . . . . . . . . . . . . . . . . . . . "
                                      ". . . . . . . . .\n"};

// Lookup table
static constexpr u64 POW2[36] {
           1,          2,           4,           8,
          16,         32,          64,         128,
         256,        512,        1024,        2048,
        4096,       8192,       16384,       32768,
       65536,     131072,      262144,      524288,
     1048576,    2097152,     4194304,     8388608,
    16777216,   33554432,    67108864,   134217728,
   268435456,  536870912,  1073741824,  2147483648,
  4294967296, 8589934592, 17179869184, 34359738368
};
static constexpr u64 POW2minus1[36] {
           0,          1,           3,           7,
          15,         31,          63,         127,
         255,        511,        1023,        2047,
        4095,       8191,       16383,       32767,
       65535,     131071,      262143,      524287,
     1048575,    2097151,     4194303,     8388607,
    16777215,   33554431,    67108863,   134217727,
   268435455,  536870911,  1073741823,  2147483647,
  4294967295, 8589934591, 17179869183, 34359738367
};
static constexpr u16 FREQ2[16] { // 2^n - 1
    0,    1,     3,     7,   15,   31,    63,   127,
  255,  511,  1023,  2047, 4095, 8191, 16383, 32767
};
static constexpr u32 POW3[17] {
         1,     3,     9,     27,     81,     243,     729,     2187,      // #8
      6561, 19683, 59049, 177147, 531441, 1594323, 4782969, 14348907,
  43046721
};
static constexpr u64 POW4[32] {    // Max 4^31 contained in u64
                    1,                   4,                 16,
                   64,                 256,               1024,
                 4096,               16384,              65536,
               262144,             1048576,            4194304,
             16777216,            67108864,          268435456,
           1073741824,          4294967296,        17179869184,
          68719476736,        274877906944,      1099511627776,
        4398046511104,      17592186044416,     70368744177664,
      281474976710656,    1125899906842624,   4503599627370496,
    18014398509481984,   72057594037927936, 288230376151711744,
  1152921504606846976, 4611686018427387904
};
static constexpr u64 POW5[23] {
               1,               5,               25,            125,
             625,            3125,            15625,          78125,
          390625,         1953125,          9765625,       48828125,
       244140625,      1220703125,       6103515625,    30517578125,
    152587890625,    762939453125,    3814697265625, 19073486328125,
  95367431640625, 476837158203125, 2384185791015625
};
static constexpr u8 NUM[123] {    // a,A=0  c,C=1  g,G=2  t,T=3  n,N=0
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,             // #20
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
  0, 0, 0
};
//static constexpr u8 REVNUM[123] {    // a,A->3  c,C->2  g,G->1  t,T->0
//  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // #20
//  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//  0, 0, 0, 0, 0, 3, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
//  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2,
//  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//  0, 0, 0
//};
//static constexpr u8 REV[123] {//a,A->84(T)  c,C->71(G)  g,G->67(C)  t,T->65(A)
//  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,   // #20
//  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,
//  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,
//  0, 0, 0,  0,  0, 84, 0, 71, 0, 0, 0, 67, 0, 0, 0, 0,  0,  0, 0,  0,
//  0, 0, 0,  0, 65,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 84, 0, 71,
//  0, 0, 0, 67,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 65,  0, 0,  0,
//  0, 0, 0
//};
static const vector<string> LEVEL { // k,[w,d,]ir,alpha,gamma/thr,ir,alpha,gamma
  "14,0,0.005,0.95",                                                  // Level 0
  "20,0,0.002,0.95",                                                  // Level 1
  "14,0,0.005,0.95:6,0,0.1,0.95",                                     // Level 2
  "18,0,0.002,0.95/3,0,0.02,0.95:13,0,0.01,0.95:6,0,0.1,0.95",        // Level 3
  "20,0,0.002,0.95/5,0,0.05,0.95:14,0,0.005,0.95/3,0,1,0.95:"
  "8,0,0.1,0.95:4,0,1,0.95"                                           // Level 4
};
static const vector<string> REFFREE_LEVEL {
  "14,0,0.05,0.95",                                                   // Level 0
  "20,0,0.02,0.95",                                                   // Level 1
  "14,0,0.05,0.95:6,0,1,0.95",                                        // Level 2
  "18,0,0.05,0.95/3,0,0.1,0.95:13,0,0.5,0.95:6,0,1,0.95",             // Level 3
  "20,0,0.05,0.95/5,0,1,0.95:14,0,0.05,0.95/3,0,0.1,0.95:"
  "8,0,1,0.95:4,0,1,0.95"                                             // Level 4
};

// Struct
struct SubSeq {
  string inName;
  string outName;
  u64    begPos;
  streamsize size;
};

struct Pos {
  i64   begRef, endRef;
  prc_t entRef, selfRef;
  i64   begTar, endTar;
  prc_t entTar, selfTar;
  u64   start;
  Pos (i64 br, i64 er, prc_t nr, prc_t sr, i64 bt, i64 et, prc_t nt, prc_t st,
    u64 s) : begRef(br), endRef(er), entRef(nr), selfRef(sr), begTar(bt), 
    endTar(et), entTar(nt), selfTar(st), start(s) {}
};

////template<u32 N>    // Up to 262144=2^18 elements
////struct LogInt      // 0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,...
////{
////  constexpr LogInt() : lg() {
////    for (u32 i=0; i!=LOG_BASE; ++i)
////      lg[i] = 0;
////    for (u32 i=LOG_BASE; i!=N; ++i)
////      lg[i] = static_cast<u8>(1 + lg[i/LOG_BASE]);
////  }
////  u8 lg[N];
////};
//// Inside function definition
////constexpr auto a = LogInt<256>();
////cerr << (int) a.lg[3];
}

#endif //SMASHPP_DEF_HPP