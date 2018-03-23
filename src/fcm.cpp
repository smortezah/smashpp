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
    auto mask32 = static_cast<u32>((4<<(m.k<<1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
    auto mask64 = static_cast<u64>((4<<(m.k<<1))-1);
    switch (m.mode) {
      case MODE::TABLE_64:     createDS(p.ref, mask32, tbl64);    break;
      case MODE::TABLE_32:     createDS(p.ref, mask32, tbl32);    break;
      case MODE::LOG_TABLE_8:  createDS(p.ref, mask32, logtbl8);  break;
      case MODE::SKETCH_8:     createDS(p.ref, mask64, sketch4);  break;
      default:                 cerr << "Error.";
    }
  }
  
//  //todo. test multithreading
//  vector<thread> thrd;  thrd.resize(4);
//  for (auto m : model) {
//    auto mask32 = static_cast<u32>((4<<(m.k<<1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
//    auto mask64 = static_cast<u64>((4<<(m.k<<1))-1);
//    switch (m.mode) {
//      case MODE::TABLE_64:
//        thrd[0] = thread(&FCM::createDS<u32,Table64*>, this,
//                         p.ref, mask32, tbl64);
//        break;
//      case MODE::TABLE_32:
//        thrd[1] = thread(&FCM::createDS<u32,Table32*>, this,
//                         p.ref, mask32, tbl32);
//        break;
//      case MODE::LOG_TABLE_8:
//        thrd[2] = thread(&FCM::createDS<u32,LogTable8*>, this,
//                         p.ref, mask32, logtbl8);
//        break;
//      case MODE::SKETCH_8:
//        thrd[3] = thread(&FCM::createDS<u64,CMLS4*>, this,
//                         p.ref, mask64, sketch4);
//        break;
//      default:    cerr << "Error.";
//    }
//  }
//  for (u8 t=0; t!=4; ++t)  if (thrd[t].joinable()) thrd[t].join();
  cerr << "Models built ";
}

template <typename T, typename U>
inline void FCM::createDS (const string& ref, T mask, U& container) {
  ifstream rf(ref);
  char c;
  for (T ctx=0; rf.get(c);) {
    if (c != '\n') {
      ctx = ((ctx<<2) & mask) | NUM[c];  // Update ctx
      container->update(ctx);
    }
  }
  rf.close();
}

//void FCM::compress (const Param& p) const {
//  cerr << "Compressing...\n";
//  array<u64, 4> aN64{0};    // Array of number of elements
//  array<u32, 4> aN32{0};
//  for (auto m : model) {
//    auto mask32 = static_cast<u32>((1<<(m.k<<1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
//    auto mask64 = static_cast<u64>((1<<(m.k<<1))-1);
//    switch (m.mode) {
//      case MODE::TABLE_64:    compressDS(p.tar, m, mask32, aN64, tbl64);  break;
//      case MODE::TABLE_32:    compressDS(p.tar, m, mask32, aN64, tbl32);  break;
//      case MODE::LOG_TABLE_8: compressDS(p.tar, m, mask32, aN64, logtbl8);break;
//      case MODE::SKETCH_8:    compressDS(p.tar, m, mask64, aN32, sketch4);break;
//      default:                cerr << "Error";
//    }
//  }
//
////  //todo. test multithreading
////  vector<thread> thrd;  thrd.resize(4);
////  array<u64, 4> aN64{0};    // Array of number of elements
////  array<u32, 4> aN32{0};
////  for (auto m : model) {
////    auto mask32 = static_cast<u32>((1<<(m.k<<1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
////    auto mask64 = static_cast<u64>((1<<(m.k<<1))-1);
////    switch (m.mode) {
////      case MODE::TABLE_64:
////        thrd[0] = thread(&FCM::compressDS<u32,array<u64,4>,Table64*>, this,
////                         p.tar, m, mask32, aN64, tbl64);
////        break;
////      case MODE::TABLE_32:
////        thrd[1] = thread(&FCM::compressDS<u32,array<u64,4>,Table32*>, this,
////                         p.tar, m, mask32, aN64, tbl32);
////        break;
////      case MODE::LOG_TABLE_8:
////        thrd[2] = thread(&FCM::compressDS<u32,array<u64,4>,LogTable8*>, this,
////                         p.tar, m, mask32, aN64, logtbl8);
////        break;
////      case MODE::SKETCH_8:
////        thrd[3] = thread(&FCM::compressDS<u64,array<u32,4>,CMLS4*>, this,
////                         p.tar, m, mask64, aN32, sketch4);
////        break;
////      default:    cerr << "Error.";
////    }
////  }
////  for (u8 t=0; t!=4; ++t)  if (thrd[t].joinable()) thrd[t].join();
////  cerr << "Compression finished ";
//}

void FCM::compress (const Param& p) const {
  cerr << "Compressing...\n";
  compressDS(p.tar);
//  cerr << "Compression finished ";
}


#include <tuple>
#include <typeinfo>
inline void FCM::compressDS (const string& tar) const {
//  Table64* a;
//  CMLS4* b;
//  for (auto m : model) {
//    if(m.mode==MODE::TABLE_64)
//      a=tbl64;
//    else
//      b=sketch4;
////    auto a=dtStruct(m.mode);
////    prob();
//  }
  
  vector<DS*> v;
  v.push_back(tbl64);
for(auto a:v)
  dynamic_cast<Table64*>(a)->print();
  
  
////  array<u64, 4> aN64{0};    // Array of number of elements
////  array<u32, 4> aN32{0};
////  for (auto m : model) {
////    auto mask32 = static_cast<u32>((1<<(m.k<<1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
////    auto mask64 = static_cast<u64>((1<<(m.k<<1))-1);
////    switch (m.mode) {
////      case MODE::TABLE_64:    compressDS(p.tar, m, mask32, aN64, tbl64);  break;
////      case MODE::TABLE_32:    compressDS(p.tar, m, mask32, aN64, tbl32);  break;
////      case MODE::LOG_TABLE_8: compressDS(p.tar, m, mask32, aN64, logtbl8);break;
////      case MODE::SKETCH_8:    compressDS(p.tar, m, mask64, aN32, sketch4);break;
////      default:                cerr << "Error";
////    }
////  }

//  auto   shl    = mdl.k<<1;  // Shift left
//  T      ctx    = 0;         // Context(s) (integer) sliding through the dataset
//  T      ctxIR  = mask;      // Inverted repeat context (integer)
//  u64    symsNo = 0;         // No. syms in target file, except \n
//  float  alpha  = mdl.alpha;
//  double sAlpha = ALPH_SZ*alpha;  // Sum of alphas
//  double sEntr  = 0;         // Sum of entropies = sum( log_2 P(s|c^t) )
//  ifstream tf(tar);
//  char c;
//  while (tf.get(c)) {
//    if (c != '\n') {
//      ++symsNo;
//      /*
//       * double prob()
//       */
//      auto numSym = NUM[c];
//      // Inverted repeat
//      if (mdl.ir) {
//        auto r = ctxIR>>2;
//        for (u8 i=0; i!=ALPH_SZ; ++i)
//          aN[i] = container->query((IRMAGIC-i)<<shl | r);
//        ctxIR = REVNUM[c]<<shl | r;          // Update ctxIR
//      }
//      auto l = ctx<<2;
//      for (u8 i=0; i!=ALPH_SZ; ++i)
//        aN[i] += container->query(l | i);
//      ctx = (l & mask) | numSym;       // Update ctx
//
//
//
//      sEntr += log2((aN[0]+aN[1]+aN[2]+aN[3]+sAlpha) / (aN[numSym]+alpha));
//    }
//  }
//  tf.close();
//  double aveEntr = sEntr/symsNo;
//  cerr << "Average Entropy (H) = " << aveEntr << '\n';
//  cerr << "Compression finished ";
}

////#include <typeinfo>
//template <typename T, typename Y, typename U>
//inline void FCM::compressDS (const string& tar, const ModelPar& mdl, T mask,
//                             Y& aN,/*Y aN,*/ const U& container) const {
//  auto   shl    = mdl.k<<1;  // Shift left
//  T      ctx    = 0;         // Context(s) (integer) sliding through the dataset
//  T      ctxIR  = mask;      // Inverted repeat context (integer)
//  u64    symsNo = 0;         // No. syms in target file, except \n
//  float  alpha  = mdl.alpha;
//  double sAlpha = ALPH_SZ*alpha;  // Sum of alphas
//  double sEntr  = 0;         // Sum of entropies = sum( log_2 P(s|c^t) )
//  ifstream tf(tar);
//  char c;
////  if (typeid(container) == typeid(Table64*)) {
//  while (tf.get(c)) {
//    if (c != '\n') {
//      ++symsNo;
//      auto numSym = NUM[c];
//      // Inverted repeat
//      if (mdl.ir) {
//        auto r = ctxIR>>2;
//        for (u8 i=0; i!=ALPH_SZ; ++i)
//          aN[i] = container->query((IRMAGIC-i)<<shl | r);
////
////        std::thread thr[4];
////        thr[0] = std::thread(&Table64::query, container, std::ref(aN), 0, 3<<shl | r);
////        thr[1] = std::thread(&Table64::query, container, std::ref(aN), 1, 2<<shl | r);
////        thr[2] = std::thread(&Table64::query, container, std::ref(aN), 2, 1<<shl | r);
////        thr[3] = std::thread(&Table64::query, container, std::ref(aN), 3, 0<<shl | r);
////        thr[0].join();
////        thr[1].join();
////        thr[2].join();
////        thr[3].join();
////
////        aN[0] = container->query(3<<shl | r);    // A
////        aN[1] = container->query(2<<shl | r);    // C
////        aN[2] = container->query(1<<shl | r);    // G
////        aN[3] = container->query(r);             // T
//        ctxIR = REVNUM[c]<<shl | r;          // Update ctxIR
//      }
//      auto l = ctx<<2;
//      for (u8 i=0; i!=ALPH_SZ; ++i)
//        aN[i] += container->query(l | i);
////
////      aN[0] += container->query(l);        // A
////      aN[1] += container->query(l | 1);    // C
////      aN[2] += container->query(l | 2);    // G
////      aN[3] += container->query(l | 3);    // T
//      ctx = (l & mask) | numSym;       // Update ctx
//
//      sEntr += log2((aN[0]+aN[1]+aN[2]+aN[3]+sAlpha) / (aN[numSym]+alpha));
//    }
//  }
////  }
//  tf.close();
//  double aveEntr = sEntr/symsNo;
//  cerr << "Average Entropy (H) = " << aveEntr << '\n';
//  cerr << "Compression finished ";
//}

template <typename T>
void FCM::prob (T ds) const {
  cerr<<"hi";
//  ds.print();
}

template <typename T>
T FCM::dtStruct (u8 mode) const {
  if      (mode==MODE::TABLE_64)     return tbl64;
  else if (mode==MODE::TABLE_32)     return tbl32;
  else if (mode==MODE::LOG_TABLE_8)  return logtbl8;
  else                               return sketch4;
}