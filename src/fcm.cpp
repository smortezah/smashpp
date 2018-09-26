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

FCM::FCM (const Param& p) {
  aveEnt = 0.0;
  config(p);
  alloc_model();
}

inline void FCM::config (const Param& p) {
  vector<string> mdls;  split(p.modelsPars.begin(),p.modelsPars.end(),':',mdls);
  for (const auto& e : mdls) {
    // Markov and tolerant models
    vector<string> m_tm;    split(e.begin(), e.end(), '/', m_tm);
    assert_empty_elem(m_tm, "incorrect model parameters.");
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
      Ms.back().child = make_shared<STMMPar>(TMs.back());
    }
  }
  set_cont();    // Set modes: TABLE_64, TABLE_32, LOG_TABLE_8, SKETCH_8
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
  for (const auto& m : Ms) {
    if (m.cont == Container::TABLE_64)
      tbl64.emplace_back(make_shared<Table64>(m.k));
    else if (m.cont == Container::TABLE_32)
      tbl32.emplace_back(make_shared<Table32>(m.k));
    else if (m.cont == Container::LOG_TABLE_8)
      lgtbl8.emplace_back(make_shared<LogTable8>(m.k));
    else if (m.cont == Container::SKETCH_8)
      cmls4.emplace_back(make_shared<CMLS4>(m.w, m.d));
  }
}

void FCM::store (const Param& p) {
  const auto nMdl = Ms.size();
  if (p.verbose)
    cerr << "Building " << nMdl << " model" << (nMdl==1 ? "" : "s")
         << " based on the reference \"" << p.ref << "\"";
  else
    cerr << "Building the model" << (nMdl==1 ? "" : "s");
  cerr << " (level " << static_cast<u16>(p.level) << ")...\n";
  (p.nthr==1 || nMdl==1) ? store_1(p) : store_n(p)/*Mult thr*/;
  cerr << "Finished";

  #ifdef DEBUG
//  for(auto a:tbl64)a->print();cerr<<'\n';
//  for(auto a:tbl32)a->print();cerr<<'\n';
//  for(auto a:lgtbl8)a->print();cerr<<'\n';
//  for(auto a:cmls4)a->print();cerr<<'\n';
  #endif
}

inline void FCM::store_1 (const Param& p) {
  auto tbl64_iter=tbl64.begin();      auto tbl32_iter=tbl32.begin();
  auto lgtbl8_iter=lgtbl8.begin();    auto cmls4_iter=cmls4.begin();
  for (const auto& m : Ms) {    // Mask: 4<<2k - 1 = 4^(k+1) - 1
    if (m.cont == Container::TABLE_64)
      store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl64_iter++);
    else if (m.cont == Container::TABLE_32)
      store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, tbl32_iter++);
    else if (m.cont == Container::LOG_TABLE_8)
      store_impl(p.ref, (4ul<<(m.k<<1u))-1 /*Mask 32*/, lgtbl8_iter++);
    else if (m.cont == Container::SKETCH_8)
      store_impl(p.ref, (4ull<<(m.k<<1u))-1/*Mask 64*/, cmls4_iter++);
    else
      err("the models cannot be built.");
  }
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
      default:  err("the models cannot be built.");
    }
    // Join
    if ((i+1) % vThrSz == 0)
      for (auto& t : thrd)  if (t.joinable()) t.join();
  }
  for (auto& t : thrd)  if (t.joinable()) t.join();  // Join leftover threads
}

template <typename Mask, typename ContIter /*Container iterator*/>
inline void FCM::store_impl (const string& ref, Mask mask, ContIter cont) {
  ifstream rf(ref);  char c;
  for (Mask ctx=0; rf.get(c);) {
    if (c != '\n') {
      ctx = ((ctx<<2u) & mask) | NUM[static_cast<u8>(c)];
      (*cont)->update(ctx);
    }
  }
  rf.close();
}

void FCM::compress (const Param& p) {
  if (p.verbose)  cerr << "Compressing the target \"" << p.tar << "\"...\n";
  else            cerr << "Compressing...\n";

  if (Ms.size()==1 && TMs.empty()) { // 1 MM
    switch (Ms[0].cont) {
      case Container::TABLE_64:     compress_1(p.tar, tbl64.begin());   break;
      case Container::TABLE_32:     compress_1(p.tar, tbl32.begin());   break;
      case Container::LOG_TABLE_8:  compress_1(p.tar, lgtbl8.begin());  break;
      case Container::SKETCH_8:     compress_1(p.tar, cmls4.begin());   break;
    }
  }
  else
    compress_n(p.tar);
  cerr << "Finished";
}

template <typename ContIter>
inline void FCM::compress_1 (const string& tar, ContIter cont) {
  // Ctx, Mir (int) sliding through the dataset
  u64 ctx{0}, ctxIr{(1ull<<(Ms[0].k<<1u))-1};
  u64 symsNo{0};                // No. syms in target file, except \n
  double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  ofstream pf(tar+PROFILE_FMT);
  ProbPar pp{Ms[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
             static_cast<u8>(Ms[0].k<<1u)};

  if (Ms[0].ir == 0) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pp.config(c, ctx);
        const auto f = freqs<decltype((*cont)->query(0))>(cont, &pp);
        const auto entr = entropy(prob(f.begin(), &pp));
        pf << entr << '\n';
        sEnt += entr;
        update_ctx(ctx, &pp);
      }
    }
  }
  else {  // With inv. rep.
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pp.config_ir(c, ctx, ctxIr);
        const auto f = freqs_ir<decltype(2*(*cont)->query(0))>(cont, &pp);
        const auto entr = entropy(prob(f.begin(), &pp));
        pf << entr << '\n';
        sEnt += entr;
        update_ctx_ir(ctx, ctxIr, &pp);
      }
    }
  }
  tf.close();
  pf.close();
  aveEnt = sEnt/symsNo;
}

inline void FCM::compress_n (const string& tar) {
  auto compP      = make_shared<CompressPar>();
  // Ctx, Mir (int) sliding through the dataset
  const auto nMdl = static_cast<u8>(Ms.size() + TMs.size());
  compP->nMdl     = nMdl;
  compP->ctx.resize(nMdl);     // Fill with zeros (resize)
  compP->ctxIr.reserve(nMdl);
  for (const auto& mm : Ms) {  // Mask: 1<<2k - 1 = 4^k - 1
    compP->ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
    if (mm.child)
      compP->ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
  }
  compP->w.resize(nMdl, 1.0/nMdl);
  compP->pp.reserve(nMdl);
  {auto maskIter = compP->ctxIr.begin();
  for (const auto& mm : Ms) {
    compP->pp.emplace_back(mm.alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
    if (mm.child)
      compP->pp.emplace_back(
        mm.child->alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
  }}

  compress_n_ave(tar, compP);
}

inline void FCM::compress_n_ave (const string &tar,
                                 shared_ptr<CompressPar> compP) {
  u64      symsNo{0};          // No. syms in target file, except \n
  double   sEnt{0};            // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  ofstream pf(tar+PROFILE_FMT);

  while (tf.get(c)) {
    if (c != '\n') {
      ++symsNo;
      compP->c = c;
      compP->nSym=NUM[static_cast<u8>(c)];  compP->ppIt=compP->pp.begin();
      compP->ctxIt=compP->ctx.begin();      compP->ctxIrIt=compP->ctxIr.begin();
      auto tbl64_it=tbl64.begin();          auto tbl32_it=tbl32.begin();
      auto lgtbl8_it=lgtbl8.begin();        auto cmls4_it=cmls4.begin();

      for (const auto& mm : Ms) {
        compP->mm = mm;
        if (mm.cont == Container::TABLE_64) {
          compress_n_impl(compP, tbl64_it);
          ++tbl64_it;
        }
        else if (mm.cont == Container::TABLE_32) {
          compress_n_impl(compP, tbl32_it);
          ++tbl32_it;
        }
        // Using "-O3" optimization flag of gcc, the program enters the
        // following IF, even when it shouldn't!!!  #gcc_bug
        else if (mm.cont == Container::LOG_TABLE_8) {
          compress_n_impl(compP, lgtbl8_it);
          ++lgtbl8_it;
        }
        else if (mm.cont == Container::SKETCH_8) {
          compress_n_impl(compP, cmls4_it);
          ++cmls4_it;
        }
        ++compP->ppIt;  ++compP->ctxIt;  ++compP->ctxIrIt;
      }

      const auto entr =
        entropy(compP->w.begin(), compP->probs.begin(), compP->probs.end());
      pf << entr << '\n';
      sEnt += entr;
    }
  }
  tf.close();
  pf.close();
  aveEnt = sEnt/symsNo;
}

template <typename ContIter>
inline void FCM::compress_n_impl (shared_ptr<CompressPar> compP,
                                  ContIter contIt) {
  compress_n_parent(compP, contIt);

  if (compP->mm.child) {
    ++compP->ppIt;  ++compP->ctxIt;  ++compP->ctxIrIt;
    if (compP->mm.child->enabled)  // NOT mm.child->enabled
      compress_n_child_enabled(compP, contIt);
    else
      compress_n_child_disabled(compP, contIt);
  }
}

template <typename ContIter>
inline void FCM::compress_n_parent (shared_ptr<CompressPar> compP,
                                    ContIter contIt) {
  compP->probs.clear();  // Essential
  compP->probs.reserve(compP->nMdl);

  if (compP->mm.ir == 0) {
    compP->ppIt->config(compP->c, *compP->ctxIt);
    const auto ppIt = compP->ppIt;
    const auto f = freqs<decltype((*contIt)->query(0))>(contIt, ppIt);
    compP->probs.emplace_back(prob(f.begin(), ppIt));
    update_ctx(*compP->ctxIt, ppIt);
  }
  else {
    compP->ppIt->config_ir(compP->c, *compP->ctxIt, *compP->ctxIrIt);
    const auto ppIt = compP->ppIt;
    const auto f = freqs_ir<decltype(2*(*contIt)->query(0))>(contIt, ppIt);
    compP->probs.emplace_back(prob(f.begin(), ppIt));
    update_ctx_ir(*compP->ctxIt, *compP->ctxIrIt, ppIt);
  }
}

template <typename ContIter>
inline void FCM::compress_n_child_enabled (shared_ptr<CompressPar> compP,
                                           ContIter contIt) {
  if (compP->mm.child->ir == 0) {
    compP->ppIt->config(*compP->ctxIt);  // l
    const auto f = freqs<decltype((*contIt)->query(0))>(contIt, compP->ppIt);
    const auto bestSym = best_sym(f.begin());
    compP->ppIt->config(bestSym);  // best_sym uses l
    if (compP->nSym == bestSym)
      compP->probs.emplace_back(
        stmm_hit_prob(compP->mm.child, f.begin(), compP->ppIt));
    else
      compP->probs.emplace_back(
        stmm_miss_prob(compP->mm.child, compP->nSym, f.begin(), compP->ppIt));
    update_ctx(*compP->ctxIt, compP->ppIt);
  }
  else {
    compP->ppIt->config_ir(*compP->ctxIt, *compP->ctxIrIt);  // l and r
    const auto f=freqs_ir<decltype(2*(*contIt)->query(0))>(contIt, compP->ppIt);
    const auto bestSym = best_sym(f.begin());
    compP->ppIt->config_ir(bestSym);  // best_sym uses l and r
    if (compP->nSym == bestSym)
      compP->probs.emplace_back(
        stmm_hit_prob(compP->mm.child, f.begin(), compP->ppIt));
    else
      compP->probs.emplace_back(stmm_miss_prob_ir(
        compP->mm.child, compP->nSym, f.begin(), compP->ppIt));
    update_ctx_ir(*compP->ctxIt, *compP->ctxIrIt, compP->ppIt);
  }
}

template <typename ContIter>
inline void FCM::compress_n_child_disabled (shared_ptr<CompressPar> compP,
                                            ContIter contIt) {
  if (compP->mm.child->ir == 0) {
    compP->ppIt->config(compP->c, *compP->ctxIt);
    const auto ppIt = compP->ppIt;
    const auto f = freqs<decltype((*contIt)->query(0))>(contIt, ppIt);
    update_ctx(*compP->ctxIt, ppIt);
    if (compP->nSym == best_sym_abs(f.begin())) {
      compP->mm.child->enabled = true;
      compP->probs.emplace_back(stmm_hit_prob(compP->mm.child, f.begin(),ppIt));
      fill(compP->w.begin(), compP->w.end(), 1.0/compP->nMdl);
    }
    else
      compP->probs.emplace_back(0.0);
  }
  else {
    compP->ppIt->config_ir(compP->c, *compP->ctxIt, *compP->ctxIrIt);
    const auto ppIt = compP->ppIt;
    const auto f = freqs_ir<decltype(2*(*contIt)->query(0))>(contIt, ppIt);
    update_ctx_ir(*compP->ctxIt, *compP->ctxIrIt, ppIt);
    if (compP->nSym == best_sym_abs(f.begin())) {
      compP->mm.child->enabled = true;
      compP->probs.emplace_back(stmm_hit_prob(compP->mm.child, f.begin(),ppIt));
      fill(compP->w.begin(), compP->w.end(), 1.0/compP->nMdl);
    }
    else
      compP->probs.emplace_back(0.0);
  }
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

template <typename OutT, typename ContIter, typename ProbParIter>
inline array<OutT,4> FCM::freqs (ContIter cont, ProbParIter pp) const {
  return array<OutT,4>
    {(*cont)->query(pp->l),
     (*cont)->query(pp->l | 1ull),
     (*cont)->query(pp->l | 2ull),
     (*cont)->query(pp->l | 3ull)};
}

template <typename OutT, typename ContIter, typename ProbParIter>
inline array<OutT,4> FCM::freqs_ir (ContIter cont, ProbParIter pp) const {
  return array<OutT,4>
    {static_cast<OutT>(
       (*cont)->query(pp->l)        + (*cont)->query((3ull<<pp->shl) | pp->r)),
     static_cast<OutT>(
       (*cont)->query(pp->l | 1ull) + (*cont)->query((2ull<<pp->shl) | pp->r)),
     static_cast<OutT>(
       (*cont)->query(pp->l | 2ull) + (*cont)->query((1ull<<pp->shl) | pp->r)),
     static_cast<OutT>(
       (*cont)->query(pp->l | 3ull) + (*cont)->query(pp->r))};
}

template <typename Par, typename Value>
void FCM::stmm_update_hist (Par stmm, Value val) {
  stmm->history = (stmm->history<<1u) | val;  // ull for 64 bits
}

template <typename Par, typename FreqIter, typename ProbParIter>
double FCM::stmm_hit_prob (Par stmm, FreqIter fFirst, ProbParIter pp) {
  stmm_update_hist(stmm, 0u);
  return prob(fFirst, pp);
}

template <typename Par, typename FreqIter, typename ProbParIter>
double FCM::stmm_miss_prob (Par stmm, u8 nSym, FreqIter fFirst, ProbParIter pp){
  stmm_update_hist(stmm, 1u);
  if (popcount(stmm->history) > stmm->thresh) {
    stmm->enabled = false;
    stmm->history = 0;
    pp->config(nSym);
    return 0.0;
  }
  else {
    return prob(fFirst, pp);
  }
}

template <typename Par, typename FreqIter, typename ProbParIter>
double FCM::stmm_miss_prob_ir (Par stmm, u8 nSym, FreqIter fFirst,
                               ProbParIter pp) {
  stmm_update_hist(stmm, 1u);
  if (popcount(stmm->history) > stmm->thresh) {
    stmm->enabled = false;
    stmm->history = 0;
    pp->config_ir(nSym);
    return 0.0;
  }
  else {
    return prob(fFirst, pp);
  }
}

template <typename FreqIter, typename ProbParIter>
inline double FCM::prob (FreqIter fFirst, ProbParIter pp) const {
  return (*(fFirst+pp->numSym) + pp->alpha) /
         (std::accumulate(fFirst,fFirst+CARDINALITY,0ull) + pp->sAlpha);
}

inline double FCM::entropy (double P) const {
  return -log2(P);
}

template <typename OutIter, typename InIter>
inline double FCM::entropy (OutIter wFirst, InIter PFirst, InIter PLast) const {
  update_weights(wFirst, PFirst, PLast);
  return -log2(std::inner_product(PFirst, PLast, wFirst, 0.0));
//  return log2(1 / std::inner_product(PFirst, PLast, wFirst, 0.0));
}

template <typename OutIter, typename InIter>
inline void FCM::update_weights (OutIter wFirst, InIter PFirst, InIter PLast)
const {
  const auto wFirstKeep = wFirst;
  for (auto mIter=Ms.begin(); PFirst!=PLast; ++mIter, ++wFirst, ++PFirst) {
    *wFirst = pow(*wFirst, mIter->gamma) * *PFirst;
    if (mIter->child) {
      ++wFirst;  ++PFirst;
      if (mIter->child->enabled)
        *wFirst = pow(*wFirst, mIter->child->gamma) * *PFirst;
      else
        *wFirst = 0.0;
    }
  }
  normalize(wFirstKeep, wFirst);
}

template <typename ProbParIter>
inline void FCM::update_ctx (u64& ctx, ProbParIter pp) const {
  ctx = (pp->l & pp->mask) | pp->numSym;
}

template <typename ProbParIter>
inline void FCM::update_ctx_ir (u64& ctx, u64& ctxIr, ProbParIter pp) const {
  ctx   = (pp->l & pp->mask) | pp->numSym;
  ctxIr = (pp->revNumSym<<pp->shl) | pp->r;
}