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

//template <class Ctx>
//ProbPar<Ctx>::ProbPar (float a, Ctx m, u8 sh)
ProbPar::ProbPar (float a, u64 m, u8 sh)
  : alpha(a), sAlpha(static_cast<double>(ALPH_SZ*alpha)), mask(m), shl(sh) {
}
//template <class Ctx>
//inline void ProbPar<Ctx>::config (char c, Ctx ctx) {
inline void ProbPar::config (char c, u64 ctx) {
  numSym = NUM[static_cast<u8>(c)];
  l      = ctx<<2u;
}
//template <class Ctx>
//inline void ProbPar<Ctx>::config (char c, Ctx ctx, Ctx ctxIr) {
inline void ProbPar::config (char c, u64 ctx, u64 ctxIr) {
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

//  for (auto a:models) {//todo
//    print(a.k, a.thresh,a.w,a.d,a.ir, a.alpha, a.gamma);
//  }

////  // Models MUST be sorted by 'k'=ctx size//todo check if a MUST
////  std::sort(models.begin(), models.end(),
////            [](const auto& lhs, const auto& rhs){ return lhs.k < rhs.k; });
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
    cerr << "Building the model" << (models.size()==1 ? "" : "s");
  cerr << " (level " << static_cast<u16>(p.level) << ")...\n";
  (p.nthr==1 || models.size()==1) ? store_1_thr(p) : store_n_thr(p)/*Mult thr*/;
  cerr << "Finished";
  
  //todo
//  for(auto a:tbl64)a->print();cerr<<'\n';
//  for(auto a:tbl32)a->print();cerr<<'\n';
//  for(auto a:lgtbl8)a->print();cerr<<'\n';
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
  vector<std::thread> thrd(vThrSz);
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
  if (models.size() == 1)  // 1 MM
    switch (models[0].cner) {
      case Container::TABLE_64:     compress_1_MM(p.tar, tbl64.begin());  break;
      case Container::TABLE_32:     compress_1_MM(p.tar, tbl32.begin());  break;
      case Container::LOG_TABLE_8:  compress_1_MM(p.tar, lgtbl8.begin()); break;
      case Container::SKETCH_8:     compress_1_MM(p.tar, cmls4.begin());  break;
    }
  else
    compress_n(p.tar);
  cerr << "Finished";
}

template <class CnerIter>
inline void FCM::compress_1_MM (const string& tar, CnerIter cnerIt) {
  u64 ctx{0}, ctxIr{(1ull << (models[0].k << 1))-1}; // Ctx, Mir (int) sliding through the dataset
  u64 symsNo{0};                // No. syms in target file, except \n
  double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  ProbPar pp{models[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
             static_cast<u8>(models[0].k << 1u)};

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

inline void FCM::compress_n (const string& tar) {
  // Ctx, Mir (int) sliding through the dataset
  const auto nMdl = models.size();
  vector<u64> ctx;      ctx.resize(nMdl);    // Fill with zeros (resize)
  vector<u64> ctxIr;    ctxIr.reserve(nMdl);
  for (const auto& m : models)  // Mask: 1<<2k - 1 = 4^k - 1
    ctxIr.emplace_back((1ull<<(m.k<<1)) - 1);
  vector<double> w (nMdl, 1.0/nMdl);
  u64 symsNo{0};                // No. syms in target file, except \n
  double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  vector<ProbPar> pp;    pp.reserve(nMdl);
  for (u8 i=0; i!=nMdl; ++i)
    pp.emplace_back(models[i].alpha, ctxIr[i],static_cast<u8>(models[i].k<<1u));
  
  while (tf.get(c))
    if (c != '\n') {
      ++symsNo;
      for (u8 i=0; i!=nMdl; ++i)
        (models[i].ir == 0) ? pp[i].config(c, ctx[i])
                            : pp[i].config(c, ctx[i], ctxIr[i]);
//      //todo. STMM
////      for (auto i=static_cast<u8>(MMs.size()); i!=MMs.size()+STMMs.size(); ++i) {
////        (STMMs[i-MMs.size()].ir==0) ? pp[i].config(c, ctx[i])
////                                    : pp[i].config(c, ctx[i], ctxIr[i]);
////      }
      // Entropy
      auto tbl64_iter=tbl64.begin();    auto tbl32_iter=tbl32.begin();
      auto lgtbl8_iter=lgtbl8.begin();  auto cmls4_iter=cmls4.begin();
      vector<double> probs;    probs.reserve(models.size());
      for (u8 i=0; i!=nMdl; ++i) {
        if (models[i].cner == Container::TABLE_64)
          (models[i].ir==0) ? probs.emplace_back(prob(tbl64_iter++, pp[i]))
                            : probs.emplace_back(probIr(tbl64_iter++, pp[i]));
        else if (models[i].cner == Container::TABLE_32)
          (models[i].ir==0) ? probs.emplace_back(prob(tbl32_iter++, pp[i]))
                            : probs.emplace_back(probIr(tbl32_iter++, pp[i]));
        else if (models[i].cner == Container::LOG_TABLE_8)
          (models[i].ir==0) ? probs.emplace_back(prob(lgtbl8_iter++, pp[i]))
                            : probs.emplace_back(probIr(lgtbl8_iter++, pp[i]));
        else if (models[i].cner == Container::SKETCH_8)
          (models[i].ir==0) ? probs.emplace_back(prob(cmls4_iter++, pp[i]))
                            : probs.emplace_back(probIr(cmls4_iter++, pp[i]));
      }
      sEnt += entropy(w.begin(), probs.begin(), probs.end());
      
      for (u8 i=0; i!=nMdl; ++i)
        (models[i].ir == 0) ? update_ctx(ctx[i], pp[i])
                            : update_ctx(ctx[i], ctxIr[i], pp[i]);
    }
  tf.close();
  aveEnt = sEnt/symsNo;
}

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

template <class CnerIter>
inline double FCM::prob (CnerIter cnerIt, const ProbPar& pp) const {
  const array<decltype((*cnerIt)->query(0)), 4> c
    {(*cnerIt)->query(pp.l),
     (*cnerIt)->query(pp.l | 1ull),
     (*cnerIt)->query(pp.l | 2ull),
     (*cnerIt)->query(pp.l | 3ull)};
  return (c[pp.numSym] + pp.alpha)
         / (std::accumulate(c.begin(),c.end(),0ull) + pp.sAlpha);
}

template <class CnerIter>
inline double FCM::probIr (CnerIter cnerIt, const ProbPar& pp) const {
  const array<decltype((*cnerIt)->query(0)+(*cnerIt)->query(0)), 4> c
    {(*cnerIt)->query(pp.l)        + (*cnerIt)->query((3ull<<pp.shl) | pp.r),
     (*cnerIt)->query(pp.l | 1ull) + (*cnerIt)->query((2ull<<pp.shl) | pp.r),
     (*cnerIt)->query(pp.l | 2ull) + (*cnerIt)->query((1ull<<pp.shl) | pp.r),
     (*cnerIt)->query(pp.l | 3ull) + (*cnerIt)->query(pp.r)};
  return (c[pp.numSym] + pp.alpha)
         / (std::accumulate(c.begin(),c.end(),0ull) + pp.sAlpha);
}

inline double FCM::entropy (double P) const {
  return -log2(P);
}

template <class OutIter, class InIter>
inline double FCM::entropy (OutIter wFirst, InIter PFirst, InIter PLast) const {
  update_weights(wFirst, PFirst, PLast);
  // log2 1 / (P0*w0 + P1*w1 + ...)
  return log2(1 / std::inner_product(PFirst, PLast, wFirst, 0.0));
}

template <class OutIter, class InIter>
inline void FCM::update_weights (OutIter wFirst, InIter PFirst, InIter PLast)
const {
  vector<double> rawW;    rawW.reserve(models.size());
  for (auto mIt=models.begin(), wFst=wFirst; PFirst!=PLast; ++mIt)
    rawW.emplace_back(pow(*wFst++, mIt->gamma) * *PFirst++);
  
  normalize(wFirst, rawW.begin(), rawW.end());
}

template <class OutIter, class InIter>
inline void FCM::normalize (OutIter oFirst, InIter iFirst, InIter iLast) const {
  for (const double sum=std::accumulate(iFirst, iLast, 0.0); iFirst != iLast;)
    *oFirst++ = *iFirst++ / sum;
}

inline void FCM::update_ctx (u64& ctx, const ProbPar& pp) const {
  ctx = (pp.l & pp.mask) | pp.numSym;
}

inline void FCM::update_ctx (u64& ctx, u64& ctxIr, const ProbPar& pp) const {
  ctx   = (pp.l & pp.mask) | pp.numSym;
  ctxIr = (pp.revNumSym<<pp.shl) | pp.r;
}