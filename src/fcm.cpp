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
using std::make_shared;

ModelPar::ModelPar (u8 k_, u64 w_, u8 d_, u8 Mir_, float Ma_, float Mg_,
                    u8 TMt_, u8 TMir_, float TMa_, float TMg_)
  : k(k_), w(w_), d(d_), Mir(Mir_), Malpha(Ma_), Mgamma(Mg_), TMthresh(TMt_),
    TMir(TMir_), TMalpha(TMa_), TMgamma(TMg_), mode(Mode::TABLE_64) {
}
ModelPar::ModelPar (u8 k_, u8 Mir_, float Ma_, float Mg_)
  : ModelPar(k_, 0, 0, Mir_, Ma_, Mg_, 0, 0, 0, 0) {
}
ModelPar::ModelPar (u8 k_, u64 w_, u8 d_, u8 Mir_, float Ma_, float Mg_)
  : ModelPar(k_, w_, d_, Mir_, Ma_, Mg_, 0, 0, 0, 0) {
}
ModelPar::ModelPar (u8 k_, u8 Mir_, float Ma_, float Mg_,
                    u8 TMt_, u8 TMir_, float TMa_, float TMg_)
  : ModelPar(k_, 0, 0, Mir_, Ma_, Mg_, TMt_, TMir_, TMa_, TMg_) {
}


//template <class ctx_t>
//Prob_s<ctx_t>::Prob_s (float a, ctx_t m, u8 sh)
//  : alpha(a), sAlpha(static_cast<double>(ALPH_SZ*alpha)), mask(m), shl(sh) {
//}
//
//template <class ctx_t>
//inline void Prob_s<ctx_t>::config (char c, ctx_t ctx) {
//  numSym = NUM[static_cast<u8>(c)];
//  l      = ctx<<2u;
//}
//
//template <class ctx_t>
//inline void Prob_s<ctx_t>::config (char c, ctx_t ctx, ctx_t ctxIr) {
//  numSym    = NUM[static_cast<u8>(c)];
//  l         = ctx<<2u;
//  revNumSym = REVNUM[static_cast<u8>(c)];
//  r         = ctxIr>>2u;
//}

FCM::FCM (const Param& p) {
  aveEnt = 0.0;
  config(p);
  alloc_models();
//  setModesComb();
//  setIRsComb();
}

inline void FCM::config (const Param& p) {
  vector<string> mdls;
  split(p.modelsPars.begin(), p.modelsPars.end(), ':', mdls);
  for (const auto& mp : mdls) {
    vector<string> MnTM;    // Markov and tolerant models
    split(mp.begin(), mp.end(), '/', MnTM);
    if (MnTM.size() == 1) {
      vector<string> M;
      split(MnTM[0].begin(), MnTM[0].end(), ',', M);
      if (M.size() == 4)
        model.emplace_back(ModelPar(static_cast<u8>(stoi(M[0])),
          static_cast<u8>(stoi(M[1])), stof(M[2]), stof(M[3])));
      else if (M.size() == 6)
        model.emplace_back(ModelPar(static_cast<u8>(stoi(M[0])),
          pow2(stoull(M[1])), static_cast<u8>(stoi(M[2])),
          static_cast<u8>(stoi(M[3])), stof(M[4]), stof(M[5])));
    }
    else if (MnTM.size() == 2) {  // Including tolerant model
      vector<string> M, TM;
      split(MnTM[0].begin(), MnTM[0].end(), ',', M);
      split(MnTM[1].begin(), MnTM[1].end(), ',', TM);
      if (M.size() == 4)
        model.emplace_back(ModelPar(static_cast<u8>(stoi(M[0])),
          static_cast<u8>(stoi(M[1])), stof(M[2]), stof(M[3]),
          static_cast<u8>(stoi(TM[0])), static_cast<u8>(stoi(TM[1])),
          stof(TM[2]), stof(TM[3])));
      else if (M.size() == 6)
        model.emplace_back(ModelPar(static_cast<u8>(stoi(M[0])),
          pow2(stoull(M[1])), static_cast<u8>(stoi(M[2])),
          static_cast<u8>(stoi(M[3])), stof(M[4]), stof(M[5]),
          static_cast<u8>(stoi(TM[0])), static_cast<u8>(stoi(TM[1])),
          stof(TM[2]), stof(TM[3])));
    }
  }
  // Set modes. & is MANDATORY, since we set 'mode'.
  for (auto& m : model) {
    if      (m.k > K_MAX_LGTBL8)    m.mode = Mode::SKETCH_8;
    else if (m.k > K_MAX_TBL32)     m.mode = Mode::LOG_TABLE_8;
    else if (m.k > K_MAX_TBL64)     m.mode = Mode::TABLE_32;
    else                            m.mode = Mode::TABLE_64;
  }
//  // Models MUST be sorted by 'k'=ctx size//todo check if a MUST
//  std::sort(model.begin(), model.end(),
//            [](const auto& lhs, const auto& rhs){ return lhs.k < rhs.k; });
}

template <class InIter, class Vec>
inline void FCM::split (InIter first, InIter last, char delim, Vec& vOut) const{
  while (true) {
    InIter found = std::find(first, last, delim);
    vOut.emplace_back(string(first,found));
    if (found == last)
      break;
    first = ++found;
  }
}

inline void FCM::alloc_models () {
  for (const auto& m : model) {
    if (m.mode == Mode::TABLE_64)
      tbl64.emplace_back(make_shared<Table64>(m.k));
    else if (m.mode == Mode::TABLE_32)
      tbl32.emplace_back(make_shared<Table32>(m.k));
    else if (m.mode == Mode::LOG_TABLE_8)
      lgtbl8.emplace_back(make_shared<LogTable8>(m.k));
    else if (m.mode == Mode::SKETCH_8)
      cmls4.emplace_back(make_shared<CMLS4>(m.w, m.d));
  }
  
  //todo
//  tbl64[0]->print();
//  cerr<<'\n';
//  tbl64[1]->print();
  
}

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

void FCM::store (const Param& p) {
  if (p.verbose)
    cerr << "Building " <<model.size()<< " model"<< (model.size()==1 ? "" : "s")
         << " based on the reference \"" << p.ref << "\"";
  else
    cerr << "Building the model" << (model.size()==1 ? "" : "s");
  cerr << " (level " << static_cast<u16>(p.level) << ")...\n";
  (p.nthr==1 || model.size()==1) ? store_1_thr(p) : store_n_thr(p) /*Mult thr*/;
  cerr << "Finished";
  
  //todo
  for(auto a:tbl64)a->print();cerr<<'\n';
}

inline void FCM::store_1_thr (const Param& p) {
//  cerr<<"1_thr";//todo
  auto tbl64_Beg = tbl64.begin();
//  int i=0;
  for (const auto& m : model) {    // Mask: 4<<2k - 1 = 4^(k+1) - 1
//    (*tbl64_Beg++)->print();//todo
    if (m.mode == Mode::TABLE_64)
      store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl64_Beg++);  // ul MANDATORY
////    store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl64);  // ul MANDATORY
//    store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl64[i++]);  // ul MANDATORY
////    else if (m.mode == Mode::TABLE_32)
////      store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl32);
////    else if (m.mode == Mode::LOG_TABLE_8)
////      store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, lgtbl8);
////    else if (m.mode == Mode::SKETCH_8)
////      store_impl(p.ref, (4ull<<(m.k<<1u))-1/*Mask 64*/, cmls4);
//    else
//      cerr << "Error: the model cannot be built.\n";
  }
//  tbl64[0]->print();//todo
}

inline void FCM::store_n_thr (const Param& p) {
  cerr<<"n_thr";//todo
//  const auto vThrSz = (p.nthr < model.size()) ? p.nthr : model.size();
//  vector<std::thread> thrd;  thrd.resize(vThrSz);
//  for (u8 i=0; i!=model.size(); ++i) {  // Mask: 4<<2k-1 = 4^(k+1)-1
//    if (model[i].mode == Mode::TABLE_64)
//      thrd[i % vThrSz] = std::thread(&FCM::store_impl<u32, shared_ptr<Table64>>,
//        this, std::cref(p.ref), (4ul<<(model[i].k<<1u))-1, std::ref(tbl64));
//    else if (model[i].mode == Mode::TABLE_32)
//      thrd[i % vThrSz] = std::thread(&FCM::store_impl<u32, shared_ptr<Table32>>,
//        this, std::cref(p.ref), (4ul<<(model[i].k<<1u))-1, std::ref(tbl32));
//    else if (model[i].mode == Mode::LOG_TABLE_8)
//      thrd[i % vThrSz] = std::thread(&FCM::store_impl<u32, shared_ptr<LogTable8>>,
//        this, std::cref(p.ref), (4ul<<(model[i].k<<1u))-1, std::ref(lgtbl8));
//    else if (model[i].mode == Mode::SKETCH_8)
//      thrd[i % vThrSz] = std::thread(&FCM::store_impl<u64, shared_ptr<CMLS4>>,
//        this, std::cref(p.ref), (4ull<<(model[i].k<<1u))-1, std::ref(cmls4));
//    // Join
//    if ((i+1) % vThrSz == 0)
//      for (auto& t : thrd)  if (t.joinable()) t.join();
//  }
//  for (auto& t : thrd)  if (t.joinable()) t.join();  // Join leftover threads
}

template <class Mask, class Container>
//inline void FCM::store_impl (const string& ref, Mask mask, Container& cntn) {
  inline void FCM::store_impl (const string& ref, Mask mask, Container cntn) {
  ifstream rf(ref);  char c;
  for (Mask ctx=0; rf.get(c);)
    if (c != '\n') {
      ctx = ((ctx<<static_cast<Mask>(2)) & mask) | NUM[static_cast<u8>(c)];
      (*cntn)->update(ctx);
//      cntn->update(ctx);
    }
  rf.close();
}

//void FCM::compress (const Param& p) {
//  if (p.verbose)  cerr << "Compressing the target \"" << p.tar << "\"...\n";
//  else            cerr << "Compressing...\n";
//  vector<u32> mask32{};  u64 mask64{};
//  if (MODE_COMB != 15) {
//    for (const auto &m : model) {
//      if (m.mode == Mode::SKETCH_8)
//        mask64 = (1ull<<(m.k<<1)) - 1;
//      else
//        mask32.emplace_back((1ul<<(m.k<<1)) - 1);  // 1<<2k-1=4^k-1
//    }
//  }
//  switch (MODE_COMB) {
//    case 1:   comp1mdl(p.tar, mask32[0], tbl64);                        break;
//    case 2:   comp1mdl(p.tar, mask32[0], tbl32);                        break;
//    case 4:   comp1mdl(p.tar, mask32[0], lgtbl8);                      break;
//    case 8:   comp1mdl(p.tar, mask64,    cmls4);                      break;
//    case 3:   comp2mdl(p.tar, mask32[0], mask32[1], tbl64,   tbl32);    break;
//    case 5:   comp2mdl(p.tar, mask32[0], mask32[1], tbl64,   lgtbl8);  break;
//    case 9:   comp2mdl(p.tar, mask32[0], mask64,    tbl64,   cmls4);  break;
//    case 6:   comp2mdl(p.tar, mask32[0], mask32[1], tbl32,   lgtbl8);  break;
//    case 10:  comp2mdl(p.tar, mask32[0], mask64,    tbl32,   cmls4);  break;
//    case 12:  comp2mdl(p.tar, mask32[0], mask64,    lgtbl8, cmls4);  break;
//    case 7:   comp3mdl(p.tar, mask32[0], mask32[1], mask32[3],
//                       tbl64, tbl32, lgtbl8);                          break;
//    case 11:  comp3mdl(p.tar, mask32[0], mask32[1], mask64,
//                       tbl64, tbl32, cmls4);                          break;
//    case 13:  comp3mdl(p.tar, mask32[0], mask32[1], mask64,
//                       tbl64, lgtbl8, cmls4);                        break;
//    case 14:  comp3mdl(p.tar, mask32[0], mask32[1], mask64,
//                       tbl32, lgtbl8, cmls4);                        break;
//    case 15:  comp4mdl(p.tar);                                          break;
//    default:  cerr << "Error: the models cannot be built.";             break;
//  }
//  cerr << "Finished";
//}

//template <class msk_t, class ds_t>
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
//template <class msk0_t, class msk1_t, class ds0_t, class ds1_t>
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
//template <class msk0_t, class msk1_t, class msk2_t,
//  class ds0_t, class ds1_t, class ds2_t>
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
//    if (m.mode == Mode::SKETCH_8)
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
//                               prob(lgtbl8,ps2), prob(cmls4,ps3)});
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
//                               prob(lgtbl8,ps2), prob(cmls4,ps3)});
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
//                               prob(lgtbl8,ps2), prob(cmls4,ps3)});
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
//                               prob(lgtbl8,ps2), prob(cmls4,ps3)});
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
//                               probIr(lgtbl8,ps2), prob(cmls4,ps3)});
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
//                               probIr(lgtbl8,ps2), prob(cmls4,ps3)});
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
//                               probIr(lgtbl8,ps2), prob(cmls4,ps3)});
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
//                               probIr(lgtbl8,ps2), prob(cmls4,ps3)});
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
//                               prob(lgtbl8,ps2), probIr(cmls4,ps3)});
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
//                               prob(lgtbl8,ps2), probIr(cmls4,ps3)});
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
//                               prob(lgtbl8,ps2), probIr(cmls4,ps3)});
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
//                               prob(lgtbl8,ps2), probIr(cmls4,ps3)});
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
//                               probIr(lgtbl8,ps2), probIr(cmls4,ps3)});
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
//                               probIr(lgtbl8,ps2), probIr(cmls4,ps3)});
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
//                               probIr(lgtbl8,ps2), probIr(cmls4,ps3)});
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
//                               probIr(lgtbl8,ps2), probIr(cmls4,ps3)});
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
//template <class ds_t, class ctx_t>
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
//template <class ds_t, class ctx_t>
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
//template <class ds_t, class ctx_t>
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
//template <class ds_t, class ctx_t>
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
//template <class ctx_t>
//inline void FCM::updateCtx (ctx_t& ctx, const Prob_s<ctx_t>& p) const {
//  ctx = (p.l & p.mask) | p.numSym;
//}
//
//template <class ctx_t>
//inline void FCM::updateCtx (ctx_t& ctx, ctx_t& ctxIr, const Prob_s<ctx_t>& p)
//                           const {
//  ctx   = (p.l & p.mask) | p.numSym;
//  ctxIr = (p.revNumSym<<p.shl) | p.r;
//}