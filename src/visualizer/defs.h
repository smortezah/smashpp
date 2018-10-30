#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>
#include "../def.hpp"
using namespace smashpp;

#define MAX(a,b) (((a)>(b))?(a):(b))

#define RELEASE_eleanora    1
#define VERSION_eleanora    1

#define BUFFER_SIZE         262144
#define DEF_VERSION         0
#define DEF_EXAMPLE         0
#define DEFAULT_FILTERSIZE  500
#define DEFAULT_MINBLOCK    100
#define MIN_CTX             1
#define MAX_DEN             1000000
#define MIN_DEN             1
#define MIN_INDEX           2
#define BGUARD              32
#define DEFAULT_MAX_COUNT   ((1 << (sizeof(ACC) * 8)) - 1)
#define MX_PMODEL           65535
#define ALPHABET_SIZE       4
#define CHECKSUMGF          1073741824
#define WATERMARK           16042014
#define DEFAULT_GAMMA       0.90
#define DEFAULT_THRESHOLD   1.85
#define MAX_HISTORYSIZE     1000000
#define REFERENCE           1
#define TARGET              0
#define EXTRA_MOD_DEN       1
#define EXTRA_MOD_CTX       3
#define EXTRA_BIN_DEN       1
#define EXTRA_BIN_CTX       8
#define EXTRA_N_DEN         1
#define EXTRA_N_CTX         8
#define EXTRA_L_DEN         1
#define EXTRA_L_CTX         8
#define MAX_STR             2048


#define DEF_HELP            0
#define DEF_VERBOSE         false
#define DEF_FORCE           false
#define DEF_THRE            1
#define DEF_INVE            true
#define DEF_REGU            true
#define DEF_KMER            16
#define DEF_MINI            40
#define DEF_REPE            100
#define DEF_WIND            10
#define DEF_EDIT            0
#define MIN_KMER            1
#define MAX_KMER            20
#define MIN_REPE            1
#define MAX_REPE            55555
#define MIN_WIND            1
#define MAX_WIND            50000
#define MIN_EDIT            0
#define MAX_EDIT            50000
#define MIN_MINI            1
#define MAX_MINI            9999999
#define DEF_TSHO            0
#define MIN_TSHO            0
#define MAX_TSHO            9999999
#define MIN_THRE            1
#define MAX_THRE            500
#define DEF_LINK            5
#define MIN_LINK            0
#define MAX_LINK            5
#define DEF_DELE            1
#define NSYM                4
#define GUARD               64
#define MAX_FILENAME        4096
#define MAX_CONTIG_NAME     30
#define DEFAULT_SCALE       150
#define MAX_POS_CACHE       100000

#endif