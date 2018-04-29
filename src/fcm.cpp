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

ModelPar::ModelPar (u8 k_, u8 t_, u64 w_, u8 d_, u8 ir_, float a_, float g_)
  : k(k_), thresh(t_), w(w_), d(d_), ir(ir_), alpha(a_), gamma(g_),
    cner(Container::TABLE_64), mode(Mode::MM) {
}
ModelPar::ModelPar (u8 k_, u8 ir_, float a_, float g_)
  : ModelPar(k_, 0, 0, 0, ir_, a_, g_) {
}
ModelPar::ModelPar (u8 k_, u64 w_, u8 d_, u8 ir_, float a_, float g_)
  : ModelPar(k_, 0, w_, d_, ir_, a_, g_) {
}
ModelPar::ModelPar (u8 k_, u8 t_, u8 ir_, float a_, float g_)
  : ModelPar(k_, t_, 0, 0, ir_, a_, g_) {
}

template <class Ctx>
ProbPar<Ctx>::ProbPar (float a, Ctx m, u8 sh)
  : alpha(a), sAlpha(static_cast<double>(ALPH_SZ*alpha)), mask(m), shl(sh) {
}

template <class Ctx>
inline void ProbPar<Ctx>::config (char c, Ctx ctx) {
  numSym = NUM[static_cast<u8>(c)];
  l      = ctx<<2u;
}

template <class Ctx>
inline void ProbPar<Ctx>::config (char c, Ctx ctx, Ctx ctxIr) {
  numSym    = NUM[static_cast<u8>(c)];
  l         = ctx<<2u;
  revNumSym = REVNUM[static_cast<u8>(c)];
  r         = ctxIr>>2u;
}

FCM::FCM (const Param& p) {
  aveEnt = 0.0;
  config(p);
  alloc_model();
}

inline void FCM::config (const Param& p) {
  vector<string> mdls;
  split(p.modelsPars.begin(), p.modelsPars.end(), ':', mdls);
  for (const auto& mp : mdls) {
    // Markov and tolerant models
    vector<string> MnTM;    split(mp.begin(), mp.end(), '/', MnTM);
    assert_empty_lm(MnTM, "Error: incorrect model parameters.");
    vector<string> M;       split(MnTM[0].begin(), MnTM[0].end(), ',', M);
    if (M.size() == 4)
      models.emplace_back(ModelPar(static_cast<u8>(stoi(M[0])),
        static_cast<u8>(stoi(M[1])), stof(M[2]), stof(M[3])));
    else if (M.size() == 6)
      models.emplace_back(ModelPar(static_cast<u8>(stoi(M[0])),
        pow2(stoull(M[1])), static_cast<u8>(stoi(M[2])),
        static_cast<u8>(stoi(M[3])), stof(M[4]), stof(M[5])));
    
    // Tolerant models
    if (MnTM.size() == 2) {
      vector<string> TM;    split(MnTM[1].begin(), MnTM[1].end(), ',', TM);
      models.emplace_back(ModelPar(static_cast<u8>(stoi(M[0])),
        static_cast<u8>(stoi(TM[0])), static_cast<u8>(stoi(TM[1])), stof(TM[2]),
        stof(TM[3])));
    }
  }
  set_mode_cner();    // Set modes: TABLE_64, TABLE_32, LOG_TABLE_8, SKETCH_8
//  set_cner_idx();

//  for (auto a:models) {//todo
//    print(a.k, a.thresh,a.w,a.d,a.ir, a.alpha, a.gamma);
//  }

//  // Models MUST be sorted by 'k'=ctx size//todo check if a MUST
//  std::sort(models.begin(), models.end(),
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

inline void FCM::set_mode_cner () {
  for (auto& m : models) {
    // Mode
    if (m.thresh != 0)              m.mode = Mode::STMM;
    else                            m.mode = Mode::MM;
    // Container
    if      (m.k > K_MAX_LGTBL8)    m.cner = Container::SKETCH_8;
    else if (m.k > K_MAX_TBL32)     m.cner = Container::LOG_TABLE_8;
    else if (m.k > K_MAX_TBL64)     m.cner = Container::TABLE_32;
    else                            m.cner = Container::TABLE_64;
  }
}

//inline void FCM::set_cner_idx () {
//  u8 tbl64Idx{0}, tbl32Idx{0}, lgtbl8Idx{0}, cmls4Idx{0};
//  for (auto& m : MMs) {
//    if      (m.cner==Container::TABLE_64)       m.cnerIdx = tbl64Idx++;
//    else if (m.cner==Container::TABLE_32)       m.cnerIdx = tbl32Idx++;
//    else if (m.cner==Container::LOG_TABLE_8)    m.cnerIdx = lgtbl8Idx++;
//    else if (m.cner==Container::SKETCH_8)       m.cnerIdx = cmls4Idx++;
//  }
//  tbl64Idx = tbl32Idx = lgtbl8Idx = cmls4Idx = 0;
//  for (auto& m : STMMs) {
//    if      (m.cner==Container::TABLE_64)       m.cnerIdx = tbl64Idx++;
//    else if (m.cner==Container::TABLE_32)       m.cnerIdx = tbl32Idx++;
//    else if (m.cner==Container::LOG_TABLE_8)    m.cnerIdx = lgtbl8Idx++;
//    else if (m.cner==Container::SKETCH_8)       m.cnerIdx = cmls4Idx++;
//  }
//}

inline void FCM::alloc_model () {
  for (const auto& m : models)
    if (m.mode == Mode::MM) {
      if (m.cner == Container::TABLE_64)
        tbl64.emplace_back(make_shared<Table64>(m.k));
      else if (m.cner == Container::TABLE_32)
        tbl32.emplace_back(make_shared<Table32>(m.k));
      else if (m.cner == Container::LOG_TABLE_8)
        lgtbl8.emplace_back(make_shared<LogTable8>(m.k));
      else if (m.cner == Container::SKETCH_8)
        cmls4.emplace_back(make_shared<CMLS4>(m.w, m.d));
    }
}

void FCM::store (const Param& p) {
  if (p.verbose)
    cerr << "Building " << models.size() << " model"
         << (models.size()==1 ? "" : "s") << " based on the reference \""
         << p.ref << "\"";
  else
    cerr << "Building the models" << (models.size()==1 ? "" : "s");
  cerr << " (level " << static_cast<u16>(p.level) << ")...\n";
  (p.nthr==1 || models.size()==1) ? store_1_thr(p) : store_n_thr(p)/*Mult thr*/;
  cerr << "Finished";
  
//  //todo
//  for(auto a:tbl64)a->print();cerr<<'\n';
//  for(auto a:cmls4)a->print();cerr<<'\n';
}

inline void FCM::store_1_thr (const Param& p) {
  auto tbl64_iter  = tbl64.begin();     auto tbl32_iter = tbl32.begin();
  auto lgtbl8_iter = lgtbl8.begin();    auto cmls4_iter = cmls4.begin();
  for (const auto& m : models)    // Mask: 4<<2k - 1 = 4^(k+1) - 1
    if (m.mode == Mode::MM) {
      if (m.cner == Container::TABLE_64)
        store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl64_iter++);
      else if (m.cner == Container::TABLE_32)
        store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl32_iter++);
      else if (m.cner == Container::LOG_TABLE_8)
        store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, lgtbl8_iter++);
      else if (m.cner == Container::SKETCH_8)
        store_impl(p.ref, (4ull<<(m.k<<1u))-1/*Mask 64*/, cmls4_iter++);
      else
        cerr << "Error: the models cannot be built.\n";
    }
}

inline void FCM::store_n_thr (const Param& p) {
  auto tbl64_iter  = tbl64.begin();     auto tbl32_iter = tbl32.begin();
  auto lgtbl8_iter = lgtbl8.begin();    auto cmls4_iter = cmls4.begin();
  const auto vThrSz = (p.nthr < models.size()) ? p.nthr : models.size();
  vector<std::thread> thrd;  thrd.resize(vThrSz);
  for (u8 i=0; i!=models.size(); ++i) {    // Mask: 4<<2k-1 = 4^(k+1)-1
    if (models[i].mode == Mode::MM) {
      switch (models[i].cner) {
        case Container::TABLE_64:
          thrd[i % vThrSz] =
            std::thread(&FCM::store_impl<u32,decltype(tbl64_iter)>, this,
              std::cref(p.ref), (4ul<<(models[i].k<<1u))-1, tbl64_iter++);
          break;
        case Container::TABLE_32:
          thrd[i % vThrSz] =
            std::thread(&FCM::store_impl<u32,decltype(tbl32_iter)>, this,
              std::cref(p.ref), (4ul<<(models[i].k<<1u))-1, tbl32_iter++);
          break;
        case Container::LOG_TABLE_8:
          thrd[i % vThrSz] =
            std::thread(&FCM::store_impl<u32,decltype(lgtbl8_iter)>, this,
              std::cref(p.ref), (4ul<<(models[i].k<<1u))-1, lgtbl8_iter++);
          break;
        case Container::SKETCH_8:
          thrd[i % vThrSz] =
            std::thread(&FCM::store_impl<u64,decltype(cmls4_iter)>, this,
              std::cref(p.ref), (4ull<<(models[i].k<<1u))-1, cmls4_iter++);
          break;
//        default:  cerr << "Error: the models cannot be built.\n";
      }
    }
    // Join
    if ((i+1) % vThrSz == 0)
      for (auto& t : thrd)  if (t.joinable()) t.join();
  }
  for (auto& t : thrd)  if (t.joinable()) t.join();  // Join leftover threads
}

template <class Mask, class CnerIter /*Container iterator*/>
inline void FCM::store_impl (const string& ref, Mask mask, CnerIter cnerIt) {
  ifstream rf(ref);  char c;
  for (Mask ctx=0; rf.get(c);)
    if (c != '\n') {
      ctx = ((ctx<<2u) & mask) | NUM[static_cast<u8>(c)];
      (*cnerIt)->update(ctx);
    }
  rf.close();
}

void FCM::compress (const Param& p) {
  if (p.verbose)  cerr << "Compressing the target \"" << p.tar << "\"...\n";
  else            cerr << "Compressing...\n";

  // todo 1 MM
  if (models.size() == 1) {
    switch (models[0].cner) {
      case Container::TABLE_64:     compress_1_MM(p.tar, tbl64.begin());  break;
      case Container::TABLE_32:     compress_1_MM(p.tar, tbl32.begin());  break;
      case Container::LOG_TABLE_8:  compress_1_MM(p.tar, lgtbl8.begin()); break;
      case Container::SKETCH_8:     compress_1_MM(p.tar, cmls4.begin());  break;
    }
  }
  else {
    compress_n(p.tar);
  }

//  vector<u32> mask32{};  u64 mask64{};
//  if (MODE_COMB != 15) {
//    for (const auto &m : models) {
//      if (m.cner == Container::SKETCH_8)
//        mask64 = (1ull<<(m.k<<1)) - 1;
//      else
//        mask32.emplace_back((1ul<<(m.k<<1)) - 1);  // 1<<2k-1=4^k-1
//    }
//  }
//  switch (MODE_COMB) {
//    case 1:   compress_1_MM(p.tar, mask32[0], tbl64);                 break;
//    case 2:   compress_1_MM(p.tar, mask32[0], tbl32);                break;
//    case 4:   compress_1_MM(p.tar, mask32[0], lgtbl8);             break;
//    case 8:   compress_1_MM(p.tar, mask64,    cmls4);              break;
//    case 3:   comp2mdl(p.tar, mask32[0], mask32[1], tbl64,   tbl32);    break;
//    case 5:   comp2mdl(p.tar, mask32[0], mask32[1], tbl64,   lgtbl8);  break;
//    case 9:   comp2mdl(p.tar, mask32[0], mask64,    tbl64,   cmls4);  break;
//    case 6:   comp2mdl(p.tar, mask32[0], mask32[1], tbl32,   lgtbl8);  break;
//    case 10:  comp2mdl(p.tar, mask32[0], mask64,    tbl32,   cmls4);  break;
//    case 12:  comp2mdl(p.tar, mask32[0], mask64,    lgtbl8, cmls4);  break;
//    case 15:  comp4mdl(p.tar);                                          break;
//    default:  cerr << "Error: the models cannot be built.";             break;
//  }
  cerr << "Finished";
}

template <
//  class msk_t,
    class CnerIter>
inline void FCM::compress_1_MM (const string& tar
  ,
//                                msk_t mask,
                                CnerIter cnerIt
) {
    u64 ctx{0}, ctxIr{(1ull<<(models[0].k<<1)) - 1}; // Ctx, Mir (int) sliding through the dataset
    u64 symsNo{0};                // No. syms in target file, except \n
    double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
    ifstream tf(
//      p.
      tar);  char c;
    ProbPar<u64> pp {models[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
                     static_cast<u8>(models[0].k<<1u)};

//  msk_t ctx{0}, ctxIr{mask};    // Ctx, Mir (int) sliding through the dataset
  if (models[0].ir == 0) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        pp.config(c, ctx);
        sEnt += entropy(prob(cnerIt, pp));
        update_ctx(ctx, pp);
      }
  }
  else if (models[0].ir == 1) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        pp.config(c, ctx, ctxIr);
        sEnt += entropy(probIr(cnerIt, pp));
        update_ctx(ctx, ctxIr, pp);    // Update ctx & ctxIr
      }
  }
  tf.close();
  aveEnt = sEnt/symsNo;
}

//template <
////  class msk_t,
//    class CnerIter>
inline void FCM::compress_n (const string& tar
//  ,
//                                msk_t mask,
//                                CnerIter cnerIt
) {
  // Ctx, Mir (int) sliding through the dataset
  vector<u64> ctx;      ctx.resize(models.size());      ctx.clear();
  vector<u64> ctxIr;    ctxIr.resize(models.size());    ctxIr.clear();
  for (const auto& m : models)
    ctxIr.emplace_back((1ull<<(m.k<<1)) - 1);  // Mask

//  vector<double> w (models.size(), 1.0/models.size());
//  u64 symsNo{0};                // No. syms in target file, except \n
//  double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(
////      p.
//      tar);  char c;
//  vector<ProbPar<u64>> pp;    pp.resize(models.size());
//  for (u8 i=0; i!=models.size(); ++i)
//    pp.emplace_back(models[i].alpha, ctxIr[i] /* mask: 1<<2k-1=4^k-1 */,
//                    static_cast<u8>(models[i].k<<1u));
//
////  vector<double> probs;    probs.resize(models.size());
//  while (tf.get(c))
//    if (c != '\n') {
//      ++symsNo;
//      for (u8 i = 0; i != models.size(); ++i) {
//        (models[i].ir == 0) ? pp[i].config(c, ctx[i])
//                            : pp[i].config(c, ctx[i], ctxIr[i]);
//      }
////      //todo. STMM
//////      for (auto i=static_cast<u8>(MMs.size()); i!=MMs.size()+STMMs.size(); ++i) {
//////        (STMMs[i-MMs.size()].ir==0) ? pp[i].config(c, ctx[i])
//////                                    : pp[i].config(c, ctx[i], ctxIr[i]);
//////      }
//      //todo.entropy
//      auto tbl64_iter=tbl64.begin();    auto tbl32_iter=tbl32.begin();
//      auto lgtbl8_iter=lgtbl8.begin();  auto cmls4_iter=cmls4.begin();
////      probs.clear();
//      vector<double> probs;    probs.resize(models.size());
//      for (u8 i=0; i!=models.size(); ++i) {
//        if (models[i].cner == Container::TABLE_64)
//          (models[i].ir==0) ? probs.emplace_back(prob(tbl64_iter++, pp[i]))
//                            : probs.emplace_back(probIr(tbl64_iter++, pp[i]));
//        else if (models[i].cner == Container::TABLE_32)
//          (models[i].ir==0) ? probs.emplace_back(prob(tbl32_iter++, pp[i]))
//                            : probs.emplace_back(probIr(tbl32_iter++, pp[i]));
//        else if (models[i].cner == Container::LOG_TABLE_8)
//          (models[i].ir==0) ? probs.emplace_back(prob(lgtbl8_iter++, pp[i]))
//                            : probs.emplace_back(probIr(lgtbl8_iter++, pp[i]));
//        else if (models[i].cner == Container::SKETCH_8)
//          (models[i].ir==0) ? probs.emplace_back(prob(cmls4_iter++, pp[i]))
//                            : probs.emplace_back(probIr(cmls4_iter++, pp[i]));
//      }
//
//      sEnt += entropy(w, probs);
//
//      for (u8 i = 0; i != models.size(); ++i) {
//        (models[i].ir == 0) ? update_ctx(ctx[i], pp[i])
//                            : update_ctx(ctx[i], ctxIr[i], pp[i]);
//      }
//    }
//  tf.close();
//  aveEnt = sEnt/symsNo;
}

//template <class msk0_t, class msk1_t, class ds0_t, class ds1_t>
//inline void FCM::comp2mdl (const string& tar, msk0_t mask0, msk1_t mask1,
//                           const ds0_t& ds0, const ds1_t& ds1) {
//  msk0_t ctx0{0}, ctxIr0{mask0};    // Ctx, Mir (int) sliding through the dataset
//  msk1_t ctx1{0}, ctxIr1{mask1};
//  u64 symsNo {0};                   // No. syms in target file, except \n
//  array<double,2> w {0.5, 0.5};
//  double sEnt {0};                  // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(tar);  char c;
//  ProbPar<msk0_t> ps0 {models[0].Malpha, mask0, static_cast<u8>(models[0].k<<1u)};
//  ProbPar<msk1_t> ps1 {models[1].Malpha, mask1, static_cast<u8>(models[1].k<<1u)};
//  if (IR_COMB == IR::DDDD) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c, ctx0);     ps1.config(c, ctx1);
//        sEnt += entropy<2>(w, {prob(ds0,ps0), prob(ds1,ps1)});
//        updateCtx(ctx0, ps0);    update_ctx(ctx1, ps1);  // Update ctx
//      }
//  }
//  else if (IR_COMB == IR::DDDI) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c, ctx0, ctxIr0);    ps1.config(c, ctx1);
//        sEnt += entropy<2>(w, {probIr(ds0,ps0), prob(ds1,ps1)});
//        updateCtx(ctx0, ctxIr0, ps0);   update_ctx(ctx1, ps1);
//      }
//  }
//  else if (IR_COMB == IR::DDID) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c, ctx0);    ps1.config(c, ctx1, ctxIr1);
//        sEnt += entropy<2>(w, {prob(ds0,ps0), probIr(ds1,ps1)});
//        updateCtx(ctx0, ps0);   update_ctx(ctx1, ctxIr1, ps1);
//      }
//  }
//  else if (IR_COMB == IR::DDII) {
//    while (tf.get(c))
//      if (c != '\n') {
//        ++symsNo;
//        ps0.config(c, ctx0, ctxIr0);    ps1.config(c, ctx1, ctxIr1);
//        sEnt += entropy<2>(w, {probIr(ds0,ps0), probIr(ds1,ps1)});
//        updateCtx(ctx0, ctxIr0, ps0);   update_ctx(ctx1, ctxIr1, ps1);
//      }
//  }
//  tf.close();
//  aveEnt = sEnt/symsNo;
//}

//// Called from main -- MUST NOT be inline
//void FCM::report (const Param& p) const {
//  ofstream f(p.report, ofstream::out | ofstream::app);
//  f << p.tar
//    << '\t' << p.ref
//    << '\t' << static_cast<u32>(models[0].Mir)
//    << '\t' << static_cast<u32>(models[0].k)
//    << '\t' << std::fixed << std::setprecision(3) << models[0].Malpha
//    << '\t' << (models[0].w==0 ? 0 : static_cast<u32>(log2(models[0].w)))
//    << '\t' << static_cast<u32>(models[0].d)
//    << '\t' << std::fixed << std::setprecision(3) << aveEnt << '\n';
//  f.close();  // Actually done, automatically
//}

template <class CnerIter, class Ctx>
inline double FCM::prob (CnerIter cnerIt, const ProbPar<Ctx>& pp) const {
  const array<decltype((*cnerIt)->query(0)), 4> c
    {(*cnerIt)->query(pp.l),
     (*cnerIt)->query(pp.l | static_cast<Ctx>(1)),
     (*cnerIt)->query(pp.l | static_cast<Ctx>(2)),
     (*cnerIt)->query(pp.l | static_cast<Ctx>(3))};
  return (c[pp.numSym] + pp.alpha)
         / (std::accumulate(c.begin(),c.end(),0ull) + pp.sAlpha);
}

template <class CnerIter, class Ctx>
inline double FCM::probIr (CnerIter cnerIt, const ProbPar<Ctx>& pp) const {
  const array<decltype((*cnerIt)->query(0)+(*cnerIt)->query(0)), 4> c
    {(*cnerIt)->query(pp.l) +
       (*cnerIt)->query((static_cast<Ctx>(3)<<pp.shl) | pp.r),
     (*cnerIt)->query(pp.l | static_cast<Ctx>(1)) +
       (*cnerIt)->query((static_cast<Ctx>(2)<<pp.shl) | pp.r),
     (*cnerIt)->query(pp.l | static_cast<Ctx>(2)) +
       (*cnerIt)->query((static_cast<Ctx>(1)<<pp.shl) | pp.r),
     (*cnerIt)->query(pp.l | static_cast<Ctx>(3)) +
       (*cnerIt)->query(pp.r)};
  return (c[pp.numSym] + pp.alpha)
         / (std::accumulate(c.begin(),c.end(),0ull) + pp.sAlpha);
}

inline double FCM::entropy (double P) const {
  return -log2(P);
}

inline double FCM::entropy (vector<double>& w, const vector<double>& Pm) const {
  // Set weights
  vector<double> rawW {};  rawW.resize(w.size());
  for (u8 i=0; i!=models.size(); ++i)
    rawW[i] = pow(w[i], models[i].gamma) * Pm[i];
  const double sumRawW = std::accumulate(rawW.begin(), rawW.end(), 0.0);
  for (auto rIt=rawW.begin(), wIt=w.begin(); rIt!=rawW.end(); ++rIt, ++wIt)
    *wIt = *rIt / sumRawW;
  // log2 1 / (Pm0*w0 + Pm1*w1 + ...)
  return log2(1 / std::inner_product(w.begin(), w.end(), Pm.begin(), 0.0));
}

template <class Ctx>
inline void FCM::update_ctx (Ctx& ctx, const ProbPar<Ctx>& pp) const {
  ctx = (pp.l & pp.mask) | pp.numSym;
}

template <class Ctx>
inline void FCM::update_ctx (Ctx& ctx, Ctx& ctxIr, const ProbPar<Ctx>& pp)
const {
  ctx   = (pp.l & pp.mask) | pp.numSym;
  ctxIr = (pp.revNumSym<<pp.shl) | pp.r;
}