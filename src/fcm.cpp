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
using std::fstream;
using std::cout;
using std::array;
using std::initializer_list;

FCM::FCM (const Param& p) : aveEnt(0.0) {
  setModels(p);
  allocModels();
  setModesComb();
  setIRsComb();
}

FCM::~FCM () {
  for (const auto& m : model) {
    switch (m.mode){
      case MODE::TABLE_64:      delete tbl64;     break;
      case MODE::TABLE_32:      delete tbl32;     break;
      case MODE::LOG_TABLE_8:   delete logtbl8;   break;
      case MODE::SKETCH_8:      delete sketch4;   break;
      default:  cerr << "Error: undefined mode " << m.mode << ".\n";
    }
  }
}

inline void FCM::setModels (const Param& p) {
  vector<string> vMdlsPars;
  split(p.modelsPars.begin(), p.modelsPars.end(), ':', vMdlsPars);
  // Set each models parameters
  for (const auto& mp : vMdlsPars) {
    vector<string> vMPar;
    split(mp.begin(), mp.end(), ',', vMPar);
    if (vMPar.size() == 3)
      model.emplace_back(
        ModelPar(static_cast<u8>(stoi(vMPar[0])),
                 static_cast<u8>(stoi(vMPar[1])), stof(vMPar[2])));
    else if (vMPar.size() == 5)
      model.emplace_back(
        ModelPar(static_cast<u8>(stoi(vMPar[0])),
                 static_cast<u8>(stoi(vMPar[1])), stof(vMPar[2]),
                 pow2(stoull(vMPar[3])), static_cast<u8>(stoi(vMPar[4]))));
  }
  // Set modes. & is MANDATORY, since we set 'mode'.
  for (auto& m : model) {
    if      (m.k > K_MAX_LGTBL8)    m.mode = MODE::SKETCH_8;
    else if (m.k > K_MAX_TBL32)     m.mode = MODE::LOG_TABLE_8;
    else if (m.k > K_MAX_TBL64)     m.mode = MODE::TABLE_32;
    else                            m.mode = MODE::TABLE_64;
  }
  // Models MUST be sorted by 'k'=ctx size
  std::sort(model.begin(), model.end(),
            [](const auto& lhs, const auto& rhs){ return lhs.k < rhs.k; });
}

template <typename inIter_t, typename vec_t/*=vector<string>*/>
inline void FCM::split (inIter_t first, inIter_t last, char delim,
                        vec_t& container) const {
  while (true) {
    inIter_t found = std::find(first, last, delim);
    container.emplace_back(string(first,found));
    if (found == last)
      break;
    first = ++found;
  }
}

inline void FCM::allocModels () {
  for (const auto& m : model) {
    switch (m.mode){
      case MODE::TABLE_64:      tbl64   = new Table64(m.k);      break;
      case MODE::TABLE_32:      tbl32   = new Table32(m.k);      break;
      case MODE::LOG_TABLE_8:   logtbl8 = new LogTable8(m.k);    break;
      case MODE::SKETCH_8:      sketch4 = new CMLS4(m.w, m.d);   break;
      default:  cerr << "Error: undefined mode " << m.mode << ".\n";
    }
  }
}

inline void FCM::setModesComb () {  // Models MUST be sorted by 'k'=ctx size
  MODE_COMB = 0;
  for (const auto& m : model)
    MODE_COMB |= 1u<<m.mode;
}

inline void FCM::setIRsComb () {    // Models MUST be sorted by 'k'=ctx size
  IR_COMB = 0;
  for (u8 i=0; i!=model.size(); ++i)
    IR_COMB |= model[i].ir<<i;
}

void FCM::buildModel (const Param& p) {
  if (p.verbose)
    cerr << "Building " <<model.size()<< " model"<< (model.size()==1 ? "" : "s")
         << " based on the reference \"" << p.ref << "\"";
  else
    cerr << "Building the model" << (model.size()==1 ? "" : "s");
  cerr << " (level " << static_cast<u16>(p.level) << ")...\n";
  (p.nthr==1 || model.size()==1) ? bldMdlOneThr(p) : bldMdlMulThr(p)/*Mul thr*/;
  cerr << "Finished";
  
  
  //todo
  sketch4->print();
}

inline void FCM::bldMdlOneThr (const Param &p) {
  for (const auto& m : model) {  // Mask: 4<<2k-1 = 4^(k+1)-1
    if (m.mode == MODE::TABLE_64)
      createDS(p.ref, (4ul<<(m.k<<1))-1 /*Mask 32*/, tbl64);  // ul is MANDATORY
    else if (m.mode == MODE::TABLE_32)
      createDS(p.ref, (4ul<<(m.k<<1))-1 /*Mask 32*/, tbl32);
    else if (m.mode == MODE::LOG_TABLE_8)
      createDS(p.ref, (4ul<<(m.k<<1))-1 /*Mask 32*/, logtbl8);
    else if (m.mode == MODE::SKETCH_8)
      createDS(p.ref, (4ull<<(m.k<<1))-1/*Mask 64*/, sketch4);
    else
      cerr << "Error: the model cannot be built.\n";
  }
}

inline void FCM::bldMdlMulThr (const Param& p) {
  const auto vThrSz = (p.nthr < model.size()) ? p.nthr : model.size();
  vector<std::thread> thrd;  thrd.resize(vThrSz);
  for (u8 i=0; i!=model.size(); ++i) {  // Mask: 4<<2k-1 = 4^(k+1)-1
    if (model[i].mode == MODE::TABLE_64)
      thrd[i % vThrSz] =
        std::thread(&FCM::createDS<u32,Table64*>, this, std::cref(p.ref),
                    (4ul<<(model[i].k<<1))-1, std::ref(tbl64));
    else if (model[i].mode == MODE::TABLE_32)
      thrd[i % vThrSz] =
        std::thread(&FCM::createDS<u32,Table32*>, this, std::cref(p.ref),
                    (4ul<<(model[i].k<<1))-1, std::ref(tbl32));
    else if (model[i].mode == MODE::LOG_TABLE_8)
      thrd[i % vThrSz] =
        std::thread(&FCM::createDS<u32,LogTable8*>, this, std::cref(p.ref),
                    (4ul<<(model[i].k<<1))-1, std::ref(logtbl8));
    else if (model[i].mode == MODE::SKETCH_8)
      thrd[i % vThrSz] =
        std::thread(&FCM::createDS<u64,CMLS4*>, this, std::cref(p.ref),
                    (4ull<<(model[i].k<<1))-1, std::ref(sketch4));
    // Join
    if ((i+1) % vThrSz == 0)
      for (auto& t : thrd)  if (t.joinable()) t.join();
  }
  for (auto& t : thrd)  if (t.joinable()) t.join();  // Join leftover threads
}

template <typename msk_t, typename ds_t>
inline void FCM::createDS (const string& ref, msk_t mask, ds_t& ds) {
  ifstream rf(ref);  char c;
  for (msk_t ctx=0; rf.get(c);) {
    if (c != '\n') {
      ctx = ((ctx<<2) & mask) | NUM[static_cast<u8>(c)];    // Update ctx
      ds->update(ctx);
    }
  }
  rf.close();
}

void FCM::compress (const Param& p) {
  if (p.verbose)
    cerr << "Compressing the target \"" << p.tar << "\"...\n";
  else
    cerr << "Compressing...\n";
  vector<u32> mask32 {};
  u64         mask64 {};
  if (MODE_COMB != 15) {
    for (const auto &m : model) {
      if (m.mode == MODE::SKETCH_8)
        mask64 = (1ull<<(m.k<<1)) - 1;
      else
        mask32.emplace_back((1ul<<(m.k<<1)) - 1);  // 1<<2k-1=4^k-1
    }
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
    case 15:  compDS4(p.tar);                                          break;
    default:  cerr << "Error: the models cannot be built.";            break;
  }
  cerr << "Finished";
}

template <typename msk_t, typename ds_t>
inline void FCM::compDS1 (const string& tar, msk_t mask, const ds_t& ds) {
  msk_t ctx{0}, ctxIr{mask};    // Ctx, ir (int) sliding through the dataset
  u64 symsNo{0};                // No. syms in target file, except \n
  double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  Prob_s<msk_t> pObj {model[0].alpha, mask, static_cast<u8>(model[0].k<<1)};
  if (IR_COMB == IR::DDDD) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        pObj.config(c, ctx);
        sEnt += log2(probR(ds, pObj));
        updateCtx(ctx, pObj);
      }
  }
  else if (IR_COMB == IR::DDDI) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        pObj.config(c, ctx, ctxIr);
        sEnt += log2(probIrR(ds, pObj));
        updateCtx(ctx, ctxIr, pObj);    // Update ctx & ctxIr
      }
  }
  tf.close();
  aveEnt = sEnt/symsNo;
}

template <typename msk0_t, typename msk1_t, typename ds0_t, typename ds1_t>
inline void FCM::compDS2 (const string& tar, msk0_t mask0, msk1_t mask1,
                          const ds0_t& ds0, const ds1_t& ds1) {
  msk0_t ctx0{0}, ctxIr0{mask0};    // Ctx, ir (int) sliding through the dataset
  msk1_t ctx1{0}, ctxIr1{mask1};
  u64 symsNo {0};                   // No. syms in target file, except \n
  array<double,2> w {0.5, 0.5};
  double sEnt {0};                  // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  Prob_s<msk0_t> ps0 {model[0].alpha, mask0, static_cast<u8>(model[0].k<<1)};
  Prob_s<msk1_t> ps1 {model[1].alpha, mask1, static_cast<u8>(model[1].k<<1)};
  if (IR_COMB == IR::DDDD) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c, ctx0);     ps1.config(c, ctx1);
        sEnt += entropy<2>(w, {prob(ds0,ps0), prob(ds1,ps1)});
//        print(w[0],w[1],log2(1/(Pm[0]*w[0] + Pm[1]*w[1])));//todo
        updateCtx(ctx0, ps0);    updateCtx(ctx1, ps1);  // Update ctx
      }
  }
  else if (IR_COMB == IR::DDDI) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c, ctx0, ctxIr0);    ps1.config(c, ctx1);
        sEnt += entropy<2>(w, {probIr(ds0,ps0), prob(ds1,ps1)});
        updateCtx(ctx0, ctxIr0, ps0);   updateCtx(ctx1, ps1);
      }
  }
  else if (IR_COMB == IR::DDID) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c, ctx0);    ps1.config(c, ctx1, ctxIr1);
        sEnt += entropy<2>(w, {prob(ds0,ps0), probIr(ds1,ps1)});
        updateCtx(ctx0, ps0);   updateCtx(ctx1, ctxIr1, ps1);
      }
  }
  else if (IR_COMB == IR::DDII) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c, ctx0, ctxIr0);    ps1.config(c, ctx1, ctxIr1);
        sEnt += entropy<2>(w, {probIr(ds0,ps0), probIr(ds1,ps1)});
        updateCtx(ctx0, ctxIr0, ps0);   updateCtx(ctx1, ctxIr1, ps1);
      }
  }
  tf.close();
  aveEnt = sEnt/symsNo;
}

template <typename msk0_t, typename msk1_t, typename msk2_t,
  typename ds0_t, typename ds1_t, typename ds2_t>
inline void FCM::compDS3 (const string& tar, msk0_t mask0, msk1_t mask1,
     msk2_t mask2, const ds0_t& ds0, const ds1_t& ds1, const ds2_t& ds2) {
  msk0_t ctx0{0}, ctxIr0{mask0};    // Ctx, ir (int) sliding through the dataset
  msk1_t ctx1{0}, ctxIr1{mask1};
  msk2_t ctx2{0}, ctxIr2{mask2};
  u64 symsNo {0};                   // No. syms in target file, except \n
  array<double,3> w {1.0/3, 1.0/3, 1.0/3};
  double sEnt {0};                  // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  Prob_s<msk0_t> ps0 {model[0].alpha, mask0, static_cast<u8>(model[0].k<<1)};
  Prob_s<msk1_t> ps1 {model[1].alpha, mask1, static_cast<u8>(model[1].k<<1)};
  Prob_s<msk2_t> ps2 {model[2].alpha, mask2, static_cast<u8>(model[2].k<<1)};
  if (IR_COMB == IR::DDDD) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c,ctx0);     ps1.config(c,ctx1);     ps2.config(c,ctx2);
        sEnt += entropy<3>(w, {prob(ds0,ps0), prob(ds1,ps1), prob(ds2,ps2)});
        updateCtx(ctx0,ps0);    updateCtx(ctx1,ps1);    updateCtx(ctx2,ps2);
      }
  }
  else if (IR_COMB == IR::DDDI) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c,ctx0,ctxIr0);   ps1.config(c,ctx1);   ps2.config(c,ctx2);
        sEnt += entropy<3>(w, {probIr(ds0,ps0), prob(ds1,ps1), prob(ds2,ps2)});
        updateCtx(ctx0,ctxIr0,ps0);  updateCtx(ctx1,ps1);  updateCtx(ctx2,ps2);
      }
  }
  else if (IR_COMB == IR::DDID) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c,ctx0);   ps1.config(c,ctx1,ctxIr1);   ps2.config(c,ctx2);
        sEnt += entropy<3>(w, {prob(ds0,ps0), probIr(ds1,ps1), prob(ds2,ps2)});
        updateCtx(ctx0,ps0);  updateCtx(ctx1,ctxIr1,ps1);  updateCtx(ctx2,ps2);
      }
  }
  else if (IR_COMB == IR::DDII) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c,ctx0,ctxIr0);   ps1.config(c,ctx1,ctxIr1);
        ps2.config(c,ctx2);
        sEnt += entropy<3>(w,{probIr(ds0,ps0), probIr(ds1,ps1), prob(ds2,ps2)});
        updateCtx(ctx0,ctxIr0,ps0);  updateCtx(ctx1,ctxIr1,ps1);
        updateCtx(ctx2,ps2);
      }
  }
  else if (IR_COMB == IR::DIDD) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c,ctx0);   ps1.config(c,ctx1);   ps2.config(c,ctx2,ctxIr2);
        sEnt += entropy<3>(w, {prob(ds0,ps0), prob(ds1,ps1), probIr(ds2,ps2)});
        updateCtx(ctx0,ps0);  updateCtx(ctx1,ps1);  updateCtx(ctx2,ctxIr2,ps2);
      }
  }
  else if (IR_COMB == IR::DIDI) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c,ctx0,ctxIr0);   ps1.config(c,ctx1);
        ps2.config(c,ctx2,ctxIr2);
        sEnt += entropy<3>(w,{probIr(ds0,ps0), prob(ds1,ps1), probIr(ds2,ps2)});
        updateCtx(ctx0,ctxIr0,ps0);  updateCtx(ctx1,ps1);
        updateCtx(ctx2,ctxIr2,ps2);
      }
  }
  else if (IR_COMB == IR::DIID) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c,ctx0);   ps1.config(c,ctx1,ctxIr1);
        ps2.config(c,ctx2,ctxIr2);
        sEnt += entropy<3>(w,{prob(ds0,ps0), probIr(ds1,ps1), probIr(ds2,ps2)});
        updateCtx(ctx0,ps0);  updateCtx(ctx1,ctxIr1,ps1);
        updateCtx(ctx2,ctxIr2,ps2);
      }
  }
  else if (IR_COMB == IR::DIII) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        ps0.config(c,ctx0,ctxIr0);   ps1.config(c,ctx1,ctxIr1);
        ps2.config(c,ctx2,ctxIr2);
        sEnt += entropy<3>(w, {probIr(ds0,ps0), probIr(ds1,ps1),
                               probIr(ds2,ps2)});
        updateCtx(ctx0,ctxIr0,ps0);  updateCtx(ctx1,ctxIr1,ps1);
        updateCtx(ctx2,ctxIr2,ps2);
      }
  }
  tf.close();
  aveEnt = sEnt/symsNo;
}

inline void FCM::compDS4 (const string& tar) {
  vector<u32> mask32 {};
  u64         mask64 {};
  for (const auto &m : model) {
    if (m.mode == MODE::SKETCH_8)
      mask64 = (1ull<<(m.k<<1)) - 1;
    else
      mask32.emplace_back((1ul<<(m.k<<1)) - 1);  // 1<<2k-1=4^k-1
  }
  u32 ctx0{0}, ctxIr0{mask32[0]};   // Ctx, ir (int) sliding through the dataset
  u32 ctx1{0}, ctxIr1{mask32[1]};
  u32 ctx2{0}, ctxIr2{mask32[2]};
  u64 ctx3{0}, ctxIr3{mask64};
  const auto ds0=tbl64;      const auto ds1=tbl32;//todo.auto* ds0=.akharesh delete
  const auto ds2=logtbl8;    const auto ds3=sketch4;
  u64 symsNo {0};                   // No. syms in target file, except \n
  array<double,4> w {0.25, 0.25, 0.25, 0.25};
  double sEnt {0};                  // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  Prob_s<u32> ps0 {model[0].alpha, mask32[0], static_cast<u8>(model[0].k<<1)};
  Prob_s<u32> ps1 {model[1].alpha, mask32[1], static_cast<u8>(model[1].k<<1)};
  Prob_s<u32> ps2 {model[2].alpha, mask32[2], static_cast<u8>(model[2].k<<1)};
  Prob_s<u64> ps3 {model[3].alpha, mask64,    static_cast<u8>(model[3].k<<1)};
  if (IR_COMB == IR::DDDD) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        cerr<<ctx0<<' '<<ctx1<<' '<<ctx2<<' '<<ctx3<<'\n';//todo
        ps0.config(c,ctx0);    ps1.config(c,ctx1);
        ps2.config(c,ctx2);    ps3.config(c,ctx3);
        sEnt += entropy<4>(w,
          {prob(ds0,ps0), prob(ds1,ps1), prob(ds2,ps2), prob(ds3,ps3)});
        updateCtx(ctx0,ps0);   updateCtx(ctx1,ps1);
        updateCtx(ctx2,ps2);   updateCtx(ctx3,ps3);
      }
  }
//  else if (IR_COMB == IR::DDDI) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);    ps1.config(c,ctx1);
//        ps2.config(c,ctx2);           ps3.config(c,ctx3);
//        sEnt += entropy<4>(w,
//          {probIr(ds0,ps0), prob(ds1,ps1), prob(ds2,ps2), prob(ds3,ps3)});
//        updateCtx(ctx0,ctxIr0,ps0);   updateCtx(ctx1,ps1);
//        updateCtx(ctx2,ps2);          updateCtx(ctx3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::DDID) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);    ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2);    ps3.config(c,ctx3);
//        sEnt += entropy<4>(w,
//          {prob(ds0,ps0), probIr(ds1,ps1), prob(ds2,ps2), prob(ds3,ps3)});
//        updateCtx(ctx0,ps0);   updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ps2);   updateCtx(ctx3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::DDII) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);    ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2);           ps3.config(c,ctx3);
//        sEnt += entropy<4>(w,
//          {probIr(ds0,ps0), probIr(ds1,ps1), prob(ds2,ps2), prob(ds3,ps3)});
//        updateCtx(ctx0,ctxIr0,ps0);   updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ps2);          updateCtx(ctx3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::DIDD) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);           ps1.config(c,ctx1);
//        ps2.config(c,ctx2,ctxIr2);    ps3.config(c,ctx3);
//        sEnt += entropy<4>(w,
//          {prob(ds0,ps0), prob(ds1,ps1), probIr(ds2,ps2), prob(ds3,ps3)});
//        updateCtx(ctx0,ps0);          updateCtx(ctx1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);   updateCtx(ctx3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::DIDI) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);    ps1.config(c,ctx1);
//        ps2.config(c,ctx2,ctxIr2);    ps3.config(c,ctx3);
//        sEnt += entropy<4>(w,
//          {probIr(ds0,ps0), prob(ds1,ps1), probIr(ds2,ps2), prob(ds3,ps3)});
//        updateCtx(ctx0,ctxIr0,ps0);   updateCtx(ctx1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);   updateCtx(ctx3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::DIID) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);           ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2,ctxIr2);    ps3.config(c,ctx3);
//        sEnt += entropy<4>(w,
//          {prob(ds0,ps0), probIr(ds1,ps1), probIr(ds2,ps2), prob(ds3,ps3)});
//        updateCtx(ctx0,ps0);          updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);   updateCtx(ctx3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::DIII) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);    ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2,ctxIr2);    ps3.config(c,ctx3);
//        sEnt += entropy<4>(w,
//          {probIr(ds0,ps0), probIr(ds1,ps1), probIr(ds2,ps2), prob(ds3,ps3)});
//        updateCtx(ctx0,ctxIr0,ps0);   updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);   updateCtx(ctx3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::IDDD) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);    ps1.config(c,ctx1);
//        ps2.config(c,ctx2);    ps3.config(c,ctx3,ctxIr3);
//        sEnt += entropy<4>(w,
//          {prob(ds0,ps0), prob(ds1,ps1), prob(ds2,ps2), probIr(ds3,ps3)});
//        updateCtx(ctx0,ps0);   updateCtx(ctx1,ps1);
//        updateCtx(ctx2,ps2);   updateCtx(ctx3,ctxIr3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::IDDI) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);    ps1.config(c,ctx1);
//        ps2.config(c,ctx2);           ps3.config(c,ctx3,ctxIr3);
//        sEnt += entropy<4>(w,
//          {probIr(ds0,ps0), prob(ds1,ps1), prob(ds2,ps2), probIr(ds3,ps3)});
//        updateCtx(ctx0,ctxIr0,ps0);   updateCtx(ctx1,ps1);
//        updateCtx(ctx2,ps2);          updateCtx(ctx3,ctxIr3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::IDID) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);    ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2);    ps3.config(c,ctx3,ctxIr3);
//        sEnt += entropy<4>(w,
//          {prob(ds0,ps0), probIr(ds1,ps1), prob(ds2,ps2), probIr(ds3,ps3)});
//        updateCtx(ctx0,ps0);   updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ps2);   updateCtx(ctx3,ctxIr3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::IDII) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);    ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2);           ps3.config(c,ctx3,ctxIr3);
//        sEnt += entropy<4>(w,
//          {probIr(ds0,ps0), probIr(ds1,ps1), prob(ds2,ps2), probIr(ds3,ps3)});
//        updateCtx(ctx0,ctxIr0,ps0);   updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ps2);          updateCtx(ctx3,ctxIr3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::IIDD) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);           ps1.config(c,ctx1);
//        ps2.config(c,ctx2,ctxIr2);    ps3.config(c,ctx3,ctxIr3);
//        sEnt += entropy<4>(w,
//          {prob(ds0,ps0), prob(ds1,ps1), probIr(ds2,ps2), probIr(ds3,ps3)});
//        updateCtx(ctx0,ps0);          updateCtx(ctx1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);   updateCtx(ctx3,ctxIr3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::IIDI) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);    ps1.config(c,ctx1);
//        ps2.config(c,ctx2,ctxIr2);    ps3.config(c,ctx3,ctxIr3);
//        sEnt += entropy<4>(w,
//          {probIr(ds0,ps0), prob(ds1,ps1), probIr(ds2,ps2), probIr(ds3,ps3)});
//        updateCtx(ctx0,ctxIr0,ps0);   updateCtx(ctx1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);   updateCtx(ctx3,ctxIr3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::IIID) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);           ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2,ctxIr2);    ps3.config(c,ctx3,ctxIr3);
//        sEnt += entropy<4>(w,
//          {prob(ds0,ps0), probIr(ds1,ps1), probIr(ds2,ps2), probIr(ds3,ps3)});
//        updateCtx(ctx0,ps0);          updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);   updateCtx(ctx3,ctxIr3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::IIII) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);    ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2,ctxIr2);    ps3.config(c,ctx3,ctxIr3);
//        sEnt += entropy<4>(w,
//          {probIr(ds0,ps0), probIr(ds1,ps1), probIr(ds2,ps2), probIr(ds3,ps3)});
//        updateCtx(ctx0,ctxIr0,ps0);   updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);   updateCtx(ctx3,ctxIr3,ps3);
//      }
//  }
}

// Called from main -- MUST NOT be inline
void FCM::report (const Param& p) const {
  ofstream f(p.report, ofstream::out | ofstream::app);
  f << p.tar
    << '\t' << p.ref
    << '\t' << static_cast<u32>(model[0].ir)
    << '\t' << static_cast<u32>(model[0].k)
    << '\t' << std::fixed << std::setprecision(3) << model[0].alpha
    << '\t' << (model[0].w==0 ? 0 : static_cast<u32>(log2(model[0].w)))
    << '\t' << static_cast<u32>(model[0].d)
    << '\t' << std::fixed << std::setprecision(3) << aveEnt << '\n';
  f.close();  // Actually done, automatically
}

template <typename ds_t, typename ctx_t>
inline double FCM::prob (const ds_t& ds, const Prob_s<ctx_t>& p) const {
  const array<decltype(ds->query(0)), 4> c
    {ds->query(p.l), ds->query(p.l|1), ds->query(p.l|2), ds->query(p.l|3)};
  return (c[p.numSym]+p.alpha)/ (std::accumulate(c.begin(),c.end(),0)+p.sAlpha);
}

template <typename ds_t, typename ctx_t>
inline double FCM::probR (const ds_t& ds, const Prob_s<ctx_t>& p) const {
  const array<decltype(ds->query(0)), 4> c
  {ds->query(p.l), ds->query(p.l|1), ds->query(p.l|2), ds->query(p.l|3)};
  return (std::accumulate(c.begin(),c.end(),0)+p.sAlpha) /(c[p.numSym]+p.alpha);
}

template <typename ds_t, typename ctx_t>
inline double FCM::probIr (const ds_t& ds, const Prob_s<ctx_t>& p) const {
  const array<decltype(ds->query(0)+ds->query(0)), 4> c
    {ds->query(p.l)  +ds->query((3<<p.shl)|p.r),
     ds->query(p.l|1)+ds->query((2<<p.shl)|p.r),
     ds->query(p.l|2)+ds->query((1<<p.shl)|p.r),
     ds->query(p.l|3)+ds->query(p.r)};
  return (c[p.numSym]+p.alpha)/ (std::accumulate(c.begin(),c.end(),0)+p.sAlpha);
}

template <typename ds_t, typename ctx_t>
inline double FCM::probIrR (const ds_t& ds, const Prob_s<ctx_t>& p) const {
  const array<decltype(ds->query(0)+ds->query(0)), 4> c
    {ds->query(p.l)  +ds->query((3<<p.shl)|p.r),
     ds->query(p.l|1)+ds->query((2<<p.shl)|p.r),
     ds->query(p.l|2)+ds->query((1<<p.shl)|p.r),
     ds->query(p.l|3)+ds->query(p.r)};
  return (std::accumulate(c.begin(),c.end(),0)+p.sAlpha) /(c[p.numSym]+p.alpha);
}

template <u8 N>
inline double FCM::entropy (array<double,N>& w,
                            const initializer_list<double>& Pm) const {
  // Set weights
  array<double,N> rawW {};
  for (auto rIt=rawW.begin(), wIt=w.begin(), pIt=Pm.begin(); rIt!=rawW.end();
       ++rIt, ++wIt, ++pIt)
    *rIt = pow(*wIt, DEF_GAMMA) * *pIt;
  for (auto rIt=rawW.begin(), wIt=w.begin(); rIt!=rawW.end(); ++rIt, ++wIt)
    *wIt = *rIt / std::accumulate(rawW.begin(), rawW.end(), 0.0);
  // log2 1 / (Pm0*w0 + Pm1*w1 + ...)
  return log2(1 / std::inner_product(w.begin(), w.end(), Pm.begin(), 0.0));
}

template <typename ctx_t>
inline void FCM::updateCtx (ctx_t& ctx, const Prob_s<ctx_t>& p) const {
  ctx = (p.l & p.mask) | p.numSym;
}

template <typename ctx_t>
inline void FCM::updateCtx (ctx_t& ctx, ctx_t& ctxIr, const Prob_s<ctx_t>& p)
                           const {
  ctx   = (p.l & p.mask) | p.numSym;
  ctxIr = (p.revNumSym<<p.shl) | p.r;
}

template <typename T>
inline void FCM::print (T in) const {
  cerr << in << '\n';
}
template <typename T, typename... Args>
inline void FCM::print (T in, Args... args) const {
  cerr << in << '\t';
  print(args...);
}