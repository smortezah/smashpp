//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include <cmath>
#include <thread>
#include "fcm.hpp"
using std::ifstream;
using std::cout;
using std::array;
using std::thread;

FCM::FCM (const Param& p) {
  model.resize(LEVEL[p.level][0]);
  for (auto m=model.begin(); m!=model.end(); ++m) {
    auto i   = m - model.begin();
    m->ir    = LEVEL[p.level][5*i+1];
    m->k     = LEVEL[p.level][5*i+2];
    m->alpha = static_cast<float>(LEVEL[p.level][5*i+3])/100;
    m->w     = power(2, LEVEL[p.level][5*i+4]);
    m->d     = LEVEL[p.level][5*i+5];
    if      (m->k > K_MAX_LGTBL8)         m->mode = MODE::SKETCH_8;
    else if (m->k > K_MAX_TBL32)          m->mode = MODE::LOG_TABLE_8;
    else if (m->k > K_MAX_TBL64)          m->mode = MODE::TABLE_32;
    else                                  m->mode = MODE::TABLE_64;
    if      (m->mode==MODE::TABLE_64)       tbl64 = new Table64(m->k);
    else if (m->mode==MODE::TABLE_32)       tbl32 = new Table32(m->k);
    else if (m->mode==MODE::LOG_TABLE_8)  logtbl8 = new LogTable8(m->k);
    else                                  sketch4 = new CMLS4(m->w, m->d);
  }
}

FCM::~FCM () {
  for (auto m : model) {
    if      (m.mode==MODE::TABLE_64)      delete tbl64;
    else if (m.mode==MODE::TABLE_32)      delete tbl32;
    else if (m.mode==MODE::LOG_TABLE_8)   delete logtbl8;
    else                                  delete sketch4;
  }
}

void FCM::buildModel (const Param& p) {
  cerr << "Building models...\n";
  for (auto m : model) {
    switch (m.mode) {
      case MODE::TABLE_64:     buildTbl64(p.ref, m.k);    break;
      case MODE::TABLE_32:     buildTbl32(p.ref, m.k);    break;
      case MODE::LOG_TABLE_8:  buildLogTbl8(p.ref, m.k);  break;
      case MODE::SKETCH_8:     buildSketch4(p.ref, m.k);  break;
      default:                 cerr << "Error.";
    }
  }
  
  //todo. test multithreading
//  vector<thread> thrd;  thrd.resize(4);
//  for (auto m : model) {
//    switch (m.mode) {
//      case MODE::TABLE_64:
//        thrd[0] = thread(&FCM::buildTbl64,   this, p.ref, m.k);  break;
//      case MODE::TABLE_32:
//        thrd[1] = thread(&FCM::buildTbl32,   this, p.ref, m.k);  break;
//      case MODE::LOG_TABLE_8:
//        thrd[2] = thread(&FCM::buildLogTbl8, this, p.ref, m.k);  break;
//      default:
//        thrd[3] = thread(&FCM::buildSketch4, this, p.ref, m.k);
//    }
//  }
//  for (u8 t=0; t!=4; ++t)  if (thrd[t].joinable()) thrd[t].join();
  cerr << "Models built ";
}

inline void FCM::buildTbl64 (const string& ref, u8 k) {
  ifstream rf(ref);
  char c;
  auto mask = static_cast<u32>((4<<(k<<1)) - 1);    // 4<<2k - 1 = 4^(k+1) - 1
  for (u32 ctx=0; rf.get(c);) {
    if (c != '\n') {
      ctx = ((ctx<<2) & mask) | NUM[c];             // Update ctx
      tbl64->update(ctx);
    }
  }
  rf.close();
}

inline void FCM::buildTbl32 (const string& ref, u8 k) {
  ifstream rf(ref);
  char c;
  auto mask = static_cast<u32>((4<<(k<<1)) - 1);
  for (u32 ctx=0; rf.get(c);) {
    if (c != '\n') {
      ctx = ((ctx<<2) & mask) | NUM[c];
      tbl32->update(ctx);
    }
  }
  rf.close();
}

inline void FCM::buildLogTbl8 (const string& ref, u8 k) {
  ifstream rf(ref);
  char c;
  auto mask = static_cast<u32>((4<<(k<<1)) - 1);
  for (u32 ctx=0; rf.get(c);) {
    if (c != '\n') {
      ctx = ((ctx<<2) & mask) | NUM[c];
      logtbl8->update(ctx);
    }
  }
  rf.close();
}

inline void FCM::buildSketch4 (const string& ref, u8 k) {
  ifstream rf(ref);
  char c;
  auto mask = static_cast<u64>((4<<(k<<1)) - 1);
  for (u64 ctx=0; rf.get(c);) {
    if (c != '\n') {
      ctx = ((ctx<<2) & mask) | NUM[c];
      sketch4->update(ctx);
    }
  }
  rf.close();
}

void FCM::compress (const Param& p) const {
  cerr << "Compressing...\n";
  for (auto m : model) {
    switch (m.mode) {
      case MODE::TABLE_64:    compressTbl64(p.tar, m.ir, m.k, m.alpha);   break;
      case MODE::TABLE_32:    compressTbl32(p.tar, m.ir, m.k, m.alpha);   break;
      case MODE::LOG_TABLE_8: compressLogTbl8(p.tar, m.ir, m.k, m.alpha); break;
      case MODE::SKETCH_8:    compressSketch4(p.tar, m.ir, m.k, m.alpha); break;
      default:                cerr << "Error";
    }
  }
//  cerr << "Compression finished ";
}

inline void FCM::compressTbl64 (const string& tar,
                                u8 ir, u8 k, float alpha) const {
  std::array<u64, 4> vN{0};// Vector of number of elements
  auto   shl    = k<<1;    // Shift left
  auto   mask   = static_cast<u32>((1<<shl) - 1); // 1<<2k -1 = 4^k-1 = 2^(2k)-1
  u32    ctx    = 0;       // Context(s) (integer) sliding through the dataset
  u32    ctxIR  = mask;    // Inverted repeat context (integer)
  u64    symsNo = 0;       // No. syms in target file, except \n
  double sAlpha = ALPH_SZ*alpha;                  // Sum of alphas
  double sEntr  = 0;       // Sum of entropies = sum( log_2 P(s|c^t) )
  ifstream tf(tar);
  char c;
  while (tf.get(c)) {
    if (c != '\n') {
      ++symsNo;
      auto numSym = NUM[c];
      // Inverted repeat
      if (ir) {
        auto r = ctxIR>>2;
        vN[0] = tbl64->query(3<<shl | r);    // A
        vN[1] = tbl64->query(2<<shl | r);    // C
        vN[2] = tbl64->query(1<<shl | r);    // G
        vN[3] = tbl64->query(r);             // T
        ctxIR = REVNUM[c]<<shl | r;          // Update ctxIR
      }
      auto l = ctx<<2;
      vN[0] += tbl64->query(l);        // A
      vN[1] += tbl64->query(l | 1);    // C
      vN[2] += tbl64->query(l | 2);    // G
      vN[3] += tbl64->query(l | 3);    // T
      ctx = (l & mask) | numSym;       // Update ctx
      
      sEntr += log2((vN[0]+vN[1]+vN[2]+vN[3]+sAlpha) / (vN[numSym]+alpha));
      
      //todo test
//      cout<<"nA="<<nA<<'\t'<<"nC="<<nC<<'\t'<<"nG="<<nG<<'\t'<<"nT="<<nT<<'\t'
//          <<"sum="<<sum<<'\n'
//          <<"sEntr="<<sEntr<<'\n';
    }
  }
  tf.close();
  double aveEntr = sEntr/symsNo;
  cerr << "Average Entropy (H) = " << aveEntr << '\n';
  cerr << "Compression finished ";
}

inline void FCM::compressTbl32 (const string& tar,
                                u8 ir, u8 k, float alpha) const {
  std::array<u64, 4> vN{0};
  auto   shl    = k<<1;
  auto   mask   = static_cast<u32>((1<<shl) - 1);
  u32    ctx    = 0;
  u32    ctxIR  = mask;
  u64    symsNo = 0;
  double sAlpha = ALPH_SZ*alpha;
  double sEntr  = 0;
  ifstream tf(tar);
  char c;
  while (tf.get(c)) {
    if (c != '\n') {
      ++symsNo;
      auto numSym = NUM[c];
      // Inverted repeat
      if (ir) {
        auto r = ctxIR>>2;
        vN[0] = tbl32->query(3<<shl | r);    // A
        vN[1] = tbl32->query(2<<shl | r);    // C
        vN[2] = tbl32->query(1<<shl | r);    // G
        vN[3] = tbl32->query(r);             // T
        ctxIR = REVNUM[c]<<shl | r;          // Update ctxIR
      }
      auto l = ctx<<2;
      vN[0] += tbl32->query(l);        // A
      vN[1] += tbl32->query(l | 1);    // C
      vN[2] += tbl32->query(l | 2);    // G
      vN[3] += tbl32->query(l | 3);    // T
      ctx = (l & mask) | numSym;       // Update ctx
      
      sEntr += log2((vN[0]+vN[1]+vN[2]+vN[3]+sAlpha) / (vN[numSym]+alpha));
    }
  }
  tf.close();
  double aveEntr = sEntr/symsNo;
  cerr << "Average Entropy (H) = " << aveEntr << '\n';
  cerr << "Compression finished ";
}

inline void FCM::compressLogTbl8 (const string& tar,
                                  u8 ir, u8 k, float alpha) const {
  std::array<u64, 4> vN{0};
  auto   shl    = k<<1;
  auto   mask   = static_cast<u32>((1<<shl) - 1);
  u32    ctx    = 0;
  u32    ctxIR  = mask;
  u64    symsNo = 0;
  double sAlpha = ALPH_SZ*alpha;
  double sEntr  = 0;
  ifstream tf(tar);
  char c;
  while (tf.get(c)) {
    if (c != '\n') {
      ++symsNo;
      auto numSym = NUM[c];
      // Inverted repeat
      if (ir) {
        auto r = ctxIR>>2;
        vN[0] = logtbl8->query(3<<shl | r);    // A
        vN[1] = logtbl8->query(2<<shl | r);    // C
        vN[2] = logtbl8->query(1<<shl | r);    // G
        vN[3] = logtbl8->query(r);             // T
        ctxIR = REVNUM[c]<<shl | r;          // Update ctxIR
      }
      auto l = ctx<<2;
      vN[0] += logtbl8->query(l);        // A
      vN[1] += logtbl8->query(l | 1);    // C
      vN[2] += logtbl8->query(l | 2);    // G
      vN[3] += logtbl8->query(l | 3);    // T
      ctx = (l & mask) | numSym;       // Update ctx
      
      sEntr += log2((vN[0]+vN[1]+vN[2]+vN[3]+sAlpha) / (vN[numSym]+alpha));
    }
  }
  tf.close();
  double aveEntr = sEntr/symsNo;
  cerr << "Average Entropy (H) = " << aveEntr << '\n';
  cerr << "Compression finished ";
}

inline void FCM::compressSketch4 (const string& tar,
                                  u8 ir, u8 k, float alpha) const {
  std::array<u32, 4> vN{0};  // 2*(2^16-1) < 32 bits
  auto   shl    = k<<1;
  auto   mask   = static_cast<u64>((1<<shl) - 1);
  u64    ctx    = 0;
  u64    ctxIR  = mask;
  u64    symsNo = 0;
  double sAlpha = ALPH_SZ*alpha;
  double sEntr  = 0;
  ifstream tf(tar);
  char c;
  while (tf.get(c)) {
    if (c != '\n') {
      ++symsNo;
      auto numSym = NUM[c];
      // Inverted repeat
      if (ir) {
        auto r = ctxIR>>2;
        vN[0] = sketch4->query(3<<shl | r);    // A
        vN[1] = sketch4->query(2<<shl | r);    // C
        vN[2] = sketch4->query(1<<shl | r);    // G
        vN[3] = sketch4->query(r);             // T
        ctxIR = REVNUM[c]<<shl | r;            // Update ctxIR
      }
      auto l = ctx<<2;
      vN[0] += sketch4->query(l);        // A
      vN[1] += sketch4->query(l | 1);    // C
      vN[2] += sketch4->query(l | 2);    // G
      vN[3] += sketch4->query(l | 3);    // T
      ctx = (l & mask) | numSym;         // Update ctx
      
      sEntr += log2((vN[0]+vN[1]+vN[2]+vN[3]+sAlpha) / (vN[numSym]+alpha));
    }
  }
  tf.close();
  double aveEntr = sEntr/symsNo;
  cerr << "Average Entropy (H) = " << aveEntr << '\n';
  cerr << "Compression finished ";
}