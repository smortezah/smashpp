//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include <cmath>
#include <thread>
#include <numeric>  // std::accumulate
#include "fcm.hpp"
using std::ifstream;
using std::fstream;
using std::cout;
using std::array;
using std::initializer_list;
using std::make_unique;

ModelPar::ModelPar (u8 k_, u64 w_, u8 d_, u8 Mir_, float Ma_, float Mg_,
                    u8 TMt_, u8 TMir_, float TMa_, float TMg_)
  : k(k_), w(w_), d(d_), Mir(Mir_), Malpha(Ma_), Mgamma(Mg_),
    TMthresh(TMt_), TMir(TMir_), TMalpha(TMa_), TMgamma(TMg_), mode(0) {
}
ModelPar::ModelPar (u8 k_)
  : ModelPar(k_, 0, 0, DEF_IR, DEF_ALPHA, DEF_GAMMA, 0, 0, 0, 0) {
}
ModelPar::ModelPar (u8 k_, u8 TMt_)
  : ModelPar(k_, 0, 0, DEF_IR, DEF_ALPHA, DEF_GAMMA,
             TMt_, DEF_IR, DEF_ALPHA, DEF_GAMMA) {
}



//template <typename ctx_t>
//Prob_s<ctx_t>::Prob_s (float a, ctx_t m, u8 sh)
//  : alpha(a), sAlpha(static_cast<double>(ALPH_SZ*alpha)), mask(m), shl(sh) {
//}
//
//template <typename ctx_t>
//inline void Prob_s<ctx_t>::config (char c, ctx_t ctx) {
//  numSym = NUM[static_cast<u8>(c)];
//  l      = ctx<<2u;
//}
//
//template <typename ctx_t>
//inline void Prob_s<ctx_t>::config (char c, ctx_t ctx, ctx_t ctxIr) {
//  numSym    = NUM[static_cast<u8>(c)];
//  l         = ctx<<2u;
//  revNumSym = REVNUM[static_cast<u8>(c)];
//  r         = ctxIr>>2u;
//}

FCM::FCM (const Param& p) {
  aveEnt = 0.0;
  config(p);
//  allocModels();
//  setModesComb();
//  setIRsComb();
}

inline void FCM::config (const Param& p) {
  vector<string> vMdlsPars;
  split(p.modelsPars.begin(), p.modelsPars.end(), ':', vMdlsPars);
  for (const auto& mp : vMdlsPars) {
    vector<string> vMMnSTMMPars;
    split(mp.begin(), mp.end(), '/', vMMnSTMMPars);
    vector<string> vMPars;
    split(vMMnSTMMPars[0].begin(), vMMnSTMMPars[0].end(), ',', vMPars);
//    if (vMPars.size() == 3)
//      model.emplace_back(ModelPar(static_cast<u8>(stoi(vMPars[0])),
//                      static_cast<u8>(stoi(vMPars[1])), stof(vMPars[2])));
//    else if (vMPars.size() == 5)
//      model.emplace_back(ModelPar(static_cast<u8>(stoi(vMPars[0])),
//                      static_cast<u8>(stoi(vMPars[1])), stof(vMPars[2]),
//                      pow2(stoull(vMPars[3])), static_cast<u8>(stoi(vMPars[4]))));
    if (vMMnSTMMPars.size() == 2) {
      vMPars.clear();
      split(vMMnSTMMPars[1].begin(), vMMnSTMMPars[1].end(), ',', vMPars);
    }
  }
//  // Set modes. & is MANDATORY, since we set 'mode'.
//  for (auto& m : model) {
//    if      (m.k > K_MAX_LGTBL8)    m.mode = MODE::SKETCH_8;
//    else if (m.k > K_MAX_TBL32)     m.mode = MODE::LOG_TABLE_8;
//    else if (m.k > K_MAX_TBL64)     m.mode = MODE::TABLE_32;
//    else                            m.mode = MODE::TABLE_64;
//  }
//  // Models MUST be sorted by 'k'=ctx size
//  std::sort(model.begin(), model.end(),
//            [](const auto& lhs, const auto& rhs){ return lhs.k < rhs.k; });
}

template <typename InIter, typename Vec>
inline void FCM::split (InIter first, InIter last, char delim, Vec& vOut) const{
  while (true) {
    InIter found = std::find(first, last, delim);
    vOut.emplace_back(string(first,found));
    if (found == last)
      break;
    first = ++found;
  }
}

//inline void FCM::allocModels () {
//  for (const auto& m : model) {
//    switch (m.mode) {
//      case MODE::TABLE_64:      tbl64   = make_unique<Table64>(m.k);      break;
//      case MODE::TABLE_32:      tbl32   = make_unique<Table32>(m.k);      break;
//      case MODE::LOG_TABLE_8:   logtbl8 = make_unique<LogTable8>(m.k);    break;
//      case MODE::SKETCH_8:      sketch4 = make_unique<CMLS4>(m.w, m.d);   break;
//      default:  cerr << "Error: undefined mode " << m.mode << ".\n";
//    }
//  }
//}
//
//inline void FCM::setModesComb () {  // Models MUST be sorted by 'k'=ctx size
//  MODE_COMB = 0;
//  for (const auto& m : model)
//    MODE_COMB |= 1u<<m.mode;
//}
//
//inline void FCM::setIRsComb () {    // Models MUST be sorted by 'k'=ctx size
//  IR_COMB = 0;
//  for (u8 i=0; i!=model.size(); ++i)
//    IR_COMB |= model[i].Mir<<i;
//}
//
//void FCM::buildModel (const Param& p) {//todo rename store_model
//  if (p.verbose)
//    cerr << "Building " <<model.size()<< " model"<< (model.size()==1 ? "" : "s")
//         << " based on the reference \"" << p.ref << "\"";
//  else
//    cerr << "Building the model" << (model.size()==1 ? "" : "s");
//  cerr << " (level " << static_cast<u16>(p.level) << ")...\n";
//  (p.nthr==1 || model.size()==1) ? bldMdl1Thr(p) : bldMdlNThr(p)/*Mul thr*/;
//  cerr << "Finished";
//}
//
//inline void FCM::bldMdl1Thr (const Param& p) {
//  for (const auto& m : model) {  // Mask: 4<<2k-1 = 4^(k+1)-1
//    if (m.mode == MODE::TABLE_64)
//      fillDS(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl64);  // ul is MANDATORY
//    else if (m.mode == MODE::TABLE_32)
//      fillDS(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl32);
//    else if (m.mode == MODE::LOG_TABLE_8)
//      fillDS(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, logtbl8);
//    else if (m.mode == MODE::SKETCH_8)
//      fillDS(p.ref, (4ull<<(m.k<<1u))-1/*Mask 64*/, sketch4);
//    else
//      cerr << "Error: the model cannot be built.\n";
//  }
//}
//
//inline void FCM::bldMdlNThr (const Param& p) {
//  const auto vThrSz = (p.nthr < model.size()) ? p.nthr : model.size();
//  vector<std::thread> thrd;  thrd.resize(vThrSz);
//  for (u8 i=0; i!=model.size(); ++i) {  // Mask: 4<<2k-1 = 4^(k+1)-1
//    if (model[i].mode == MODE::TABLE_64)
//      thrd[i % vThrSz] = std::thread(&FCM::fillDS<u32, unique_ptr<Table64>>,
//        this, std::cref(p.ref), (4ul<<(model[i].k<<1u))-1, std::ref(tbl64));
//    else if (model[i].mode == MODE::TABLE_32)
//      thrd[i % vThrSz] = std::thread(&FCM::fillDS<u32, unique_ptr<Table32>>,
//        this, std::cref(p.ref), (4ul<<(model[i].k<<1u))-1, std::ref(tbl32));
//    else if (model[i].mode == MODE::LOG_TABLE_8)
//      thrd[i % vThrSz] = std::thread(&FCM::fillDS<u32, unique_ptr<LogTable8>>,
//        this, std::cref(p.ref), (4ul<<(model[i].k<<1u))-1, std::ref(logtbl8));
//    else if (model[i].mode == MODE::SKETCH_8)
//      thrd[i % vThrSz] = std::thread(&FCM::fillDS<u64, unique_ptr<CMLS4>>,
//        this, std::cref(p.ref), (4ull<<(model[i].k<<1u))-1, std::ref(sketch4));
//    // Join
//    if ((i+1) % vThrSz == 0)
//      for (auto& t : thrd)  if (t.joinable()) t.join();
//  }
//  for (auto& t : thrd)  if (t.joinable()) t.join();  // Join leftover threads
//}
//
//template <typename msk_t, typename ds_t>
//inline void FCM::fillDS (const string& ref, msk_t mask, ds_t& ds) {
//  ifstream rf(ref);  char c;
//  for (msk_t ctx=0; rf.get(c);) {
//    if (c != '\n') {
//      ctx = ((ctx<<static_cast<msk_t>(2)) & mask) | NUM[static_cast<u8>(c)];
//      ds->update(ctx);
//    }
//  }
//  rf.close();
//}
//
//void FCM::compress (const Param& p) {
//  if (p.verbose)  cerr << "Compressing the target \"" << p.tar << "\"...\n";
//  else            cerr << "Compressing...\n";
//  vector<u32> mask32{};  u64 mask64{};
//  if (MODE_COMB != 15) {
//    for (const auto &m : model) {
//      if (m.mode == MODE::SKETCH_8)
//        mask64 = (1ull<<(m.k<<1)) - 1;
//      else
//        mask32.emplace_back((1ul<<(m.k<<1)) - 1);  // 1<<2k-1=4^k-1
//    }
//  }
//  switch (MODE_COMB) {
//    case 1:   comp1mdl(p.tar, mask32[0], tbl64);                        break;
//    case 2:   comp1mdl(p.tar, mask32[0], tbl32);                        break;
//    case 4:   comp1mdl(p.tar, mask32[0], logtbl8);                      break;
//    case 8:   comp1mdl(p.tar, mask64,    sketch4);                      break;
//    case 3:   comp2mdl(p.tar, mask32[0], mask32[1], tbl64,   tbl32);    break;
//    case 5:   comp2mdl(p.tar, mask32[0], mask32[1], tbl64,   logtbl8);  break;
//    case 9:   comp2mdl(p.tar, mask32[0], mask64,    tbl64,   sketch4);  break;
//    case 6:   comp2mdl(p.tar, mask32[0], mask32[1], tbl32,   logtbl8);  break;
//    case 10:  comp2mdl(p.tar, mask32[0], mask64,    tbl32,   sketch4);  break;
//    case 12:  comp2mdl(p.tar, mask32[0], mask64,    logtbl8, sketch4);  break;
//    case 7:   comp3mdl(p.tar, mask32[0], mask32[1], mask32[3],
//                       tbl64, tbl32, logtbl8);                          break;
//    case 11:  comp3mdl(p.tar, mask32[0], mask32[1], mask64,
//                       tbl64, tbl32, sketch4);                          break;
//    case 13:  comp3mdl(p.tar, mask32[0], mask32[1], mask64,
//                       tbl64, logtbl8, sketch4);                        break;
//    case 14:  comp3mdl(p.tar, mask32[0], mask32[1], mask64,
//                       tbl32, logtbl8, sketch4);                        break;
//    case 15:  comp4mdl(p.tar);                                          break;
//    default:  cerr << "Error: the models cannot be built.";             break;
//  }
//  cerr << "Finished";
//}
//
//template <typename msk_t, typename ds_t>
//inline void FCM::comp1mdl (const string& tar, msk_t mask, const ds_t& ds) {
//  msk_t ctx{0}, ctxIr{mask};    // Ctx, Mir (int) sliding through the dataset
//  u64 symsNo{0};                // No. syms in target file, except \n
//  double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(tar);  char c;
//  Prob_s<msk_t> ps {model[0].Malpha, mask, static_cast<u8>(model[0].k<<1u)};
//  if (IR_COMB == IR::DDDD) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps.config(c, ctx);
//        sEnt += log2(probR(ds, ps));
//        updateCtx(ctx, ps);
//      }
//  }
//  else if (IR_COMB == IR::DDDI) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps.config(c, ctx, ctxIr);
//        sEnt += log2(probIrR(ds, ps));
//        updateCtx(ctx, ctxIr, ps);    // Update ctx & ctxIr
//      }
//  }
//  tf.close();
//  aveEnt = sEnt/symsNo;
//}
//
//template <typename msk0_t, typename msk1_t, typename ds0_t, typename ds1_t>
//inline void FCM::comp2mdl (const string& tar, msk0_t mask0, msk1_t mask1,
//                           const ds0_t& ds0, const ds1_t& ds1) {
//  msk0_t ctx0{0}, ctxIr0{mask0};    // Ctx, Mir (int) sliding through the dataset
//  msk1_t ctx1{0}, ctxIr1{mask1};
//  u64 symsNo {0};                   // No. syms in target file, except \n
//  array<double,2> w {0.5, 0.5};
//  double sEnt {0};                  // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(tar);  char c;
//  Prob_s<msk0_t> ps0 {model[0].Malpha, mask0, static_cast<u8>(model[0].k<<1u)};
//  Prob_s<msk1_t> ps1 {model[1].Malpha, mask1, static_cast<u8>(model[1].k<<1u)};
//  if (IR_COMB == IR::DDDD) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c, ctx0);     ps1.config(c, ctx1);
//        sEnt += entropy<2>(w, {prob(ds0,ps0), prob(ds1,ps1)});
//        updateCtx(ctx0, ps0);    updateCtx(ctx1, ps1);  // Update ctx
//      }
//  }
//  else if (IR_COMB == IR::DDDI) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c, ctx0, ctxIr0);    ps1.config(c, ctx1);
//        sEnt += entropy<2>(w, {probIr(ds0,ps0), prob(ds1,ps1)});
//        updateCtx(ctx0, ctxIr0, ps0);   updateCtx(ctx1, ps1);
//      }
//  }
//  else if (IR_COMB == IR::DDID) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c, ctx0);    ps1.config(c, ctx1, ctxIr1);
//        sEnt += entropy<2>(w, {prob(ds0,ps0), probIr(ds1,ps1)});
//        updateCtx(ctx0, ps0);   updateCtx(ctx1, ctxIr1, ps1);
//      }
//  }
//  else if (IR_COMB == IR::DDII) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c, ctx0, ctxIr0);    ps1.config(c, ctx1, ctxIr1);
//        sEnt += entropy<2>(w, {probIr(ds0,ps0), probIr(ds1,ps1)});
//        updateCtx(ctx0, ctxIr0, ps0);   updateCtx(ctx1, ctxIr1, ps1);
//      }
//  }
//  tf.close();
//  aveEnt = sEnt/symsNo;
//}
//
//template <typename msk0_t, typename msk1_t, typename msk2_t,
//  typename ds0_t, typename ds1_t, typename ds2_t>
//inline void FCM::comp3mdl (const string& tar, msk0_t mask0, msk1_t mask1,
//           msk2_t mask2, const ds0_t& ds0, const ds1_t& ds1, const ds2_t& ds2) {
//  msk0_t ctx0{0}, ctxIr0{mask0};    // Ctx, Mir (int) sliding through the dataset
//  msk1_t ctx1{0}, ctxIr1{mask1};
//  msk2_t ctx2{0}, ctxIr2{mask2};
//  u64 symsNo {0};                   // No. syms in target file, except \n
//  array<double,3> w {1.0/3, 1.0/3, 1.0/3};
//  double sEnt {0};                  // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(tar);  char c;
//  Prob_s<msk0_t> ps0 {model[0].Malpha, mask0, static_cast<u8>(model[0].k<<1u)};
//  Prob_s<msk1_t> ps1 {model[1].Malpha, mask1, static_cast<u8>(model[1].k<<1u)};
//  Prob_s<msk2_t> ps2 {model[2].Malpha, mask2, static_cast<u8>(model[2].k<<1u)};
//  if (IR_COMB == IR::DDDD) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);     ps1.config(c,ctx1);     ps2.config(c,ctx2);
//        sEnt += entropy<3>(w, {prob(ds0,ps0), prob(ds1,ps1), prob(ds2,ps2)});
//        updateCtx(ctx0,ps0);    updateCtx(ctx1,ps1);    updateCtx(ctx2,ps2);
//      }
//  }
//  else if (IR_COMB == IR::DDDI) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);   ps1.config(c,ctx1);   ps2.config(c,ctx2);
//        sEnt += entropy<3>(w, {probIr(ds0,ps0), prob(ds1,ps1), prob(ds2,ps2)});
//        updateCtx(ctx0,ctxIr0,ps0);  updateCtx(ctx1,ps1);  updateCtx(ctx2,ps2);
//      }
//  }
//  else if (IR_COMB == IR::DDID) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);   ps1.config(c,ctx1,ctxIr1);   ps2.config(c,ctx2);
//        sEnt += entropy<3>(w, {prob(ds0,ps0), probIr(ds1,ps1), prob(ds2,ps2)});
//        updateCtx(ctx0,ps0);  updateCtx(ctx1,ctxIr1,ps1);  updateCtx(ctx2,ps2);
//      }
//  }
//  else if (IR_COMB == IR::DDII) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);   ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2);
//        sEnt += entropy<3>(w,{probIr(ds0,ps0), probIr(ds1,ps1), prob(ds2,ps2)});
//        updateCtx(ctx0,ctxIr0,ps0);  updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ps2);
//      }
//  }
//  else if (IR_COMB == IR::DIDD) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);   ps1.config(c,ctx1);   ps2.config(c,ctx2,ctxIr2);
//        sEnt += entropy<3>(w, {prob(ds0,ps0), prob(ds1,ps1), probIr(ds2,ps2)});
//        updateCtx(ctx0,ps0);  updateCtx(ctx1,ps1);  updateCtx(ctx2,ctxIr2,ps2);
//      }
//  }
//  else if (IR_COMB == IR::DIDI) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);   ps1.config(c,ctx1);
//        ps2.config(c,ctx2,ctxIr2);
//        sEnt += entropy<3>(w,{probIr(ds0,ps0), prob(ds1,ps1), probIr(ds2,ps2)});
//        updateCtx(ctx0,ctxIr0,ps0);  updateCtx(ctx1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);
//      }
//  }
//  else if (IR_COMB == IR::DIID) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);   ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2,ctxIr2);
//        sEnt += entropy<3>(w,{prob(ds0,ps0), probIr(ds1,ps1), probIr(ds2,ps2)});
//        updateCtx(ctx0,ps0);  updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);
//      }
//  }
//  else if (IR_COMB == IR::DIII) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);   ps1.config(c,ctx1,ctxIr1);
//        ps2.config(c,ctx2,ctxIr2);
//        sEnt += entropy<3>(w, {probIr(ds0,ps0), probIr(ds1,ps1),
//                               probIr(ds2,ps2)});
//        updateCtx(ctx0,ctxIr0,ps0);  updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);
//      }
//  }
//  tf.close();
//  aveEnt = sEnt/symsNo;
//}
//
//inline void FCM::comp4mdl (const string& tar) {
//  vector<u32> mask32 {};
//  u64         mask64 {};
//  for (const auto &m : model) {
//    if (m.mode == MODE::SKETCH_8)
//      mask64 = (1ull<<(m.k<<1u)) - 1;
//    else
//      mask32.emplace_back((1ul<<(m.k<<1u)) - 1);  // 1<<2k-1=4^k-1
//  }
//  u32 ctx0{0}, ctxIr0{mask32[0]};   // Ctx, Mir (int) sliding through the dataset
//  u32 ctx1{0}, ctxIr1{mask32[1]};
//  u32 ctx2{0}, ctxIr2{mask32[2]};
//  u64 ctx3{0}, ctxIr3{mask64};
//  u64 symsNo {0};                   // No. syms in target file, except \n
//  array<double,4> w {0.25, 0.25, 0.25, 0.25};
//  double sEnt {0};                  // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(tar);  char c;
//  Prob_s<u32> ps0 {model[0].Malpha, mask32[0], static_cast<u8>(model[0].k<<1u)};
//  Prob_s<u32> ps1 {model[1].Malpha, mask32[1], static_cast<u8>(model[1].k<<1u)};
//  Prob_s<u32> ps2 {model[2].Malpha, mask32[2], static_cast<u8>(model[2].k<<1u)};
//  Prob_s<u64> ps3 {model[3].Malpha, mask64,    static_cast<u8>(model[3].k<<1u)};
//  if (IR_COMB == IR::DDDD) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0);    ps1.config(c,ctx1);
//        ps2.config(c,ctx2);    ps3.config(c,ctx3);
//        sEnt += entropy<4>(w, {prob(tbl64,ps0),   prob(tbl32,ps1),
//                               prob(logtbl8,ps2), prob(sketch4,ps3)});
//        updateCtx(ctx0,ps0);   updateCtx(ctx1,ps1);
//        updateCtx(ctx2,ps2);   updateCtx(ctx3,ps3);
//      }
//  }
//  else if (IR_COMB == IR::DDDI) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c,ctx0,ctxIr0);    ps1.config(c,ctx1);
//        ps2.config(c,ctx2);           ps3.config(c,ctx3);
//        sEnt += entropy<4>(w, {probIr(tbl64,ps0), prob(tbl32,ps1),
//                               prob(logtbl8,ps2), prob(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {prob(tbl64,ps0),   probIr(tbl32,ps1),
//                               prob(logtbl8,ps2), prob(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {probIr(tbl64,ps0), probIr(tbl32,ps1),
//                               prob(logtbl8,ps2), prob(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {prob(tbl64,ps0),     prob(tbl32,ps1),
//                               probIr(logtbl8,ps2), prob(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {probIr(tbl64,ps0),   prob(tbl32,ps1),
//                               probIr(logtbl8,ps2), prob(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {prob(tbl64,ps0),     probIr(tbl32,ps1),
//                               probIr(logtbl8,ps2), prob(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {probIr(tbl64,ps0),   probIr(tbl32,ps1),
//                               probIr(logtbl8,ps2), prob(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {prob(tbl64,ps0),   prob(tbl32,ps1),
//                               prob(logtbl8,ps2), probIr(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {probIr(tbl64,ps0), prob(tbl32,ps1),
//                               prob(logtbl8,ps2), probIr(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {prob(tbl64,ps0),   probIr(tbl32,ps1),
//                               prob(logtbl8,ps2), probIr(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {probIr(tbl64,ps0), probIr(tbl32,ps1),
//                               prob(logtbl8,ps2), probIr(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {prob(tbl64,ps0),     prob(tbl32,ps1),
//                               probIr(logtbl8,ps2), probIr(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {probIr(tbl64,ps0),   prob(tbl32,ps1),
//                               probIr(logtbl8,ps2), probIr(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {prob(tbl64,ps0),     probIr(tbl32,ps1),
//                               probIr(logtbl8,ps2), probIr(sketch4,ps3)});
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
//        sEnt += entropy<4>(w, {probIr(tbl64,ps0),   probIr(tbl32,ps1),
//                               probIr(logtbl8,ps2), probIr(sketch4,ps3)});
//        updateCtx(ctx0,ctxIr0,ps0);   updateCtx(ctx1,ctxIr1,ps1);
//        updateCtx(ctx2,ctxIr2,ps2);   updateCtx(ctx3,ctxIr3,ps3);
//      }
//  }
//  tf.close();
//  aveEnt = sEnt/symsNo;
//}
//
//// Called from main -- MUST NOT be inline
//void FCM::report (const Param& p) const {
//  ofstream f(p.report, ofstream::out | ofstream::app);
//  f << p.tar
//    << '\t' << p.ref
//    << '\t' << static_cast<u32>(model[0].Mir)
//    << '\t' << static_cast<u32>(model[0].k)
//    << '\t' << std::fixed << std::setprecision(3) << model[0].Malpha
//    << '\t' << (model[0].w==0 ? 0 : static_cast<u32>(log2(model[0].w)))
//    << '\t' << static_cast<u32>(model[0].d)
//    << '\t' << std::fixed << std::setprecision(3) << aveEnt << '\n';
//  f.close();  // Actually done, automatically
//}
//
//template <typename ds_t, typename ctx_t>
//inline double FCM::prob (const ds_t& ds, const Prob_s<ctx_t>& p) const {
//  const array<decltype(ds->query(0)), 4> c
//    {ds->query(p.l),
//     ds->query(p.l | static_cast<ctx_t>(1)),
//     ds->query(p.l | static_cast<ctx_t>(2)),
//     ds->query(p.l | static_cast<ctx_t>(3))};
//  return (c[p.numSym] + p.Malpha)
//         / (std::accumulate(c.begin(),c.end(),0ull) + p.sAlpha);
//}
//
//template <typename ds_t, typename ctx_t>
//inline double FCM::probR (const ds_t& ds, const Prob_s<ctx_t>& p) const {
//  const array<decltype(ds->query(0)), 4> c
//    {ds->query(p.l),
//     ds->query(p.l | static_cast<ctx_t>(1)),
//     ds->query(p.l | static_cast<ctx_t>(2)),
//     ds->query(p.l | static_cast<ctx_t>(3))};
//  return (std::accumulate(c.begin(),c.end(),0ull) + p.sAlpha)
//         / (c[p.numSym] + p.Malpha);
//}
//
//template <typename ds_t, typename ctx_t>
//inline double FCM::probIr (const ds_t& ds, const Prob_s<ctx_t>& p) const {
//  const array<decltype(ds->query(0)+ds->query(0)), 4> c
//    {ds->query(p.l) + ds->query((static_cast<ctx_t>(3)<<p.shl) | p.r),
//     ds->query(p.l | static_cast<ctx_t>(1)) +
//       ds->query((static_cast<ctx_t>(2)<<p.shl) | p.r),
//     ds->query(p.l | static_cast<ctx_t>(2)) +
//       ds->query((static_cast<ctx_t>(1)<<p.shl) | p.r),
//     ds->query(p.l | static_cast<ctx_t>(3)) + ds->query(p.r)};
//  return (c[p.numSym] + p.Malpha)
//         / (std::accumulate(c.begin(),c.end(),0ull) + p.sAlpha);
//}
//
//template <typename ds_t, typename ctx_t>
//inline double FCM::probIrR (const ds_t& ds, const Prob_s<ctx_t>& p) const {
//  const array<decltype(ds->query(0)+ds->query(0)), 4> c
//    {ds->query(p.l) + ds->query((static_cast<ctx_t>(3)<<p.shl) | p.r),
//     ds->query(p.l | static_cast<ctx_t>(1)) +
//       ds->query((static_cast<ctx_t>(2)<<p.shl) | p.r),
//     ds->query(p.l | static_cast<ctx_t>(2)) +
//       ds->query((static_cast<ctx_t>(1)<<p.shl) | p.r),
//     ds->query(p.l | static_cast<ctx_t>(3)) + ds->query(p.r)};
//  return (std::accumulate(c.begin(),c.end(),0ull) + p.sAlpha)
//         / (c[p.numSym] + p.Malpha);
//}
//
//template <u8 N>
//inline double FCM::entropy (array<double,N>& w,
//                            const initializer_list<double>& Pm) const {
//  // Set weights
//  array<double,N> rawW {};
//  for (auto rIt=rawW.begin(), wIt=w.begin(), pIt=Pm.begin(); rIt!=rawW.end();
//       ++rIt, ++wIt, ++pIt)
//    *rIt = pow(*wIt, DEF_GAMMA) * *pIt;
//  const double sumRawW = std::accumulate(rawW.begin(), rawW.end(), 0.0);
//  for (auto rIt=rawW.begin(), wIt=w.begin(); rIt!=rawW.end(); ++rIt, ++wIt)
//    *wIt = *rIt / sumRawW;
//  // log2 1 / (Pm0*w0 + Pm1*w1 + ...)
//  return log2(1 / std::inner_product(w.begin(), w.end(), Pm.begin(), 0.0));
//}
//
//template <typename ctx_t>
//inline void FCM::updateCtx (ctx_t& ctx, const Prob_s<ctx_t>& p) const {
//  ctx = (p.l & p.mask) | p.numSym;
//}
//
//template <typename ctx_t>
//inline void FCM::updateCtx (ctx_t& ctx, ctx_t& ctxIr, const Prob_s<ctx_t>& p)
//                           const {
//  ctx   = (p.l & p.mask) | p.numSym;
//  ctxIr = (p.revNumSym<<p.shl) | p.r;
//}
//
//template <typename T>
//inline void FCM::print (T in) const {
//  cerr << in << '\n';
//}
//template <typename T, typename... Args>
//inline void FCM::print (T in, Args... args) const {
//  cerr << in << '\t';
//  print(args...);
//}