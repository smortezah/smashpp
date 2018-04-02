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
  setModels(p);
  allocModels();
  setModesComb();
  setIRsComb();
}

FCM::~FCM () {
  for (const auto& m : model) {
    if      (m.mode==MODE::TABLE_64)      delete tbl64;
    else if (m.mode==MODE::TABLE_32)      delete tbl32;
    else if (m.mode==MODE::LOG_TABLE_8)   delete logtbl8;
    else                                  delete sketch4;
  }
}

inline void FCM::setModels (const Param& p) {
  model.resize(LEVEL[p.level][0]);
  for (auto m=model.begin(); m!=model.end(); ++m) {
    auto i   = m-model.begin();
    m->ir    = LEVEL[p.level][5*i+1];
    m->k     = LEVEL[p.level][5*i+2];
    m->alpha = static_cast<float>(LEVEL[p.level][5*i+3])/100;
    m->w     = power(2, LEVEL[p.level][5*i+4]);
    m->d     = LEVEL[p.level][5*i+5];
    if      (m->k>K_MAX_LGTBL8)   m->mode = MODE::SKETCH_8;
    else if (m->k>K_MAX_TBL32)    m->mode = MODE::LOG_TABLE_8;
    else if (m->k>K_MAX_TBL64)    m->mode = MODE::TABLE_32;
    else                          m->mode = MODE::TABLE_64;
  }
}

inline void FCM::allocModels () {
  for (const auto& m : model) {
    if      (m.mode==MODE::TABLE_64)        tbl64 = new Table64(m.k);
    else if (m.mode==MODE::TABLE_32)        tbl32 = new Table32(m.k);
    else if (m.mode==MODE::LOG_TABLE_8)   logtbl8 = new LogTable8(m.k);
    else                                  sketch4 = new CMLS4(m.w, m.d);
  }
}

inline void FCM::setModesComb () {
  u8 isT64=0, isT32=0, isLT=0, isSk=0;
  for (const auto& m : model) {
    if      (m.mode==MODE::TABLE_64)      isT64 = 1;
    else if (m.mode==MODE::TABLE_32)      isT32 = 1<<1;
    else if (m.mode==MODE::LOG_TABLE_8)   isLT  = 1<<2;
    else if (m.mode==MODE::SKETCH_8)      isSk  = 1<<3;
  }
  MODE_COMB = isSk | isLT | isT32 | isT64;
}

inline void FCM::setIRsComb () {
  switch (model.size()) {
    case 1:   IR_COMB =                                   model[0].ir;  break;
    case 2:   IR_COMB =                  model[1].ir<<1 | model[0].ir;  break;
    case 3:   IR_COMB = model[2].ir<<2 | model[1].ir<<1 | model[0].ir;  break;
    case 4:   IR_COMB = model[3].ir<<3 |
                        model[2].ir<<2 | model[1].ir<<1 | model[0].ir;  break;
    default:  cerr<<"Error.\n";
  }
}

void FCM::buildModel (const Param& p) {
  cerr << "Building models...\n";
  for (const auto& m : model) {
    auto mask32 = static_cast<u32>((4<<(m.k<<1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
    auto mask64 = static_cast<u64>((4<<(m.k<<1))-1);
    switch (m.mode) {
      case MODE::TABLE_64:     createDS(p.ref, mask32, tbl64);    break;
      case MODE::TABLE_32:     createDS(p.ref, mask32, tbl32);    break;
      case MODE::LOG_TABLE_8:  createDS(p.ref, mask32, logtbl8);  break;
      case MODE::SKETCH_8:     createDS(p.ref, mask64, sketch4);  break;
      default:                 cerr << "Error.\n";
    }
  }
  
//  //todo. test multithreading
//  vector<thread> thrd;  thrd.resize(4);
//  for (const auto& m : model) {
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
      ctx = ((ctx<<2) & mask) | NUM[c];    // Update ctx
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

//void FCM::compress (const Param& p) const {
//  cerr << "Compressing...\n";
////  array<u64, 4> aN64{0};    // Array of number of elements
////  array<u32, 4> aN32{0};
////  for (const auto& m : model) {
////    auto mask32 = static_cast<u32>((1 << (m.k << 1))-1);  // 4<<2k - 1 = 4^(k+1) - 1
////    auto mask64 = static_cast<u64>((1 << (m.k << 1))-1);
//////    switch (m.mode) {
//////      case MODE::TABLE_64:    compressDS(p.tar, m, mask32, aN64, tbl64);  break;
//////      case MODE::TABLE_32:    compressDS(p.tar, m, mask32, aN64, tbl32);  break;
//////      case MODE::LOG_TABLE_8: compressDS(p.tar, m, mask32, aN64, logtbl8);break;
//////      case MODE::SKETCH_8:    compressDS(p.tar, m, mask64, aN32, sketch4);break;
//////      default:                cerr << "Error";
//////    }
////  }
////  if (model.size() == 1)
//
////  auto mask = 0;
////  if (typeid(sketch4)==typeid(CMLS4))
////    mask = const_cast<u64>((1 << (model[0].k << 1))-1);
////  else
////    mask = dynamic_cast<u32>((1 << (model[0].k << 1))-1);
////
////  cerr << typeid(mask).name();
//
//
////  compressDS1(p.tar);
////  cerr << "Compression finished ";
//}

void FCM::compress (const Param& p) const {
  cerr << "Compressing...\n";
  vector<u32> mask32;  mask32.resize(model.size());
  u64 mask64 = 0;
  for (const auto& m : model) {
    mask32.emplace_back(static_cast<u32>((1<<(m.k<<1))-1));//1<<2k-1=4^k-1
    if (m.mode==MODE::SKETCH_8)  mask64=static_cast<u64>((1<<(m.k<<1))-1);
  }
  
//  std::tuple<u32,u64> m (mask32[0],mask64);
//  std::tuple<Table64*,CMLS4*> t (tbl64,sketch4);
//  a(m,t);
  a(std::tuple<u32,u64>(mask32[0],mask64),
    std::tuple<Table64*,CMLS4*>(tbl64,sketch4));
  
  switch (MODE_COMB) {
//    case 1:   compDS1(p.tar, mask32[0], tbl64);                        break;
//    case 2:   compDS1(p.tar, mask32[0], tbl32);                        break;
//    case 4:   compDS1(p.tar, mask32[0], logtbl8);                      break;
//    case 8:   compDS1(p.tar, mask64,    sketch4);                      break;
//    case 3:   compDS2(p.tar, mask32[0], mask32[1], tbl64,   tbl32);    break;
//    case 5:   compDS2(p.tar, mask32[0], mask32[1], tbl64,   logtbl8);  break;
//    case 9:   compDS2(p.tar, mask32[0], mask64,    tbl64,   sketch4);  break;
//    case 6:   compDS2(p.tar, mask32[0], mask32[1], tbl32,   logtbl8);  break;
//    case 10:  compDS2(p.tar, mask32[0], mask64,    tbl32,   sketch4);  break;
//    case 12:  compDS2(p.tar, mask32[0], mask64,    logtbl8, sketch4);  break;
////    case 7:   compDS3(p.tar, mask32[0], mask32[1], mask32[3],
////                      tbl64, tbl32, logtbl8);                          break;
////    case 11:  compDS3(p.tar, mask32[0], mask32[1], mask64,
////                      tbl64, tbl32, sketch4);                          break;
////    case 13:  compDS3(p.tar, mask32[0], mask32[1], mask64,
////                      tbl64, logtbl8, sketch4);                        break;
////    case 14:  compDS3(p.tar, mask32[0], mask32[1], mask64,
////                      tbl32, logtbl8, sketch4);                        break;
////    case 15:  compressDS4(p.tar, tbl64, tbl32, logtbl8, sketch4);         break;
    default:  cerr << "Error";                                         break;
  }
  
//  cerr << "Compression finished ";
}

template <typename mask_t, typename ds_t>
inline void FCM::compDS1 (const string &tar, mask_t mask,
                          const ds_t &container) const {
////  double aveEnt = 0;
////  if (!model[0].ir)
////    aveEnt = aveEnt1D(tar, mask, aN, container);
////  else
////    aveEnt = aveEnt1I(tar, mask, aN, container);
////
  const auto shl{model[0].k<<1};    // Shift left
  mask_t ctx{0}, ctxIR{mask};       // Ctx, ir (int) sliding through the dataset
  u64 symsNo{0};                    // No. syms in target file, except \n
  const float alpha{model[0].alpha};
  const double sAlpha{ALPH_SZ*alpha}; // Sum of alphas
  double sEnt{0};                   // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);
  char c;
  if (IR_COMB==IR::DDDD) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[c];
        auto l = ctx<<2;
        auto z0=container->query(l);      auto z1=container->query(l | 1);
        auto z2=container->query(l | 2);  auto z3=container->query(l | 3);
        ctx = (l & mask) | numSym;    // Update ctx
        decltype(z0) z[4] {z0, z1, z2, z3};
        sEnt  += log2((z0+z1+z2+z3+sAlpha) / (z[numSym]+alpha));
      }
    }
  }
  else if (IR_COMB==IR::DDDI) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[c];
        auto l=ctx<<2;    auto r=ctxIR>>2;
        auto z0 = container->query(l)     + container->query(3<<shl | r);
        auto z1 = container->query(l | 1) + container->query(2<<shl | r);
        auto z2 = container->query(l | 2) + container->query(1<<shl | r);
        auto z3 = container->query(l | 3) + container->query(r);
        ctx = (l & mask) | numSym;     // Update ctx
        ctxIR = REVNUM[c]<<shl | r;    // Update ctxIR
        decltype(z0) z[4] {z0, z1, z2, z3};
        sEnt += log2((z0+z1+z2+z3+sAlpha) / (z[numSym]+alpha));

//        // Inverted repeat
//        auto r = ctxIR>>2;
//        for (u8 i=0; i!=ALPH_SZ; ++i)
//          aN[i] = container->query((IRMAGIC-i)<<shl | r);
//        ctxIR = REVNUM[c]<<shl | r;          // Update ctxIR
//
//        auto l = ctx<<2;
//        for (u8 i=0; i!=ALPH_SZ; ++i)
//          aN[i] += container->query(l | i);
//        auto numSym = NUM[c];
//        ctx = (l & mask) | numSym;       // Update ctx
//        sEnt += log2((aN[0]+aN[1]+aN[2]+aN[3]+sAlpha) / (aN[numSym]+alpha));
      }
    }
  }
  tf.close();
  double aveEnt = sEnt/symsNo;
  cerr << "Average Entropy (H) = " << aveEnt << " bps" << '\n';
  cerr << "Compression finished ";
}

//template <typename mask_t, typename cnt_t, typename ds_t>
//inline double FCM::aveEnt1D (const string &tar, mask_t mask, cnt_t &aN,
//                             const ds_t &container) const {
//  const auto shl{model[0].k<<1};    // Shift left
//  mask_t ctx{0}, ctxIR{mask};       // Ctx, ir (int) sliding through the dataset
//  u64 symsNo{0};                    // No. syms in target file, except \n
//  const float alpha{model[0].alpha};
//  const double sAlpha{ALPH_SZ*alpha};  // Sum of alphas
//  double sEnt{0};                  // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(tar);
//  char c;
//  while (tf.get(c)) {
//    if (c != '\n') {
//      ++symsNo;
//      auto l = ctx<<2;
//      for (u8 i=0; i!=ALPH_SZ; ++i)
//        aN[i] += container->query(l | i);
//      auto numSym = NUM[c];
//      ctx = (l & mask) | numSym;       // Update ctx
//
//      sEnt += log2((aN[0]+aN[1]+aN[2]+aN[3]+sAlpha) / (aN[numSym]+alpha));
//    }
//  }
//  tf.close();
//  return sEnt/symsNo;
//}
//
//template <typename mask_t, typename cnt_t, typename ds_t>
//inline double FCM::aveEnt1I (const string &tar, mask_t mask, cnt_t &aN,
//                             const ds_t &container) const {
//  const auto shl{model[0].k<<1};
//  mask_t ctx{0}, ctxIR{mask};
//  u64 symsNo{0};
//  const float alpha{model[0].alpha};
//  const double sAlpha{ALPH_SZ*alpha};
//  double sEnt{0};
//  ifstream tf(tar);
//  char c;
//  while (tf.get(c)) {
//    if (c != '\n') {
//      ++symsNo;
//      // Inverted repeat
//      auto r = ctxIR>>2;
//      for (u8 i=0; i!=ALPH_SZ; ++i)
//        aN[i] = container->query((IRMAGIC-i)<<shl | r);
//      ctxIR = REVNUM[c]<<shl | r;          // Update ctxIR
//
//      auto l = ctx<<2;
//      for (u8 i=0; i!=ALPH_SZ; ++i)
//        aN[i] += container->query(l | i);
//      auto numSym = NUM[c];
//      ctx = (l & mask) | numSym;       // Update ctx
//
//      sEnt += log2((aN[0]+aN[1]+aN[2]+aN[3]+sAlpha) / (aN[numSym]+alpha));
//    }
//  }
//  tf.close();
//  return sEnt/symsNo;
//}

template <typename mask0_t, typename mask1_t, typename ds0_t, typename ds1_t>
inline void FCM::a (const std::tuple<mask0_t,mask1_t> m,
                    const std::tuple<ds0_t,ds1_t>& t) const {
  auto container0 = std::get<0>(t);
  container0->print();
};



template <typename mask0_t, typename mask1_t, typename ds0_t, typename ds1_t>
inline void FCM::compDS2 (const string& tar, mask0_t mask0, mask1_t mask1,
                    const ds0_t& container0, const ds1_t& container1) const {
  const auto shl0{model[0].k<<1}, shl1{model[1].k<<1};   // Shift left
  mask0_t ctx0{0}, ctxIR0{mask0};   // Ctx, ir (int) sliding through the dataset
  mask1_t ctx1{0}, ctxIR1{mask1};
  u64 symsNo{0};               // No. syms in target file, except \n
  const float alpha0{model[0].alpha}, alpha1{model[1].alpha};
  const double sAlpha0{ALPH_SZ*alpha0}, sAlpha1{ALPH_SZ*alpha1};  // Sum of alphas
  double w0{0.5}, w1{0.5};
  double Pm0{}, Pm1{};  // P of model 0, model 1
  double sEnt{0}; // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);
  char c;
  if (IR_COMB==IR::DDDD) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[c];
        auto l0=ctx0<<2;    auto l1=ctx1<<2;
        auto z00 = container0->query(l0);    auto z01 = container0->query(l0|1);
        auto z02 = container0->query(l0|2);  auto z03 = container0->query(l0|3);
        auto z10 = container1->query(l1);    auto z11 = container1->query(l1|1);
        auto z12 = container1->query(l1|2);  auto z13 = container1->query(l1|3);
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        { decltype(z00) z[4] {z00, z01, z02, z03};
          Pm0 = (z[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0); }
        { decltype(z10) z[4] {z10, z11, z12, z13};
          Pm1 = (z[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1); }
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        w0 = rawW0 / (rawW0+rawW1);
        w1 = rawW1 / (rawW0+rawW1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
      }
    }
  }
  else if (IR_COMB==IR::DDDI) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[c];
        auto l0=ctx0<<2;    auto l1=ctx1<<2;    auto r0=ctxIR0>>2;
        auto z00 = container0->query(l0)     + container0->query(3<<shl0 | r0);
        auto z01 = container0->query(l0 | 1) + container0->query(2<<shl0 | r0);
        auto z02 = container0->query(l0 | 2) + container0->query(1<<shl0 | r0);
        auto z03 = container0->query(l0 | 3) + container0->query(r0);
        auto z10 = container1->query(l1);    auto z11 = container1->query(l1|1);
        auto z12 = container1->query(l1|2);  auto z13 = container1->query(l1|3);
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctxIR0 = REVNUM[c]<<shl0 | r0;    // Update ctxIR
        { decltype(z00) z[4] {z00, z01, z02, z03};
          Pm0 = (z[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0); }
        { decltype(z10) z[4] {z10, z11, z12, z13};
          Pm1 = (z[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1); }
//        decltype(z00) z0[4] {z00, z01, z02, z03};
//        decltype(z10) z1[4] {z10, z11, z12, z13};
//        Pm0 = (z0[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0);
//        Pm1 = (z1[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        w0 = rawW0 / (rawW0+rawW1);
        w1 = rawW1 / (rawW0+rawW1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
      }
    }
  }
  else if (IR_COMB==IR::DDID) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[c];
        auto l0=ctx0<<2;    auto l1=ctx1<<2;    auto r1=ctxIR1>>2;
        auto z00 = container0->query(l0);    auto z01 = container0->query(l0|1);
        auto z02 = container0->query(l0|2);  auto z03 = container0->query(l0|3);
        auto z10 = container1->query(l1)     + container1->query(3<<shl1 | r1);
        auto z11 = container1->query(l1 | 1) + container1->query(2<<shl1 | r1);
        auto z12 = container1->query(l1 | 2) + container1->query(1<<shl1 | r1);
        auto z13 = container1->query(l1 | 3) + container1->query(r1);
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctxIR1 = REVNUM[c]<<shl1 | r1;    // Update ctxIR
        { decltype(z00) z[4] {z00, z01, z02, z03};
          Pm0 = (z[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0); }
        { decltype(z10) z[4] {z10, z11, z12, z13};
          Pm1 = (z[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1); }
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        w0 = rawW0 / (rawW0+rawW1);
        w1 = rawW1 / (rawW0+rawW1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
      }
    }
  }
  else if (IR_COMB==IR::DDII) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[c];
        auto l0=ctx0<<2;  auto l1=ctx1<<2; auto r0=ctxIR0>>2; auto r1=ctxIR1>>2;
        auto z00 = container0->query(l0)     + container0->query(3<<shl0 | r0);
        auto z01 = container0->query(l0 | 1) + container0->query(2<<shl0 | r0);
        auto z02 = container0->query(l0 | 2) + container0->query(1<<shl0 | r0);
        auto z03 = container0->query(l0 | 3) + container0->query(r0);
        auto z10 = container1->query(l1)     + container1->query(3<<shl1 | r1);
        auto z11 = container1->query(l1 | 1) + container1->query(2<<shl1 | r1);
        auto z12 = container1->query(l1 | 2) + container1->query(1<<shl1 | r1);
        auto z13 = container1->query(l1 | 3) + container1->query(r1);
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctxIR0 = REVNUM[c]<<shl0 | r0;    // Update ctxIR
        ctxIR1 = REVNUM[c]<<shl1 | r1;
        { decltype(z00) z[4] {z00, z01, z02, z03};
          Pm0 = (z[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0); }
        { decltype(z10) z[4] {z10, z11, z12, z13};
          Pm1 = (z[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1); }
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        w0 = rawW0 / (rawW0+rawW1);
        w1 = rawW1 / (rawW0+rawW1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
      }
    }
  }
  tf.close();
  double aveEnt = sEnt/symsNo;
  cerr << "Average Entropy (H) = " << aveEnt << " bps" << '\n';
  cerr << "Compression finished ";
}

//template <typename mask0_t, typename mask1_t, typename mask2_t,
//  typename ds0_t, typename ds1_t, typename ds2_t>
//inline void FCM::compDS3 (const string& tar, mask0_t mask0, mask1_t mask1,
//                mask2_t mask2, const ds0_t& container0, const ds1_t& container1,
//                const ds2_t& container2) const {
//  const auto shl0{model[0].k<<1}, shl1{model[1].k<<1}, shl2{model[2].k<<1};
//  mask0_t ctx0{0}, ctxIR0{mask0};   // Ctx, ir (int) sliding through the dataset
//  mask1_t ctx1{0}, ctxIR1{mask1};
//  mask2_t ctx2{0}, ctxIR2{mask2};
//  u64 symsNo{0};               // No. syms in target file, except \n
//  const float alpha0{model[0].alpha}, alpha1{model[1].alpha},
//              alpha2{model[2].alpha};
//  const double sAlpha0{ALPH_SZ*alpha0}, sAlpha1{ALPH_SZ*alpha1},
//               sAlpha2{ALPH_SZ*alpha2};  // Sum of alphas
//  double w0{1/3}, w1{1/3}, w2{1/3};
//  double Pm0{}, Pm1{}, Pm2{};  // P of model 0, model 1, model 2
//  double sEnt{0}; // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(tar);
//  char c;
//  if (IR_COMB==IR::DDDD) {
//    while (tf.get(c)) {
//      if (c != '\n') {
//        ++symsNo;
//        auto numSym = NUM[c];
//        auto l0=ctx0<<2;  auto l1=ctx1<<2;  auto l2=ctx2<<2;
//        auto z00 = container0->query(l0);    auto z01 = container0->query(l0|1);
//        auto z02 = container0->query(l0|2);  auto z03 = container0->query(l0|3);
//        auto z10 = container1->query(l1);    auto z11 = container1->query(l1|1);
//        auto z12 = container1->query(l1|2);  auto z13 = container1->query(l1|3);
//        auto z20 = container2->query(l2);    auto z21 = container2->query(l2|1);
//        auto z22 = container2->query(l2|2);  auto z23 = container2->query(l2|3);
//        ctx0 = (l0 & mask0) | numSym;     // Update ctx
//        ctx1 = (l1 & mask1) | numSym;
//        ctx2 = (l2 & mask2) | numSym;
//        decltype(z00) z0[4] {z00, z01, z02, z03};
//        decltype(z10) z1[4] {z10, z11, z12, z13};
//        decltype(z20) z2[4] {z20, z21, z22, z23};
//        Pm0 = (z0[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0);
//        Pm1 = (z1[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1);
//        Pm2 = (z2[numSym]+alpha2) / (z20+z21+z22+z23+sAlpha2);
//        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
//        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
//        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
//        w0 = rawW0 / (rawW0+rawW1+rawW2);
//        w1 = rawW1 / (rawW0+rawW1+rawW2);
//        w2 = rawW2 / (rawW0+rawW1+rawW2);
//        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
//      }
//    }
//  }
//  else if (IR_COMB==IR::DDDI) {
//    while (tf.get(c)) {
//      if (c != '\n') {
//        ++symsNo;
//        auto numSym = NUM[c];
//        auto l0=ctx0<<2;  auto l1=ctx1<<2;  auto l2=ctx2<<2;  auto r0=ctxIR0>>2;
//        auto z00 = container0->query(l0)     + container0->query(3<<shl0 | r0);
//        auto z01 = container0->query(l0 | 1) + container0->query(2<<shl0 | r0);
//        auto z02 = container0->query(l0 | 2) + container0->query(1<<shl0 | r0);
//        auto z03 = container0->query(l0 | 3) + container0->query(r0);
//        auto z10 = container1->query(l1);    auto z11 = container1->query(l1|1);
//        auto z12 = container1->query(l1|2);  auto z13 = container1->query(l1|3);
//        auto z20 = container2->query(l2);    auto z21 = container2->query(l2|1);
//        auto z22 = container2->query(l2|2);  auto z23 = container2->query(l2|3);
//        ctx0 = (l0 & mask0) | numSym;    // Update ctx
//        ctx1 = (l1 & mask1) | numSym;
//        ctx2 = (l2 & mask2) | numSym;
//        ctxIR0 = REVNUM[c]<<shl0 | r0;      // Update ctxIR
//        decltype(z00) z0[4] {z00, z01, z02, z03};
//        decltype(z10) z1[4] {z10, z11, z12, z13};
//        decltype(z20) z2[4] {z20, z21, z22, z23};
//        Pm0 = (z0[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0);
//        Pm1 = (z1[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1);
//        Pm2 = (z2[numSym]+alpha2) / (z20+z21+z22+z23+sAlpha2);
//        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
//        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
//        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
//        w0 = rawW0 / (rawW0+rawW1+rawW2);
//        w1 = rawW1 / (rawW0+rawW1+rawW2);
//        w2 = rawW2 / (rawW0+rawW1+rawW2);
//        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
//      }
//    }
//  }
//  else if (IR_COMB==IR::DDID) {
//    while (tf.get(c)) {
//      if (c != '\n') {
//        ++symsNo;
//        auto numSym = NUM[c];
//        auto l0=ctx0<<2;  auto l1=ctx1<<2;  auto l2=ctx2<<2;  auto r1=ctxIR1>>2;
//        auto z00 = container0->query(l0);    auto z01 = container0->query(l0|1);
//        auto z02 = container0->query(l0|2);  auto z03 = container0->query(l0|3);
//        auto z10 = container1->query(l1)     + container1->query(3<<shl1 | r1);
//        auto z11 = container1->query(l1 | 1) + container1->query(2<<shl1 | r1);
//        auto z12 = container1->query(l1 | 2) + container1->query(1<<shl1 | r1);
//        auto z13 = container1->query(l1 | 3) + container1->query(r1);
//        auto z20 = container2->query(l2);    auto z21 = container2->query(l2|1);
//        auto z22 = container2->query(l2|2);  auto z23 = container2->query(l2|3);
//        ctx0 = (l0 & mask0) | numSym;    // Update ctx
//        ctx1 = (l1 & mask1) | numSym;
//        ctx2 = (l2 & mask2) | numSym;
//        ctxIR1 = REVNUM[c]<<shl1 | r1;      // Update ctxIR
//        decltype(z00) z0[4] {z00, z01, z02, z03};
//        decltype(z10) z1[4] {z10, z11, z12, z13};
//        decltype(z20) z2[4] {z20, z21, z22, z23};
//        Pm0 = (z0[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0);
//        Pm1 = (z1[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1);
//        Pm2 = (z2[numSym]+alpha2) / (z20+z21+z22+z23+sAlpha2);
//        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
//        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
//        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
//        w0 = rawW0 / (rawW0+rawW1+rawW2);
//        w1 = rawW1 / (rawW0+rawW1+rawW2);
//        w2 = rawW2 / (rawW0+rawW1+rawW2);
//        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
//      }
//    }
//  }
//  else if (IR_COMB==IR::DDII) {
//    while (tf.get(c)) {
//      if (c != '\n') {
//        ++symsNo;
//        auto numSym = NUM[c];
//        auto l0=ctx0<<2;    auto l1=ctx1<<2;    auto l2=ctx2<<2;
//        auto r0=ctxIR0>>2;  auto r1=ctxIR1>>2;
//        auto z00 = container0->query(l0)     + container0->query(3<<shl0 | r0);
//        auto z01 = container0->query(l0 | 1) + container0->query(2<<shl0 | r0);
//        auto z02 = container0->query(l0 | 2) + container0->query(1<<shl0 | r0);
//        auto z03 = container0->query(l0 | 3) + container0->query(r0);
//        auto z10 = container1->query(l1)     + container1->query(3<<shl1 | r1);
//        auto z11 = container1->query(l1 | 1) + container1->query(2<<shl1 | r1);
//        auto z12 = container1->query(l1 | 2) + container1->query(1<<shl1 | r1);
//        auto z13 = container1->query(l1 | 3) + container1->query(r1);
//        auto z20 = container2->query(l2);    auto z21 = container2->query(l2|1);
//        auto z22 = container2->query(l2|2);  auto z23 = container2->query(l2|3);
//        ctx0 = (l0 & mask0) | numSym;    // Update ctx
//        ctx1 = (l1 & mask1) | numSym;
//        ctx2 = (l2 & mask2) | numSym;
//        ctxIR0 = REVNUM[c]<<shl0 | r0;     // Update ctxIR
//        ctxIR1 = REVNUM[c]<<shl1 | r1;
//        decltype(z00) z0[4] {z00, z01, z02, z03};
//        decltype(z10) z1[4] {z10, z11, z12, z13};
//        decltype(z20) z2[4] {z20, z21, z22, z23};
//        Pm0 = (z0[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0);
//        Pm1 = (z1[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1);
//        Pm2 = (z2[numSym]+alpha2) / (z20+z21+z22+z23+sAlpha2);
//        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
//        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
//        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
//        w0 = rawW0 / (rawW0+rawW1+rawW2);
//        w1 = rawW1 / (rawW0+rawW1+rawW2);
//        w2 = rawW2 / (rawW0+rawW1+rawW2);
//        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
//      }
//    }
//  }
//  else if (IR_COMB==IR::DIDD) {
//    while (tf.get(c)) {
//      if (c != '\n') {
//        ++symsNo;
//        auto numSym = NUM[c];
//        auto l0=ctx0<<2;  auto l1=ctx1<<2;  auto l2=ctx2<<2;  auto r2=ctxIR2>>2;
//        auto z00 = container0->query(l0);    auto z01 = container0->query(l0|1);
//        auto z02 = container0->query(l0|2);  auto z03 = container0->query(l0|3);
//        auto z10 = container1->query(l1);    auto z11 = container1->query(l1|1);
//        auto z12 = container1->query(l1|2);  auto z13 = container1->query(l1|3);
//        auto z20 = container2->query(l2)     + container2->query(3<<shl2 | r2);
//        auto z21 = container2->query(l2 | 1) + container2->query(2<<shl2 | r2);
//        auto z22 = container2->query(l2 | 2) + container2->query(1<<shl2 | r2);
//        auto z23 = container2->query(l2 | 3) + container2->query(r2);
//        ctx0 = (l0 & mask0) | numSym;     // Update ctx
//        ctx1 = (l1 & mask1) | numSym;
//        ctx2 = (l2 & mask2) | numSym;
//        ctxIR2 = REVNUM[c]<<shl2 | r2;    // Update ctxIR
//        decltype(z00) z0[4] {z00, z01, z02, z03};
//        decltype(z10) z1[4] {z10, z11, z12, z13};
//        decltype(z20) z2[4] {z20, z21, z22, z23};
//        Pm0 = (z0[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0);
//        Pm1 = (z1[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1);
//        Pm2 = (z2[numSym]+alpha2) / (z20+z21+z22+z23+sAlpha2);
//        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
//        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
//        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
//        w0 = rawW0 / (rawW0+rawW1+rawW2);
//        w1 = rawW1 / (rawW0+rawW1+rawW2);
//        w2 = rawW2 / (rawW0+rawW1+rawW2);
//        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
//      }
//    }
//  }
//  else if (IR_COMB==IR::DIDI) {
//    while (tf.get(c)) {
//      if (c != '\n') {
//        ++symsNo;
//        auto numSym = NUM[c];
//        auto l0=ctx0<<2;    auto l1=ctx1<<2;    auto l2=ctx2<<2;
//        auto r0=ctxIR0>>2;  auto r2=ctxIR2>>2;
//        auto z00 = container0->query(l0)     + container0->query(3<<shl0 | r0);
//        auto z01 = container0->query(l0 | 1) + container0->query(2<<shl0 | r0);
//        auto z02 = container0->query(l0 | 2) + container0->query(1<<shl0 | r0);
//        auto z03 = container0->query(l0 | 3) + container0->query(r0);
//        auto z10 = container1->query(l1);    auto z11 = container1->query(l1|1);
//        auto z12 = container1->query(l1|2);  auto z13 = container1->query(l1|3);
//        auto z20 = container2->query(l2)     + container2->query(3<<shl2 | r2);
//        auto z21 = container2->query(l2 | 1) + container2->query(2<<shl2 | r2);
//        auto z22 = container2->query(l2 | 2) + container2->query(1<<shl2 | r2);
//        auto z23 = container2->query(l2 | 3) + container2->query(r2);
//        ctx0 = (l0 & mask0) | numSym;     // Update ctx
//        ctx1 = (l1 & mask1) | numSym;
//        ctx2 = (l2 & mask2) | numSym;
//        ctxIR0 = REVNUM[c]<<shl0 | r0;    // Update ctxIR
//        ctxIR2 = REVNUM[c]<<shl2 | r2;
//        decltype(z00) z0[4] {z00, z01, z02, z03};
//        decltype(z10) z1[4] {z10, z11, z12, z13};
//        decltype(z20) z2[4] {z20, z21, z22, z23};
//        Pm0 = (z0[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0);
//        Pm1 = (z1[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1);
//        Pm2 = (z2[numSym]+alpha2) / (z20+z21+z22+z23+sAlpha2);
//        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
//        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
//        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
//        w0 = rawW0 / (rawW0+rawW1+rawW2);
//        w1 = rawW1 / (rawW0+rawW1+rawW2);
//        w2 = rawW2 / (rawW0+rawW1+rawW2);
//        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
//      }
//    }
//  }
//  else if (IR_COMB==IR::DIID) {
//    while (tf.get(c)) {
//      if (c != '\n') {
//        ++symsNo;
//        auto numSym = NUM[c];
//        auto l0=ctx0<<2;    auto l1=ctx1<<2;    auto l2=ctx2<<2;
//        auto r1=ctxIR1>>2;  auto r2=ctxIR2>>2;
//        auto z00 = container0->query(l0);    auto z01 = container0->query(l0|1);
//        auto z02 = container0->query(l0|2);  auto z03 = container0->query(l0|3);
//        auto z10 = container1->query(l1)     + container1->query(3<<shl1 | r1);
//        auto z11 = container1->query(l1 | 1) + container1->query(2<<shl1 | r1);
//        auto z12 = container1->query(l1 | 2) + container1->query(1<<shl1 | r1);
//        auto z13 = container1->query(l1 | 3) + container1->query(r1);
//        auto z20 = container2->query(l2)     + container2->query(3<<shl2 | r2);
//        auto z21 = container2->query(l2 | 1) + container2->query(2<<shl2 | r2);
//        auto z22 = container2->query(l2 | 2) + container2->query(1<<shl2 | r2);
//        auto z23 = container2->query(l2 | 3) + container2->query(r2);
//        ctx0 = (l0 & mask0) | numSym;     // Update ctx
//        ctx1 = (l1 & mask1) | numSym;
//        ctx2 = (l2 & mask2) | numSym;
//        ctxIR1 = REVNUM[c]<<shl1 | r1;    // Update ctxIR
//        ctxIR2 = REVNUM[c]<<shl2 | r2;
//        decltype(z00) z0[4] {z00, z01, z02, z03};
//        decltype(z10) z1[4] {z10, z11, z12, z13};
//        decltype(z20) z2[4] {z20, z21, z22, z23};
//        Pm0 = (z0[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0);
//        Pm1 = (z1[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1);
//        Pm2 = (z2[numSym]+alpha2) / (z20+z21+z22+z23+sAlpha2);
//        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
//        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
//        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
//        w0 = rawW0 / (rawW0+rawW1+rawW2);
//        w1 = rawW1 / (rawW0+rawW1+rawW2);
//        w2 = rawW2 / (rawW0+rawW1+rawW2);
//        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
//      }
//    }
//  }
//  else if (IR_COMB==IR::DIII) {
//    while (tf.get(c)) {
//      if (c != '\n') {
//        ++symsNo;
//        auto numSym = NUM[c];
//        auto l0=ctx0<<2;    auto l1=ctx1<<2;    auto l2=ctx2<<2;
//        auto r0=ctxIR0>>2;  auto r1=ctxIR1>>2;  auto r2=ctxIR2>>2;
//        auto z00 = container0->query(l0)     + container0->query(3<<shl0 | r0);
//        auto z01 = container0->query(l0 | 1) + container0->query(2<<shl0 | r0);
//        auto z02 = container0->query(l0 | 2) + container0->query(1<<shl0 | r0);
//        auto z03 = container0->query(l0 | 3) + container0->query(r0);
//        auto z10 = container1->query(l1)     + container1->query(3<<shl1 | r1);
//        auto z11 = container1->query(l1 | 1) + container1->query(2<<shl1 | r1);
//        auto z12 = container1->query(l1 | 2) + container1->query(1<<shl1 | r1);
//        auto z13 = container1->query(l1 | 3) + container1->query(r1);
//        auto z20 = container2->query(l2)     + container2->query(3<<shl2 | r2);
//        auto z21 = container2->query(l2 | 1) + container2->query(2<<shl2 | r2);
//        auto z22 = container2->query(l2 | 2) + container2->query(1<<shl2 | r2);
//        auto z23 = container2->query(l2 | 3) + container2->query(r2);
//        ctx0 = (l0 & mask0) | numSym;     // Update ctx
//        ctx1 = (l1 & mask1) | numSym;
//        ctx2 = (l2 & mask2) | numSym;
//        ctxIR0 = REVNUM[c]<<shl0 | r0;    // Update ctxIR
//        ctxIR1 = REVNUM[c]<<shl1 | r1;
//        ctxIR2 = REVNUM[c]<<shl2 | r2;
//        decltype(z00) z0[4] {z00, z01, z02, z03};
//        decltype(z10) z1[4] {z10, z11, z12, z13};
//        decltype(z20) z2[4] {z20, z21, z22, z23};
//        Pm0 = (z0[numSym]+alpha0) / (z00+z01+z02+z03+sAlpha0);
//        Pm1 = (z1[numSym]+alpha1) / (z10+z11+z12+z13+sAlpha1);
//        Pm2 = (z2[numSym]+alpha2) / (z20+z21+z22+z23+sAlpha2);
//        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
//        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
//        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
//        w0 = rawW0 / (rawW0+rawW1+rawW2);
//        w1 = rawW1 / (rawW0+rawW1+rawW2);
//        w2 = rawW2 / (rawW0+rawW1+rawW2);
//        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
//      }
//    }
//  }
//  tf.close();
//  double aveEnt = sEnt/symsNo;
//  cerr << "Average Entropy (H) = " << aveEnt << " bps" << '\n';
//  cerr << "Compression finished ";
//}