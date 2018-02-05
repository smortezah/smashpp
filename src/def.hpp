//
// Created by morteza on 02-02-2018.
//

#ifndef SMASHPP_DEF_HPP
#define SMASHPP_DEF_HPP

#include <iostream>

// Version
#define VERSION    "18.02"
#define DEV_YEARS  "2018"

// Typedef
typedef signed char         i8;
typedef unsigned char       u8;
typedef signed short        i16;
typedef unsigned short      u16;
typedef signed int          i32;
typedef unsigned int        u32;
typedef signed long long    i64;
typedef unsigned long long  u64;

// Constant
#define DEF_THR   1
#define TAB_COL   6
#define ALPH_SZ   5
#define IR_MAGIC  4
#define BLK_SZ    8192    // 8K

// Macro
//#define LOOP(i,S)     for(const char& (i) : (S))
//#define LOOP2(i,j,S)  LOOP(i,S) LOOP(j,S)

// Lookup table
constexpr u64 POW5[28] = {    // 5^0 to 5^27. Needs < 64 bits
                      1,                  5,                  25,
                    125,                625,                3125,
                  15625,              78125,              390625,
                1953125,            9765625,            48828125,
              244140625,         1220703125,          6103515625,
            30517578125,       152587890625,        762939453125,
          3814697265625,     19073486328125,      95367431640625,
        476837158203125,   2384185791015625,   11920928955078100,
      59604644775390600, 298023223876953000, 1490116119384770000,
    7450580596923830000
};
constexpr u8 NUM[123] = {    // a,A=0  c,C=1  n,N=2  g,G=3  t,T=4
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 2, 0,
    0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 4, 0, 0, 0,
    0, 0, 0
};
constexpr u8 REV[123] = {    // a,A->T  c,C->G  n,N->N  g,G->C  t,T->A
    0, 0, 0,  0,  0,  0, 0,  0, 0, 0,  0,  0, 0, 0, 0, 0,  0,  0,  0,  0,  //20
    0, 0, 0,  0,  0,  0, 0,  0, 0, 0,  0,  0, 0, 0, 0, 0,  0,  0,  0,  0,
    0, 0, 0,  0,  0,  0, 0,  0, 0, 0,  0,  0, 0, 0, 0, 0,  0,  0,  0,  0,
    0, 0, 0,  0,  0,'T', 0,'G', 0, 0,  0,'C', 0, 0, 0, 0,  0,  0,'N',  0,
    0, 0, 0,  0,'A',  0, 0,  0, 0, 0,  0,  0, 0, 0, 0, 0,  0,'T',  0,'G',
    0, 0, 0,'C',  0,  0, 0,  0, 0, 0,'N',  0, 0, 0, 0, 0,'A',  0,  0,  0,
    0, 0, 0
};

#endif //SMASHPP_DEF_HPP