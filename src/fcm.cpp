//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include <cmath>
#include "fcm.hpp"
//#include "cmls.hpp"
using std::ifstream;
using std::cout;
using std::array;

FCM::FCM (const Param& p) {
  bool tFound = false;
  bool sFound = false;
  for (auto i=static_cast<char>(LEVEL[p.level][0]-1); i>=0; --i) {
    if      (LEVEL[p.level][6*i+4]=='t') { tFound=true;  break; }
    else if (LEVEL[p.level][6*i+4]=='s') { sFound=true;  break; }
  }
  if (tFound)  tbl  = new Table64();
  if (sFound)  skch = new CMLS();
}

FCM::~FCM () {
  delete tbl;
  delete skch;
}

void FCM::buildModel (const Param& p) {
  u64      ctx;                // Context (integer) to slide in the file
  u64      maxPV=POW5[p.k[0]];    // Max Place Value//todo. change k[0]
  u64      ctxIR;              // Inverted repeat context (integer)
  u8       curr;               // Current symbol (integer)
  u64      ctxIRCurr;          // Concat IR context - current symbol
  double   a=p.alpha[0], sa=ALPH_SZ*a;//todo. change alpha[0]
  ifstream rf;                 // Ref file
  char     c;                  // To read from ref file
  rf.open(p.ref);
  cerr << "Building models...\n";
  
  // Table64
  if (p.mode == 't') {
//    tbl = new double[TAB_COL*maxPV];
//    for (u64 i=0; i!=TAB_COL*maxPV; ++i) {
//      tbl[i] = (i%TAB_COL==ALPH_SZ) ? sa : a;
//    }
//    ctx   = 0;
//    ctxIR = maxPV-1;
//    // Fill tbl by no. occurrences of symbols A,C,N,G,T
//    while (rf.get(c)) {
//      if (c != '\n') {
//        curr = NUM[c];
//        u64 rowIdx;
//
//        // Inverted repeats
//        if (p.ir[0]) {//todo. change ir[0]
//          ctxIRCurr = ctxIR + (IRMAGIC-curr)*maxPV;
//          ctxIR     = ctxIRCurr/ALPH_SZ;      // Update ctxIR
//          rowIdx    = ctxIR*TAB_COL;
//          ++tbl[rowIdx+ctxIRCurr%ALPH_SZ];
//          ++tbl[rowIdx+ALPH_SZ];              // 'sum' col
//        }
//
//        rowIdx = ctx*TAB_COL;
//        ++tbl[rowIdx+curr];
//        ++tbl[rowIdx+ALPH_SZ];
//        // Update ctx.  (rowIdx - k) == (k * ALPH_SIZE)
//        ctx = (rowIdx-ctx)%maxPV + curr;             // Fastest
////        ctx = (ctx*ALPH_SZ)%maxPV + curr;             // Fastest
////        ctx = (rowIdx-ctx+curr)%maxPV;             // Faster
////        ctx = (ctx%POW5[p.k-1])*ALPH_SZ + curr;    // Fast
//      }
//    }
  }
  // Sketch
  else if (p.mode == 's') {
    ctx = 0;
    auto mask = static_cast<u64>((4<<(p.k[0]<<1)) - 1); // 4<<2k -1 = 4^(k+1) -1
    // Fill the sketch by no. occurrences of A,C,G,T
    while (rf.get(c)) {
      if (c != '\n') {
        ctx = ((ctx<<2) & mask) | NUM[c];    // Update ctx
        skch->update(ctx);
      }
    }
  }
  // Hash table
//  else if (p.mode == 'h') { //todo. remove
//    ctx   = 0;
//    ctxIR = maxPV-1;
//    // Fill tbl by no. occurrences of symbols A,sk,N,G,T
//    while (rf.get(c)) {
//      if (c!='\n') {
//        curr = NUM[c];
//        // Inverted repeats
//        if (p.ir[0]) {//todo. change ir[0]
//          ctxIRCurr = ctxIR + (IR_MAGIC-curr)*maxPV;
//          ctxIR     = ctxIRCurr/ALPH_SZ;       // Update ctxIR
//          ++htbl[ctxIR][ctxIRCurr%ALPH_SZ];
//        }
//
//        ++htbl[ctx][curr];
//        ctx = (ctx*ALPH_SZ)%maxPV + curr;        // Update ctx
//      }
//    }
//  }
  else
    cerr << "Error.\n";
  
  rf.close();
  cerr << "Models built ";
}

void FCM::compress (const Param& p) const {
  double   a=p.alpha[0], sa=ALPH_SZ*a;
  ifstream tf;
  char     c;
  u8       curr;           // Current symbol (integer)
  u64      maxPV=POW5[p.k[0]];
  u64      ctx=0;          // Context(s) (integer) sliding through the dataset
  u64      ctxIR=0;        // Inverted repeat context (integer)
  double   sEntr=0;        // Sum of entropies = sum( log_2 P(s|c^t) )
  u64      symsNo=0;       // No. syms in target file, except \n
  tf.open(p.tar);
  
  cerr << "Compressing...\n";
  // Table64
  if (p.mode == 't') {
//      while (tf.get(c)) {
//        if (c!='\n') {
//          ++symsNo;
//          curr       = NUM[c];
//          u64 rowIdx = ctx*TAB_COL;
//          sEntr     += log2(tbl[rowIdx+ALPH_SZ]/tbl[rowIdx+curr]);
//          ctx        = (rowIdx-ctx)%maxPV + curr;    // Update ctx
//        }
//      }
  }
  // Sketch
  else if (p.mode == 's') {
    std::array<u64, 4> vN{0}; // Vector of number of elements
    auto shl  = static_cast<u64>(2 * p.k[0]);    // 2*k
    auto mask = static_cast<u64>((4<<shl) - 1); // 4<<2k -1 = 4^(k+1) -1
    ctxIR = mask;
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        // Inverted repeat
        if (p.ir[0]) {//todo. change ir[0]
          u64 r = ctxIR>>2;                  // Update ctx
          vN[0] = skch->query(r | 3<<shl); // A
          vN[1] = skch->query(r | 2<<shl); // C
          vN[2] = skch->query(r | 1<<shl); // G
          vN[3] = skch->query(r);          // T
//          ctxIR  = (ctxIR>>2) | (IRMAGIC-NUM[c])<<shl;    // Update ctx
        }
        
        u64 l  = (ctx<<2) & mask;    // Update ctx
        vN[0] += skch->query(l);     // A
        vN[1] += skch->query(l | 1); // C
        vN[2] += skch->query(l | 2); // G
        vN[3] += skch->query(l | 3); // T
        
        sEntr += log2((vN[0]+vN[1]+vN[2]+vN[3]+sa) / (vN[NUM[c]]+a));
        
//        cout<<"nA="<<nA<<'\t'<<"nC="<<nC<<'\t'<<"nG="<<nG<<'\t'<<"nT="<<nT<<'\t'
//            <<"sum="<<sum<<'\n'
//            <<"sEntr="<<sEntr<<'\n';
      }
    }
  }
  // Hash table
  else if (p.mode == 'h') { //todo. remove
//    while (tf.get(c)) {
//      if (c!='\n') {
//        ++symsNo;
//        curr    = NUM[c];
//        auto hi = htbl.find(ctx);
//        if (hi!=htbl.end()) {
//          auto ar = hi->second;
//          u64 sum=0;    for (const auto &e : ar)  sum+=e;
//          sEntr += log2((sum+sa)/(ar[curr]+a));
//        }
//        else {
//          sEntr += log2(ALPH_SZ);
//        }
//        ctx = (ctx*ALPH_SZ)%maxPV + curr;    // Update ctx
//      }
//    }
  }
  
  tf.close();
  double aveEntr = sEntr/symsNo;
  cerr << "Average Entropy (H) = " << aveEntr << '\n';
  cerr << "Compression finished ";
}

void FCM::printTbl (const Param &p) const {
//  u64 rowSize = POW5[p.k[0]];//todo. change k[0]
//  for (u8 i=0; i!=rowSize; ++i) {
//    for (u8 j=0; j!=TAB_COL; ++j)
//      cerr << tbl[i*TAB_COL+j] << '\t';
//    cerr << '\n';
//  }
}

void FCM::printHashTbl () const {
  for (const auto& e : htbl) {
    for (const auto& v : e.second)
      cerr << v << '\t';
    cerr << "-> " << e.first << '\n';
  }
}