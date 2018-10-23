//
// Created by morteza on 02-02-2018.
//

//#define DEBUG 1 //todo


#ifndef SMASHPP_DEF_HPP
#define SMASHPP_DEF_HPP

#include <iostream>
#include <unordered_map>
#include <chrono>
#include <iomanip>      // setw, setprecision
#include <vector>
using namespace std;

// Version
static const string VERSION   {"18.10"};
static const string DEV_YEARS {"2018"};

// Typedef
////typedef signed char         i8;
using i8     = signed char;
using u8     = unsigned char;
using i16    = signed short;
using u16    = unsigned short;
using i32    = signed int;
using u32    = unsigned int;
using i64    = signed long long;
using u64    = unsigned long long;
using dur_t  = std::chrono::duration<double>;
using prec_t = double;  // Precision type -- MUST be double

// Metaprogram
// Power a^n
constexpr u64 sqr (u64 a) { return a * a; }
constexpr u64 power (u64 a, u64 n) {
  return n==0 ? 1 : sqr(power(a, n>>1u)) * (n&1ull ? a : 1);
}

constexpr u64 pow2 (u64 n) noexcept { // Not sure faster than above, but simpler
  return 1ull<<n;  // ull is MANDATORY
}
//constexpr u64 pow4 (u8 n) { // Not sure if faster than above, but simpler
//  return 1ull<<(n<<1);
//}

// Constant
static constexpr u8    DEF_THR      {4};  // Default # threads
static constexpr u8    DEF_LVL      {0};  // Default level
static constexpr u32   DEF_WS       {257};// Default window size -- filter
const  string          DEF_WT       {"0"};// Default window type -- filter
static constexpr float DEF_THRESH   {2};// Default threshold   -- filter
static constexpr u8    CARDIN       {4};  // CARDINALITY = Alphabet size
static constexpr u8    IRMAGIC      {3};  // Calc Mir syms based on syms
static constexpr u8    K_MAX_TBL64  {11}; // Max ctx dept. table 64 (128 MB mem)
static constexpr u8    K_MAX_TBL32  {13}; // Max ...       table 32 (1   GB mem)
static constexpr u8    K_MAX_LGTBL8 {14}; // Max ...   log table 8  (1   GB mem)
static constexpr u32   BLK_SZ       {8192}; // 8K
static constexpr u64   DEF_W        {pow2(29ull)};  // Default w of CML sketch
static constexpr u8    DEF_D        {5};  // Default depth of CML sketch
static constexpr u32   G            {64}; // Machine word size - univers hash fn
static constexpr u8    LOG_BASE     {2};  // Logarithmic counting
static constexpr u8    DEF_IR       {0};  // Default inverted repeats for models
static constexpr float DEF_ALPHA    {0.001};// Default alpha for models
static constexpr float DEF_GAMMA    {0.9};  // Default gamma for multiple models
const  string          PRF_FMT      {".prf"};  // Format for inf. profile file
static constexpr u8    DEF_PRF_PREC {3};  // Precisions for floats in Inf. prof
static constexpr u8    DEF_FIL_PREC {3};  // Precisions for floats in filt. file
const  string          POS_FMT      {".pos"};  // Format for positions file
const  string          FIL_FMT      {".fil"};  // Format for filtered files
const  string          SEG_LBL      {"_seg"};  // Label for segment files
static constexpr float PI           {3.14159265f};
//const  string          OUT_SEP      {"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"};
const  string          OUT_SEP      {"\n"};
//static constexpr int   FILE_BUF     {8*1024};  // 8K
static constexpr int   FILE_BUF     {3};
static constexpr u8    BIT_K        {6};
static constexpr u8    BIT_D        {3};
static constexpr u8    BIT_IR       {1};
static constexpr u8    BIT_THRESH   {4};
static constexpr u8    BIT_NSYM     {2};
static constexpr u8    BIT_NMDL     {4};
static constexpr u8    BIT_LEVEL    {5};
static constexpr u8    BIT_THREAD   {4};
static constexpr u8    BIT_WSIZE    {15};

// faster than a[]
//using htbl_t = std::unordered_map<u64, std::array<u64,CARDIN>>;

// Enum
enum class Container {TABLE_64, TABLE_32, LOG_TABLE_8, SKETCH_8}; //Data structs
//enum class Mode {MM, STMM};
enum class WType     {RECTANGULAR, HAMMING, HANN, BLACKMAN, TRIANGULAR, WELCH,
                      SINE, NUTTALL};  // Types of windowing function

// Macro
//#define LOOP16(n)  n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n
//#define SEQ16      0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
//#define LOOP(i,S)     for(const char& (i) : (S))
//#define LOOP2(i,j,S)  LOOP(i,S) LOOP(j,S)

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
static constexpr u8 NUM[123] {    // a,A=0  c,C=1  g,G=2  t,T=3
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
static constexpr u8 REV[123] { // a,A->84(T)  c,C->71(G)  g,G->67(C)  t,T->65(A)
  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,     // #20
  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,
  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,
  0, 0, 0,  0,  0, 84, 0, 71, 0, 0, 0, 67, 0, 0, 0, 0,  0,  0, 0,  0,
  0, 0, 0,  0, 65,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 84, 0, 71,
  0, 0, 0, 67,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 65,  0, 0,  0,
  0, 0, 0
};
static const vector<vector<u8>> LEVEL { // 'k' of multiple models MUST be sorted
//  static constexpr u8 LEVEL[14][16] { // 'k' of multiple models MUST be sorted
// #mdl, Mir,  k, 100*Malpha, log2 w,  d
  {1,     0, 20,    100,      30,    DEF_D},    // Level 0
//  {1,     0,  1,    100,      0,     0},
//  {1,     0,  2,    100,      0,     0},
//  {1,     0,  3,    100,      0,     0},
//  {1,     0,  4,    100,      0,     0},
//  {1,     0,  5,    100,      0,     0},
//  {1,     0,  6,    100,      0,     0},
//  {1,     0,  7,    100,      0,     0},
//  {1,     0,  8,    100,      0,     0},
//  {1,     0,  9,    100,      0,     0},
//  {1,     0, 10,    100,      0,     0},
//  {1,     0, 11,    100,      0,     0},
//  {1,     0, 12,    100,      0,     0},
//  {1,     0, 13,    100,      0,     0},
//  {1,     0, 14,    100,      0,     0}
//  {2,     0, 4,   100,       0,         0,
//          0, 12,   100,      10,    DEF_D},
//  {3,     0,  10,    100,       0,         0,
//          0, 13,    99,       0,         0,
//          0, 20,    99,      15,    DEF_D}
};

// Struct
struct SubSeq {
  string     inName;
  string     outName;
  u64        begPos;
  streamsize size;
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

#endif //SMASHPP_DEF_HPP