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
using namespace smashpp;

FCM::FCM (Param& p) {
  aveEnt = static_cast<prec_t>(0);
  config(std::move(p.rmodelsPars), std::move(p.tmodelsPars));
  if (p.verbose && p.showInfo) { show_info(p);    p.showInfo=false; }
  alloc_model();
}

inline void FCM::config (string&& rmodelsPars, string&& tmodelsPars) {
  set_Ms_TMs(rmodelsPars.begin(), rmodelsPars.end(), rMs, rTMs);
  set_cont(rMs);

  set_Ms_TMs(tmodelsPars.begin(), tmodelsPars.end(), tMs, tTMs);
  set_cont(tMs);
}

template <typename Iter>
inline void FCM::set_Ms_TMs
(Iter begin, Iter end, vector<MMPar>& Ms, vector<STMMPar>& TMs) {
  vector<string> mdls;      split(begin, end, ':', mdls);
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
    else if (m.size() == 6) {
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
}

inline void FCM::set_cont (vector<MMPar>& Ms) {
  for (auto& m : Ms) {
    if      (m.k > K_MAX_LGTBL8)    m.cont = Container::SKETCH_8;
    else if (m.k > K_MAX_TBL32)     m.cont = Container::LOG_TABLE_8;
    else if (m.k > K_MAX_TBL64)     m.cont = Container::TABLE_32;
    else                            m.cont = Container::TABLE_64;
  }
}

inline void FCM::show_info (const Param& p) const {
  const u8 lblWidth=20, colWidth=8,
           tblWidth=52;//static_cast<u8>(lblWidth+Ms.size()*colWidth);

  const auto rule = [] (u8 n, string&& s) {
    for (auto i=n/s.size(); i--;) { cerr<<s; }    cerr<<'\n';
  };
  const auto toprule  = [=] () { rule(tblWidth, "~"); };
  const auto midrule  = [=] () { rule(tblWidth, "~"); };
  const auto botrule  = [=] () { rule(tblWidth, " "); };
  const auto label    = [&] (const string& s){cerr<<setw(lblWidth)  <<left<<s;};
  const auto header   = [&] (const string& s){cerr<<setw(2*colWidth)<<left<<s;};
  const auto info_MM  = [&] (const vector<MMPar>& Ms, char c) {
    int i = 0;
    for (const auto& e : Ms) {
      cerr << setw(colWidth) << left;
      switch (c) {
      case 'm':  cerr<<++i;                                              break;
      case 'k':  cerr<<static_cast<int>(e.k);                            break;
      case 'w':  cerr<<(e.w==0 ? "0" : "2^"+to_string(int(log2(e.w))));  break;
      case 'd':  cerr<<static_cast<int>(e.d);                            break;
      case 'i':  cerr<<(e.ir ? "yes" : "no");/*todo 0 1 2 be ja yes no*/ break;
      case 'a':  cerr<<e.alpha;                                          break;
      case 'g':  cerr<<e.gamma;                                          break;
      default:                                                           break;
      }
    }
  };
  const auto info_STMM = [&] (const vector<MMPar>& Ms, char c) {
    for (const auto& e : Ms) {
      cerr << setw(colWidth) << left;
      if (e.child)
        switch (c) {
        case 't':  cerr<<static_cast<int>(e.child->thresh);  break;
        case 'i':  cerr<<(e.child->ir ? "yes" : "no");       break;
        case 'a':  cerr<<e.child->alpha;                     break;
        case 'g':  cerr<<e.child->gamma;                     break;
        default:                                             break;
        }
      else
        cerr<<'-';
    }
  };
  const auto info_filter = [&] (char c) {
    cerr << setw(colWidth) << left;
    switch (c) {
    case 'f':  cerr<<p.print_win_type();  break;
    case 'w':  cerr<<p.wsize;             break;
    case 't':  cerr<<p.thresh;            break;
    default:                              break;
    }
  };
  const auto info_file = [&] (char c) {
    cerr << setw(2*colWidth) << left;
    switch (c) {
    case '1':  cerr.imbue(locale("en_US.UTF8")); cerr<<file_size(p.ref);  break;
    case 'r':  cerr<<p.ref;                                               break;
    case '2':  cerr.imbue(locale("en_US.UTF8")); cerr<<file_size(p.tar);  break;
    case 't':  cerr<<p.tar;                                               break;
    default:                                                              break;
    }
  };
  const auto rmm_row    = [=] (string&& lbl, char c) {
    label(lbl);    info_MM(rMs, c);                  cerr<<'\n';
  };
  const auto rstmm_row  = [=] (string&& lbl, char c) {
    label(lbl);    if (c!='h') info_STMM(rMs, c);    cerr<<'\n';
  };
  const auto tmm_row    = [=] (string&& lbl, char c) {
    label(lbl);    info_MM(tMs, c);                  cerr<<'\n';
  };
  const auto tstmm_row  = [=] (string&& lbl, char c) {
    label(lbl);    if (c!='h') info_STMM(tMs, c);    cerr<<'\n';
  };
  const auto filter_row = [=] (string&& lbl, char c) {
    label(lbl);    if (c!='h') info_filter(c);       cerr<<'\n';
  };
  const auto file_row   = [=] (string&& lbl, string&& s1, string&& s2) {
    label(lbl);
    if (s1.size()==1) { char c1=s1[0],c2=s2[0];  info_file(c1); info_file(c2); }
    else              {                          header(s1);    header(s2);    }
    cerr << '\n';
  };

  toprule();
  rmm_row("Ref Model(s)",                    'm');
  midrule();
  rmm_row("Context size (\U0001D705)    ",   'k');
  bool hasSketch = false;
  for (const auto& e : rMs)
    if (e.cont==Container::SKETCH_8) { hasSketch=true;  break; }
  if (hasSketch) {
    rmm_row("Sketch width (\U0001D464)    ", 'w');
    rmm_row("Sketch depth (\U0001D451)    ", 'd');
  }
  rmm_row("Inv. repeat  (ir)",               'i');
  rmm_row("Alpha        (\U0001D6FC)    ",   'a');
  rmm_row("Gamma        (\U0001D6FE)    ",   'g');
  botrule();  //cerr << '\n';

  toprule();
  rstmm_row("Ref Substituttional Model(s)",  'h');
  midrule();
  rstmm_row("No. Subst.   (\U0001D70F)    ", 't');
  rstmm_row("Inv. repeat  (ir)",             'i');
  rstmm_row("Alpha        (\U0001D6FC)    ", 'a');
  rstmm_row("Gamma        (\U0001D6FE)    ", 'g');
  botrule();  //cerr << '\n';

  toprule();
  tmm_row("Tar Model(s)",                    'm');
  midrule();
  tmm_row("Context size (\U0001D705)    ",   'k');
  hasSketch = false;
  for (const auto& e : tMs)
    if (e.cont==Container::SKETCH_8) { hasSketch=true;  break; }
  if (hasSketch) {
    tmm_row("Sketch width (\U0001D464)    ", 'w');
    tmm_row("Sketch depth (\U0001D451)    ", 'd');
  }
  tmm_row("Inv. repeat  (ir)", 'i');
  tmm_row("Alpha        (\U0001D6FC)    ",   'a');
  tmm_row("Gamma        (\U0001D6FE)    ",   'g');
  botrule();  //cerr << '\n';

  toprule();
  tstmm_row("Tar Substituttional Model(s)",  'h');
  midrule();
  tstmm_row("No. Subst.   (\U0001D70F)    ", 't');
  tstmm_row("Inv. repeat  (ir)", 'i');
  tstmm_row("Alpha        (\U0001D6FC)    ", 'a');
  tstmm_row("Gamma        (\U0001D6FE)    ", 'g');
  botrule();  //cerr << '\n';

  if (!p.compress) {
    toprule();
    filter_row("Filter & Segment",           'h');
    midrule();
    filter_row("Window function",            'f');
    filter_row("Window size",                'w');
    if (p.manThresh) {
      filter_row("Threshold",                't');
    }
    botrule();  //cerr << '\n';
  }

  toprule();
  file_row("Files",     "Name", "Size (B)");
  midrule();
  file_row("Reference", "r",    "1");
  file_row("Target",    "t",    "2");
  botrule();
}

inline void FCM::alloc_model () {
  for (const auto& m : rMs) {
    switch (m.cont) {
    case Container::SKETCH_8:
      cmls4.emplace_back(make_unique<CMLS4>(m.w, m.d));    break;
    case Container::LOG_TABLE_8:
      lgtbl8.emplace_back(make_unique<LogTable8>(m.k));    break;
    case Container::TABLE_32:
      tbl32.emplace_back(make_unique<Table32>(m.k));       break;
    case Container::TABLE_64:
      tbl64.emplace_back(make_unique<Table64>(m.k));       break;
    }
  }
}

void FCM::store (const Param& p) {
  const auto nMdl = rMs.size();
  cerr << "Building the model" << (nMdl==1 ?"" :"s") << " based on \"" << p.ref
       << "\" (level " << static_cast<u16>(p.level) << ")...\n";

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
  for (const auto& m : rMs) {    // Mask: 1<<2k - 1 = 4^k - 1
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
  const auto vThrSz = (p.nthr < rMs.size()) ? p.nthr : rMs.size();
  vector<std::thread> thrd(vThrSz);
  for (u8 i=0; i!=rMs.size(); ++i) {    // Mask: 1<<2k-1 = 4^k-1
    switch (rMs[i].cont) {
    case Container::SKETCH_8:
      thrd[i % vThrSz] = std::thread(&FCM::store_impl<u64,decltype(cmls4_iter)>,
        this, std::cref(p.ref), (1ull << (rMs[i].k << 1u)) - 1ull, cmls4_iter++);
      break;
    case Container::LOG_TABLE_8:
      thrd[i % vThrSz] =std::thread(&FCM::store_impl<u32,decltype(lgtbl8_iter)>,
        this, std::cref(p.ref), (1ul << (rMs[i].k << 1u)) - 1ul, lgtbl8_iter++);
      break;
    case Container::TABLE_32:
      thrd[i % vThrSz] = std::thread(&FCM::store_impl<u32,decltype(tbl32_iter)>,
        this, std::cref(p.ref), (1ul << (rMs[i].k << 1u)) - 1ul, tbl32_iter++);
      break;
    case Container::TABLE_64:
      thrd[i % vThrSz] = std::thread(&FCM::store_impl<u32,decltype(tbl64_iter)>,
        this, std::cref(p.ref), (1ul << (rMs[i].k << 1u)) - 1ul, tbl64_iter++);
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
  ifstream rf(ref);  //char c;
  Mask ctx=0;
  for (vector<char> buffer(FILE_BUF,0); rf.peek()!=EOF;) {
    rf.read(buffer.data(), FILE_BUF);
    for (auto it=buffer.begin(); it!=buffer.begin()+rf.gcount(); ++it) {
      const auto c = *it;
//    for (Mask ctx=0; rf.get(c);) { // Slower
      if (c!='N' && c!='\n') {
        ctx = ((ctx & mask)<<2u) | NUM[static_cast<u8>(c)];
        (*cont)->update(ctx);
      }
    }
  }
  rf.close();
}

void FCM::compress (const Param& p) {
  cerr << OUT_SEP << "Compressing \"" << p.tar << "\"...\n";

  if (rMs.size()==1 && rTMs.empty())  // 1 MM
    switch (rMs[0].cont) {
    case Container::SKETCH_8:     compress_1(p, cmls4.begin());   break;
    case Container::LOG_TABLE_8:  compress_1(p, lgtbl8.begin());  break;
    case Container::TABLE_32:     compress_1(p, tbl32.begin());   break;
    case Container::TABLE_64:     compress_1(p, tbl64.begin());   break;
    }
  else
    compress_n(p);

  cerr << "Done!\n";
//  if (p.verbose)
    cerr << "Average Entropy = "
         << std::fixed << setprecision(DEF_PRF_PREC) << aveEnt << " bps\n";
}

template <typename ContIter>
inline void FCM::compress_1 (const Param& par, ContIter cont) {
  // Ctx, Mir (int) sliding through the dataset
  u64      ctx{0},   ctxIr{(1ull<<(rMs[0].k<<1u))-1};
  u64      symsNo{0};            // No. syms in target file, except \n
  prec_t   sumEnt{0};            // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(par.tar);
  ofstream pf(gen_name(par.ref, par.tar, Format::PROFILE));
  ProbPar  pp{rMs[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
              static_cast<u8>(rMs[0].k<<1u)};
  const auto totalSize = file_size(par.tar);

  for (vector<char> buffer(FILE_BUF,0); tf.peek()!=EOF;) {
    tf.read(buffer.data(), FILE_BUF);
    for (auto it=buffer.begin(); it!=buffer.begin()+tf.gcount(); ++it) {
      const auto c = *it;
//      while (tf.get(c)) { // Slower
      if (c!='N' && c!='\n') {
        ++symsNo;
        if (rMs[0].ir == 0) {  // Branch prediction: 1 miss, totalSize-1 hits
          pp.config(c, ctx);
          array<decltype((*cont)->query(0)), 4> f {};
          freqs(f, cont, pp.l);
          const auto entr = entropy(prob(f.begin(), &pp));
          pf /*<< std::fixed*/ << setprecision(DEF_PRF_PREC) << entr << '\n';
          sumEnt += entr;
          update_ctx(ctx, &pp);
        }
        else {
          pp.config_ir(c, ctx, ctxIr);
          array<decltype(2*(*cont)->query(0)),4> f {};
          freqs_ir(f, cont, &pp);
          const auto entr = entropy(prob(f.begin(), &pp));
          pf /*<< std::fixed*/ << setprecision(DEF_PRF_PREC) << entr << '\n';
          sumEnt += entr;
          update_ctx_ir(ctx, ctxIr, &pp);
        }
        show_progress(symsNo, totalSize);
      }
    }
  }

  remove_progress_trace();
  tf.close();  pf.close();
  aveEnt = sumEnt/symsNo;
}

inline void FCM::compress_n (const Param& par) {
  u64 symsNo{0};               // No. syms in target file, except \n
  prec_t sumEnt{0};            // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(par.tar);
  ofstream pf(gen_name(par.ref, par.tar, Format::PROFILE));
  auto cp = make_shared<CompressPar>();
  // Ctx, Mir (int) sliding through the dataset
  const auto nMdl = static_cast<u8>(rMs.size() + rTMs.size());
  cp->nMdl = nMdl;
  cp->ctx.resize(nMdl);        // Fill with zeros (resize)
  cp->ctxIr.reserve(nMdl);
  for (const auto& mm : rMs) {  // Mask: 1<<2k - 1 = 4^k - 1
    cp->ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
    if (mm.child)  cp->ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
  }
  cp->w.resize(nMdl, static_cast<prec_t>(1)/nMdl);
  cp->wNext.resize(nMdl, static_cast<prec_t>(0));
  cp->pp.reserve(nMdl);
  auto maskIter = cp->ctxIr.begin();
  for (const auto& mm : rMs) {
    cp->pp.emplace_back(mm.alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
    if (mm.child)  cp->pp.emplace_back(
                     mm.child->alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
  }
  const auto totalSize = file_size(par.tar);

  for (vector<char> buffer(FILE_BUF,0); tf.peek()!=EOF;) {
    tf.read(buffer.data(), FILE_BUF);
    for (auto it=buffer.begin(); it!=buffer.begin()+tf.gcount(); ++it) {
      const auto c = *it;
//    while (tf.get(c)) { // Slower
      if (c!='N' && c!='\n') {
        ++symsNo;
        cp->c=c;                        cp->nSym=NUM[static_cast<u8>(c)];
        cp->ppIt=cp->pp.begin();
        cp->ctxIt=cp->ctx.begin();      cp->ctxIrIt=cp->ctxIr.begin();
        cp->probs.clear();              cp->probs.reserve(nMdl);
        auto tbl64_it=tbl64.begin();    auto tbl32_it=tbl32.begin();
        auto lgtbl8_it=lgtbl8.begin();  auto cmls4_it=cmls4.begin();

        u8 n = 0;  // Counter for the models
        for (const auto& mm : rMs) {
          cp->mm = mm;
          switch (mm.cont) {
          case Container::SKETCH_8:    compress_n_impl(cp,cmls4_it++, n); break;
          case Container::LOG_TABLE_8: compress_n_impl(cp,lgtbl8_it++,n); break;
          case Container::TABLE_32:    compress_n_impl(cp,tbl32_it++, n); break;
          case Container::TABLE_64:    compress_n_impl(cp,tbl64_it++, n); break;
          }
          ++n;
          ++cp->ppIt;  ++cp->ctxIt;  ++cp->ctxIrIt;
        }

        const auto ent=entropy(cp->w.begin(),cp->probs.begin(),cp->probs.end());
        pf /*<< std::fixed*/ << setprecision(DEF_PRF_PREC) << ent << '\n';
        normalize(cp->w.begin(), cp->wNext.begin(), cp->wNext.end());
////        update_weights(cp->w.begin(), cp->probs.begin(), cp->probs.end());
        sumEnt += ent;
        show_progress(symsNo, totalSize);
      }
    }
  }
  remove_progress_trace();
  tf.close();  pf.close();
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
  const auto weight_next = [=] (prec_t w, prec_t g, prec_t p) -> prec_t {
    return pow(w, g) * p;
  };

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
  const auto weight_next = [=](prec_t w, prec_t g, prec_t p) -> prec_t {
    return pow(w, g) * p;
  };

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

void FCM::self_compress (const Param& p) {
  cerr << OUT_SEP << "Compressing \"" << p.seq << "\"...\n";
  self_compress_alloc();

  if (tMs.size()==1 && tTMs.empty())  // 1 MM
    switch (tMs[0].cont) {
    case Container::SKETCH_8:     self_compress_1(p, cmls4.begin());   break;
    case Container::LOG_TABLE_8:  self_compress_1(p, lgtbl8.begin());  break;
    case Container::TABLE_32:     self_compress_1(p, tbl32.begin());   break;
    case Container::TABLE_64:     self_compress_1(p, tbl64.begin());   break;
    }
  else
    self_compress_n(p);

  cerr << "Done!\n";
}

inline void FCM::self_compress_alloc () {
  for (auto& e : cmls4)     e.reset();
  for (auto& e : lgtbl8)    e.reset();
  for (auto& e : tbl32)     e.reset();
  for (auto& e : tbl64)     e.reset();
  cmls4.clear();    lgtbl8.clear();    tbl32.clear();    tbl64.clear();

  for (const auto& m : tMs) {
    switch (m.cont) {
    case Container::SKETCH_8:
      cmls4.emplace_back(make_unique<CMLS4>(m.w, m.d));    break;
    case Container::LOG_TABLE_8:
      lgtbl8.emplace_back(make_unique<LogTable8>(m.k));    break;
    case Container::TABLE_32:
      tbl32.emplace_back(make_unique<Table32>(m.k));       break;
    case Container::TABLE_64:
      tbl64.emplace_back(make_unique<Table64>(m.k));       break;
    }
  }
}

template <typename ContIter>
inline void FCM::self_compress_1 (const smashpp::Param& par, ContIter cont) {
  u64      ctx{0},   ctxIr{(1ull<<(tMs[0].k<<1u))-1};
  u64      symsNo{0};
  prec_t   sumEnt{0};
  ifstream seqF(par.seq);
  ProbPar  pp{tMs[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
              static_cast<u8>(tMs[0].k<<1u)};
  const auto totalSize = file_size(par.seq);

  for (vector<char> buffer(FILE_BUF,0); seqF.peek()!=EOF;) {
    seqF.read(buffer.data(), FILE_BUF);
    for (auto it=buffer.begin(); it!=buffer.begin()+seqF.gcount(); ++it) {
      const auto c = *it;
      if (c!='N' && c!='\n') {
        ++symsNo;
        if (tMs[0].ir == 0) {
          pp.config(c, ctx);
          array<decltype((*cont)->query(0)), 4> f {};
          freqs(f, cont, pp.l);
          const auto entr = entropy(prob(f.begin(), &pp));
          cout /*<< std::fixed*/ << setprecision(DEF_PRF_PREC) << entr<<'\n';//todo comment
          sumEnt += entr;
          (*cont)->update(pp.l | pp.numSym);
          update_ctx(ctx, &pp);
        }
        else {
          pp.config_ir(c, ctx, ctxIr);
          array<decltype(2*(*cont)->query(0)),4> f {};
          freqs_ir(f, cont, &pp);
          const auto entr = entropy(prob(f.begin(), &pp));
          cout /*<< std::fixed*/ << setprecision(DEF_PRF_PREC) << entr<<'\n';//todo comment
          sumEnt += entr;
          (*cont)->update(pp.l | pp.numSym);
          update_ctx_ir(ctx, ctxIr, &pp);
        }
        show_progress(symsNo, totalSize);
      }
    }
  }
  remove_progress_trace();
  ofstream sf(gen_name("", par.seq, Format::SELF));
  sf << std::fixed << setprecision(DEF_PRF_PREC) << sumEnt/symsNo << '\n';
  sf.close();
  seqF.close();
}

inline void FCM::self_compress_n (const Param& par) {
  u64 symsNo{0};
  prec_t sumEnt{0};
  ifstream seqF(par.seq);
  auto cp = make_shared<CompressPar>();
  const auto nMdl = static_cast<u8>(tMs.size() + tTMs.size());
  cp->nMdl = nMdl;
  cp->ctx.resize(nMdl);        // Fill with zeros (resize)
  cp->ctxIr.reserve(nMdl);
  for (const auto& mm : tMs) {  // Mask: 1<<2k - 1 = 4^k - 1
    cp->ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
    if (mm.child)  cp->ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
  }
  cp->w.resize(nMdl, static_cast<prec_t>(1)/nMdl);
  cp->wNext.resize(nMdl, static_cast<prec_t>(0));
  cp->pp.reserve(nMdl);
  auto maskIter = cp->ctxIr.begin();
  for (const auto& mm : tMs) {
    cp->pp.emplace_back(mm.alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
    if (mm.child)  cp->pp.emplace_back(
                     mm.child->alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
  }
  const auto totalSize = file_size(par.seq);

  for (vector<char> buffer(FILE_BUF,0); seqF.peek()!=EOF;) {
    seqF.read(buffer.data(), FILE_BUF);
    for (auto it=buffer.begin(); it!=buffer.begin()+seqF.gcount(); ++it) {
      const auto c = *it;
      if (c!='N' && c!='\n') {
        ++symsNo;
        cp->c=c;                        cp->nSym=NUM[static_cast<u8>(c)];
        cp->ppIt=cp->pp.begin();
        cp->ctxIt=cp->ctx.begin();      cp->ctxIrIt=cp->ctxIr.begin();
        cp->probs.clear();              cp->probs.reserve(nMdl);
        auto tbl64_it=tbl64.begin();    auto tbl32_it=tbl32.begin();
        auto lgtbl8_it=lgtbl8.begin();  auto cmls4_it=cmls4.begin();

        u8 n = 0;  // Counter for the models
        for (const auto& mm : tMs) {
          cp->mm = mm;
          switch (mm.cont) {
          case Container::SKETCH_8:
            self_compress_n_impl(cp, cmls4_it++,  n);  break;
          case Container::LOG_TABLE_8:
            self_compress_n_impl(cp, lgtbl8_it++, n);  break;
          case Container::TABLE_32:
            self_compress_n_impl(cp, tbl32_it++,  n);  break;
          case Container::TABLE_64:
            self_compress_n_impl(cp, tbl64_it++,  n);  break;
          }
          ++n;
          ++cp->ppIt;  ++cp->ctxIt;  ++cp->ctxIrIt;
        }

        const auto ent=entropy(cp->w.begin(),cp->probs.begin(),cp->probs.end());
        cout /*<< std::fixed*/ << setprecision(DEF_PRF_PREC) << ent << '\n';
        normalize(cp->w.begin(), cp->wNext.begin(), cp->wNext.end());
////        update_weights(cp->w.begin(), cp->probs.begin(), cp->probs.end());
        sumEnt += ent;
        show_progress(symsNo, totalSize);
      }
    }
  }
  remove_progress_trace();
  //todo
  cerr<<std::fixed << setprecision(DEF_PRF_PREC) << sumEnt/symsNo << '\n';
//  ofstream sf(gen_name("", par.seq, Format::SELF));
//  sf /*<< std::fixed*/ << setprecision(DEF_PRF_PREC) << sumEnt/symsNo << '\n';
//  sf.close();
  seqF.close();
}

template <typename ContIter>
inline void FCM::self_compress_n_impl
(shared_ptr<CompressPar> cp, ContIter cont, u8& n) const {
  u64 valUpd = 0;
  self_compress_n_parent(cp, cont, n, valUpd);
  if (cp->mm.child) {
    ++cp->ppIt;  ++cp->ctxIt;  ++cp->ctxIrIt;
    self_compress_n_child(cp, cont, ++n);
  }
  (*cont)->update(valUpd);
}

template <typename ContIter>
inline void FCM::self_compress_n_parent
(shared_ptr<CompressPar> cp, ContIter cont, u8 n, u64& valUpd) const {
  const auto weight_next = [=] (prec_t w, prec_t g, prec_t p) -> prec_t {
    return pow(w, g) * p;
  };

  if (cp->mm.ir == 0) {
    cp->ppIt->config(cp->c, *cp->ctxIt);
    array<decltype((*cont)->query(0)),4> f {};
    freqs(f, cont, cp->ppIt->l);
    const auto P = prob(f.begin(), cp->ppIt);
    cp->probs.emplace_back(P);
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.gamma, P);
    valUpd = cp->ppIt->l | cp->ppIt->numSym;
    update_ctx(*cp->ctxIt, cp->ppIt);
  }
  else {
    cp->ppIt->config_ir(cp->c, *cp->ctxIt, *cp->ctxIrIt);
    array<decltype(2*(*cont)->query(0)),4> f {};
    freqs_ir(f, cont, cp->ppIt);
    const auto P = prob(f.begin(), cp->ppIt);
    cp->probs.emplace_back(P);
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.gamma, P);
    valUpd = cp->ppIt->l | cp->ppIt->numSym;
    update_ctx_ir(*cp->ctxIt, *cp->ctxIrIt, cp->ppIt);
  }
}

template <typename ContIter>
inline void FCM::self_compress_n_child
(shared_ptr<CompressPar> cp, ContIter cont, u8 n) const {
  const auto weight_next = [=](prec_t w, prec_t g, prec_t p) -> prec_t {
    return pow(w, g) * p;
  };

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
//    << '\t' << std::fixed << std::setprecision(3) << sumEnt << '\n';
//  f.close();  // Actually done, automatically
//}

template <typename OutT, typename ContIter>
inline void FCM::freqs (array<OutT,4>& a, ContIter cont, u64 l) const {
  a = {(*cont)->query(l),
       (*cont)->query(l | 1ull),
       (*cont)->query(l | 2ull),
       (*cont)->query(l | 3ull)};
}

//todo
//template <typename OutT, typename ContIter, typename ProbParIter>
//inline void FCM::freqs_ir1
//(array<OutT, 4>& a, ContIter cont, ProbParIter pp) const {
//  a = {static_cast<OutT>((*cont)->query((3ull<<pp->shl) | pp->r)),
//       static_cast<OutT>((*cont)->query((2ull<<pp->shl) | pp->r)),
//       static_cast<OutT>((*cont)->query((1ull<<pp->shl) | pp->r)),
//       static_cast<OutT>((*cont)->query(                  pp->r))};
//}

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

template <typename FreqIter, typename ProbParIter>
inline prec_t FCM::prob (FreqIter fFirst, ProbParIter pp) const {
  return (*(fFirst+pp->numSym) + pp->alpha) /
         std::accumulate(fFirst, fFirst+CARDIN, pp->sAlpha);
//  return (*(fFirst+pp->numSym) + pp->alpha) /
//         (std::accumulate(fFirst,fFirst+CARDIN,0ull) + pp->sAlpha);
}

inline prec_t FCM::entropy (prec_t P) const {
  // First version
  return log2(1/P);
//  return -log2(P);

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