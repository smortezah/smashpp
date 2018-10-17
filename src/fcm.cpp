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
  aveEnt = static_cast<prec_t>(0);
  config(p);
  if (p.verbose)
    show_in(p);
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

inline void FCM::show_in (const Param& p) const {
  for (auto i=0u, j=0u; i != Ms.size(); ++i) {
    cerr
      << "Model " << i+1 << ':'                                          <<'\n'
      << "  [+] Context order ............ " << (int)Ms[i].k             <<'\n';
    if (Ms[i].w)  cerr
      << "  [+] Width of sketch .......... " << Ms[i].w                  <<'\n';
    if (Ms[i].d)  cerr
      << "  [+] Depth of sketch .......... " << (int)Ms[i].d             <<'\n';
    cerr
      << "  [+] Inverted repeats ......... " << (Ms[i].ir ? "yes" : "no")<<'\n'
      << "  [+] Alpha .................... " << Ms[i].alpha              <<'\n'
      << "  [+] Gamma .................... " << Ms[i].gamma              <<'\n'
                                                                         <<'\n';
    if (Ms[i].child) {
      cerr
      << "Substitutional Tolerant Model, based on Model " << i+1 << ':'  <<'\n'
      << "  [+] Substitutions allowed .... " << (int)TMs[j].thresh       <<'\n'
      << "  [+] Inverted repeats ......... " << (TMs[j].ir ? "yes":"no") <<'\n'
      << "  [+] Alpha .................... " << TMs[j].alpha             <<'\n'
      << "  [+] Gamma .................... " << TMs[j].gamma             <<'\n'
                                                                         <<'\n';
      ++j;
    }
  }
  cerr<< "Reference file:"                                               <<'\n'
      << "  [+] Name ..................... " << p.ref                    <<'\n'
      << "  [+] Size (bytes) ............. " << file_size(p.ref)         <<'\n'
                                                                         <<'\n'
      << "Target file:"                                                  <<'\n'
      << "  [+] Name ..................... " << p.tar                    <<'\n'
      << "  [+] Size (bytes) ............. " << file_size(p.tar)         <<'\n'
                                                                         <<'\n'
      << "Filter and segment:"                                           <<'\n'
      << "  [+] Windowing function ....... " << p.print_win_type()       <<'\n'
      << "  [+] Window size .............. " << p.wsize                  <<'\n'
      << "  [+] Threshold ................ " << p.thresh                 <<'\n';
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
  cerr << OUT_SEP << "Building the model" << (nMdl == 1 ? "" : "s")
       << " based on \"" << p.ref << "\" (level "
       << static_cast<u16>(p.level) << ")...\n";

  (p.nthr==1 || nMdl==1) ? store_1(p) : store_n(p)/*Mult thr*/;

  #ifdef DEBUG
//  for(auto a:tbl64)a->print();cerr<<'\n';
//  for(auto a:tbl32)a->print();cerr<<'\n';
//  for(auto a:lgtbl8)a->print();cerr<<'\n';
//  for(auto a:cmls4)a->print();cerr<<'\n';
  #endif

    cerr << "Done!\n";
}

inline void FCM::store_1 (const Param& p) {
  auto tbl64_iter=tbl64.begin();      auto tbl32_iter=tbl32.begin();
  auto lgtbl8_iter=lgtbl8.begin();    auto cmls4_iter=cmls4.begin();
  for (const auto& m : Ms) {    // Mask: 1<<2k - 1 = 4^k - 1
    if (m.cont == Container::TABLE_64)
      store_impl(p.ref, (1ul<<(m.k<<1u))-1ul  /*Mask 32*/, tbl64_iter++);
//      store_impl(p.ref, (4ul<<(m.k<<1u))-1  /*Mask 32*/, tbl64_iter++);//todo
    else if (m.cont == Container::TABLE_32)
      store_impl(p.ref, (1ul<<(m.k<<1u))-1ul  /*Mask 32*/, tbl32_iter++);
    else if (m.cont == Container::LOG_TABLE_8)
      store_impl(p.ref, (1ul<<(m.k<<1u))-1ul  /*Mask 32*/, lgtbl8_iter++);
    else if (m.cont == Container::SKETCH_8)
      store_impl(p.ref, (1ull<<(m.k<<1u))-1ull/*Mask 64*/, cmls4_iter++);
    else
      err("the models cannot be built.");
  }
}

inline void FCM::store_n (const Param& p) {
  auto tbl64_iter=tbl64.begin();      auto tbl32_iter=tbl32.begin();
  auto lgtbl8_iter=lgtbl8.begin();    auto cmls4_iter=cmls4.begin();
  const auto vThrSz = (p.nthr < Ms.size()) ? p.nthr : Ms.size();
  vector<std::thread> thrd(vThrSz);
  for (u8 i=0; i!=Ms.size(); ++i) {    // Mask: 1<<2k-1 = 4^k-1
    switch (Ms[i].cont) {
     case Container::TABLE_64:
       thrd[i % vThrSz] =
         std::thread(&FCM::store_impl<u32,decltype(tbl64_iter)>, this,
           std::cref(p.ref), (1ul<<(Ms[i].k<<1u))-1ul, tbl64_iter++);
//           std::cref(p.ref), (4ul<<(Ms[i].k<<1u))-1, tbl64_iter++);//todo
       break;
     case Container::TABLE_32:
       thrd[i % vThrSz] =
         std::thread(&FCM::store_impl<u32,decltype(tbl32_iter)>, this,
           std::cref(p.ref), (1ul<<(Ms[i].k<<1u))-1ul, tbl32_iter++);
       break;
     case Container::LOG_TABLE_8:
       thrd[i % vThrSz] =
         std::thread(&FCM::store_impl<u32,decltype(lgtbl8_iter)>, this,
           std::cref(p.ref), (1ul<<(Ms[i].k<<1u))-1ul, lgtbl8_iter++);
       break;
     case Container::SKETCH_8:
       thrd[i % vThrSz] =
         std::thread(&FCM::store_impl<u64,decltype(cmls4_iter)>, this,
           std::cref(p.ref), (1ull<<(Ms[i].k<<1u))-1ull, cmls4_iter++);
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
    if (c!='N' && c!='\n') {
//      ctx = ((ctx<<2u) & mask) | NUM[static_cast<u8>(c)];//todo
      ctx = ((ctx & mask)<<2u) | NUM[static_cast<u8>(c)];
      (*cont)->update(ctx);
    }
  }
  rf.close();
}

void FCM::compress (const Param& p) {
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

  cerr << "Done!\n";
//  if (p.verbose)//todo maybe remove comment
    cerr << "Average Entropy = "
         << std::fixed << setprecision(DEF_PRF_PREC) << aveEnt << " bps\n";
}

template <typename ContIter>
inline void FCM::compress_1 (const string& tar, const string& ref,
                             ContIter cont) {
  // Ctx, Mir (int) sliding through the dataset
  u64      ctx{0},   ctxIr{(1ull<<(Ms[0].k<<1u))-1};
  u64      symsNo{0};            // No. syms in target file, except \n
  prec_t   sEnt{0};              // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  ofstream pf(ref+"_"+tar+PRF_FMT);
  ProbPar  pp{Ms[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
              static_cast<u8>(Ms[0].k<<1u)};

  if (Ms[0].ir == 0) {
    while (tf.get(c)) {
      if (c!='N' && c!='\n') {
        ++symsNo;
        pp.config(c, ctx);
        array<decltype((*cont)->query(0)),4> f {};
        freqs(f, cont, pp.l);
//        freqs(f, cont, &pp);
        const auto entr = entropy(prob(f.begin(), &pp));
      //todo remove comment
      pf /*todo << std::fixed*/ << setprecision(DEF_PRF_PREC) << entr << '\n';
//        sEnt += ceil(entr*1000)/1000;//todo
        sEnt += entr;
        update_ctx(ctx, &pp);
      }
    }
  }
  else {  // With inv. rep.
    while (tf.get(c)) {
      if (c!='N' && c!='\n') {
        ++symsNo;
        pp.config_ir(c, ctx, ctxIr);
        array<decltype(2*(*cont)->query(0)),4> f {};
        freqs_ir(f, cont, &pp);
        const auto entr = entropy(prob(f.begin(), &pp));
      //todo remove comment
//      pf /*todo << std::fixed*/ << setprecision(DEF_PRF_PREC) << entr << '\n';
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
  cp->w.resize(nMdl, static_cast<prec_t>(1)/nMdl);
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
  prec_t   sEnt{0};            // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  ofstream pf(ref+"_"+tar+PRF_FMT);

  while (tf.get(c)) {
    if (c!='N' && c!='\n') {
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
        if (mm.cont == Container::TABLE_64)
          compress_n_impl(cp, tbl64_it++);
        else if (mm.cont == Container::TABLE_32)
          compress_n_impl(cp, tbl32_it++);
        // Using "-O3" optimization flag of gcc, the program may enter the
        // following IF, even when it shouldn't!!!  #gcc_bug
        else if (mm.cont == Container::LOG_TABLE_8)
          compress_n_impl(cp, lgtbl8_it++);
        else if (mm.cont == Container::SKETCH_8)
          compress_n_impl(cp, cmls4_it++);

        ++cp->ppIt;  ++cp->ctxIt;  ++cp->ctxIrIt;
      }

      const auto entr=entropy(cp->w.begin(), cp->probs.begin(),cp->probs.end());
      //todo remove comment
      pf /*todo << std::fixed*/ << setprecision(DEF_PRF_PREC) << entr << '\n';
      update_weights(cp->w.begin(), cp->probs.begin(), cp->probs.end());
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
//    ++cp->ppIt;  ++cp->ctxIt;  ++cp->ctxIrIt;//todo
    compress_n_child(cp, contIt);//todo
//    if (cp->mm.child->enabled)
//      compress_n_child_enabled(cp, contIt);
//    else
//      compress_n_child_disabled(cp, contIt);
  }
}

template <typename ContIter>
inline void FCM::compress_n_parent(shared_ptr<CompressPar> cp, ContIter contIt){
  if (cp->mm.ir == 0) {
    cp->ppIt->config(cp->c, *cp->ctxIt);
    const auto ppIt = cp->ppIt;
    array<decltype((*contIt)->query(0)),4> f {};
    freqs(f, contIt, ppIt->l);
//    freqs(f, contIt, ppIt);
    cp->probs.emplace_back(prob(f.begin(), ppIt));
    update_ctx(*cp->ctxIt, ppIt);
  }
  else {
    cp->ppIt->config_ir(cp->c, *cp->ctxIt, *cp->ctxIrIt);
    const auto ppIt = cp->ppIt;
    array<decltype(2*(*contIt)->query(0)),4> f {};
    freqs_ir(f, contIt, ppIt);
    cp->probs.emplace_back(prob(f.begin(), ppIt));
    update_ctx_ir(*cp->ctxIt, *cp->ctxIrIt, ppIt);
  }
}

//todo
template <typename ContIter>
inline void FCM::compress_n_child (shared_ptr<CompressPar> cp, ContIter contIt){
  ++cp->ppIt;  ++cp->ctxIt;  ++cp->ctxIrIt;//todo

  //if(IR==0) //todo
  cp->ppIt->config(cp->c, *cp->ctxIt);
  array<decltype((*contIt)->query(0)),4> f {};
  freqs(f, contIt, cp->ppIt->l);

  if (!cp->mm.child->enabled)
    cp->probs.emplace_back(static_cast<prec_t>(0));
  else
    cp->probs.emplace_back(prob(f.begin(), cp->ppIt));

  correct_stmm(f.begin(), cp->mm.child, cp->nSym, cp->ppIt);

  update_ctx(*cp->ctxIt, cp->ppIt);
}



template <typename ContIter>
inline void FCM::compress_n_child_enabled (shared_ptr<CompressPar> cp,
                                           ContIter contIt) {
  if (cp->mm.child->ir == 0) {
    cp->ppIt->config(cp->c, *cp->ctxIt);
    array<decltype((*contIt)->query(0)),4> f {};
    freqs(f, contIt, cp->ppIt->l);
//    freqs(f, contIt, cp->ppIt);
    const auto best = best_id(f.begin());
    if (best == static_cast<u8>(255)) {
      cp->probs.emplace_back(miss_prob_stmm(cp->mm.child, f.begin(), cp->ppIt));
    }
    else if (best==static_cast<u8>(254) || best==cp->nSym) {
      cp->probs.emplace_back(hit_prob_stmm(cp->mm.child, f.begin(), cp->ppIt));
    }
    else {
//      cp->mm.child->history = 0;
//      cp->probs.emplace_back(prob(f.begin(), cp->ppIt));
      cp->probs.emplace_back(miss_prob_stmm(cp->mm.child, f.begin(), cp->ppIt));
      cp->ppIt->config(best);
    }
    update_ctx(*cp->ctxIt, cp->ppIt);
  }
  else {//todoo modify based on non ir
    cp->ppIt->config_ir(cp->c, *cp->ctxIt, *cp->ctxIrIt);  // l and r
    array<decltype(2*(*contIt)->query(0)),4> f {};
    freqs_ir(f, contIt, cp->ppIt);
    const auto best = best_id(f.begin());
    if (best == static_cast<u8>(255)) {
      cp->probs.emplace_back(miss_prob_stmm(cp->mm.child, f.begin(), cp->ppIt));
    }
    else if (best==static_cast<u8>(254) || best==cp->nSym) {
      cp->probs.emplace_back(hit_prob_stmm(cp->mm.child, f.begin(), cp->ppIt));
    }
    else {
      cp->probs.emplace_back(prob(f.begin(), cp->ppIt));
      cp->ppIt->config_ir(best);
    }
    update_ctx_ir(*cp->ctxIt, *cp->ctxIrIt, cp->ppIt);
  }
}

template <typename ContIter>
inline void FCM::compress_n_child_disabled (shared_ptr<CompressPar> cp,
                                            ContIter contIt) {
  if (cp->mm.child->ir == 0) {
    cp->ppIt->config(cp->c, *cp->ctxIt);
    const auto ppIt = cp->ppIt;
    array<decltype((*contIt)->query(0)),4> f {};
    freqs(f, contIt, ppIt->l);
//    freqs(f, contIt, ppIt);
    const auto best = best_id(f.begin());
    if (best==static_cast<u8>(255) || best==static_cast<u8>(254)) {
      cp->probs.emplace_back(static_cast<prec_t>(0));
    }
//    else if (best == cp->nSym) {
//      cp->mm.child->enabled = true;
//      cp->mm.child->history = 0;
//      cp->probs.emplace_back(prob(f.begin(), ppIt));
////      cp->probs.emplace_back(stmm_miss_prob(cp->mm.child, f.begin(), cp->ppIt));
//      fill(cp->w.begin(), cp->w.end(), static_cast<prec_t>(1)/cp->nMdl);
//    }
    else {
      cp->mm.child->enabled = true;
#ifdef ARRAY_HISTORY
      std::fill(cp->mm.child->history.begin(), cp->mm.child->history.end(),
                false);
#else
      cp->mm.child->history = 0;
#endif
      cp->probs.emplace_back(prob(f.begin(), ppIt));
//      cp->probs.emplace_back(hit_prob_stmm(cp->mm.child, f.begin(), cp->ppIt));
//      cp->probs.emplace_back(miss_prob_stmm(cp->mm.child, f.begin(), cp->ppIt));
      fill(cp->w.begin(), cp->w.end(), static_cast<prec_t>(1)/cp->nMdl);
    }
    update_ctx(*cp->ctxIt, ppIt);
  }
  else {//todoo modify based on non ir
    cp->ppIt->config_ir(cp->c, *cp->ctxIt, *cp->ctxIrIt);
    const auto ppIt = cp->ppIt;
    array<decltype(2*(*contIt)->query(0)),4> f {};
    freqs_ir(f, contIt, ppIt);
    const auto best = best_id(f.begin());
    if (best==static_cast<u8>(255) || best==static_cast<u8>(254)) {
      cp->probs.emplace_back(static_cast<prec_t>(0));
    }
    else {
      cp->mm.child->enabled = true;
      cp->probs.emplace_back(prob(f.begin(), ppIt));
      fill(cp->w.begin(), cp->w.end(), static_cast<prec_t>(1)/cp->nMdl);
    }
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

//todo check performance
//template <typename OutT, typename ContIter, typename ProbParIter>
//inline void FCM::freqs (array<OutT,4>& a, ContIter cont, ProbParIter pp) const {
//  a = {(*cont)->query(pp->l),
//       (*cont)->query(pp->l | 1ull),
//       (*cont)->query(pp->l | 2ull),
//       (*cont)->query(pp->l | 3ull)};
//}
template <typename OutT, typename ContIter>
inline void FCM::freqs (array<OutT,4>& a, ContIter cont, u64 l) const {
  a = {(*cont)->query(l),
       (*cont)->query(l | 1ull),
       (*cont)->query(l | 2ull),
       (*cont)->query(l | 3ull)};
}

template <typename OutT, typename ContIter, typename ProbParIter>
inline void FCM::freqs_ir (array<OutT,4>& a, ContIter cont, ProbParIter pp)
const {
  a = {static_cast<OutT>(
        (*cont)->query(pp->l)        + (*cont)->query((3ull<<pp->shl) | pp->r)),
       static_cast<OutT>(
        (*cont)->query(pp->l | 1ull) + (*cont)->query((2ull<<pp->shl) | pp->r)),
       static_cast<OutT>(
        (*cont)->query(pp->l | 2ull) + (*cont)->query((1ull<<pp->shl) | pp->r)),
       static_cast<OutT>(
        (*cont)->query(pp->l | 3ull) + (*cont)->query(pp->r))};
}

template <typename FreqIter, typename ParIter, typename ProbParIter>
inline void FCM::correct_stmm (FreqIter first, ParIter stmm, u8 nSym,
                               ProbParIter pp) {
  const auto best = best_id(first);
  if (best == static_cast<u8>(255)) {
    if (stmm->enabled)
      miss_stmm(stmm);
  }
  else if (best == static_cast<u8>(254)) {
    if (stmm->enabled)
      hit_stmm(stmm);
  }
  else {
    if (!stmm->enabled) {
      stmm->enabled = true;
      stmm->history = 0u;
    }
    else {
      if (best == nSym)
        hit_stmm(stmm);
      else {
        miss_stmm(stmm);
        pp->config(best);
      }
    }
  }
}

template <typename FreqIter>
inline u8 FCM::best_id (FreqIter first) const {
//  if (are_all(first, 0)) {
//  if (are_all(first, 1)) {
  if (are_all(first, 0) || are_all(first, 1)) {
    return static_cast<u8>(255);
  }
  const auto maxPos = std::max_element(first, first+CARDIN);
  if (has_multi_max(first, maxPos)) {
    return static_cast<u8>(254);
  }
  return maxPos-first;
}

#ifdef ARRAY_HISTORY
template <typename Hist, typename Value>
inline void FCM::update_hist_stmm (Hist& history, Value val) {
  std::rotate(history.begin(), history.begin()+1, history.end());
  history.back() = val;
}

template <typename Par, typename FreqIter, typename ProbParIter>
inline prec_t FCM::hit_prob_stmm (Par stmm, FreqIter fFirst, ProbParIter pp) {
  stmm_update_hist(stmm->history, false);
  return prob(fFirst, pp);
}

template <typename Par, typename FreqIter, typename ProbParIter>
inline prec_t FCM::miss_prob_stmm (Par stmm, FreqIter fFirst, ProbParIter pp) {
  if (pop_count(stmm->history.begin(),stmm->k) > stmm->thresh) {
    stmm->enabled = false;
    return static_cast<prec_t>(0);
  }
  else {
    stmm_update_hist(stmm->history, true);
    return prob(fFirst, pp);
  }
}
#else
template <typename Hist, typename Value>
inline void FCM::update_hist_stmm (Hist& history, Value val, u32 mask) {
  history = ((history<<1u) | val) & mask;  // ull for 64 bits
}

//todo
template <typename Par>
inline void FCM::hit_stmm (Par stmm) {
  update_hist_stmm(stmm->history, 0u, stmm->mask);
}

template <typename Par>
inline void FCM::miss_stmm (Par stmm) {
  if (pop_count(stmm->history) > stmm->thresh)
    stmm->enabled = false;
  else
    update_hist_stmm(stmm->history, 1u, stmm->mask);
}



template <typename Par, typename FreqIter, typename ProbParIter>
inline prec_t FCM::hit_prob_stmm (Par stmm, FreqIter fFirst, ProbParIter pp) {
  update_hist_stmm(stmm->history, 0u, stmm->mask);
  return prob(fFirst, pp);
}

template <typename Par, typename FreqIter, typename ProbParIter>
inline prec_t FCM::miss_prob_stmm (Par stmm, FreqIter fFirst, ProbParIter pp) {
  if (pop_count(stmm->history) > stmm->thresh) {
    stmm->enabled = false;
    return static_cast<prec_t>(0);
  }
  else {
    update_hist_stmm(stmm->history, 1u, stmm->mask);
    return prob(fFirst, pp);
  }
}
#endif

template <typename FreqIter, typename ProbParIter>
inline prec_t FCM::prob (FreqIter fFirst, ProbParIter pp) const {
//  return (*(fFirst+pp->numSym) + pp->alpha) /
//         std::accumulate(fFirst, fFirst+CARDIN, pp->sAlpha);
  return (*(fFirst+pp->numSym) + pp->alpha) /
         (std::accumulate(fFirst,fFirst+CARDIN,0ull) + pp->sAlpha);
}

inline prec_t FCM::entropy (prec_t P) const {
  return -log2(P);
}

template <typename OutIter, typename InIter>
inline prec_t FCM::entropy (OutIter wFirst, InIter PFirst, InIter PLast) const {
  return log2(1 / inner_product(PFirst, PLast, wFirst, static_cast<prec_t>(0)));
//  return -log2(inner_product(PFirst, PLast, wFirst, static_cast<prec_t>(0)));
}

template <typename OutIter, typename InIter>
inline void FCM::update_weights (OutIter wFirst, InIter PFirst, InIter PLast)
const {
  const auto wFirstKeep = wFirst;
  for (auto mIter=Ms.begin(); PFirst!=PLast; ++mIter, ++wFirst, ++PFirst) {
    *wFirst = pow(*wFirst, mIter->gamma) * *PFirst;
    if (mIter->child) {
      ++wFirst;  ++PFirst;
//      if (mIter->child->enabled)//todo maybe not needed
        *wFirst = pow(*wFirst, mIter->child->gamma) * *PFirst;
//      else//todo maybe not needed
//        *wFirst = static_cast<prec_t>(0);
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
