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

  //todo
//  for(auto a:tbl64)a->print();cerr<<'\n';
//  for(auto a:tbl32)a->print();cerr<<'\n';
//  for(auto a:lgtbl8)a->print();cerr<<'\n';
//  for(auto a:cmls4)a->print();cerr<<'\n';
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
  if (Ms.size()==1 && TMs.empty())  // 1 MM
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

template <typename ContIter>
inline void FCM::compress_1 (const string& tar, ContIter cont) {
  // Ctx, Mir (int) sliding through the dataset
  u64 ctx{0}, ctxIr{(1ull<<(Ms[0].k<<1u))-1};
  u64 symsNo{0};                // No. syms in target file, except \n
  double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  ProbPar pp{Ms[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
             static_cast<u8>(Ms[0].k<<1u)};

  if (Ms[0].ir == 0) {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pp.config(c, ctx);
        const auto f = freqs<decltype((*cont)->query(0))>(cont, &pp);//todo
        sEnt += entropy(prob(f.begin(), &pp));//todo
        update_ctx(ctx, &pp);
      }
    }
  }
  else /*if (Ms[0].ir == 1)*/ {
    while (tf.get(c)) {
      if (c != '\n') {
        ++symsNo;
        pp.config_ir(c, ctx, ctxIr);
        const auto f = freqs_ir<decltype(2*(*cont)->query(0))>(cont, &pp);//todo
        sEnt += entropy(prob(f.begin(), &pp));//todo
        update_ctx_ir(ctx, ctxIr, &pp);
      }
    }
  }
  tf.close();
  aveEnt = sEnt/symsNo;
}

#include <bitset>//todo
inline void FCM::compress_n (const string& tar) {
  // Ctx, Mir (int) sliding through the dataset
  const auto nMdl = Ms.size() + TMs.size();
  vector<u64> ctx(nMdl);    // Fill with zeros (resize)
  vector<u64> ctxIr;    ctxIr.reserve(nMdl);
  for (const auto& mm : Ms) {  // Mask: 1<<2k - 1 = 4^k - 1
    ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
    if (mm.child)
      ctxIr.emplace_back((1ull<<(mm.k<<1))-1);
  }
  vector<double> w (nMdl, 1.0/nMdl);
  u64 symsNo{0};                // No. syms in target file, except \n
  double sEnt{0};               // Sum of entropies = sum(log_2 P(s|c^t))
  ifstream tf(tar);  char c;
  vector<ProbPar> pp;    pp.reserve(nMdl);
  {auto maskIter = ctxIr.begin();
  for (const auto& mm : Ms) {
    pp.emplace_back(mm.alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
    if (mm.child)
      pp.emplace_back(mm.child->alpha, *maskIter++, static_cast<u8>(mm.k<<1u));
  }}

  while (tf.get(c)) {
    if (c != '\n') {
      ++symsNo;
      auto ppIt      = pp.begin();
      auto ctxIt     = ctx.begin();
      auto ctxIrIt   = ctxIr.begin();
      auto tbl64_it  = tbl64.begin();
      auto tbl32_it  = tbl32.begin();
      auto lgtbl8_it = lgtbl8.begin();
      auto cmls4_it  = cmls4.begin();
      vector<double> probs;
      //todo. probs.reserve(nMdl); check not insert 0 when not consider stmm

      for (const auto& mm : Ms) {
        if (mm.ir == 0) {
          if (mm.cont == Container::TABLE_64) {
            ppIt->config(c, *ctxIt);
            auto f = freqs<u64>(tbl64_it, ppIt);
//            cerr<<c<<'\n'<<"ctx="<<*ctxIt<<'\t'<<"mm freqs=\t";//todo
//            for(auto e:f)cerr<<e<<' '; cerr<<'\n';//todo
            probs.emplace_back(prob(f.begin(), ppIt));
            update_ctx(*ctxIt, ppIt);

            if (mm.child) {
              ++ppIt;  ++ctxIt;  ++ctxIrIt;

              if (mm.child->enabled) {
                if (mm.child->ir == 0) {
                  ppIt->config(*ctxIt);  // l
                  f = freqs<u64>(tbl64_it, ppIt);
//                  cerr<<"ctx="<<*ctxIt<<'\t'<<"stmm freqs=\t";//todo
//                  for(auto e:f)cerr<<e<<' '; cerr<<'\n';//todo
                  const auto best = best_sym(f.begin(), f.end());
                  ppIt->config(best);  // best_sym uses l
//                  cerr<<"best="<<int(best)<<'\n';//todo
                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f.begin(), ppIt));
                  }
//                  std::bitset<16> x(mm.child->history);  cerr<<x<<' ';//todo
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(*ctxIt, *ctxIrIt);  // l and r
                  f = freqs_ir<u64>(tbl64_it, ppIt);
                  const auto best = best_sym(f.begin(), f.end());
                  ppIt->config_ir(best);  // best_sym uses l and r

                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config_ir(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
              else {
//                cerr<<"disable\n";//todo
                if (mm.child->ir == 0) {
                  ppIt->config(c, *ctxIt);
                  f = freqs<u64>(tbl64_it, ppIt);
//                  cerr<<"ctx="<<*ctxIt<<'\t'<<"stmm freqs=\t";//todo
//                  for(auto e:f)cerr<<e<<' '; cerr<<'\n';//todo
                  if (NUM[static_cast<u8>(c)]==best_sym_abs(f.begin(),f.end())){
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
//                  cerr<<"best_abs="<<int(best_sym_abs(f.begin(), f.end()))<<'\n';
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(c, *ctxIt, *ctxIrIt);
                  f = freqs_ir<u64>(tbl64_it, ppIt);
                  if (NUM[static_cast<u8>(c)]==best_sym_abs(f.begin(),f.end())){
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
            }
            ++tbl64_it;
          }
          else if (mm.cont == Container::TABLE_32) {
            ppIt->config(c, *ctxIt);
            auto f32 = freqs<u32>(tbl32_it, ppIt);
            probs.emplace_back(prob(f32.begin(), ppIt));
            update_ctx(*ctxIt, ppIt);

            if (mm.child) {
              ++ppIt;  ++ctxIt;  ++ctxIrIt;

              if (mm.child->enabled) {
                if (mm.child->ir == 0) {
                  ppIt->config(*ctxIt);  // l
                  f32 = freqs<u32>(tbl32_it, ppIt);
                  const auto best = best_sym(f32.begin(), f32.end());
                  ppIt->config(best);  // best_sym uses l
                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f32.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f32.begin(), ppIt));
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(*ctxIt, *ctxIrIt);  // l and r
                  const auto f64 = freqs_ir<u64>(tbl32_it, ppIt);
                  const auto best = best_sym(f64.begin(), f64.end());
                  ppIt->config_ir(best);  // best_sym uses l and r

                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f64.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config_ir(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f64.begin(), ppIt));
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
              else {
                if (mm.child->ir == 0) {
                  ppIt->config(c, *ctxIt);
                  f32 = freqs<u32>(tbl32_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f32.begin(), f32.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f32.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(c, *ctxIt, *ctxIrIt);
                  const auto f64 = freqs_ir<u64>(tbl32_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f64.begin(), f64.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f64.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
            }
            ++tbl32_it;
          }
          else if (mm.cont == Container::LOG_TABLE_8) {
            ppIt->config(c, *ctxIt);
            auto f = freqs<u64>(lgtbl8_it, ppIt);
            probs.emplace_back(prob(f.begin(), ppIt));
            update_ctx(*ctxIt, ppIt);

            if (mm.child) {
              ++ppIt;  ++ctxIt;  ++ctxIrIt;

              if (mm.child->enabled) {
                if (mm.child->ir == 0) {
                  ppIt->config(*ctxIt);  // l
                  f = freqs<u64>(lgtbl8_it, ppIt);
                  const auto best = best_sym(f.begin(), f.end());
                  ppIt->config(best);  // best_sym uses l
                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(*ctxIt, *ctxIrIt);  // l and r
                  f = freqs_ir<u64>(lgtbl8_it, ppIt);
                  const auto best = best_sym(f.begin(), f.end());
                  ppIt->config_ir(best);  // best_sym uses l and r

                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config_ir(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
              else {
                if (mm.child->ir == 0) {
                  ppIt->config(c, *ctxIt);
                  f = freqs<u64>(lgtbl8_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f.begin(), f.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(c, *ctxIt, *ctxIrIt);
                  f = freqs_ir<u64>(lgtbl8_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f.begin(), f.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
            }
            ++lgtbl8_it;
          }
          else if (mm.cont == Container::SKETCH_8) {
            ppIt->config(c, *ctxIt);
            auto f16 = freqs<u16>(cmls4_it, ppIt);
            probs.emplace_back(prob(f16.begin(), ppIt));
            update_ctx(*ctxIt, ppIt);

            if (mm.child) {
              ++ppIt;  ++ctxIt;  ++ctxIrIt;

              if (mm.child->enabled) {
                if (mm.child->ir == 0) {
                  ppIt->config(*ctxIt);  // l
                  f16 = freqs<u16>(cmls4_it, ppIt);
                  const auto best = best_sym(f16.begin(), f16.end());
                  ppIt->config(best);  // best_sym uses l
                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f16.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f16.begin(), ppIt));
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(*ctxIt, *ctxIrIt);  // l and r
                  const auto f32 = freqs_ir<u32>(cmls4_it, ppIt);
                  const auto best = best_sym(f32.begin(), f32.end());
                  ppIt->config_ir(best);  // best_sym uses l and r

                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f32.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config_ir(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f32.begin(), ppIt));
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
              else {
                if (mm.child->ir == 0) {
                  ppIt->config(c, *ctxIt);
                  f16 = freqs<u16>(cmls4_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f16.begin(), f16.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f16.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(c, *ctxIt, *ctxIrIt);
                  const auto f32 = freqs_ir<u32>(cmls4_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f32.begin(), f32.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f32.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
            }
            ++cmls4_it;
          }
        }
        else /*if (mm.ir==1)*/ {
          if (mm.cont == Container::TABLE_64) {
            ppIt->config_ir(c, *ctxIt, *ctxIrIt);
            auto f = freqs_ir<u64>(tbl64_it, ppIt);
            probs.emplace_back(prob(f.begin(), ppIt));
            update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);

            if (mm.child) {
              ++ppIt;  ++ctxIt;  ++ctxIrIt;

              if (mm.child->enabled) {
                if (mm.child->ir == 0) {
                  ppIt->config(*ctxIt);  // l
                  f = freqs<u64>(tbl64_it, ppIt);
                  const auto best = best_sym(f.begin(), f.end());
                  ppIt->config(best);  // best_sym uses l
                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(*ctxIt, *ctxIrIt);  // l and r
                  f = freqs_ir<u64>(tbl64_it, ppIt);
                  const auto best = best_sym(f.begin(), f.end());
                  ppIt->config_ir(best);  // best_sym uses l and r

                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config_ir(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
              else {
                if (mm.child->ir == 0) {
                  ppIt->config(c, *ctxIt);
                  f = freqs<u64>(tbl64_it, ppIt);
                  if (NUM[static_cast<u8>(c)]==best_sym_abs(f.begin(),f.end())){
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(c, *ctxIt, *ctxIrIt);
                  f = freqs_ir<u64>(tbl64_it, ppIt);
                  if (NUM[static_cast<u8>(c)]==best_sym_abs(f.begin(),f.end())){
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
            }
            ++tbl64_it;
          }
          else if (mm.cont == Container::TABLE_32) {
            ppIt->config_ir(c, *ctxIt, *ctxIrIt);
            auto f64 = freqs_ir<u64>(tbl32_it, ppIt);
            probs.emplace_back(prob(f64.begin(), ppIt));
            update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);

            if (mm.child) {
              ++ppIt;  ++ctxIt;  ++ctxIrIt;

              if (mm.child->enabled) {
                if (mm.child->ir == 0) {
                  ppIt->config(*ctxIt);  // l
                  const auto f32 = freqs<u32>(tbl32_it, ppIt);
                  const auto best = best_sym(f32.begin(), f32.end());
                  ppIt->config(best);  // best_sym uses l
                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f32.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f32.begin(), ppIt));
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(*ctxIt, *ctxIrIt);  // l and r
                  f64 = freqs_ir<u64>(tbl32_it, ppIt);
                  const auto best = best_sym(f64.begin(), f64.end());
                  ppIt->config_ir(best);  // best_sym uses l and r

                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f64.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config_ir(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f64.begin(), ppIt));
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
              else {
                if (mm.child->ir == 0) {
                  ppIt->config(c, *ctxIt);
                  const auto f32 = freqs<u32>(tbl32_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f32.begin(), f32.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f32.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(c, *ctxIt, *ctxIrIt);
                  f64 = freqs_ir<u64>(tbl32_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f64.begin(), f64.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f64.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
            }
            ++tbl32_it;
          }
          else if (mm.cont == Container::LOG_TABLE_8) {
            ppIt->config_ir(c, *ctxIt, *ctxIrIt);
            auto f = freqs_ir<u64>(lgtbl8_it, ppIt);
            probs.emplace_back(prob(f.begin(), ppIt));
            update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);

            if (mm.child) {
              ++ppIt;  ++ctxIt;  ++ctxIrIt;

              if (mm.child->enabled) {
                if (mm.child->ir == 0) {
                  ppIt->config(*ctxIt);  // l
                  f = freqs<u64>(lgtbl8_it, ppIt);
                  const auto best = best_sym(f.begin(), f.end());
                  ppIt->config(best);  // best_sym uses l
                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(*ctxIt, *ctxIrIt);  // l and r
                  f = freqs_ir<u64>(lgtbl8_it, ppIt);
                  const auto best = best_sym(f.begin(), f.end());
                  ppIt->config_ir(best);  // best_sym uses l and r

                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config_ir(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f.begin(), ppIt));
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
              else {
                if (mm.child->ir == 0) {
                  ppIt->config(c, *ctxIt);
                  f = freqs<u64>(lgtbl8_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f.begin(), f.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(c, *ctxIt, *ctxIrIt);
                  f = freqs_ir<u64>(lgtbl8_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f.begin(), f.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
            }
            ++lgtbl8_it;
          }
          else if (mm.cont == Container::SKETCH_8) {
            ppIt->config_ir(c, *ctxIt, *ctxIrIt);
            auto f32 = freqs_ir<u32>(cmls4_it, ppIt);
            probs.emplace_back(prob(f32.begin(), ppIt));
            update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);

            if (mm.child) {
              ++ppIt;  ++ctxIt;  ++ctxIrIt;

              if (mm.child->enabled) {
                if (mm.child->ir == 0) {
                  ppIt->config(*ctxIt);  // l
                  const auto f16 = freqs<u16>(cmls4_it, ppIt);
                  const auto best = best_sym(f16.begin(), f16.end());
                  ppIt->config(best);  // best_sym uses l
                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f16.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f16.begin(), ppIt));
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(*ctxIt, *ctxIrIt);  // l and r
                  f32 = freqs_ir<u32>(cmls4_it, ppIt);
                  const auto best = best_sym(f32.begin(), f32.end());
                  ppIt->config_ir(best);  // best_sym uses l and r

                  if (NUM[static_cast<u8>(c)] == best) {
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f32.begin(), ppIt));
                  }
                  else {
                    tm_miss(mm.child);
                    if (popcount(mm.child->history) > mm.child->thresh) {
                      mm.child->enabled = false;
                      mm.child->history = 0;
                      ppIt->config_ir(NUM[static_cast<u8>(c)]);
                      probs.emplace_back(0.0);
                    }
                    else
                      probs.emplace_back(prob(f32.begin(), ppIt));
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
              else {
                if (mm.child->ir == 0) {
                  ppIt->config(c, *ctxIt);
                  const auto f16 = freqs<u16>(cmls4_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f16.begin(), f16.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f16.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx(*ctxIt, ppIt);
                }
                else {
                  ppIt->config_ir(c, *ctxIt, *ctxIrIt);
                  f32 = freqs_ir<u32>(cmls4_it, ppIt);
                  if (NUM[static_cast<u8>(c)] ==
                      best_sym_abs(f32.begin(), f32.end())) {
                    mm.child->enabled = true;
                    tm_hit(mm.child);
                    probs.emplace_back(prob(f32.begin(), ppIt));
                    fill(w.begin(), w.end(), 1.0/nMdl);
                  }
                  else {
                    probs.emplace_back(0.0);
                  }
                  update_ctx_ir(*ctxIt, *ctxIrIt, ppIt);
                }
              }
            }
            ++cmls4_it;
          }
        }
        ++ppIt;  ++ctxIt;  ++ctxIrIt;
      }

      sEnt += entropy(w.begin(), probs.begin(), probs.end());
    }
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
    {(*cont)->query(pp->l)        + (*cont)->query((3ull<<pp->shl) | pp->r),
     (*cont)->query(pp->l | 1ull) + (*cont)->query((2ull<<pp->shl) | pp->r),
     (*cont)->query(pp->l | 2ull) + (*cont)->query((1ull<<pp->shl) | pp->r),
     (*cont)->query(pp->l | 3ull) + (*cont)->query(pp->r)};
}

template <typename Iter>
inline u8 FCM::best_sym (Iter first, Iter last) const {
  return static_cast<u8>(std::max_element(first,last) - first);
}

template <typename ContIter, typename ProbParIter>
inline u8 FCM::best_sym_ir (ContIter cont, ProbParIter pp) const {
  const auto c = freqs_ir<decltype(2*(*cont)->query(0))>(cont, pp);
  return static_cast<u8>(std::max_element(c.begin(),c.end()) - c.begin());
}

template <typename Iter>
inline u8 FCM::best_sym_abs (Iter first, Iter last) const {
  const auto max_pos = std::max_element(first, last);
  for (auto it=first; it!=last; ++it)
    if (it!=max_pos && *it==*max_pos)
      return 255;
  return static_cast<u8>(max_pos - first);
}

template <typename Par>
void FCM::tm_hit (Par stmm) {
  stmm->history <<= 1u;  // ull for 64 bits
//  cerr<<"hit ";//todo
}

template <typename Par>
void FCM::tm_miss (Par stmm) {
  stmm->history = (stmm->history<<1u) | 1u;  // ull for 64 bits
}

template <typename Iter>
inline double FCM::prob_frml (Iter it, u8 numSym, float a, float sa) const {
  return (*(it+numSym) + a) / (std::accumulate(it,it+CARDINALITY,0ull) + sa);//todo
}

template <typename FreqIter, typename ProbParIter>
inline double FCM::prob (FreqIter fFirst, ProbParIter pp) const {
  return prob_frml(fFirst, pp->numSym, pp->alpha, pp->sAlpha);
}
//template <typename ContIter, typename ProbParIter>
//inline double FCM::prob_ir (const ContIter cont, ProbParIter pp) const {
//  const auto c = freqs_ir<decltype(2*(*cont)->query(0))>(cont, pp);
//  return prob_frml(c.begin(), pp->numSym, pp->alpha, pp->sAlpha);
//}

inline double FCM::entropy (double P) const {
  return -log2(P);
}

template <typename OutIter, typename InIter>
inline double FCM::entropy (OutIter wFirst, InIter PFirst, InIter PLast) const {
//  cerr<<"w=("<<*wFirst<<","<<*(wFirst+1)<<")\tp=("<<*PFirst//todo
//      <<","<<*(PLast-1)<<")\n";//todo
  update_weights(wFirst, PFirst, PLast);
//  cerr<<"w0="<<*wFirst<<" w1="<<*(wFirst+1)<<"\n\n";//todo
  // log2 1 / (P0*w0 + P1*w1 + ...)
//  return -log2(std::inner_product(PFirst, PLast, wFirst, 0.0));
  return log2(1 / std::inner_product(PFirst, PLast, wFirst, 0.0));
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

template <typename Iter>
inline void FCM::normalize (Iter first, Iter last) const {
  for (const double sum=std::accumulate(first,last,0.0); first!=last; ++first)
    *first /= sum;    // *first = *first / sum;
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