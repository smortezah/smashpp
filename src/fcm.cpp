//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include "fcm.hpp"
using std::ifstream;
using std::cout;

void FCM::buildModel (const Parameters& p)
{
//    std::cout << p.ir << ' ' << (int) p.ctx << ' ' << p.alpha;
    
    u64 ctx;                // Context (integer) that slides in the dataset
    u64 maxPV = POW5[p.ctx]; // Max Place Value
    u8  curr;             // Current symbol integer
    ifstream rf(p.ref); // Ref file
    char c;
    tbl = new u64[TAB_COL*maxPV];
    u64 rowIdx;        // To update tbl
    
    
    ctx = 0;
    
    // Fill tbl by no. occurrences of symbols A,C,N,G,T
    for (u32 i=0; i!=BLK_SZ && rf.get(c); ++i) {
        cerr<<i;
//        curr=NUM[c];
//        rowIdx=ctx*TAB_COL;
//        ++tbl[rowIdx+curr];  // Update tbl
//        ++tbl[rowIdx+ALPH_SZ];   // Update 'sum' col
////                        if (++tbl[ rowIdx + ALPH_SIZE ] >= MAX_N_BASE_SUM)
////                        {
////                            ++n_div;               // Count no. of divisions
////                            for (u8 j = ALPH_SUM_SIZE; j--;)
////                                tbl[ rowIdx + j ] >>= 1;
////                        }
//
//        // Update ctx.
//        // (rowIdx - ctx) == (ctx * ALPH_SIZE)
/////         context = (u64) (rowIndex - context + currSymInt) % maxPlaceValue;
/////         context = (u64) (rowIndex - context) % maxPlaceValue + currSymInt;
//        ctx = (u64) (ctx%POW5[p.ctx-1])*ALPH_SZ + curr;
    }
    
    rf.close();
}

/*
 * Print table
 */
void FCM::printTable (const Parameters& p)
{
    for (u8 i=0; i!=p.ctx; ++i) {
        for (u8 j=0; j!=TAB_COL; ++j)
            cout << tbl[i*TAB_COL+j] << '\t';
        cout << '\n';
    }
}