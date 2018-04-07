//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include <cmath>
#include <thread>
#include <future>//todo
#include <numeric>//todo
#include <functional>//todo
#include "fcm.hpp"
using std::ifstream;
using std::cout;
using std::array;

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
    else if (m.mode==MODE::SKETCH_8)      delete sketch4;
  }
}

inline void FCM::setModels (const Param& p) {
  model.resize(LEVEL[p.level][0]);
  for (auto m=model.begin(); m!=model.end(); ++m) {
    auto i   = m - model.begin();
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
  cerr << "Building the model" << (model.size()==1 ? "" : "s") << "...\n";
  if (p.nthr==1 || model.size()==1)  bldMdlOneThr(p);    // No multithread
  else                               bldMdlMulThr(p);    // Multithread
  cerr << "The model" << (model.size()==1 ? "" : "s") << " built ";
  
  //todo
//  tbl64->print();
////  tbl32->print();
  sketch4->print();
}

inline void FCM::bldMdlOneThr (const Param &p) {
  for (const auto& m : model) {
    if (m.mode == MODE::TABLE_64) {
      auto mask32 = static_cast<u32>((4<<(m.k<<1)) - 1);  // 4<<2k-1 = 4^(k+1)-1
      createDS(p.ref, mask32, tbl64);
    }
    else if (m.mode == MODE::TABLE_32) {
      auto mask32 = static_cast<u32>((4<<(m.k<<1)) - 1);
      createDS(p.ref, mask32, tbl32);
    }
    else if (m.mode == MODE::LOG_TABLE_8) {
      auto mask32 = static_cast<u32>((4<<(m.k<<1)) - 1);
      createDS(p.ref, mask32, logtbl8);
    }
    else if (m.mode == MODE::SKETCH_8) {
      auto mask64 = static_cast<u64>((4<<(m.k<<1)) - 1);
      createDS(p.ref, mask64, sketch4);
    }
    else
      cerr << "Error.\n";
  }
}

inline void FCM::bldMdlMulThr (const Param& p) {
  const auto vThrSz = (p.nthr < model.size()) ? p.nthr : model.size();
  vector<std::thread> thrd;
  thrd.resize(vThrSz);
  for (u8 i=0; i!=model.size(); ++i) {
    if (model[i].mode == MODE::TABLE_64) {
      auto mask32 = static_cast<u32>((4<<(model[i].k<<1))-1);//4<<2k-1=4^(k+1)-1
      thrd[i % vThrSz] = std::thread(&FCM::createDS<u32,Table64*>, this,
                                     std::cref(p.ref), mask32, std::ref(tbl64));
    }
    else if (model[i].mode == MODE::TABLE_32) {
      auto mask32 = static_cast<u32>((4<<(model[i].k<<1)) - 1);
      thrd[i % vThrSz] = std::thread(&FCM::createDS<u32,Table32*>, this,
                                     std::cref(p.ref), mask32, std::ref(tbl32));
    }
    else if (model[i].mode == MODE::LOG_TABLE_8) {
      auto mask32 = static_cast<u32>((4<<(model[i].k<<1)) - 1);
      thrd[i % vThrSz] = std::thread(&FCM::createDS<u32,LogTable8*>, this,
                                   std::cref(p.ref), mask32, std::ref(logtbl8));
    }
    else if (model[i].mode == MODE::SKETCH_8) {
      auto mask64 = static_cast<u64>((4<<(model[i].k<<1)) - 1);
      thrd[i % vThrSz] = std::thread(&FCM::createDS<u64,CMLS4*>, this,
                                   std::cref(p.ref), mask64, std::ref(sketch4));
    }
    // Join
    if ((i+1) % vThrSz == 0)
      for (auto& t : thrd)  if (t.joinable()) t.join();
  }
  for (auto& t : thrd)  if (t.joinable()) t.join();  // Join leftover threads
}

template <typename mask_t, typename ds_t>
inline void FCM::createDS (const string& ref, mask_t mask, ds_t& container) {
  ifstream rf(ref);
  char c;
  for (mask_t ctx=0; rf.get(c);) {
    if (c != '\n') {
      ctx = ((ctx<<2) & mask) | NUM[static_cast<u8>(c)];    // Update ctx
//      ctx = ((ctx<<2) & mask) | NUM[c];    // Update ctx//todo
      container->update(ctx);
    }
  }
  rf.close();
}

void FCM::compress (const Param& p) const {
  cerr << "Compressing...\n";
  vector<u32> mask32;  mask32.resize(model.size());
  u64 mask64 = 0;
  for (const auto& m : model) {
    mask32.emplace_back(static_cast<u32>((1<<(m.k<<1)) - 1));  // 1<<2k-1=4^k-1
    if (m.mode==MODE::SKETCH_8)  mask64=static_cast<u64>((1<<(m.k<<1)) - 1);
  }
  switch (MODE_COMB) {
    case 1:   compDS1(p.tar, mask32[0], tbl64);                        break;
    case 2:   compDS1(p.tar, mask32[0], tbl32);                        break;
    case 4:   compDS1(p.tar, mask32[0], logtbl8);                      break;
    case 8:   compDS1(p.tar, mask64,    sketch4);                      break;
    case 3:   compDS2(p.tar, mask32[0], mask32[1], tbl64,   tbl32);    break;
    case 5:   compDS2(p.tar, mask32[0], mask32[1], tbl64,   logtbl8);  break;
    case 9:   compDS2(p.tar, mask32[0], mask64,    tbl64,   sketch4);  break;
    case 6:   compDS2(p.tar, mask32[0], mask32[1], tbl32,   logtbl8);  break;
    case 10:  compDS2(p.tar, mask32[0], mask64,    tbl32,   sketch4);  break;
    case 12:  compDS2(p.tar, mask32[0], mask64,    logtbl8, sketch4);  break;
    case 7:   compDS3(p.tar, mask32[0], mask32[1], mask32[3],
                      tbl64, tbl32, logtbl8);                          break;
    case 11:  compDS3(p.tar, mask32[0], mask32[1], mask64,
                      tbl64, tbl32, sketch4);                          break;
    case 13:  compDS3(p.tar, mask32[0], mask32[1], mask64,
                      tbl64, logtbl8, sketch4);                        break;
    case 14:  compDS3(p.tar, mask32[0], mask32[1], mask64,
                      tbl32, logtbl8, sketch4);                        break;
//    case 15:  compressDS4(p.tar, tbl64, tbl32, logtbl8, sketch4);      break;
    default:  cerr << "Error";                                         break;
  }
  
//  cerr << "Compression finished ";
}

template <typename mask_t, typename ds_t>
inline void FCM::compDS1 (const string& tar, mask_t mask,
//                          const
ds_t& ds) const{
  const auto  shl    {model[0].k<<1};    // Shift left
  mask_t      ctx    {0},              ctxIR{mask};       // Ctx, ir (int) sliding through the dataset
  u64         symsNo {0};                    // No. syms in target file, except \n
  const float alpha  {model[0].alpha};
//  const double sAlpha{ALPH_SZ*alpha}; // Sum of alphas
  double      sEnt   {0};                   // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream    tf     (tar);
  char        c;
  
  
  //todo
//  auto func = std::bind(&CMLS4::query, ds, std::placeholders::_1);
//  auto fut = std::async(std::launch::async, func, 1);

//  auto fut = std::async(std::launch::async, &CMLS4::query, ds, 1);
//  cerr<< fut.get();      // waits for is_prime to return
  
  if (IR_COMB==IR::DDDD) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l = ctx<<2;
        sEnt += log2(probR(ds, l, alpha, numSym));
        ctx = (l & mask) | numSym;    // Update ctx
      }
    }
  }
  else if (IR_COMB==IR::DDDI) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l=ctx<<2;    auto r=ctxIR>>2;
        sEnt += log2(probIrR(ds, l, r, shl, alpha, numSym));
        ctx   = (l & mask) | numSym;                    // Update ctx
        ctxIR = REVNUM[static_cast<u8>(c)]<<shl | r;    // Update ctxIR
      }
    }
  }
  tf.close();
  double aveEnt = sEnt/symsNo;
  cerr << "Average Entropy (H) = " << aveEnt << " bps" << '\n';
  cerr << "Compression finished ";
}

template <typename mask0_t, typename mask1_t, typename ds0_t, typename ds1_t>
inline void FCM::compDS2 (const string& tar, mask0_t mask0, mask1_t mask1,
                          const ds0_t& ds0, const ds1_t& ds1) const {
  const auto shl0{model[0].k<<1}, shl1{model[1].k<<1};   // Shift left
  mask0_t ctx0{0}, ctxIR0{mask0};   // Ctx, ir (int) sliding through the dataset
  mask1_t ctx1{0}, ctxIR1{mask1};
  u64 symsNo{0};               // No. syms in target file, except \n
  const float alpha0{model[0].alpha}, alpha1{model[1].alpha};
//  const double sAlpha0{ALPH_SZ*alpha0}, sAlpha1{ALPH_SZ*alpha1};  // Sum of alphas
  double w0{0.5}, w1{0.5};
  double Pm0{}, Pm1{};  // P of model 0, model 1
  double sEnt{0}; // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);
  char c;
  if (IR_COMB==IR::DDDD) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2;    auto l1=ctx1<<2;
        Pm0 = prob(ds0, l0, alpha0, numSym);
        Pm1 = prob(ds1, l1, alpha1, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        w0 = rawW0 / (rawW0+rawW1);
        w1 = rawW1 / (rawW0+rawW1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
//        print(w0,w1,log2(1/(Pm0*w0 + Pm1*w1)));//todo
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
      }
    }
  }
  else if (IR_COMB==IR::DDDI) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2;    auto r0=ctxIR0>>2;    auto l1=ctx1<<2;
        Pm0 = probIr(ds0, l0, r0, shl0, alpha0, numSym);
        Pm1 = prob(ds1, l1, alpha1, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        w0 = rawW0 / (rawW0+rawW1);
        w1 = rawW1 / (rawW0+rawW1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctxIR0 = REVNUM[static_cast<u8>(c)]<<shl0 | r0;    // Update ctxIR
      }
    }
  }
  else if (IR_COMB==IR::DDID) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2;    auto l1=ctx1<<2;    auto r1=ctxIR1>>2;
        Pm0 = prob(ds0, l0, alpha0, numSym);
        Pm1 = probIr(ds1, l1, r1, shl1, alpha1, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        w0 = rawW0 / (rawW0+rawW1);
        w1 = rawW1 / (rawW0+rawW1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctxIR1 = REVNUM[static_cast<u8>(c)]<<shl1 | r1;    // Update ctxIR
      }
    }
  }
  else if (IR_COMB==IR::DDII) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2; auto r0=ctxIR0>>2;  auto l1=ctx1<<2; auto r1=ctxIR1>>2;
        Pm0 = probIr(ds0, l0, r0, shl0, alpha0, numSym);
        Pm1 = probIr(ds1, l1, r1, shl1, alpha1, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        w0 = rawW0 / (rawW0+rawW1);
        w1 = rawW1 / (rawW0+rawW1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctxIR0 = REVNUM[static_cast<u8>(c)]<<shl0 | r0;    // Update ctxIR
        ctxIR1 = REVNUM[static_cast<u8>(c)]<<shl1 | r1;
      }
    }
  }
  tf.close();
  double aveEnt = sEnt/symsNo;
  cerr << "Average Entropy (H) = " << aveEnt << " bps" << '\n';
  cerr << "Compression finished ";
}

template <typename mask0_t, typename mask1_t, typename mask2_t,
  typename ds0_t, typename ds1_t, typename ds2_t>
inline void FCM::compDS3 (const string& tar, mask0_t mask0, mask1_t mask1,
    mask2_t mask2, const ds0_t& ds0, const ds1_t& ds1, const ds2_t& ds2) const {
  const auto shl0{model[0].k<<1}, shl1{model[1].k<<1}, shl2{model[2].k<<1};
  mask0_t ctx0{0}, ctxIR0{mask0};   // Ctx, ir (int) sliding through the dataset
  mask1_t ctx1{0}, ctxIR1{mask1};
  mask2_t ctx2{0}, ctxIR2{mask2};
  u64 symsNo{0};               // No. syms in target file, except \n
  const float alpha0{model[0].alpha}, alpha1{model[1].alpha},
              alpha2{model[2].alpha};
//  const double sAlpha0{ALPH_SZ*alpha0}, sAlpha1{ALPH_SZ*alpha1},
//               sAlpha2{ALPH_SZ*alpha2};  // Sum of alphas
  double w0{1.0/3}, w1{w0}, w2{w0};
  double Pm0{}, Pm1{}, Pm2{};  // P of model 0, model 1, model 2
  double sEnt{0}; // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);
  char c;
  if (IR_COMB==IR::DDDD) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2;  auto l1=ctx1<<2;  auto l2=ctx2<<2;
        Pm0 = prob(ds0, l0, alpha0, numSym);
        Pm1 = prob(ds1, l1, alpha1, numSym);
        Pm2 = prob(ds2, l2, alpha2, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
        w0 = rawW0 / (rawW0+rawW1+rawW2);
        w1 = rawW1 / (rawW0+rawW1+rawW2);
        w2 = rawW2 / (rawW0+rawW1+rawW2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctx2 = (l2 & mask2) | numSym;
      }
    }
  }
  else if (IR_COMB==IR::DDDI) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2;  auto r0=ctxIR0>>2;  auto l1=ctx1<<2;  auto l2=ctx2<<2;
        Pm0 = probIr(ds0, l0, r0, shl0, alpha0, numSym);
        Pm1 = prob(ds1, l1, alpha1, numSym);
        Pm2 = prob(ds2, l2, alpha2, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
        w0 = rawW0 / (rawW0+rawW1+rawW2);
        w1 = rawW1 / (rawW0+rawW1+rawW2);
        w2 = rawW2 / (rawW0+rawW1+rawW2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0 = (l0 & mask0) | numSym;    // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctx2 = (l2 & mask2) | numSym;
        ctxIR0 = REVNUM[static_cast<u8>(c)]<<shl0 | r0;      // Update ctxIR
      }
    }
  }
  else if (IR_COMB==IR::DDID) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2;  auto l1=ctx1<<2;  auto r1=ctxIR1>>2;  auto l2=ctx2<<2;
        Pm0 = prob(ds0, l0, alpha0, numSym);
        Pm1 = probIr(ds1, l1, r1, shl1, alpha1, numSym);
        Pm2 = prob(ds2, l2, alpha2, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
        w0 = rawW0 / (rawW0+rawW1+rawW2);
        w1 = rawW1 / (rawW0+rawW1+rawW2);
        w2 = rawW2 / (rawW0+rawW1+rawW2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0 = (l0 & mask0) | numSym;    // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctx2 = (l2 & mask2) | numSym;
        ctxIR1 = REVNUM[static_cast<u8>(c)]<<shl1 | r1;      // Update ctxIR
      }
    }
  }
  else if (IR_COMB==IR::DDII) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2;  auto r0=ctxIR0>>2;
        auto l1=ctx1<<2;  auto r1=ctxIR1>>2;  auto l2=ctx2<<2;
        Pm0 = probIr(ds0, l0, r0, shl0, alpha0, numSym);
        Pm1 = probIr(ds1, l1, r1, shl1, alpha1, numSym);
        Pm2 = prob(ds2, l2, alpha2, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
        w0 = rawW0 / (rawW0+rawW1+rawW2);
        w1 = rawW1 / (rawW0+rawW1+rawW2);
        w2 = rawW2 / (rawW0+rawW1+rawW2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0 = (l0 & mask0) | numSym;    // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctx2 = (l2 & mask2) | numSym;
        ctxIR0 = REVNUM[static_cast<u8>(c)]<<shl0 | r0;     // Update ctxIR
        ctxIR1 = REVNUM[static_cast<u8>(c)]<<shl1 | r1;
      }
    }
  }
  else if (IR_COMB==IR::DIDD) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2;  auto l1=ctx1<<2;  auto l2=ctx2<<2;  auto r2=ctxIR2>>2;
        Pm0 = prob(ds0, l0, alpha0, numSym);
        Pm1 = prob(ds1, l1, alpha1, numSym);
        Pm2 = probIr(ds2, l2, r2, shl2, alpha2, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
        w0 = rawW0 / (rawW0+rawW1+rawW2);
        w1 = rawW1 / (rawW0+rawW1+rawW2);
        w2 = rawW2 / (rawW0+rawW1+rawW2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctx2 = (l2 & mask2) | numSym;
        ctxIR2 = REVNUM[static_cast<u8>(c)]<<shl2 | r2;    // Update ctxIR
      }
    }
  }
  else if (IR_COMB==IR::DIDI) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2;  auto r0=ctxIR0>>2;  auto l1=ctx1<<2;
        auto l2=ctx2<<2;  auto r2=ctxIR2>>2;
        Pm0 = probIr(ds0, l0, r0, shl0, alpha0, numSym);
        Pm1 = prob(ds1, l1, alpha1, numSym);
        Pm2 = probIr(ds2, l2, r2, shl2, alpha2, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
        w0 = rawW0 / (rawW0+rawW1+rawW2);
        w1 = rawW1 / (rawW0+rawW1+rawW2);
        w2 = rawW2 / (rawW0+rawW1+rawW2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctx2 = (l2 & mask2) | numSym;
        ctxIR0 = REVNUM[static_cast<u8>(c)]<<shl0 | r0;    // Update ctxIR
        ctxIR2 = REVNUM[static_cast<u8>(c)]<<shl2 | r2;
      }
    }
  }
  else if (IR_COMB==IR::DIID) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2;  auto l1=ctx1<<2;    auto r1=ctxIR1>>2;
        auto l2=ctx2<<2;  auto r2=ctxIR2>>2;
        Pm0 = prob(ds0, l0, alpha0, numSym);
        Pm1 = probIr(ds1, l1, r1, shl1, alpha1, numSym);
        Pm2 = probIr(ds2, l2, r2, shl2, alpha2, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
        w0 = rawW0 / (rawW0+rawW1+rawW2);
        w1 = rawW1 / (rawW0+rawW1+rawW2);
        w2 = rawW2 / (rawW0+rawW1+rawW2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctx2 = (l2 & mask2) | numSym;
        ctxIR1 = REVNUM[static_cast<u8>(c)]<<shl1 | r1;    // Update ctxIR
        ctxIR2 = REVNUM[static_cast<u8>(c)]<<shl2 | r2;
      }
    }
  }
  else if (IR_COMB==IR::DIII) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        auto numSym = NUM[static_cast<u8>(c)];
        auto l0=ctx0<<2; auto r0=ctxIR0>>2;  auto l1=ctx1<<2; auto r1=ctxIR1>>2;
        auto l2=ctx2<<2; auto r2=ctxIR2>>2;
        Pm0 = probIr(ds0, l0, r0, shl0, alpha0, numSym);
        Pm1 = probIr(ds1, l1, r1, shl1, alpha1, numSym);
        Pm2 = probIr(ds2, l2, r2, shl2, alpha2, numSym);
        auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
        auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
        auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
        w0 = rawW0 / (rawW0+rawW1+rawW2);
        w1 = rawW1 / (rawW0+rawW1+rawW2);
        w2 = rawW2 / (rawW0+rawW1+rawW2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0 = (l0 & mask0) | numSym;     // Update ctx
        ctx1 = (l1 & mask1) | numSym;
        ctx2 = (l2 & mask2) | numSym;
        ctxIR0 = REVNUM[static_cast<u8>(c)]<<shl0 | r0;    // Update ctxIR
        ctxIR1 = REVNUM[static_cast<u8>(c)]<<shl1 | r1;
        ctxIR2 = REVNUM[static_cast<u8>(c)]<<shl2 | r2;
      }
    }
  }
  tf.close();
  double aveEnt = sEnt/symsNo;
  cerr << "Average Entropy (H) = " << aveEnt << " bps" << '\n';
  cerr << "Compression finished ";
}

template <typename ds_t, typename ctx_t>
inline double FCM::prob (const ds_t& ds, ctx_t l, float a, u8 numSym) const {
  const auto c0 {ds->query(l)};
  const auto c1 {ds->query(l|1)};
  const auto c2 {ds->query(l|2)};
  const auto c3 {ds->query(l|3)};
  const decltype(c0) c[4] {c0, c1, c2, c3};
  return (c[numSym]+a) / (c0+c1+c2+c3+static_cast<double>(ALPH_SZ*a));
}

template <typename ds_t, typename ctx_t>
inline double FCM::probR (const ds_t& ds, ctx_t l, float a, u8 numSym) const {
  //todo. sAlpha ro be onvane input bede be in function ke vase har character
  //todo. nakhad hesab kone
//        vector<std::thread> t;
//if (typeid(ds)==typeid(CMLS4*)){
//  t.push_back(std::thread(&CMLS4::query, ds, l));
//}
//std::thread t0(&Table64::query, ds, l);
//std::thread t1(&Table32::query, ds, l);
//std::thread t2(&LogTable8::query, ds, l);
//std::thread t3(&CMLS4::query, ds, l);
//  t0.join();

//std::thread thrd[1];
//thrd[0] = std::thread(&FCM::createDS<u32,Table64*>, this,
//                      std::cref(p.ref), mask32, std::ref(tbl64));
//            thrd[0].join();

//  auto q=std::async(m, 6);
//  cerr<<q.get();

//  std::future<bool> fut = std::async(&FCM::is_prime, *this, 3);
//  bool ret = fut.get();      // waits for is_prime to return
//  if (ret) std::cout << "It is prime!\n";
//  else std::cout << "It is not prime.\n";

  
  const auto c0 {ds->query(l)};
  const auto c1 {ds->query(l|1)};
  const auto c2 {ds->query(l|2)};
  const auto c3 {ds->query(l|3)};
  const decltype(c0) c[4] {c0, c1, c2, c3};
  return (c0+c1+c2+c3+static_cast<double>(ALPH_SZ*a)) / (c[numSym]+a);
}

template <typename ds_t, typename ctxL_t, typename ctxR_t, typename shift_t>
inline double FCM::probIr (const ds_t& ds, ctxL_t l, ctxR_t r, shift_t shl,
                           float a, u8 numSym) const {
  const auto c0 {ds->query(l)  +ds->query((3<<shl)|r)};
  const auto c1 {ds->query(l|1)+ds->query((2<<shl)|r)};
  const auto c2 {ds->query(l|2)+ds->query((1<<shl)|r)};
  const auto c3 {ds->query(l|3)+ds->query(r)};
  const decltype(c0) c[4] {c0, c1, c2, c3};
  return (c[numSym]+a) / (c0+c1+c2+c3+static_cast<double>(ALPH_SZ*a));
}

template <typename ds_t, typename ctxL_t, typename ctxR_t, typename shift_t>
inline double FCM::probIrR (const ds_t& ds, ctxL_t l, ctxR_t r, shift_t shl,
                            float a, u8 numSym) const {
  const auto c0 {ds->query(l)  +ds->query((3<<shl)|r)};
  const auto c1 {ds->query(l|1)+ds->query((2<<shl)|r)};
  const auto c2 {ds->query(l|2)+ds->query((1<<shl)|r)};
  const auto c3 {ds->query(l|3)+ds->query(r)};
  const decltype(c0) c[4] {c0, c1, c2, c3};
  return (c0+c1+c2+c3+static_cast<double>(ALPH_SZ*a)) / (c[numSym]+a);
}

template <typename T>
inline void FCM::print (T in) const { cerr<<in<<'\n'; }
template <typename T, typename... Args>
inline void FCM::print (T in, Args... args) const {
  cerr << in << '\t';
  print(args...);
}