//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include <stdlib.h>
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
//    tbl = new u64[TAB_COL*maxPV];
    tbl = new float[TAB_COL*maxPV];
    u64 rowIdx;
    
    
    ctx   = 0;
    ctxIR = maxPV-1;
    
    // Fill tbl by no. occurrences of symbols A,C,N,G,T
//    for (u32 i=0; rf.get(c) && i!=BLK_SZ; ++i) {
    while (rf.get(c)) {
        if (c!='\n') {
            curr = NUM[c];
            
            // Inverted repeats
            if (p.ir) {
                ctxIRCurr = ctxIR + (IR_MAGIC-curr)*maxPV; //Concat: ctxIR, curr
                ctxIR     = ctxIRCurr/ALPH_SZ;    // Update ctxIR (integer)
                rowIdx    = ctxIR*TAB_COL;
                ++tbl[rowIdx+ctxIRCurr%ALPH_SZ];
                ++tbl[rowIdx+ALPH_SZ];            // 'sum' col
            }
            
            rowIdx = ctx*TAB_COL;
            ++tbl[rowIdx+curr];
            ++tbl[rowIdx+ALPH_SZ];
            
            // Update ctx.  (rowIdx - k) == (k * ALPH_SIZE)
            ctx = (rowIdx-ctx)%maxPV + curr;             // Fastest
//            ctx = (rowIdx-ctx+curr)%maxPV;             // Faster
//            ctx = (ctx%POW5[p.k-1])*ALPH_SZ + curr;    // Fast
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