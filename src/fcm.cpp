//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include <cmath>
#include "fcm.hpp"
using std::ifstream;
using std::cout;
using std::array;

/*
 * Constructor and destructor
 */
FCM::FCM (const Parameters& p)
{
    // 6*(5^k_1 + 5^k_2 + ...) > 6*5^12 => mode: hash table 'h'
    mode = (POW5[p.k] > POW5[TAB_MAX_K]) ? 'h' : 't';
}
FCM::~FCM ()
{
//    mode=='h' ? delete tbl : delete ;
    delete tbl;
}

/*
 * Build FCM (finite-context model)
 */
void FCM::buildModel (const Parameters& p)
{
    u64  ctx;                  // Context (integer) to slide in the file
    u64  maxPV=POW5[p.k];    // Max Place Value
    u64  ctxIR;                // Inverted repeat context (integer)
    u8   curr;                 // Current symbol integer
    u64  ctxIRCurr;           // Concat of IR context and current symbol
    ifstream rf(p.ref);       // Ref file
    char c;                   // Chars read from ref file
    u64  rowIdx=0;
    double a=p.alpha, sa=ALPH_SZ*a;
    
    cerr << "Building models...\n";
    
    switch (mode) {
        case 't':
            tbl=new double[TAB_COL*maxPV];
            for (u64 i=0; i!=TAB_COL*maxPV; ++i) {
                if (i%TAB_COL==5)  tbl[i]=sa;
                else               tbl[i]=a;
            }
        
        
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
            break;
            
        case 'h':
            
            ctx   = 0;
            ctxIR = maxPV-1;
            
            // Fill tbl by no. occurrences of symbols A,C,N,G,T
            while (rf.get(c)) {
                if (c!='\n') {
                    curr = NUM[c];
                    
                    // Inverted repeats
                    if (p.ir) {
                        ctxIRCurr = ctxIR + (IR_MAGIC-curr)*maxPV; //Concat: ctxIR, curr
                        ctxIR     = ctxIRCurr/ALPH_SZ;    // Update ctxIR (integer)
                        ++htbl[ctxIR][ctxIRCurr%ALPH_SZ];
                    }
                    
                    ++htbl[ctx][curr];
                    
                    ctx = (ctx*ALPH_SZ)%maxPV + curr;    // Update ctx
                }
            }
            break;
        
        default:
            cerr << "Error.\n";
            break;
    }

    rf.close();

    cerr << "Models built ";
}

/*
 * Compress
 */
void FCM::compress (const Parameters& p) const
{
//    double a=p.alpha, sa=ALPH_SZ*a;
//    const string tfName = p.tar;
//    ifstream tf(tfName);
//    char c;
//    u8 curr;           // Current symbol in integer format
//    u64 maxPV=POW5[p.k];
//    u64 ctx=0;// Context(s) (integer) sliding through the dataset
//    u64 rowIdx;                   // Index of a row in the table
//    double sEntr=0;   // Sum of entropies for different symbols
//    u64 symsNo=0;  // Number of symbols
//    double aveEntr=0;   // Average entropy (H)
//
//    cerr << "Compressing...\n";
//
//    switch (mode) {
//        case 't':
//            while (tf.get(c)) {
//                if (c!='\n') {
//                    ++symsNo;             // No. syms in target file, except \n
//
//                    curr   = NUM[c];    // Current symbol (integer)
//                    rowIdx = ctx*TAB_COL;
//                    sEntr += log2(tbl[rowIdx+ALPH_SZ]/tbl[rowIdx+curr]); // sum( log_2 P(s|c^t) )
////                    sEntr += log2((tbl[rowIdx+ALPH_SZ]+sa)/(tbl[rowIdx+curr]+a)); // sum( log_2 P(s|c^t) )
//
//                    // Update ctx.  (rowIdx - ctx[i]) = (ctx[i] * ALPH_SIZE)
//                    ctx = (rowIdx-ctx)%maxPV + curr;             // Fastest
////            ctx = (rowIdx-ctx+curr)%maxPV;             // Faster
////            ctx = (ctx%POW5[p.k-1])*ALPH_SZ + curr;    // Fast
//                }
//            }
//            break;
//
//        case 'h':
//            u64 sum;
//            array<u64,ALPH_SZ> ar;
//
//            while (tf.get(c)) {
//                if (c!='\n') {
//                    ++symsNo;             // No. syms in target file, except \n
//
//                    curr = NUM[c];
//
//                    ar   = htbl.at(ctx);
//                    sum  = (ar[0]+ar[1]) + (ar[2]) + (ar[3]+ar[4]);
////                    sum=0;    for (const auto& e : ar)  sum+=e;
//
//                    sEntr += log2((sum+sa)/(ar[curr]+a)); // sum( log_2 P(s|c^t) )
////                    sEntr += log2((sum+sa)/(htbl[ctx][curr]+a)); // sum( log_2 P(s|c^t) )
//
//                    ctx = (ctx*ALPH_SZ)%maxPV + curr; // Update ctx
//                }
//            }
//            break;
//
//        default:
//            cerr << "Error.\n";
//            break;
//    }
//
//    tf.close();                      // Close target file
//
//    aveEntr = sEntr/symsNo;
//
//    cerr << "aveEntr=" << aveEntr << '\n';
//
//    cerr << "Compression finished ";
}

/*
 * Print table
 */
void FCM::printTbl (const Parameters &p) const
{
    u64 rowSize=POW5[p.k];
    for (u8 i=0; i!=rowSize; ++i) {
        for (u8 j=0; j!=TAB_COL; ++j)
            cout << tbl[i*TAB_COL+j] << '\t';
        cout << '\n';
    }
}

/*
 * Print hash table
 */
void FCM::printHashTbl () const
{
    for (const auto& e : htbl) {
        for (const auto& v : e.second)
            cerr << v << "\t";
        cerr << "-> " << e.first << '\n';
    }
}
