//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include <cmath>
#include <thread>
#include <numeric>  // std::accumulate
#include "fcm.hpp"
#include "assert.hpp"
#include "fn.hpp"
using std::ifstream;
using std::fstream;
using std::cout;
using std::array;
using std::initializer_list;
using std::make_shared;

MMPar::MMPar (u8 k_, u64 w_, u8 d_, u8 ir_, float a_, float g_)
  : k(k_), w(w_), d(d_), ir(ir_), alpha(a_), gamma(g_),
    cont(Container::TABLE_64), child(nullptr) {
}
MMPar::MMPar (u8 k_, u8 ir_, float a_, float g_)
  : MMPar(k_, 0, 0, ir_, a_, g_) {
}

STMMPar::STMMPar (u8 k_, u8 t_, u8 ir_, float a_, float g_)
  : k(k_), thresh(t_), ir(ir_), alpha(a_), gamma(g_), enable(true) {
}

ProbPar::ProbPar (float a, u64 m, u8 sh)
  : alpha(a), sAlpha(static_cast<double>(ALPH_SZ*alpha)), mask(m), shl(sh) {
}
inline void ProbPar::config (char c, u64 ctx) {
  numSym = NUM[static_cast<u8>(c)];
  l      = ctx<<2u;
}
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
  for (const auto& e : mdls) {
    // Markov and tolerant models
    vector<string> m_tm;    split(e.begin(), e.end(), '/', m_tm);
    assert_empty_elem(m_tm, "Error: incorrect model parameters.");
    vector<string> m;       split(m_tm[0].begin(), m_tm[0].end(), ',', m);
    if (m.size() == 4)
      Ms.emplace_back(
        MMPar(static_cast<u8>(stoi(m[0])), static_cast<u8>(stoi(m[1])),
              stof(m[2]), stof(m[3]))
      );
    else if (m.size() == 6)
      Ms.emplace_back(
        MMPar(static_cast<u8>(stoi(m[0])), pow2(stoull(m[1])),
              static_cast<u8>(stoi(m[2])), static_cast<u8>(stoi(m[3])),
              stof(m[4]), stof(m[5]))
      );
    
    // Tolerant models
    if (m_tm.size() == 2) {
      vector<string> tm;    split(m_tm[1].begin(), m_tm[1].end(), ',', tm);
      TMs.emplace_back(
        STMMPar(static_cast<u8>(stoi(m[0])), static_cast<u8>(stoi(tm[0])),
                static_cast<u8>(stoi(tm[1])), stof(tm[2]), stof(tm[3]))
      );
      Ms.back().child = make_unique<STMMPar>(TMs.back());
    }
  }
  set_cont();    // Set modes: TABLE_64, TABLE_32, LOG_TABLE_8, SKETCH_8
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

inline void FCM::set_cont () {
  for (auto& m : Ms) {
    if      (m.k > K_MAX_LGTBL8)    m.cont = Container::SKETCH_8;
    else if (m.k > K_MAX_TBL32)     m.cont = Container::LOG_TABLE_8;
    else if (m.k > K_MAX_TBL64)     m.cont = Container::TABLE_32;
    else                            m.cont = Container::TABLE_64;
  }
}

inline void FCM::alloc_model () {
  for (const auto& m : Ms)
    if (m.cont == Container::TABLE_64)
      tbl64.emplace_back(make_shared<Table64>(m.k));
    else if (m.cont == Container::TABLE_32)
      tbl32.emplace_back(make_shared<Table32>(m.k));
    else if (m.cont == Container::LOG_TABLE_8)
      lgtbl8.emplace_back(make_shared<LogTable8>(m.k));
    else if (m.cont == Container::SKETCH_8)
      cmls4.emplace_back(make_shared<CMLS4>(m.w, m.d));
}

void FCM::store (const Param& p) {
  const auto noMdls = Ms.size();
  if (p.verbose)
    cerr << "Building " << noMdls << " model" << (noMdls==1 ? "" : "s")
         << " based on the reference \"" << p.ref << "\"";
  else
    cerr << "Building the model" << (noMdls==1 ? "" : "s");
  cerr << " (level " << static_cast<u16>(p.level) << ")...\n";
  (p.nthr==1 || noMdls==1) ? store_1(p) : store_n(p)/*Mult thr*/;
  cerr << "Finished";

  //todo
//  for(auto a:tbl64)a->print();cerr<<'\n';
//  for(auto a:tbl32)a->print();cerr<<'\n';
//  for(auto a:lgtbl8)a->print();cerr<<'\n';
//  for(auto a:cmls4)a->print();cerr<<'\n';
}

inline void FCM::store_1 (const Param& p) {
  auto tbl64_iter  = tbl64.begin();     auto tbl32_iter = tbl32.begin();
  auto lgtbl8_iter = lgtbl8.begin();    auto cmls4_iter = cmls4.begin();
  for (const auto& m : Ms)    // Mask: 4<<2k - 1 = 4^(k+1) - 1
    if (m.cont == Container::TABLE_64)
      store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl64_iter++);
    else if (m.cont == Container::TABLE_32)
      store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl32_iter++);
    else if (m.cont == Container::LOG_TABLE_8)
      store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, lgtbl8_iter++);
    else if (m.cont == Container::SKETCH_8)
      store_impl(p.ref, (4ull<<(m.k<<1u))-1/*Mask 64*/, cmls4_iter++);
    else
      cerr << "Error: the models cannot be built.\n";
}

inline void FCM::store_n (const Param& p) {
  auto tbl64_iter  = tbl64.begin();     auto tbl32_iter = tbl32.begin();
  auto lgtbl8_iter = lgtbl8.begin();    auto cmls4_iter = cmls4.begin();
  const auto vThrSz = (p.nthr < Ms.size()) ? p.nthr : Ms.size();
  vector<std::thread> thrd(vThrSz);
  for (u8 i=0; i!=Ms.size(); ++i) {    // Mask: 4<<2k-1 = 4^(k+1)-1
    switch (Ms[i].cont) {
      case Container::TABLE_64:
        thrd[i % vThrSz] =
          std::thread(&FCM::store_impl<u32,decltype(tbl64_iter)>, this,
            std::cref(p.ref), (4ul<<(Ms[i].k<<1u))-1, tbl64_iter++);
        break;
      case Container::TABLE_32:
        thrd[i % vThrSz] =
          std::thread(&FCM::store_impl<u32,decltype(tbl32_iter)>, this,
            std::cref(p.ref), (4ul<<(Ms[i].k<<1u))-1, tbl32_iter++);
        break;
      case Container::LOG_TABLE_8:
        thrd[i % vThrSz] =
          std::thread(&FCM::store_impl<u32,decltype(lgtbl8_iter)>, this,
            std::cref(p.ref), (4ul<<(Ms[i].k<<1u))-1, lgtbl8_iter++);
        break;
      case Container::SKETCH_8:
        thrd[i % vThrSz] =
          std::thread(&FCM::store_impl<u64,decltype(cmls4_iter)>, this,
            std::cref(p.ref), (4ull<<(Ms[i].k<<1u))-1, cmls4_iter++);
        break;
//      default:  cerr << "Error: the models cannot be built.\n";
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
  if (Ms.size()==1 && TMs.size()==0)  // 1 MM
    switch (Ms[0].cont) {
      case Container::TABLE_64:     compress_1(p.tar, tbl64.begin());   break;
      case Container::TABLE_32:     compress_1(p.tar, tbl32.begin());   break;
      case Container::LOG_TABLE_8:  compress_1(p.tar, lgtbl8.begin());  break;
      case Container::SKETCH_8:     compress_1(p.tar, cmls4.begin());   break;
    }
  else
    compress_n(p.tar);
  cerr << "Finished";
}

template <class CnerIter>
inline void FCM::compress_1 (const string& tar, CnerIter cnerIt) {
  // Ctx, Mir (int) sliding through the dataset
  u64 ctx{0}, ctxIr{(1ull<<(Ms[0].k<<1u))-1};
  u64 symsNo{0};                // No. syms in target file, except \n
  double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  ProbPar pp{Ms[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
             static_cast<u8>(Ms[0].k<<1u)};

  if (Ms[0].ir == 0) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        pp.config(c, ctx);
        sEnt += entropy(prob(cnerIt, &pp));
        update_ctx(ctx, &pp);
      }
  }
  else if (Ms[0].ir == 1) {
    while (tf.get(c))
      if (c != '\n') {
        ++symsNo;
        pp.config(c, ctx, ctxIr);
        sEnt += entropy(probIr(cnerIt, &pp));
        update_ctx(ctx, ctxIr, &pp);    // Update ctx & ctxIr
      }
  }
  tf.close();
  aveEnt = sEnt/symsNo;
}

inline void FCM::compress_n (const string& tar) {
//  // Ctx, Mir (int) sliding through the dataset
//  const auto nMdl = models.size();
//  vector<u64> ctx;      ctx.resize(nMdl);    // Fill with zeros (resize)
//  vector<u64> ctxIr;    ctxIr.reserve(nMdl);
//  for (const auto& m : models)  // Mask: 1<<2k - 1 = 4^k - 1
//    ctxIr.emplace_back((1ull<<(m.k<<1)) - 1);
//  vector<double> w (nMdl, 1.0/nMdl);
//  u64 symsNo{0};                // No. syms in target file, except \n
//  double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(tar);  char c;
//  vector<ProbPar> pp;    pp.reserve(nMdl);
//  for (u8 i=0; i!=nMdl; ++i)
//    pp.emplace_back(models[i].alpha, ctxIr[i],static_cast<u8>(models[i].k<<1u));
//
//  while (tf.get(c))
//    if (c != '\n') {
//      ++symsNo;
//      for (u8 i=0; i!=nMdl; ++i)
//        (models[i].ir==0) ? pp[i].config(c, ctx[i])
//                          : pp[i].config(c, ctx[i], ctxIr[i]);
//      // Entropy
//      auto tbl64_iter=tbl64.begin();    auto tbl32_iter=tbl32.begin();
//      auto lgtbl8_iter=lgtbl8.begin();  auto cmls4_iter=cmls4.begin();
//      auto ppIter = pp.begin();
//      vector<double> probs;    probs.reserve(models.size());
//      for (u8 i=0; i!=nMdl; ++i) {
//        if (models[i].cont == Container::TABLE_64)
//          (models[i].ir == 0)
//            ? probs.emplace_back(prob(tbl64_iter++, ppIter++))
//            : probs.emplace_back(probIr(tbl64_iter++, ppIter++));
////        else if (models[i].cont == Container::TABLE_32)
////          (models[i].ir == 0)
////            ? probs.emplace_back(prob(tbl32_iter++, ppIter++))
////            : probs.emplace_back(probIr(tbl32_iter++, ppIter++));
////        else if (models[i].cont == Container::LOG_TABLE_8)
////          (models[i].ir == 0)
////            ? probs.emplace_back(prob(lgtbl8_iter++, ppIter++))
////            : probs.emplace_back(probIr(lgtbl8_iter++, ppIter++));
////        else if (models[i].cont == Container::SKETCH_8)
////          (models[i].ir == 0)
////            ? probs.emplace_back(prob(cmls4_iter++, ppIter++))
////            : probs.emplace_back(probIr(cmls4_iter++, ppIter++));
//      }
////      sEnt += entropy(w.begin(), probs.begin(), probs.end());
////
////      ppIter = pp.begin();
////      for (u8 i=0; i!=nMdl; ++i)
////        (models[i].ir==0) ? update_ctx(ctx[i], ppIter++)
////                          : update_ctx(ctx[i], ctxIr[i], ppIter++);
//    }
//  tf.close();
//  aveEnt = sEnt/symsNo;
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

template <class CnerIter, class ProbParIter>
inline double FCM::prob (CnerIter cnerIt, ProbParIter pp) const {
  const array<decltype((*cnerIt)->query(0)), 4> c
    {(*cnerIt)->query(pp->l),
     (*cnerIt)->query(pp->l | 1ull),
     (*cnerIt)->query(pp->l | 2ull),
     (*cnerIt)->query(pp->l | 3ull)};
  return (c[pp->numSym] + pp->alpha)
         / (std::accumulate(c.begin(),c.end(),0ull) + pp->sAlpha);
}

template <class CnerIter, class ProbParIter>
inline double FCM::probIr (const CnerIter cnerIt, ProbParIter pp) const {
  const array<decltype((*cnerIt)->query(0)+(*cnerIt)->query(0)), 4> c
    {(*cnerIt)->query(pp->l)        + (*cnerIt)->query((3ull<<pp->shl) | pp->r),
     (*cnerIt)->query(pp->l | 1ull) + (*cnerIt)->query((2ull<<pp->shl) | pp->r),
     (*cnerIt)->query(pp->l | 2ull) + (*cnerIt)->query((1ull<<pp->shl) | pp->r),
     (*cnerIt)->query(pp->l | 3ull) + (*cnerIt)->query(pp->r)};
  return (c[pp->numSym] + pp->alpha)
         / (std::accumulate(c.begin(),c.end(),0ull) + pp->sAlpha);
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
//todo
template <class OutIter, class InIter>
inline void FCM::update_weights (OutIter wFirst, InIter PFirst, InIter PLast)
const {
//  vector<double> rawW;    rawW.reserve(models.size());
//  for (auto mIt=models.begin(), wFst=wFirst; PFirst!=PLast; ++mIt)
//    rawW.emplace_back(pow(*wFst++, mIt->gamma) * *PFirst++);
//
//  normalize(wFirst, rawW.begin(), rawW.end());
}

template <class OutIter, class InIter>
inline void FCM::normalize (OutIter oFirst, InIter iFirst, InIter iLast) const {
  for (const double sum=std::accumulate(iFirst, iLast, 0.0); iFirst != iLast;)
    *oFirst++ = *iFirst++ / sum;
}

template <class ProbParIter>
inline void FCM::update_ctx (u64& ctx, ProbParIter pp) const {
  ctx = (pp->l & pp->mask) | pp->numSym;
}

template <class ProbParIter>
inline void FCM::update_ctx (u64& ctx, u64& ctxIr, ProbParIter pp) const {
  ctx   = (pp->l & pp->mask) | pp->numSym;
  ctxIr = (pp->revNumSym<<pp->shl) | pp->r;
}