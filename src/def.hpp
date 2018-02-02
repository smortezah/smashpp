//
// Created by morteza on 02-02-2018.
//

#ifndef SMASHPP_DEF_HPP
#define SMASHPP_DEF_HPP

#include <iostream>
using std::cout;
using std::string;

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
#define DEF_THR  1

// Macro
//#define LOOP(i,S)     for(const char& (i) : (S))
//#define LOOP2(i,j,S)  LOOP(i,S) LOOP(j,S)

// Lookup table
constexpr u8 NUM[123] = {    // a,A=1  c,C=2  g,G=4  t,T=8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2,
    0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0
};
constexpr u8 REV[123] = {    // a,A->T  c,C->G  g,G->C  t,T->A
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'T', 0, 'G', 0, 0, 0, 'C', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'A', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    'T', 0, 'G', 0, 0, 0, 'C', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'A', 0, 0,
    0, 0, 0, 0
};

// Usage guide
void help ()
{
    cout                                                                << '\n'
       << "NAME"                                                        << '\n'
       << "      Smash++ v" << VERSION << " - rearrangements finder"    << '\n'
                                                                        << '\n'
       << "AUTHORS"                                                     << '\n'
       << "      Morteza Hosseini    seyedmorteza@ua.pt"                << '\n'
       << "      Diogo Pratas        pratas@ua.pt"                      << '\n'
       << "      Armando J. Pinho    ap@ua.pt"                          << '\n'
                                                                        << '\n'
       << "SYNOPSIS"                                                    << '\n'
//       << "      ./smashpp [OPTION]...  [-d] [IN_FILE] > [OUT_FILE]"    << '\n'
//                                                                        << '\n'
//       << "SAMPLE"                                                      << '\n'
//       << "      Compress   & Encrypt:   ./cryfa -k pass.txt in.fq "
//       <<                                                  "> comp"     << '\n'
//       << "      Decompress & Decrypt:   ./cryfa -k pass.txt -d comp "
//       <<                                                  "> orig.fq"  << '\n'
//                                                                        << '\n'
//       << "      Shuffle    & Encrypt:   ./cryfa -k pass.txt in > enc"  << '\n'
//       << "      Unshuffle  & Decrypt:   ./cryfa -k pass.txt -d enc > "
//       <<                                                        "orig" << '\n'
//                                                                        << '\n'
//       << "DESCRIPTION"                                                 << '\n'
//       << "      Compress and encrypt FASTA/FASTQ files."               << '\n'
//       << "      Shuffle and encrypt any other text-based files."       << '\n'
//                                                                        << '\n'
//       << "      The KEY_FILE specifies a file including the password." << '\n'
//                                                                        << '\n'
//       << "      -h,  --help"                                           << '\n'
//       << "           usage guide"                                      << '\n'
//                                                                        << '\n'
//       << "      -k [KEY_FILE],  --key [KEY_FILE]"                      << '\n'
//       << "           key file name -- MANDATORY"                       << '\n'
//                                                                        << '\n'
//       << "      -d,  --dec"                                            << '\n'
//       << "           decompress & decrypt"                             << '\n'
//                                                                        << '\n'
//       << "      -v,  --verbose"                                        << '\n'
//       << "           verbose mode (more information)"                  << '\n'
//                                                                        << '\n'
//       << "      -s,  --disable_shuffle"                                << '\n'
//       << "           disable input shuffling"                          << '\n'
//                                                                        << '\n'
//       << "      -t [NUMBER],  --thread [NUMBER]"                       << '\n'
//       << "           number of threads"                                << '\n'
                                                                        << '\n'
       << "COPYRIGHT"                                                   << '\n'
       << "      Copyright (C) " << DEV_YEARS
                                 << ", IEETA, University of Aveiro."    << '\n'
       << "      This is a Free software, under GPLv3. You may redistribute \n"
       << "      copies of it under the terms of the GNU - General Public   \n"
       << "      License v3 <http://www.gnu.org/licenses/gpl.html>. There   \n"
       << "      is NOT ANY WARRANTY, to the extent permitted by law."  << '\n';
    
    exit(1);
}

#endif //SMASHPP_DEF_HPP