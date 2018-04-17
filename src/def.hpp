//
// Created by morteza on 02-02-2018.
//

#ifndef SMASHPP_DEF_HPP
#define SMASHPP_DEF_HPP

#include <iostream>
#include <unordered_map>
#include <chrono>
#include <iomanip>      // setw, setprecision

// Version
static const std::string VERSION   {"18.04"};
static const std::string DEV_YEARS {"2018"};

// Typedef
typedef signed char         i8;
typedef unsigned char       u8;
typedef signed short        i16;
typedef unsigned short      u16;
typedef signed int          i32;
typedef unsigned int        u32;
typedef signed long long    i64;
typedef unsigned long long  u64;
typedef std::chrono::duration<double>  dur_t;

// Metaprogram
// Power a^n
constexpr u64 sqr (u64 a) { return a * a; }
constexpr u64 power (u64 a, u64 n) {
  return n==0 ? 1 : sqr(power(a, n>>1)) * (n&1 ? a : 1);
}

// Constant
static constexpr u8  DEF_THR      {4};    // Default # threads
static constexpr u8  DEF_LVL      {0};    // Default level
static constexpr u8  ALPH_SZ      {4};    // Alphabet size
static constexpr u8  IRMAGIC      {3};    // Calc ir syms based on syms
static constexpr u8  K_MAX_TBL64  {11};   // Max ctx dept. table 64 (128 MB mem)
static constexpr u8  K_MAX_TBL32  {13};   // Max ...       table 32 (1   GB mem)
static constexpr u8  K_MAX_LGTBL8 {14};   // Max ...   log table 8  (1   GB mem)
static constexpr u32 BLK_SZ       {8192}; // 8K
static constexpr u64 DEF_W        {power(2,20)};  // Default w of CML sketch
static constexpr u8  DEF_D        {5};    // Default depth of CML sketch
static constexpr u32 G            {64};   // Machine word size - univers hash fn
static constexpr u8  LOG_BASE     {2};    // Logarithmic counting
static constexpr float DEF_GAMMA  {0.1};  // Default gamma for multiple models

typedef std::unordered_map<u64, std::array<u64,ALPH_SZ>>  htbl_t; //faster t a[]

// Enum
enum MODE {TABLE_64, TABLE_32, LOG_TABLE_8, SKETCH_8};  // Data structures
// Inv. repeats, 1 & 2 & 3 & 4 models. D: direct(bit 0), I: inv.(bit 1)
enum IR {DDDD, DDDI, DDID, DDII, DIDD, DIDI, DIID, DIII,
         IDDD, IDDI, IDID, IDII, IIDD, IIDI, IIID, IIII};

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
constexpr u64 pow2 (u64 n) { // Not sure if faster than above, but simpler
  return 1ull<<n;  // ull is MANDATORY
}
static constexpr u16 FREQ2[16] {
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
//constexpr u64 pow4 (u8 n) { // Not sure if faster than above, but simpler
//  return 1ull<<(n<<1);
//}
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
static constexpr u8 REVNUM[123] {    // a,A->3  c,C->2  g,G->1  t,T->0
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,             // #20
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0
};
static constexpr u8 REV[123] { // a,A->84(T)  c,C->71(G)  g,G->67(C)  t,T->65(A)
  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,     // #20
  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,
  0, 0, 0,  0,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0, 0,  0,
  0, 0, 0,  0,  0, 84, 0, 71, 0, 0, 0, 67, 0, 0, 0, 0,  0,  0, 0,  0,
  0, 0, 0,  0, 65,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 84, 0, 71,
  0, 0, 0, 67,  0,  0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 65,  0, 0,  0,
  0, 0, 0
};
static const std::vector<std::vector<u8>> LEVEL {    // 'k' of multiple models MUST be sorted
//  static constexpr u8 LEVEL[14][16] {    // 'k' of multiple models MUST be sorted
// #mdl, ir,  k, 100*alpha, log2 w,  d
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

// Global function
inline std::chrono::time_point<std::chrono::high_resolution_clock> now () {
  return std::chrono::high_resolution_clock::now();
}

template <typename T>
void hms (T elapsed) {
//  std::chrono::duration<double, std::milli> ms = elapsed;
  const auto durSec =
    std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
  const auto h = durSec / 3600;
  const auto m = (durSec % 3600) / 60;
  const auto s = durSec % 60;
  
  std::cerr << " in " << h << ":" << m << ":" << s << " hour:min:sec.\n";
}

//template<u32 N>    // Up to 262144=2^18 elements
//struct LogInt      // 0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,...
//{
//  constexpr LogInt() : lg() {
//    for (u32 i=0; i!=LOG_BASE; ++i)
//      lg[i] = 0;
//    for (u32 i=LOG_BASE; i!=N; ++i)
//      lg[i] = static_cast<u8>(1 + lg[i/LOG_BASE]);
//  }
//  u8 lg[N];
//};
// Inside function definition
//constexpr auto a = LogInt<256>();
//cerr << (int) a.lg[3];

#endif //SMASHPP_DEF_HPP