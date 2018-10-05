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
    vector<string> m;       split(m_tm[0].begin(), m_tm[0].end(), ',', m);
    if (m.size() == 4) {
      if (stoi(m[0]) > K_MAX_LGTBL8)
        Ms.emplace_back(
          MMPar(static_cast<u8>(stoi(m[0])), DEF_W, DEF_D,
                static_cast<u8>(stoi(m[1])), stof(m[2]), stof(m[3])));
      else
        Ms.emplace_back(
          MMPar(static_cast<u8>(stoi(m[0])), static_cast<u8>(stoi(m[1])),
                stof(m[2]), stof(m[3])));
    }
    else if (m.size() == 6){
      Ms.emplace_back(
        MMPar(static_cast<u8>(stoi(m[0])), pow2(stoull(m[1])),
              static_cast<u8>(stoi(m[2])), static_cast<u8>(stoi(m[3])),
              stof(m[4]), stof(m[5])));
    }
    
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
  const auto t0{now()};
  const auto nMdl = Ms.size();
  cerr << OUT_SEP << "Building " << (p.verbose ? to_string(nMdl) : "the")
       << " model" << (nMdl == 1 ? "" : "s") << " based on \"" << p.ref << "\""
       << " (level " << static_cast<u16>(p.level) << ")...\n";

  (p.nthr==1 || nMdl==1) ? store_1(p) : store_n(p)/*Mult thr*/;

  #ifdef DEBUG
//  for(auto a:tbl64)a->print();cerr<<'\n';
//  for(auto a:tbl32)a->print();cerr<<'\n';
//  for(auto a:lgtbl8)a->print();cerr<<'\n';
//  for(auto a:cmls4)a->print();cerr<<'\n';
  #endif

  cerr << "Finished in ";
  const auto t1{now()};
  hms(t1-t0);
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
  const auto t0{now()};
  cerr << OUT_SEP << "Compressing \"" << p.tar << "\"...\n";

  if (Ms.size()==1 && TMs.empty())  // 1 MM
    switch (Ms[0].cont) {
     case Container::TABLE_64:    compress_1(p.tar,p.ref,tbl64.begin());  break;
     case Container::TABLE_32:    compress_1(p.tar,p.ref,tbl32.begin());  break;
     case Container::LOG_TABLE_8: compress_1(p.tar,p.ref,lgtbl8.begin()); break;
     case Container::SKETCH_8:    compress_1(p.tar,p.ref,cmls4.begin());  break;
    }
  else
    compress_n(p.tar, p.ref);

  cerr << "Finished in ";
  const auto t1{now()};
  hms(t1-t0);

  if (p.verbose)
    cerr << "Average Entropy (H) = " << aveEnt << " bps\n";
}

template <typename ContIter>
inline void FCM::compress_1 (const string& tar, const string& ref,
                             ContIter cont) {
  // Ctx, Mir (int) sliding through the dataset
  u64      ctx{0},   ctxIr{(1ull<<(Ms[0].k<<1u))-1};
  u64      symsNo{0};            // No. syms in target file, except \n
  double   sEnt{0};              // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  ofstream pf(ref+"_"+tar+PROFILE_FMT);
  ProbPar  pp{Ms[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
              static_cast<u8>(Ms[0].k<<1u)};

  if (Ms[0].ir == 0) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pp.config(c, ctx);
        const auto f = freqs<decltype((*cont)->query(0))>(cont, &pp);
        const auto entr = entropy(prob(f.begin(), &pp));
        pf /*todo << std::fixed*/ << setprecision(DEF_PRF_PREC) << entr << '\n';
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
        pf /*todo << std::fixed*/ << setprecision(DEF_PRF_PREC) << entr << '\n';
        sEnt += entr;
        update_ctx_ir(ctx, ctxIr, &pp);
      }
    }
  }
  tf.close();
  pf.close();
  aveEnt = sEnt/symsNo;
}

inline void FCM::compress_n (const string& tar, const string& ref) {
  auto cp = make_shared<CompressPar>();
  // Ctx, Mir (int) sliding through the dataset
  const auto nMdl = static_cast<u8>(Ms.size() + TMs.size());
  cp->nMdl = nMdl;
  cp->ctx.resize(nMdl);     // Fill with zeros (resize)
  cp->ctxIr.reserve(nMdl);
  for (const auto& mm : Ms) {  // Mask: 1<<2k - 1 = 4^k - 1
    cp->ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
    if (mm.child)
      cp->ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
  }
  cp->w.resize(nMdl, 1.0/nMdl);
  cp->pp.reserve(nMdl);
  auto maskIter = cp->ctxIr.begin();
  for (const auto& mm : Ms) {
    cp->pp.emplace_back(mm.alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
    if (mm.child)
      cp->pp.emplace_back(
        mm.child->alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
  }

  compress_n_ave(tar, ref, cp);
}

inline void FCM::compress_n_ave (const string &tar, const string& ref,
                                 shared_ptr<CompressPar> cp){
  u64      symsNo{0};          // No. syms in target file, except \n
  double   sEnt{0};            // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  ofstream pf(ref+"_"+tar+PROFILE_FMT);

  while (tf.get(c)) {
    if (c != '\n') {
      ++symsNo;
      cp->c       = c;
      cp->nSym    = NUM[static_cast<u8>(c)];
      cp->ppIt    = cp->pp.begin();
      cp->ctxIt   = cp->ctx.begin();
      cp->ctxIrIt = cp->ctxIr.begin();
      cp->probs.clear();                 cp->probs.reserve(cp->nMdl);
      auto tbl64_it=tbl64.begin();       auto tbl32_it=tbl32.begin();
      auto lgtbl8_it=lgtbl8.begin();     auto cmls4_it=cmls4.begin();

      for (const auto& mm : Ms) {
        cp->mm = mm;
        if (mm.cont == Container::TABLE_64) {
          compress_n_impl(cp, tbl64_it);
          ++tbl64_it;
        }
        else if (mm.cont == Container::TABLE_32) {
          compress_n_impl(cp, tbl32_it);
          ++tbl32_it;
        }
        // Using "-O3" optimization flag of gcc, the program enters the
        // following IF, even when it shouldn't!!!  #gcc_bug
        else if (mm.cont == Container::LOG_TABLE_8) {
          compress_n_impl(cp, lgtbl8_it);
          ++lgtbl8_it;
        }
        else if (mm.cont == Container::SKETCH_8) {
          compress_n_impl(cp, cmls4_it);
          ++cmls4_it;
        }
        ++cp->ppIt;  ++cp->ctxIt;  ++cp->ctxIrIt;
      }

      const auto entr=entropy(cp->w.begin(), cp->probs.begin(),cp->probs.end());
      update_weights(cp->w.begin(), cp->probs.begin(), cp->probs.end());
      pf /*todo << std::fixed*/ << setprecision(DEF_PRF_PREC) << entr << '\n';
      sEnt += entr;
    }
  }
  tf.close();
  pf.close();
  aveEnt = sEnt/symsNo;
}

template <typename ContIter>
inline void FCM::compress_n_impl (shared_ptr<CompressPar> cp, ContIter contIt) {
  compress_n_parent(cp, contIt);

  if (cp->mm.child) {
    ++cp->ppIt;  ++cp->ctxIt;  ++cp->ctxIrIt;
    if (cp->mm.child->enabled)  // NOT mm.child->enabled
      compress_n_child_enabled(cp, contIt);
    else
      compress_n_child_disabled(cp, contIt);
  }
}

template <typename ContIter>
inline void FCM::compress_n_parent(shared_ptr<CompressPar> cp, ContIter contIt){
  if (cp->mm.ir == 0) {
    cp->ppIt->config(cp->c, *cp->ctxIt);
    const auto ppIt = cp->ppIt;
    const auto f = freqs<decltype((*contIt)->query(0))>(contIt, ppIt);
    cp->probs.emplace_back(prob(f.begin(), ppIt));
    update_ctx(*cp->ctxIt, ppIt);
  }
  else {
    cp->ppIt->config_ir(cp->c, *cp->ctxIt, *cp->ctxIrIt);
    const auto ppIt = cp->ppIt;
    const auto f = freqs_ir<decltype(2*(*contIt)->query(0))>(contIt, ppIt);
    cp->probs.emplace_back(prob(f.begin(), ppIt));
    update_ctx_ir(*cp->ctxIt, *cp->ctxIrIt, ppIt);
  }
}

template <typename ContIter>
inline void FCM::compress_n_child_enabled (shared_ptr<CompressPar> cp,
                                           ContIter contIt) {
  if (cp->mm.child->ir == 0) {
    cp->ppIt->config(cp->c, *cp->ctxIt);
    const auto f = freqs<decltype((*contIt)->query(0))>(contIt, cp->ppIt);
    const auto val = f[cp->nSym];
    const auto max_pos = std::max_element(f.begin(), f.end());
    if (val==*max_pos && val!=0)
      cp->probs.emplace_back(stmm_hit_prob(cp->mm.child, f.begin(), cp->ppIt));
    else
      cp->probs.emplace_back(stmm_miss_prob(cp->mm.child, f.begin(), cp->ppIt));
    if (cp->mm.child->enabled)
      cp->ppIt->config(static_cast<u8>(max_pos-f.begin()));  // Uses l
    update_ctx(*cp->ctxIt, cp->ppIt);
  }
  else {
    cp->ppIt->config_ir(cp->c, *cp->ctxIt, *cp->ctxIrIt);  // l and r
    const auto f = freqs_ir<decltype(2*(*contIt)->query(0))>(contIt, cp->ppIt);
    const auto val = f[cp->nSym];
    const auto max_pos = std::max_element(f.begin(), f.end());
    if (val==*max_pos && val!=0)
      cp->probs.emplace_back(stmm_hit_prob(cp->mm.child, f.begin(), cp->ppIt));
    else
      cp->probs.emplace_back(stmm_miss_prob(cp->mm.child, f.begin(), cp->ppIt));
    if (cp->mm.child->enabled)
      cp->ppIt->config_ir(static_cast<u8>(max_pos-f.begin()));  // Uses l
    update_ctx_ir(*cp->ctxIt, *cp->ctxIrIt, cp->ppIt);
  }
}

template <typename ContIter>
inline void FCM::compress_n_child_disabled (shared_ptr<CompressPar> cp,
                                            ContIter contIt) {
  if (cp->mm.child->ir == 0) {
    cp->ppIt->config(cp->c, *cp->ctxIt);
    const auto ppIt = cp->ppIt;
    const auto f = freqs<decltype((*contIt)->query(0))>(contIt, ppIt);
    if (cp->nSym == best_sym_abs(f.begin())) {
      cp->mm.child->enabled = true;
      cp->probs.emplace_back(stmm_hit_prob(cp->mm.child, f.begin(), ppIt));
      fill(cp->w.begin(), cp->w.end(), 1.0/cp->nMdl);
    }
    else
      cp->probs.emplace_back(0.0);
    update_ctx(*cp->ctxIt, ppIt);
  }
  else {
    cp->ppIt->config_ir(cp->c, *cp->ctxIt, *cp->ctxIrIt);
    const auto ppIt = cp->ppIt;
    const auto f = freqs_ir<decltype(2*(*contIt)->query(0))>(contIt, ppIt);
    if (cp->nSym == best_sym_abs(f.begin())) {
      cp->mm.child->enabled = true;
      cp->probs.emplace_back(stmm_hit_prob(cp->mm.child, f.begin(), ppIt));
      fill(cp->w.begin(), cp->w.end(), 1.0/cp->nMdl);
    }
    else
      cp->probs.emplace_back(0.0);
    update_ctx_ir(*cp->ctxIt, *cp->ctxIrIt, ppIt);
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
  stmm->history = ((stmm->history<<1u) | val) & stmm->mask;  // ull for 64 bits
}

template <typename Par, typename FreqIter, typename ProbParIter>
double FCM::stmm_hit_prob (Par stmm, FreqIter fFirst, ProbParIter pp) {
  stmm_update_hist(stmm, 0u);
  return prob(fFirst, pp);
}

template <typename Par, typename FreqIter, typename ProbParIter>
double FCM::stmm_miss_prob (Par stmm, FreqIter fFirst, ProbParIter pp) {
  if (popcount(stmm->history) > stmm->thresh) {
    stmm->enabled = false;
    stmm->history = 0;
    return 0.0;
  }
  else {
    stmm_update_hist(stmm, 1u);
    return prob(fFirst, pp);
  }
}

template <typename FreqIter, typename ProbParIter>
inline double FCM::prob (FreqIter fFirst, ProbParIter pp) const {
  return (*(fFirst+pp->numSym) + pp->alpha) /
         std::accumulate(fFirst, fFirst+CARDINALITY, pp->sAlpha);
///  return (*(fFirst+pp->numSym) + pp->alpha) /
///         (std::accumulate(fFirst,fFirst+CARDINALITY,0ull) + pp->sAlpha);
}

inline double FCM::entropy (double P) const {
  return -log2(P);
}

template <typename OutIter, typename InIter>
inline double FCM::entropy (OutIter wFirst, InIter PFirst, InIter PLast) const {
  return -log2(std::inner_product(PFirst, PLast, wFirst, 0.0));
///  return log2(1 / std::inner_product(PFirst, PLast, wFirst, 0.0));
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