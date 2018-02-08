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
FCM::FCM (const Param& p)
{
    // 6*(5^k_1 + 5^k_2 + ...) > 6*5^12 => mode: hash table 'h'
    mode = (POW5[p.k] > POW5[TAB_MAX_K]) ? 'h' : 't';
}
FCM::~FCM ()
{
    delete tbl;
}

/*
 * Build FCM (finite-context model)
 */
void FCM::buildModel (const Param& p)
{
    u64      ctx;                // Context (integer) to slide in the file
    u64      maxPV=POW5[p.k];    // Max Place Value
    u64      ctxIR;              // Inverted repeat context (integer)
    u8       curr;               // Current symbol (integer)
    u64      ctxIRCurr;          // Concat IR context - current symbol
    ifstream rf;                 // Ref file
    char     c;                  // To read from ref file
    double   a=p.alpha, sa=ALPH_SZ*a;
    
    rf.open(p.ref);
    if (!rf) {
        cerr << "Error: the file \"" << p.ref << "\" cannot be opened, "
             << "or it is empty.\n";
        rf.close();
        throw EXIT_FAILURE;
    }
    
    cerr << "Building models...\n";
    
    switch (mode) {
      case 't':
          tbl = new double[TAB_COL*maxPV];
          for (u64 i=0; i!=TAB_COL*maxPV; ++i) {
              tbl[i] = (i%TAB_COL==ALPH_SZ) ? sa : a;
          }
          ctx   = 0;
          ctxIR = maxPV-1;
          
          // Fill tbl by no. occurrences of symbols A,C,N,G,T
          while (rf.get(c)) {
              if (c!='\n') {
                  curr = NUM[c];
                  u64 rowIdx;
                  
                  // Inverted repeats
                  if (p.ir) {
                      ctxIRCurr = ctxIR + (IR_MAGIC-curr)*maxPV;
                      ctxIR     = ctxIRCurr/ALPH_SZ;      // Update ctxIR
                      rowIdx    = ctxIR*TAB_COL;
                      ++tbl[rowIdx+ctxIRCurr%ALPH_SZ];
                      ++tbl[rowIdx+ALPH_SZ];              // 'sum' col
                  }
                  
                  rowIdx = ctx*TAB_COL;
                  ++tbl[rowIdx+curr];
                  ++tbl[rowIdx+ALPH_SZ];
                  // Update ctx.  (rowIdx - k) == (k * ALPH_SIZE)
                  ctx = (rowIdx-ctx)%maxPV + curr;             // Fastest
//                  ctx = (rowIdx-ctx+curr)%maxPV;             // Faster
//                  ctx = (ctx%POW5[p.k-1])*ALPH_SZ + curr;    // Fast
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
                      ctxIRCurr = ctxIR + (IR_MAGIC-curr)*maxPV;
                      ctxIR     = ctxIRCurr/ALPH_SZ;       // Update ctxIR
                      ++htbl[ctxIR][ctxIRCurr%ALPH_SZ];
                  }
                  
                  ++htbl[ctx][curr];
                  ctx = (ctx*ALPH_SZ)%maxPV + curr;        // Update ctx
              }
          }
          break;
    
      default:  cerr << "Error.\n";  break;
    }

    rf.close();
    
    cerr << "Models built ";
}

/*
 * Compress
 */
void FCM::compress (const Param& p) const
{
    double   a=p.alpha, sa=ALPH_SZ*a;
    ifstream tf;
    char     c;
    u8       curr;           // Current symbol (integer)
    u64      maxPV=POW5[p.k];
    u64      ctx=0;          // Context(s) (integer) sliding through the dataset
    double   sEntr=0;        // Sum of entropies = sum( log_2 P(s|c^t) )
    u64      symsNo=0;       // No. syms in target file, except \n
    
    tf.open(p.tar);
//    if (!tf) {
//        cerr << "Error: the file \"" << p.tar << "\" cannot be opened, "
//             << "or it is empty.\n";
//        tf.close();
//        throw EXIT_FAILURE;
//    }
    
    cerr << "Compressing...\n";

    switch (mode) {
      case 't':
          while (tf.get(c)) {
              if (c!='\n') {
                  ++symsNo;
                  curr       = NUM[c];
                  u64 rowIdx = ctx*TAB_COL;
                  sEntr     += log2(tbl[rowIdx+ALPH_SZ]/tbl[rowIdx+curr]);
                  ctx        = (rowIdx-ctx)%maxPV + curr;    // Update ctx
              }
          }
          break;

      case 'h':
          while (tf.get(c)) {
              if (c!='\n') {
                  ++symsNo;
                  curr    = NUM[c];
                  auto hi = htbl.find(ctx);
                  if (hi != htbl.end()) {
                      auto ar = hi->second;
                      u64 sum=0;    for (const auto &e : ar)  sum+=e;
//                  sum    = (ar[0]+ar[1]) + (ar[2]) + (ar[3]+ar[4]);
                      sEntr += log2((sum+sa)/(ar[curr]+a));
                  }
                  else {
                      sEntr += log2(ALPH_SZ);
                  }
                  ctx = (ctx*ALPH_SZ)%maxPV + curr;    // Update ctx
              }
          }
          break;
          
      default:  cerr<<"Error.\n";  break;
    }

    tf.close();
    
    double aveEntr = sEntr/symsNo;
    cerr << "Average Entropy (H) = " << aveEntr << '\n';

    cerr << "Compression finished ";
}

/*
 * Print table
 */
void FCM::printTbl (const Param &p) const
{
    u64 rowSize=POW5[p.k];
    for (u8 i=0; i!=rowSize; ++i) {
        for (u8 j=0; j!=TAB_COL; ++j)
            cerr << tbl[i*TAB_COL+j] << '\t';
        cerr << '\n';
    }
}

/*
 * Print hash table
 */
void FCM::printHashTbl () const
{
    for (const auto& e : htbl) {
        for (const auto& v : e.second)
            cerr << v << '\t';
        cerr << "-> " << e.first << '\n';
    }
}