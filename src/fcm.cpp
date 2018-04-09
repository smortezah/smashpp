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
//  sketch4->print();
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
inline void FCM::compDS1 (const string& tar, mask_t mask, const ds_t& ds) const{
  mask_t ctx{0}, ctxIr{mask};       // Ctx, ir (int) sliding through the dataset
  u64 symsNo{0};                    // No. syms in target file, except \n
  double sEnt{0};                   // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  Prob_s<mask_t> pObj {model[0].alpha, mask, static_cast<u8>(model[0].k<<1)};
  if (IR_COMB==IR::DDDD) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj.numSym=NUM[static_cast<u8>(c)];    pObj.l=ctx<<2;
        sEnt += log2(probR(ds, pObj));
        ctx   = updateCtx(pObj);    // Update ctx
      }
    }
  }
  else if (IR_COMB==IR::DDDI) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj.numSym=NUM[static_cast<u8>(c)];            pObj.l=ctx<<2;
        pObj.revNumSym = REVNUM[static_cast<u8>(c)];    pObj.r=ctxIr>>2;
        sEnt += log2(probIrR(ds, pObj));
        ctx   = updateCtx(pObj);      // Update ctx
        ctxIr = updateCtxIr(pObj);    // Update ctxIr
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
  mask0_t ctx0{0}, ctxIr0{mask0};   // Ctx, ir (int) sliding through the dataset
  mask1_t ctx1{0}, ctxIr1{mask1};
  u64 symsNo{0};                    // No. syms in target file, except \n
  double w0{0.5}, w1{0.5};
  double Pm0{}, Pm1{};              // P of model 0, model 1
  double sEnt{0};                   // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  Prob_s<mask0_t> pObj0 {model[0].alpha, mask0, static_cast<u8>(model[0].k<<1)};
  Prob_s<mask1_t> pObj1 {model[1].alpha, mask1, static_cast<u8>(model[1].k<<1)};
  if (IR_COMB==IR::DDDD) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = NUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2;         pObj1.l=ctx1<<2;
        Pm0=prob(ds0, pObj0);    Pm1=prob(ds1, pObj1);
        setWeight(w0, w1, Pm0, Pm1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
//        print(w0,w1,log2(1/(Pm0*w0 + Pm1*w1)));//todo
        ctx0=updateCtx(pObj0);   ctx1=updateCtx(pObj1);  // Update ctx
      }
    }
  }
  else if (IR_COMB==IR::DDDI) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = NUM[static_cast<u8>(c)];
        pObj0.revNumSym = REVNUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2;    pObj0.r=ctxIr0>>2;    pObj1.l=ctx1<<2;
        Pm0=probIr(ds0, pObj0);    Pm1=prob(ds1, pObj1);
        setWeight(w0, w1, Pm0, Pm1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
        ctx0=updateCtx(pObj0); ctxIr0=updateCtxIr(pObj0); ctx1=updateCtx(pObj1);
      }
    }
  }
  else if (IR_COMB==IR::DDID) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = NUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2;    pObj1.l=ctx1<<2;    pObj1.r=ctxIr1>>2;
        pObj1.revNumSym = REVNUM[static_cast<u8>(c)];
        Pm0=prob(ds0, pObj0);    Pm1=probIr(ds1, pObj1);
        setWeight(w0, w1, Pm0, Pm1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
        ctx0=updateCtx(pObj0); ctx1=updateCtx(pObj1); ctxIr1=updateCtxIr(pObj1);
      }
    }
  }
  else if (IR_COMB==IR::DDII) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = NUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2; pObj0.r=ctxIr0>>2; pObj1.l=ctx1<<2; pObj1.r=ctxIr1>>2;
        pObj0.revNumSym = pObj1.revNumSym = REVNUM[static_cast<u8>(c)];
        Pm0=probIr(ds0, pObj0);   Pm1=probIr(ds1, pObj1);
        setWeight(w0, w1, Pm0, Pm1);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1));
        ctx0=updateCtx(pObj0);    ctxIr0=updateCtxIr(pObj0);
        ctx1=updateCtx(pObj1);    ctxIr1=updateCtxIr(pObj1);
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
  mask0_t ctx0{0}, ctxIr0{mask0};   // Ctx, ir (int) sliding through the dataset
  mask1_t ctx1{0}, ctxIr1{mask1};
  mask2_t ctx2{0}, ctxIr2{mask2};
  u64 symsNo{0};               // No. syms in target file, except \n
  double w0{1.0/3}, w1{w0}, w2{w0};
  double Pm0{}, Pm1{}, Pm2{};  // P of model 0, model 1, model 2
  double sEnt{0}; // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  Prob_s<mask0_t> pObj0 {model[0].alpha, mask0, static_cast<u8>(model[0].k<<1)};
  Prob_s<mask1_t> pObj1 {model[1].alpha, mask1, static_cast<u8>(model[1].k<<1)};
  Prob_s<mask2_t> pObj2 {model[2].alpha, mask2, static_cast<u8>(model[2].k<<1)};
  if (IR_COMB==IR::DDDD) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = pObj2.numSym = NUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2;    pObj1.l=ctx1<<2;    pObj2.l=ctx2<<2;
        Pm0=prob(ds0, pObj0);    Pm1=prob(ds1, pObj1);    Pm2=prob(ds2, pObj2);
        setWeight(w0, w1, w2, Pm0, Pm1, Pm2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0=updateCtx(pObj0);   ctx1=updateCtx(pObj1);   ctx2=updateCtx(pObj2);
      }
    }
  }
  else if (IR_COMB==IR::DDDI) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = pObj2.numSym = NUM[static_cast<u8>(c)];
        pObj0.revNumSym = REVNUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2;  pObj0.r=ctxIr0>>2;  pObj1.l=ctx1<<2;  pObj2.l=ctx2<<2;
        Pm0=probIr(ds0, pObj0);   Pm1=prob(ds1, pObj1);   Pm2=prob(ds2, pObj2);
        setWeight(w0, w1, w2, Pm0, Pm1, Pm2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0=updateCtx(pObj0);    ctxIr0=updateCtxIr(pObj0);
        ctx1=updateCtx(pObj1);    ctx2=updateCtx(pObj2);
      }
    }
  }
  else if (IR_COMB==IR::DDID) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = pObj2.numSym = NUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2;    pObj1.l=ctx1<<2;    pObj1.r=ctxIr1>>2;
        pObj1.revNumSym = REVNUM[static_cast<u8>(c)];    pObj2.l=ctx2<<2;
        Pm0=prob(ds0, pObj0);   Pm1=probIr(ds1, pObj1);   Pm2=prob(ds2, pObj2);
        setWeight(w0, w1, w2, Pm0, Pm1, Pm2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0=updateCtx(pObj0); ctx1=updateCtx(pObj1); ctxIr1=updateCtxIr(pObj1);
        ctx2=updateCtx(pObj2);
      }
    }
  }
  else if (IR_COMB==IR::DDII) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = pObj2.numSym = NUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2; pObj0.r=ctxIr0>>2; pObj1.l=ctx1<<2; pObj1.r=ctxIr1>>2;
        pObj0.revNumSym = pObj1.revNumSym = REVNUM[static_cast<u8>(c)];
        pObj2.l=ctx2<<2;
        Pm0=probIr(ds0, pObj0);  Pm1=probIr(ds1, pObj1);  Pm2=prob(ds2, pObj2);
        setWeight(w0, w1, w2, Pm0, Pm1, Pm2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0=updateCtx(pObj0); ctxIr0=updateCtxIr(pObj0);
        ctx1=updateCtx(pObj1); ctxIr1=updateCtxIr(pObj1); ctx2=updateCtx(pObj2);
      }
    }
  }
  else if (IR_COMB==IR::DIDD) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = pObj2.numSym = NUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2;  pObj1.l=ctx1<<2;  pObj2.l=ctx2<<2;  pObj2.r=ctxIr2>>2;
        pObj2.revNumSym = REVNUM[static_cast<u8>(c)];
        Pm0=prob(ds0, pObj0);  Pm1=prob(ds1, pObj1);  Pm2=probIr(ds2, pObj2);
        setWeight(w0, w1, w2, Pm0, Pm1, Pm2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0=updateCtx(pObj0);    ctx1=updateCtx(pObj1);
        ctx2=updateCtx(pObj2);    ctxIr2=updateCtxIr(pObj2);
      }
    }
  }
  else if (IR_COMB==IR::DIDI) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = pObj2.numSym = NUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2;  pObj0.r=ctxIr0>>2;  pObj1.l=ctx1<<2;
        pObj2.l=ctx2<<2;  pObj2.r=ctxIr2>>2;
        pObj0.revNumSym = pObj2.revNumSym = REVNUM[static_cast<u8>(c)];
        Pm0=probIr(ds0, pObj0);  Pm1=prob(ds1, pObj1);  Pm2=probIr(ds2, pObj2);
        setWeight(w0, w1, w2, Pm0, Pm1, Pm2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0=updateCtx(pObj0); ctxIr0=updateCtxIr(pObj0); ctx1=updateCtx(pObj1);
        ctx2=updateCtx(pObj2); ctxIr2=updateCtxIr(pObj2);
      }
    }
  }
  else if (IR_COMB==IR::DIID) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = pObj2.numSym = NUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2;  pObj1.l=ctx1<<2;    pObj1.r=ctxIr1>>2;
        pObj2.l=ctx2<<2;  pObj2.r=ctxIr2>>2;
        pObj1.revNumSym = pObj2.revNumSym = REVNUM[static_cast<u8>(c)];
        Pm0=prob(ds0, pObj0);  Pm1=probIr(ds1, pObj1);  Pm2=probIr(ds2, pObj2);
        setWeight(w0, w1, w2, Pm0, Pm1, Pm2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0=updateCtx(pObj0); ctx1=updateCtx(pObj1); ctxIr1=updateCtxIr(pObj1);
        ctx2=updateCtx(pObj2); ctxIr2=updateCtxIr(pObj2);
      }
    }
  }
  else if (IR_COMB==IR::DIII) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pObj0.numSym = pObj1.numSym = pObj2.numSym = NUM[static_cast<u8>(c)];
        pObj0.l=ctx0<<2; pObj0.r=ctxIr0>>2; pObj1.l=ctx1<<2; pObj1.r=ctxIr1>>2;
        pObj2.l=ctx2<<2; pObj2.r=ctxIr2>>2;
        pObj0.revNumSym = pObj1.revNumSym = pObj2.revNumSym =
          REVNUM[static_cast<u8>(c)];
        Pm0=probIr(ds0, pObj0); Pm1=probIr(ds1, pObj1); Pm2=probIr(ds2, pObj2);
        setWeight(w0, w1, w2, Pm0, Pm1, Pm2);
        sEnt += log2(1/(Pm0*w0 + Pm1*w1 + Pm2*w2));
        ctx0=updateCtx(pObj0);    ctxIr0=updateCtxIr(pObj0);
        ctx1=updateCtx(pObj1);    ctxIr1=updateCtxIr(pObj1);
        ctx2=updateCtx(pObj2);    ctxIr2=updateCtxIr(pObj2);
      }
    }
  }
  tf.close();
  double aveEnt = sEnt/symsNo;
  cerr << "Average Entropy (H) = " << aveEnt << " bps" << '\n';
  cerr << "Compression finished ";
}

template <typename ds_t, typename ctx_t>
inline double FCM::prob (const ds_t& ds, const Prob_s<ctx_t>& p) const {
  const decltype(ds->query(0)) c[4]
    {ds->query(p.l), ds->query(p.l|1), ds->query(p.l|2), ds->query(p.l|3)};
  return (c[p.numSym]+p.alpha) /
         (c[0]+c[1]+c[2]+c[3]+static_cast<double>(ALPH_SZ*p.alpha));
}

template <typename ds_t, typename ctx_t>
inline double FCM::probR (const ds_t& ds, const Prob_s<ctx_t>& p) const {
  const decltype(ds->query(0)) c[4]
    {ds->query(p.l), ds->query(p.l|1), ds->query(p.l|2), ds->query(p.l|3)};
  return (c[0]+c[1]+c[2]+c[3]+static_cast<double>(ALPH_SZ*p.alpha))
         / (c[p.numSym]+p.alpha);
}

template <typename ds_t, typename ctx_t>
inline double FCM::probIr (const ds_t& ds, const Prob_s<ctx_t>& p) const {
  const decltype(ds->query(0)) c[4]
    {ds->query(p.l)  +ds->query((3<<p.shl)|p.r),
     ds->query(p.l|1)+ds->query((2<<p.shl)|p.r),
     ds->query(p.l|2)+ds->query((1<<p.shl)|p.r),
     ds->query(p.l|3)+ds->query(p.r)};
  return (c[p.numSym]+p.alpha)
         / (c[0]+c[1]+c[2]+c[3]+static_cast<double>(ALPH_SZ*p.alpha));
}

template <typename ds_t, typename ctx_t>
inline double FCM::probIrR (const ds_t& ds, const Prob_s<ctx_t>& p) const {
  const decltype(ds->query(0)) c[4]
    {ds->query(p.l)  +ds->query((3<<p.shl)|p.r),
     ds->query(p.l|1)+ds->query((2<<p.shl)|p.r),
     ds->query(p.l|2)+ds->query((1<<p.shl)|p.r),
     ds->query(p.l|3)+ds->query(p.r)};
  return (c[0]+c[1]+c[2]+c[3]+static_cast<double>(ALPH_SZ*p.alpha))
         / (c[p.numSym]+p.alpha);
}

inline void FCM::setWeight (double& w0, double& w1,
                            double Pm0, double Pm1) const{
  const auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
  const auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
  w0 = rawW0 / (rawW0+rawW1);
  w1 = rawW1 / (rawW0+rawW1);
}

inline void FCM::setWeight (double& w0, double& w1, double& w2,
                            double Pm0, double Pm1, double Pm2) const {
  const auto rawW0 = pow(w0, DEF_GAMMA) * Pm0;
  const auto rawW1 = pow(w1, DEF_GAMMA) * Pm1;
  const auto rawW2 = pow(w2, DEF_GAMMA) * Pm2;
  const auto sum   = rawW0 + rawW1 + rawW2;
  w0 = rawW0 / sum;
  w1 = rawW1 / sum;
  w2 = rawW2 / sum;
}

template <typename ctx_t>
inline ctx_t FCM::updateCtx (const Prob_s<ctx_t>& p) const {
  return (p.l & p.mask) | p.numSym;
}

template <typename ctx_t>
inline ctx_t FCM::updateCtxIr (const Prob_s<ctx_t>& p) const {
  return (p.revNumSym<<p.shl) | p.r;
}

template <typename T>
inline void FCM::print (T in) const { cerr<<in<<'\n'; }
template <typename T, typename... Args>
inline void FCM::print (T in, Args... args) const {
  cerr << in << '\t';
  print(args...);
}