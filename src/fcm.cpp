//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include "fcm.hpp"
using std::ifstream;
using std::cout;

void FCM::buildModel (const Parameters& p)
{
    u64 ctx;                  // Context (integer) to slide in the file
    u64 maxPV = POW5[p.k];    // Max Place Value
    u64 ctxIR;                // Inverted repeat context (integer)
    u8  curr;                 // Current symbol integer
    u64 ctxIRCurr;           // Concat of IR context and current symbol
    ifstream rf(p.ref);       // Ref file
    char c;                   // Chars read from ref file
    tbl = new u64[TAB_COL*maxPV];
    u64 rowIdx;
    
    
    ctx   = 0;
    ctxIR = maxPV-1;
    
    // Fill tbl by no. occurrences of symbols A,C,N,G,T
    for (u32 i=0; rf.get(c) && i!=BLK_SZ; ++i) {
        if (c!='\n') {
            curr = NUM[c];
            
            if (p.ir) {   // Considering IRs to update table
                // Concatenation of IR context and current symbol
                ctxIRCurr = ctxIR + (IR_MAGIC-curr)*maxPV;
                // Update inverted repeat context (integer)
                ctxIR = ctxIRCurr / ALPH_SZ;
        
                // Update table
                rowIdx = ctxIR * TAB_COL;
                ++tbl[rowIdx+ctxIRCurr%ALPH_SZ];
                ++tbl[rowIdx+ALPH_SZ];    // 'sum' col
                // Update 'sum' col, then check for overflow
//                            if (++table[ rowIndex + ALPH_SIZE ]
//                                >= MAX_N_BASE_SUM)
//                            {
///                                ++n_div;           // Count no. of divisions
//                                for (u8 j = ALPH_SUM_SIZE; j--;)
//                                    table[ rowIndex + j ] >>= 1;
//                            }
            }
            
            rowIdx = ctx * TAB_COL;
            ++tbl[rowIdx+curr];
            ++tbl[rowIdx+ALPH_SZ];
//                        if (++tbl[ rowIdx + ALPH_SIZE ] >= MAX_N_BASE_SUM)
//                        {
//                            ++n_div;               // Count no. of divisions
//                            for (u8 j = ALPH_SUM_SIZE; j--;)
//                                tbl[ rowIdx + j ] >>= 1;
//                        }
    
            // Update ctx
            // (rowIdx - k) == (k * ALPH_SIZE)
///         ctx = (u64) (rowIdx - ctx + curr) % maxPV;
///         ctx = (u64) (rowIdx - ctx) % maxPV + curr;
            ctx = (ctx%POW5[p.k-1])*ALPH_SZ + curr;
        }
    }
    
    rf.close();
}

/*
 * Print table
 */
void FCM::printTable (const Parameters& p)
{
    for (u8 i=0; i!=POW5[p.k]; ++i) {
        for (u8 j=0; j!=TAB_COL; ++j)
            cout << tbl[i*TAB_COL+j] << '\t';
        cout << '\n';
    }
}