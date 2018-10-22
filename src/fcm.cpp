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
    switch (m.cont) {
    case Container::SKETCH_8:
      cmls4.emplace_back(make_shared<CMLS4>(m.w, m.d));    break;
    case Container::LOG_TABLE_8:
      lgtbl8.emplace_back(make_shared<LogTable8>(m.k));    break;
    case Container::TABLE_32:
      tbl32.emplace_back(make_shared<Table32>(m.k));       break;
    case Container::TABLE_64:
      tbl64.emplace_back(make_shared<Table64>(m.k));       break;
    }
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
    switch (m.cont) {
    case Container::SKETCH_8:
      store_impl(p.ref, (1ull<<(m.k<<1u))-1ull/*Mask 64*/, cmls4_iter++); break;
    case Container::LOG_TABLE_8:
      store_impl(p.ref, (1ul<<(m.k<<1u))-1ul /*Mask 32*/, lgtbl8_iter++); break;
    case Container::TABLE_32:
      store_impl(p.ref, (1ul<<(m.k<<1u))-1ul /*Mask 32*/, tbl32_iter++);  break;
    case Container::TABLE_64:
      store_impl(p.ref, (1ul<<(m.k<<1u))-1ul /*Mask 32*/, tbl64_iter++);  break;
    default:
      err("the models cannot be built.");
    }
  }
}

inline void FCM::store_n (const Param& p) {
  auto tbl64_iter=tbl64.begin();      auto tbl32_iter=tbl32.begin();
  auto lgtbl8_iter=lgtbl8.begin();    auto cmls4_iter=cmls4.begin();
  const auto vThrSz = (p.nthr < Ms.size()) ? p.nthr : Ms.size();
  vector<std::thread> thrd(vThrSz);
  for (u8 i=0; i!=Ms.size(); ++i) {    // Mask: 1<<2k-1 = 4^k-1
    switch (Ms[i].cont) {
    case Container::SKETCH_8:
      thrd[i % vThrSz] = std::thread(&FCM::store_impl<u64,decltype(cmls4_iter)>,
        this, std::cref(p.ref), (1ull << (Ms[i].k << 1u)) - 1ull, cmls4_iter++);
      break;
    case Container::LOG_TABLE_8:
      thrd[i % vThrSz] =std::thread(&FCM::store_impl<u32,decltype(lgtbl8_iter)>,
        this, std::cref(p.ref), (1ul << (Ms[i].k << 1u)) - 1ul, lgtbl8_iter++);
      break;
    case Container::TABLE_32:
      thrd[i % vThrSz] = std::thread(&FCM::store_impl<u32,decltype(tbl32_iter)>,
        this, std::cref(p.ref), (1ul << (Ms[i].k << 1u)) - 1ul, tbl32_iter++);
      break;
    case Container::TABLE_64:
      thrd[i % vThrSz] = std::thread(&FCM::store_impl<u32,decltype(tbl64_iter)>,
        this, std::cref(p.ref), (1ul << (Ms[i].k << 1u)) - 1ul, tbl64_iter++);
      break;
    default:
      err("the models cannot be built.");
    }
    // Join
    if ((i+1) % vThrSz == 0)
      for (auto& t : thrd)  if (t.joinable()) t.join();
  }
  for (auto& t : thrd)  if (t.joinable()) t.join();  // Join leftover threads
}

template <typename Mask, typename ContIter /*Container iterator*/>
inline void FCM::store_impl (const string& ref, Mask mask, ContIter cont) {
//  ifstream rf(ref);
//  char* buffer = new char[FILE_BUF];

  char buffer[FILE_BUF];
  FILE* rf = fopen(ref.c_str(), "r");

//  for (Mask ctx=0; rf.read(buffer,sizeof(buffer));) {
//  for (int i=0; i!=sizeof(buffer); ++i) {
  Mask ctx=0;
    for (size_t readSize=0; (readSize = fread(buffer,1,FILE_BUF,rf));) {
    for(int i=0; i!=readSize; ++i) {
    const char c = buffer[i];
    if (c!='N' && c!='\n') {
      ctx = ((ctx & mask)<<2u) | NUM[static_cast<u8>(c)];
      (*cont)->update(ctx);
    }
  }
  }
//  // Slower
////  for (Mask ctx=0; rf.get(c);) {
////    if (c!='N' && c!='\n') {
////      ctx = ((ctx & mask)<<2u) | NUM[static_cast<u8>(c)];
////      (*cont)->update(ctx);
////    }
////  }
fclose(rf);
//  delete[] buffer;
//  rf.close();
}

void FCM::compress (const Param& p) {
  cerr << OUT_SEP << "Compressing \"" << p.tar << "\"...\n";

  if (Ms.size()==1 && TMs.empty())  // 1 MM
    switch (Ms[0].cont) {
    case Container::SKETCH_8:    compress_1(p.tar,p.ref,cmls4.begin());   break;
    case Container::LOG_TABLE_8: compress_1(p.tar,p.ref,lgtbl8.begin());  break;
    case Container::TABLE_32:    compress_1(p.tar,p.ref,tbl32.begin());   break;
    case Container::TABLE_64:    compress_1(p.tar,p.ref,tbl64.begin());   break;
    }
  else
    compress_n(p.tar, p.ref);

  cerr << "Done!\n";
  if (p.verbose)
    cerr << "Average Entropy = "
         << std::fixed << setprecision(DEF_PRF_PREC) << aveEnt << " bps\n";
}

template <typename ContIter>
inline void FCM::compress_1
(const string& tar, const string& ref, ContIter cont) {
  // Ctx, Mir (int) sliding through the dataset
  u64      ctx{0},   ctxIr{(1ull<<(Ms[0].k<<1u))-1};
  u64      symsNo{0};            // No. syms in target file, except \n
  prec_t   sumEnt{0};            // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(tar);  char c;
  ofstream pf(ref+"_"+tar+PRF_FMT);
  ProbPar  pp{Ms[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
              static_cast<u8>(Ms[0].k<<1u)};
//  char* buffer = new char[FILE_BUF];
  char buffer[FILE_BUF];

  FILE* tf = fopen(tar.c_str(), "r");

  if (Ms[0].ir == 0) {
//    while (tf.read(buffer,FILE_BUF)) {
    for (size_t readSize=0; (readSize = fread(buffer,1,FILE_BUF,tf));) {
//      while (tf.read(buffer,sizeof(buffer))) {
//      for(int i=0; i!= sizeof(buffer); ++i) {
        for(int i=0; i!=readSize; ++i) {
      const char c = buffer[i];
//    while (tf.get(c)) {
      if (c!='N' && c!='\n') {
        ++symsNo;
        pp.config(c, ctx);
        array<decltype((*cont)->query(0)),4> f {};
        freqs(f, cont, pp.l);
        const auto entr = entropy(prob(f.begin(), &pp));
        pf /*<< std::fixed*/ << setprecision(DEF_PRF_PREC) << entr << '\n';
        sumEnt += entr;
        update_ctx(ctx, &pp);
      }
      }
    }
  }//todo
//  else {  // With inv. rep.
//    while (tf.read(buffer,sizeof(buffer))) {
//      for(int i=0; i!= sizeof(buffer); ++i) {
//      const char c = buffer[i];
////    while (tf.get(c)) {
//      if (c!='N' && c!='\n') {
//        ++symsNo;
//        pp.config_ir(c, ctx, ctxIr);
//        array<decltype(2*(*cont)->query(0)),4> f {};
//        freqs_ir(f, cont, &pp);
//        const auto entr = entropy(prob(f.begin(), &pp));
//        pf /*<< std::fixed*/ << setprecision(DEF_PRF_PREC) << entr << '\n';
//        sumEnt += entr;
//        update_ctx_ir(ctx, ctxIr, &pp);
//      }
//      }
//    }
//  }
//  delete[] buffer;
//  tf.close();
fclose(tf);
  pf.close();
  aveEnt = sumEnt/symsNo;
}

inline void FCM::compress_n (const string& tar, const string& ref) {
  u64 symsNo{0};               // No. syms in target file, except \n
  prec_t sumEnt{0};            // Sum of entropies = sum(log_2 P(s|c^t))
//  ifstream tf(tar);  //char c;
  ofstream pf(ref+"_"+tar+PRF_FMT);
  auto cp = make_shared<CompressPar>();
  // Ctx, Mir (int) sliding through the dataset
  const auto nMdl = static_cast<u8>(Ms.size() + TMs.size());
  cp->nMdl = nMdl;
  cp->ctx.resize(nMdl);        // Fill with zeros (resize)
  cp->ctxIr.reserve(nMdl);
  for (const auto& mm : Ms) {  // Mask: 1<<2k - 1 = 4^k - 1
    cp->ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
    if (mm.child)  cp->ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
  }
  cp->w.resize(nMdl, static_cast<prec_t>(1)/nMdl);
  cp->wNext.resize(nMdl, static_cast<prec_t>(0));
  cp->pp.reserve(nMdl);
  auto maskIter = cp->ctxIr.begin();
  for (const auto& mm : Ms) {
    cp->pp.emplace_back(mm.alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
    if (mm.child)  cp->pp.emplace_back(
                     mm.child->alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
  }
//  char* buffer = new char[FILE_BUF];
  char buffer[FILE_BUF];
  FILE* tf = fopen(tar.c_str(), "r");

//  while (tf.read(buffer,sizeof(buffer))) {
//  for(int i=0; i!= sizeof(buffer); ++i) {
    for (size_t readSize=0; (readSize = fread(buffer,1,FILE_BUF,tf));) {
    for(int i=0; i!=readSize; ++i) {
    const char c = buffer[i];
//  while (tf.get(c)) { // Slower
    if (c!='N' && c!='\n') {
      ++symsNo;
      cp->c=c;                        cp->nSym=NUM[static_cast<u8>(c)];
      cp->ppIt=cp->pp.begin();
      cp->ctxIt=cp->ctx.begin();      cp->ctxIrIt=cp->ctxIr.begin();
      cp->probs.clear();              cp->probs.reserve(nMdl);
      auto tbl64_it=tbl64.begin();    auto tbl32_it=tbl32.begin();
      auto lgtbl8_it=lgtbl8.begin();  auto cmls4_it=cmls4.begin();

      u8 n = 0;  // Counter for the models
      for (const auto& mm : Ms) {
        cp->mm = mm;
        switch (mm.cont) {
         case Container::SKETCH_8:    compress_n_impl(cp, cmls4_it++, n); break;
         case Container::LOG_TABLE_8: compress_n_impl(cp, lgtbl8_it++,n); break;
         case Container::TABLE_32:    compress_n_impl(cp, tbl32_it++, n); break;
         case Container::TABLE_64:    compress_n_impl(cp, tbl64_it++, n); break;
        }
        ++n;
        ++cp->ppIt;  ++cp->ctxIt;  ++cp->ctxIrIt;
      }

      const auto ent = entropy(cp->w.begin(),cp->probs.begin(),cp->probs.end());
      pf /*<< std::fixed*/ << setprecision(DEF_PRF_PREC) << ent << '\n';
      normalize(cp->w.begin(), cp->wNext.begin(), cp->wNext.end());
////      update_weights(cp->w.begin(), cp->probs.begin(), cp->probs.end());

      sumEnt += ent;
    }
  }
  }
//  delete[] buffer;
//  tf.close();
fclose(tf);
    pf.close();
  aveEnt = sumEnt/symsNo;
}

template <typename ContIter>
inline void FCM::compress_n_impl
(shared_ptr<CompressPar> cp, ContIter cont, u8& n) const {
  compress_n_parent(cp, cont, n);
  if (cp->mm.child) {
    ++cp->ppIt;  ++cp->ctxIt;  ++cp->ctxIrIt;
    compress_n_child(cp, cont, ++n);
  }
}

template <typename ContIter>
inline void FCM::compress_n_parent
(shared_ptr<CompressPar> cp, ContIter cont, u8 n) const {
  if (cp->mm.ir == 0) {
    cp->ppIt->config(cp->c, *cp->ctxIt);
    array<decltype((*cont)->query(0)),4> f {};
    freqs(f, cont, cp->ppIt->l);
    const auto P = prob(f.begin(), cp->ppIt);
    cp->probs.emplace_back(P);
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.gamma, P);
    update_ctx(*cp->ctxIt, cp->ppIt);
  }
  else {
    cp->ppIt->config_ir(cp->c, *cp->ctxIt, *cp->ctxIrIt);
    array<decltype(2*(*cont)->query(0)),4> f {};
    freqs_ir(f, cont, cp->ppIt);
    const auto P = prob(f.begin(), cp->ppIt);
    cp->probs.emplace_back(P);
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.gamma, P);
    update_ctx_ir(*cp->ctxIt, *cp->ctxIrIt, cp->ppIt);
  }
}

template <typename ContIter>
inline void FCM::compress_n_child
(shared_ptr<CompressPar> cp, ContIter cont, u8 n) const {
  if (cp->mm.child->ir == 0) {
    cp->ppIt->config(cp->c, *cp->ctxIt);
    array<decltype((*cont)->query(0)),4> f {};
    freqs(f, cont, cp->ppIt->l);
    const auto P = prob(f.begin(), cp->ppIt);
    cp->probs.emplace_back(P);
    correct_stmm(cp, f.begin());
//    if (correct_stmm(cp,f.begin()))
//      fill(cp->wNext.begin(),cp->wNext.end(),static_cast<prec_t>(1)/cp->nMdl);
//    else
      cp->wNext[n] = weight_next(cp->w[n], cp->mm.child->gamma, P);
    update_ctx(*cp->ctxIt, cp->ppIt);
  }
  else {
    cp->ppIt->config_ir(cp->c, *cp->ctxIt, *cp->ctxIrIt);  // l and r
    array<decltype(2*(*cont)->query(0)),4> f {};
    freqs_ir(f, cont, cp->ppIt);
    const auto P = prob(f.begin(), cp->ppIt);
    cp->probs.emplace_back(P);
    correct_stmm(cp, f.begin());
//    if (correct_stmm(cp,f.begin()))
//      fill(cp->wNext.begin(),cp->wNext.end(),static_cast<prec_t>(1)/cp->nMdl);
//    else
      cp->wNext[n] = weight_next(cp->w[n], cp->mm.child->gamma, P);
    update_ctx_ir(*cp->ctxIt, *cp->ctxIrIt, cp->ppIt);
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
//inline void FCM::freqs (array<OutT,4>& a, ContIter cont, ProbParIter pp)const{
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
inline void FCM::freqs_ir
(array<OutT, 4>& a, ContIter cont, ProbParIter pp) const {
  a = {static_cast<OutT>(
        (*cont)->query(pp->l)        + (*cont)->query((3ull<<pp->shl) | pp->r)),
       static_cast<OutT>(
        (*cont)->query(pp->l | 1ull) + (*cont)->query((2ull<<pp->shl) | pp->r)),
       static_cast<OutT>(
        (*cont)->query(pp->l | 2ull) + (*cont)->query((1ull<<pp->shl) | pp->r)),
       static_cast<OutT>(
        (*cont)->query(pp->l | 3ull) + (*cont)->query(pp->r))};
}

template <typename FreqIter>
inline void FCM::correct_stmm
(shared_ptr<CompressPar> cp, FreqIter fFirst) const {
  auto stmm = cp->mm.child;
  const auto best = best_id(fFirst);
  if (stmm->enabled) {
    if (best==static_cast<u8>(255))
      miss_stmm(stmm);
    else if (best==static_cast<u8>(254) || best==cp->nSym)
      hit_stmm(stmm);
    else {
      miss_stmm(stmm);
      stmm->ir==0 ? cp->ppIt->config(best) : cp->ppIt->config_ir(best);
    }
  }
  else if (!stmm->enabled &&
           best!=static_cast<u8>(255) && best!=static_cast<u8>(254)) {
    stmm->enabled = true;
    #ifdef ARRAY_HISTORY
    std::fill(stmm->history.begin(), stmm->history.end(), false);
    #else
    stmm->history = 0u;
    #endif
  }
}
////template <typename FreqIter>
////inline bool FCM::correct_stmm
////(shared_ptr<CompressPar> cp, const FreqIter& fFirst) {
////  auto stmm = cp->mm.child;
////  const auto best = best_id(fFirst);
////  if (stmm->enabled) {
////    if (best==static_cast<u8>(255))
////      miss_stmm(stmm);
////    else if (best==static_cast<u8>(254) || best==cp->nSym)
////      hit_stmm(stmm);
////    else {
////      miss_stmm(stmm);
////      stmm->ir==0 ? cp->ppIt->config(best) : cp->ppIt->config_ir(best);
////    }
////  }
////  else if (!stmm->enabled &&
////           best!=static_cast<u8>(255) && best!=static_cast<u8>(254)) {
////    stmm->enabled = true;
////    #ifdef ARRAY_HISTORY
////    std::fill(stmm->history.begin(), stmm->history.end(), false);
////    #else
////    stmm->history = 0u;
////    #endif
////    // The following makes the output entropy worst
////    return true;
////  }
////  return false;
////}

template <typename FreqIter>
inline u8 FCM::best_id (FreqIter fFirst) const {
//  if (are_all(fFirst, 0) || are_all(fFirst, 1)) {
//  if (are_all(fFirst, 0)) { // The same as GeCo
  if (are_all(fFirst, 1)) { // Seems to be the best
    return static_cast<u8>(255);
  }
  const auto maxPos = std::max_element(fFirst, fFirst+CARDIN);
  if (has_multi_max(fFirst, maxPos)) {
    return static_cast<u8>(254);
  }
  return maxPos-fFirst;
}

#ifdef ARRAY_HISTORY
template <typename History, typename Value>
inline void FCM::update_hist_stmm (History& hist, Value val) const {
  std::rotate(hist.begin(), hist.begin()+1, hist.end());
  hist.back() = val;
}

template <typename TmPar>
inline void FCM::hit_stmm (const TmPar& stmm) const {
  stmm_update_hist(stmm->history, false);
}

template <typename TmPar>
inline void FCM::miss_stmm (TmPar stmm) const {
  if (pop_count(stmm->history.begin(),stmm->k) > stmm->thresh)
    stmm->enabled = false;
  else
    stmm_update_hist(stmm->history, true);
}
#else
template <typename History, typename Value>
inline void FCM::update_hist_stmm (History& hist, Value val, u32 mask) const {
  hist = ((hist<<1u) | val) & mask;  // ull for 64 bits
}

template <typename TmPar /*Tolerant model parameter*/>
inline void FCM::hit_stmm (const TmPar& stmm) const {
  update_hist_stmm(stmm->history, 0u, stmm->mask);
}

template <typename TmPar>
inline void FCM::miss_stmm (TmPar stmm) const {
  if (pop_count(stmm->history) > stmm->thresh)
    stmm->enabled = false;
  else
    update_hist_stmm(stmm->history, 1u, stmm->mask);
}
#endif

inline prec_t FCM::weight_next (prec_t weight, prec_t gamma, prec_t prob) const{
  return pow(weight, gamma) * prob;
}

template <typename FreqIter, typename ProbParIter>
inline prec_t FCM::prob (FreqIter fFirst, ProbParIter pp) const {
  return (*(fFirst+pp->numSym) + pp->alpha) /
         std::accumulate(fFirst, fFirst+CARDIN, pp->sAlpha);
//  return (*(fFirst+pp->numSym) + pp->alpha) /
//         (std::accumulate(fFirst,fFirst+CARDIN,0ull) + pp->sAlpha);
}

inline prec_t FCM::entropy (prec_t P) const {
  // First version
  return -log2(P);

  // Second version
//  static auto prevP  = static_cast<prec_t>(1); // Can't be zero
//  static auto result = static_cast<prec_t>(1); // Can't be zero
//  if (P == prevP)
//    return result;
//  prevP  = P;
//  result = -log2(P);
//  return result;

  // Third version
//  static constexpr int n_buckets = 4;  // Should be a power of 2
//  static struct { prec_t p; prec_t result; } cache[n_buckets];
//  static int last_read_i = 0;
//  static int last_written_i = 0;
//  int i = last_read_i;
//  do {
//    if (cache[i].p == P)
//      return cache[i].result;
//    i = (i+1) % n_buckets;
//  } while (i != last_read_i);
//  last_read_i = last_written_i = (last_written_i+1) % n_buckets;
//  cache[last_written_i].p = P;
//  cache[last_written_i].result = -log2(P);
//  return cache[last_written_i].result;
}

template <typename WIter, typename PIter>
inline prec_t FCM::entropy (WIter wFirst, PIter PFirst, PIter PLast) const {
  return log2(1 / inner_product(PFirst, PLast, wFirst, static_cast<prec_t>(0)));
//  return -log2(inner_product(PFirst, PLast, wFirst, static_cast<prec_t>(0)));
}
//template <typename OutIter, typename InIter>
//inline prec_t FCM::entropy (OutIter wFirst, InIter PFirst, InIter PLast) const {
//  return log2(1 / inner_product(PFirst, PLast, wFirst, static_cast<prec_t>(0)));
////  return -log2(inner_product(PFirst, PLast, wFirst, static_cast<prec_t>(0)));
//}

//template <typename OutIter, typename InIter>
//inline void FCM::update_weights
//(OutIter wFirst, InIter PFirst, InIter PLast) const {
//  const auto wFirstKeep = wFirst;
//  for (auto mIter=Ms.begin(); PFirst!=PLast; ++mIter, ++wFirst, ++PFirst) {
//    *wFirst = pow(*wFirst, mIter->gamma) * *PFirst;
//    if (mIter->child) {
//      ++wFirst;  ++PFirst;
//////      if (mIter->child->enabled)  // Lowers the performance
//        *wFirst = pow(*wFirst, mIter->child->gamma) * *PFirst;
//////      else                        // Lowers the performance
//////        *wFirst = static_cast<prec_t>(0);
//    }
//  }
//  normalize(wFirstKeep, wFirst);
//}

template <typename ProbParIter>
inline void FCM::update_ctx (u64& ctx, ProbParIter pp) const {
  ctx = (pp->l & pp->mask) | pp->numSym;
}

template <typename ProbParIter>
inline void FCM::update_ctx_ir
(u64& ctx, u64& ctxIr, ProbParIter pp) const {
  ctx   = (pp->l & pp->mask) | pp->numSym;
  ctxIr = (pp->revNumSym<<pp->shl) | pp->r;
}