#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>
#include "../def.hpp"
using namespace smashpp;

#define MAX(a,b) (((a)>(b))?(a):(b))

static const string RELEASE_eleanora {"1"};
static const string VERSION_eleanora {"1"};

//#define BUFFER_SIZE         262144
//#define DEF_EXAMPLE         0
//#define DEFAULT_FILTERSIZE  500
//#define DEFAULT_MINBLOCK    100
//#define MIN_CTX             1
//#define MAX_DEN             1000000
//#define MIN_DEN             1
//#define MIN_INDEX           2
//#define BGUARD              32
//#define DEFAULT_MAX_COUNT   ((1 << (sizeof(ACC) * 8)) - 1)
//#define MX_PMODEL           65535
//#define ALPHABET_SIZE       4
//#define CHECKSUMGF          1073741824
//#define WATERMARK           16042014
//#define DEFAULT_GAMMA       0.90
//#define DEFAULT_THRESHOLD   1.85
//#define MAX_HISTORYSIZE     1000000
//#define REFERENCE           1
//#define TARGET              0
//#define EXTRA_MOD_DEN       1
//#define EXTRA_MOD_CTX       3
//#define EXTRA_BIN_DEN       1
//#define EXTRA_BIN_CTX       8
//#define EXTRA_N_DEN         1
//#define EXTRA_N_CTX         8
//#define EXTRA_L_DEN         1
//#define EXTRA_L_CTX         8
//#define MAX_STR             2048

static constexpr bool   DEF_HELP          {false};
static constexpr bool   DEF_VERSION       {false};
static constexpr bool   DEF_VERBOSE       {false};
static constexpr bool   DEF_FORCE         {false};
static constexpr bool   DEF_INVE          {true};
static constexpr bool   DEF_REGU          {true};
static const     string DEF_IMAGE         {"map.svg"};
static constexpr u32    DEF_LINK          {5};
static constexpr u32    MIN_LINK          {0};
static constexpr u32    MAX_LINK          {5};
static constexpr u32    DEF_WIDT          {40};
static constexpr u32    MIN_WIDT          {10};
static constexpr u32    MAX_WIDT          {5000};
static constexpr u32    DEF_SPAC          {160};
static constexpr u32    MIN_SPAC          {0};
static constexpr u32    MAX_SPAC          {5000};
static constexpr u32    DEF_MULT          {46};  // 5 colors without overlapping
static constexpr u32    MIN_MULT          {1};
static constexpr u32    MAX_MULT          {255};
static constexpr u32    DEF_BEGI          {0};
static constexpr u32    MIN_BEGI          {0};
static constexpr u32    MAX_BEGI          {255};
static constexpr u32    DEF_MINP          {0};
static constexpr u32    MIN_MINP          {1};
static constexpr u32    MAX_MINP          {99999999};
static constexpr double DEF_PAINT_CX      {70.0};
static constexpr double DEF_PAINT_CY      {75.0};
static constexpr double DEF_PAINT_TX      {50.0};
static constexpr double DEF_PAINT_TY      {82.0};
static const     string DEF_PAINT_BGCOLOR {"#ffffff"};
static constexpr u32    DEF_PAINT_SCALE   {150};
static constexpr u32    DEF_PAINT_EXTRA   {150};
static const     string DEF_PAINT_REF     {"REF"};
static const     string DEF_PAINT_TAR     {"TAR"};
#define LEVEL_SATURATION  160
#define LEVEL_VALUE       160

//#define DEF_THRE            1
//#define DEF_KMER            16
//#define DEF_MINI            40
//#define DEF_REPE            100
//#define DEF_WIND            10
//#define DEF_EDIT            0
//#define MIN_KMER            1
//#define MAX_KMER            20
//#define MIN_REPE            1
//#define MAX_REPE            55555
//#define MIN_WIND            1
//#define MAX_WIND            50000
//#define MIN_EDIT            0
//#define MAX_EDIT            50000
//#define MIN_MINI            1
//#define MAX_MINI            9999999
//#define DEF_TSHO            0
//#define MIN_TSHO            0
//#define MAX_TSHO            9999999
//#define MIN_THRE            1
//#define MAX_THRE            500
//#define DEF_DELE            1
//#define NSYM                4
//#define GUARD               64
//#define MAX_FILENAME        4096
//#define MAX_CONTIG_NAME     30
#define DEFAULT_SCALE       150
//#define MAX_POS_CACHE       100000

#endif