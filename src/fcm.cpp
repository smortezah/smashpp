// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2020, IEETA, University of Aveiro, Portugal.

#include "fcm.hpp"

#include <cmath>
#include <fstream>
#include <numeric>  // std::accumulate
#include <thread>
#include <array>

#include "assert.hpp"
#include "container.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "naming.hpp"
#include "number.hpp"
#include "par.hpp"
using namespace smashpp;

FCM::FCM(std::unique_ptr<Param>& par)
    : aveEnt(static_cast<prc_t>(0)),
      rMs(par->refMs),
      tarSegID(0),
      entropyN(par->entropyN) {
  set_cont(rMs);
  rTMsSize = 0;
  for (const auto& e : rMs)
    if (e.child) ++rTMsSize;

  tMs = par->tarMs;
  set_cont(tMs);
  tTMsSize = 0;
  for (const auto& e : tMs)
    if (e.child) ++tTMsSize;

  alloc_model();
}

inline void FCM::set_cont(std::vector<MMPar>& Ms) {
  for (auto& m : Ms) {
    if (m.k > K_MAX_LGTBL8)
      m.cont = Container::sketch_8;
    else if (m.k > K_MAX_TBL32)
      m.cont = Container::log_table_8;
    else if (m.k > K_MAX_TBL64)
      m.cont = Container::table_32;
    else
      m.cont = Container::table_64;
  }
}

inline void FCM::show_info(std::unique_ptr<Param>& par) const {
  constexpr uint8_t lblWidth = 20;
  constexpr uint8_t colWidth = 8;
  constexpr uint8_t tblWidth =
      52;  // static_cast<uint8_t>(lblWidth + Ms.size()*colWidth);

  const auto rule = [](uint8_t n, std::string&& s) {
    for (auto i = n / s.size(); i--;) std::cerr << s;
    std::cerr << '\n';
  };
  const auto toprule = [=]() {};
  // const auto toprule = [=]() { rule(tblWidth, "~"); };
  const auto midrule = [=]() { rule(tblWidth, "="); };
  // const auto midrule = [=]() { rule(tblWidth, "~"); };
  const auto botrule = [=]() { rule(tblWidth, " "); };
  const auto label = [=](std::string s) {
    std::cerr << std::setw(lblWidth) << std::left << s;
  };
  const auto header = [=](std::string s) {
    std::cerr << std::setw(2 * colWidth) << std::left << s;
  };
  const auto info_MM = [&](const std::vector<MMPar>& Ms, char c) {
    int i = 0;
    for (const auto& e : Ms) {
      std::cerr << std::setw(colWidth) << std::left;
      switch (c) {
        case 'm':
          std::cerr << ++i;
          break;
        case 'k':
          std::cerr << static_cast<int>(e.k);
          break;
        case 'w':
          std::cerr << (e.w == 0 ? "0"
                                 : "2^" + std::to_string(static_cast<int>(
                                              std::log2(e.w))));
          break;
        case 'd':
          std::cerr << static_cast<int>(e.d);
          break;
        case 'i':
          std::cerr << (e.ir == 0 ? "reg" : e.ir == 1 ? "inv" : "reg+inv");
          break;
        case 'a':
          std::cerr << e.alpha;
          break;
        case 'g':
          std::cerr << e.gamma;
          break;
        default:
          break;
      }
    }
  };
  const auto info_STMM = [&](const std::vector<MMPar>& Ms, char c) {
    for (const auto& e : Ms) {
      std::cerr << std::setw(colWidth) << std::left;
      if (e.child) switch (c) {
          case 't':
            std::cerr << static_cast<int>(e.child->thresh);
            break;
          case 'i':
            std::cerr << (e.child->ir == 0
                              ? "reg"
                              : e.child->ir == 1 ? "inv" : "reg+inv");
            break;
          case 'a':
            std::cerr << e.child->alpha;
            break;
          case 'g':
            std::cerr << e.child->gamma;
            break;
          default:
            break;
        }
      else
        std::cerr << '-';
    }
  };
  const auto info_filter = [&](char c) {
    std::cerr << std::setw(colWidth) << std::left;
    switch (c) {
      case 'f':
        std::cerr << par->print_win_type();
        break;
      case 's':
        std::cerr << par->print_filter_scale();
        break;
      case 'w':
        std::cerr << par->filt_size;
        break;
      case 't':
        std::cerr << par->thresh;
        break;
      default:
        break;
    }
  };
  const auto info_file = [&](char c) {
    std::cerr << std::setw(2 * colWidth) << std::left;
    // const auto lacale = "en_US.UTF8";
    // std::setlocale(LC_ALL, "en_US.UTF-8");
    switch (c) {
      case '1':
        // std::cerr.imbue(std::locale(lacale));
        std::cerr << file_size(par->ref);
        break;
      // case 'r':  cerr<<par->ref;  break;
      case 'r':
        std::cerr << par->refName;
        break;
      case '2':
        // std::cerr.imbue(std::locale(lacale));
        std::cerr << file_size(par->tar);
        break;
      // case 't':  cerr<<par->tar;  break;
      case 't':
        std::cerr << par->tarName;
        break;
      default:
        break;
    }
  };
  const auto rmm_row = [=](std::string&& lbl, char c) {
    label(lbl);
    info_MM(rMs, c);
    std::cerr << '\n';
  };
  const auto rstmm_row = [=](std::string&& lbl, char c) {
    label(lbl);
    if (c != 'h') info_STMM(rMs, c);
    std::cerr << '\n';
  };
  const auto tmm_row = [=](std::string&& lbl, char c) {
    label(lbl);
    info_MM(tMs, c);
    std::cerr << '\n';
  };
  const auto tstmm_row = [=](std::string&& lbl, char c) {
    label(lbl);
    if (c != 'h') info_STMM(tMs, c);
    std::cerr << '\n';
  };
  const auto filter_row = [=](std::string&& lbl, char c) {
    label(lbl);
    if (c != 'h') info_filter(c);
    std::cerr << '\n';
  };
  const auto file_row = [=](std::string&& lbl, std::string&& s1,
                            std::string&& s2) {
    label(lbl);
    if (s1.size() == 1) {
      char c1 = s1[0];
      char c2 = s2[0];
      info_file(c1);
      info_file(c2);
    } else {
      header(s1);
      header(s2);
    }
    std::cerr << '\n';
  };

  toprule();
  rmm_row(bold("Ref Model           "), 'm');
  midrule();
  rmm_row("Context size (k)", 'k');
  bool hasSketch{false};
  for (const auto& e : rMs)
    if (e.cont == Container::sketch_8) {
      hasSketch = true;
      break;
    }
  if (hasSketch) {
    rmm_row("Sketch width (w)", 'w');
    rmm_row("Sketch depth (d)", 'd');
  }
  rmm_row("Inv. repeat  (ir)", 'i');
  rmm_row("Alpha        (a)", 'a');
  rmm_row("Gamma        (g)", 'g');
  botrule();  // cerr << '\n';

  {
    bool show_rstmm = false;
    for (const auto& model : rMs)
      if (model.child) {
        show_rstmm = true;
        break;
      }
    if (show_rstmm) {
      toprule();
      rstmm_row(bold("Ref Substituttional Model           "), 'h');
      midrule();
      rstmm_row("No. Subst.   (t)", 't');
      rstmm_row("Inv. repeat  (ir)", 'i');
      rmm_row("Alpha        (a)", 'a');
      rmm_row("Gamma        (g)", 'g');
      botrule();  // cerr << '\n';
    }
  }

  toprule();
  tmm_row(bold("Tar Model           "), 'm');
  midrule();
  rmm_row("Context size (k)", 'k');
  hasSketch = false;
  for (const auto& e : tMs)
    if (e.cont == Container::sketch_8) {
      hasSketch = true;
      break;
    }
  if (hasSketch) {
    rmm_row("Sketch width (w)", 'w');
    rmm_row("Sketch depth (d)", 'd');
  }
  tmm_row("Inv. repeat  (ir)", 'i');
  rmm_row("Alpha        (a)", 'a');
  rmm_row("Gamma        (g)", 'g');
  botrule();  // cerr << '\n';

  {
    bool show_tstmm = false;
    for (const auto& model : tMs)
      if (model.child) {
        show_tstmm = true;
        break;
      }
    if (show_tstmm) {
      toprule();
      tstmm_row(bold("Tar Substituttional Model           "), 'h');
      midrule();
      rstmm_row("No. Subst.   (t)", 't');
      tstmm_row("Inv. repeat  (ir)", 'i');
      rmm_row("Alpha        (a)", 'a');
      rmm_row("Gamma        (g)", 'g');
      botrule();  // cerr << '\n';
    }
  }

  if (!par->compress) {
    toprule();
    filter_row(bold("Filter & Segment"), 'h');
    midrule();
    filter_row("Window function", 'f');
    if (par->manFilterScale) filter_row("Filter scale", 's');
    if (!par->manFilterScale) filter_row("Window size", 'w');
    if (par->manThresh) filter_row("Threshold", 't');
    botrule();  // cerr << '\n';
  }

  toprule();
  file_row(bold("File                "), bold("Name            "),
           bold("Size (B)"));
  midrule();
  file_row("Reference", "r", "1");
  file_row("Target", "t", "2");
  botrule();
}

inline void FCM::alloc_model() {
  for (const auto& m : rMs) {
    switch (m.cont) {
      case Container::sketch_8:
        cmls4.push_back(std::make_unique<CMLS4>(m.w, m.d));
        break;
      case Container::log_table_8:
        lgtbl8.push_back(std::make_unique<LogTable8>(m.k));
        break;
      case Container::table_32:
        tbl32.push_back(std::make_unique<Table32>(m.k));
        break;
      case Container::table_64:
        tbl64.push_back(std::make_unique<Table64>(m.k));
        break;
    }
  }
}

void FCM::store(std::unique_ptr<Param>& par, uint8_t round) {
  if (round == 1 || par->verbose) {
    par->message = (round == 3) ? "    " : "";
    par->message += "[+] Creating model";
    if (rMs.size() > 1) par->message += "s";
    par->message += " of ";
    par->message += tarSegMsg.empty()
                        ? italic(par->refName)
                        : italic(tarSegMsg + std::to_string(tarSegID));
    par->message += " ";
    std::cerr << par->message << "...";
  }

  (par->nthr == 1 || rMs.size() == 1) ? store_1(par)
                                      : store_n(par) /*Multiple threads*/;

  if (round == 1 || par->verbose)
    std::cerr << "\r" << par->message << "done." << '\n';
}

inline void FCM::store_1(std::unique_ptr<Param>& par) {
  auto tbl64_iter = std::begin(tbl64);
  auto tbl32_iter = std::begin(tbl32);
  auto lgtbl8_iter = std::begin(lgtbl8);
  auto cmls4_iter = std::begin(cmls4);

  for (const auto& m : rMs) {  // Mask: 1<<2k - 1 = 4^k - 1
    switch (m.cont) {
      case Container::log_table_8:
        store_impl(par->ref, (1ul << (2 * m.k)) - 1ul /*Mask 32*/,
                   lgtbl8_iter++);
        break;
      case Container::sketch_8:
        store_impl(par->ref, (1ull << (2 * m.k)) - 1ull /*Mask 64*/,
                   cmls4_iter++);
        break;
      case Container::table_64:
        store_impl(par->ref, (1ul << (2 * m.k)) - 1ul /*Mask 32*/,
                   tbl64_iter++);
        break;
      case Container::table_32:
        store_impl(par->ref, (1ul << (2 * m.k)) - 1ul /*Mask 32*/,
                   tbl32_iter++);
        break;
      default:
        break;
    }
  }
}

inline void FCM::store_n(std::unique_ptr<Param>& par) {
  auto tbl64_iter = std::begin(tbl64);
  auto tbl32_iter = std::begin(tbl32);
  auto lgtbl8_iter = std::begin(lgtbl8);
  auto cmls4_iter = std::begin(cmls4);
  const auto vThrSz = (par->nthr < rMs.size()) ? par->nthr : rMs.size();
  std::vector<std::thread> thrd(vThrSz);

  for (uint8_t i = 0; i != rMs.size(); ++i) {  // Mask: 1<<2k-1 = 4^k-1
    switch (rMs[i].cont) {
      case Container::sketch_8:
        thrd[i % vThrSz] = std::thread(
            &FCM::store_impl<uint64_t, decltype(cmls4_iter)>, this,
            std::cref(par->ref), (1ull << (2 * rMs[i].k)) - 1ull, cmls4_iter++);
        break;
      case Container::log_table_8:
        thrd[i % vThrSz] = std::thread(
            &FCM::store_impl<uint32_t, decltype(lgtbl8_iter)>, this,
            std::cref(par->ref), (1ul << (2 * rMs[i].k)) - 1ul, lgtbl8_iter++);
        break;
      case Container::table_64:
        thrd[i % vThrSz] = std::thread(
            &FCM::store_impl<uint32_t, decltype(tbl64_iter)>, this,
            std::cref(par->ref), (1ul << (2 * rMs[i].k)) - 1ul, tbl64_iter++);
        break;
      case Container::table_32:
        thrd[i % vThrSz] = std::thread(
            &FCM::store_impl<uint32_t, decltype(tbl32_iter)>, this,
            std::cref(par->ref), (1ul << (2 * rMs[i].k)) - 1ul, tbl32_iter++);
        break;
      default:
        break;
    }
    // Join
    if ((i + 1) % vThrSz == 0)
      for (auto& t : thrd)
        if (t.joinable()) t.join();
  }
  for (auto& t : thrd)
    if (t.joinable()) t.join();  // Join leftover threads
}

template <typename Mask, typename ContIter /*Container iterator*/>
inline void FCM::store_impl(std::string ref, Mask mask, ContIter cont) {
  std::ifstream rf(ref);
  Mask ctx = 0;
  
  for (std::vector<char> buffer(FILE_READ_BUF, 0); rf.peek() != EOF;) {
    rf.read(buffer.data(), FILE_READ_BUF);
    for (auto it = std::begin(buffer); it != std::begin(buffer) + rf.gcount();
         ++it) {
      const auto c = *it;
      if (c != '\n') {
        ctx = ((ctx & mask) << 2u) | base_code(c);
        (*cont)->update(ctx);
      }
    }
  }

  rf.close();
}

void FCM::compress(std::unique_ptr<Param>& par, uint8_t round) {
  if (par->verbose) {
    par->message = (round == 3) ? "    " : "";
    par->message += "[+] Compressing " + italic(par->tarName) + " ";
  } else {
    if (round == 1) {
      par->message =
          "[+] Compressing & filtering " + italic(par->tarName) + " ";
      std::cerr << par->message << "...";
    }
  }

  if (rMs.size() == 1 && rTMsSize == 0)  // 1 MM
    switch (rMs[0].cont) {
      case Container::sketch_8:
        compress_1(par, std::begin(cmls4));
        break;
      case Container::log_table_8:
        compress_1(par, std::begin(lgtbl8));
        break;
      case Container::table_32:
        compress_1(par, std::begin(tbl32));
        break;
      case Container::table_64:
        compress_1(par, std::begin(tbl64));
        break;
    }
  else
    compress_n(par);

  if (par->verbose) {
    std::cerr << "\r" << par->message << "finished. Ave. entropy = "
              << fixed_precision(PREC_PRF, aveEnt) << " bps." << '\n';
  } else {
    // if (round == 1) std::cerr << "\r" << par->message << "...";
  }
}

template <typename ContIter>
void FCM::compress_1(std::unique_ptr<Param>& par, ContIter cont) {
  uint64_t ctx{0};  // Ctx, Mir (int) sliding through the dataset
  uint64_t ctxIr{(1ull << (2 * rMs[0].k)) - 1};
  uint64_t symsNo{0};  // No. syms in target file, except \n
  prc_t sumEnt{0};     // Sum of entropies = sum(log_2 P(s|c^t))
  ProbPar prob_par{rMs[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
                   static_cast<uint8_t>(rMs[0].k << 1u)};
  std::ifstream tar_file(par->tar);
  std::ofstream prf_file(
      gen_name(par->ID, par->ref, par->tar, Format::profile));
  const auto totalSize = file_size(par->tar);
  std::vector<prc_t> entropies;
  entropies.reserve(FILE_WRITE_BUF);
  auto write_entropies = [&]() {
    for (auto e : entropies) prf_file << precision(PREC_PRF, e) << '\n';
  };
  uint64_t sample_step_index = 0;

  for (std::vector<char> buffer(FILE_READ_BUF, 0); tar_file.peek() != EOF;) {
    tar_file.read(buffer.data(), FILE_READ_BUF);
    for (auto it = std::begin(buffer);
         it != std::begin(buffer) + tar_file.gcount(); ++it) {
      auto c = *it;
      if (c == '\n') continue;

      prc_t entr;
      bool sample_taken = (sample_step_index % par->sampleStep == 0);

      if (rMs[0].ir == 0) {  // Branch prediction: 1 miss, totalSize-1 hits
        if (c != 'N') {
          prob_par.config_ir0(c, ctx);
          if (sample_taken) {
            auto f = freqs_ir0<decltype((*cont)->query(0))>(cont, prob_par.l);
            entr = entropy(std::begin(f), &prob_par);
          }
        } else {
          // c = TAR_ALT_N;//todo
          prob_par.config_ir0(c, ctx);
          entr = entropyN;
        }
        update_ctx_ir0(ctx, &prob_par);
        } else if (rMs[0].ir == 1) {
          if (c != 'N') {
            prob_par.config_ir1(c, ctxIr);
            if (sample_taken) {
              auto f = freqs_ir1<decltype(2 * (*cont)->query(0))>(
                  cont, prob_par.shl, prob_par.r);
              entr = entropy(std::begin(f), &prob_par);
            }
          } else {
            // c = TAR_ALT_N;//todo
            prob_par.config_ir1(c, ctxIr);
            entr = entropyN;
          }
          update_ctx_ir1(ctxIr, &prob_par);
        } else if (rMs[0].ir == 2) {
          if (c != 'N') {
            prob_par.config_ir2(c, ctx, ctxIr);
            if (sample_taken) {
              auto f =
                  freqs_ir2<decltype(2 * (*cont)->query(0))>(cont,
                  &prob_par);
              entr = entropy(std::begin(f), &prob_par);
            }
          } else {
            // c = TAR_ALT_N;//todo
            prob_par.config_ir2(c, ctx, ctxIr);
            entr = entropyN;
          }
          update_ctx_ir2(ctx, ctxIr, &prob_par);
      }

      if (sample_taken) {
        ++symsNo;
        sumEnt += entr;
        entropies.push_back(entr);
      }
      ++sample_step_index;
      // prf_file << precision(PREC_PRF, entr) << '\n';
      if (par->verbose) show_progress(symsNo, totalSize, par->message);
    }

    if (entropies.size() >= FILE_WRITE_BUF) {
      write_entropies();
      entropies.clear();
      entropies.reserve(FILE_WRITE_BUF);
    }
  }
  write_entropies();

  tar_file.close();
  prf_file.close();
  aveEnt = sumEnt / symsNo;
}

void FCM::compress_n(std::unique_ptr<Param>& par) {
  uint64_t symsNo{0};  // No. syms in target file, except \n
  prc_t sumEnt{0};     // Sum of entropies = sum(log_2 P(s|c^t))
  auto cp = std::make_unique<CompressPar>();
  const auto nMdl = static_cast<uint8_t>(rMs.size()) + rTMsSize;
  cp->nMdl = nMdl;
  // Ctx, Mir (int) sliding through the dataset
  cp->ctx.resize(nMdl);  // Fill with zeros (resize)
  cp->ctxIr.reserve(nMdl);
  for (const auto& mm : rMs) {  // Mask: 1<<2k - 1 = 4^k - 1
    cp->ctxIr.push_back((1ull << (2 * mm.k)) - 1);
    if (mm.child) cp->ctxIr.push_back((1ull << (2 * mm.k)) - 1);
  }
  cp->w.resize(nMdl, static_cast<prc_t>(1) / nMdl);
  cp->wNext.resize(nMdl, static_cast<prc_t>(0));
  cp->pp.reserve(nMdl);
  auto maskIter = std::begin(cp->ctxIr);
  for (const auto& mm : rMs) {
    cp->pp.emplace_back(mm.alpha, *maskIter++, static_cast<uint8_t>(2 * mm.k));
    if (mm.child)
      cp->pp.emplace_back(mm.child->alpha, *maskIter++,
                          static_cast<uint8_t>(2 * mm.k));
  }
  std::ifstream tar_file(par->tar);
  std::ofstream prf_file(
      gen_name(par->ID, par->ref, par->tar, Format::profile));
  const auto totalSize = file_size(par->tar);
  std::vector<prc_t> entropies;
  entropies.reserve(FILE_WRITE_BUF);
  auto write_entropies = [&]() {
    for (auto e : entropies) prf_file << precision(PREC_PRF, e) << '\n';
  };
  uint64_t sample_step_index = 0;

  const auto compress_n_impl = [&](auto& cp, auto cont, uint8_t& n) {
    compress_n_parent(cp, cont, n);
    if (cp->mm.child) {
      ++cp->ppIt;
      ++cp->ctxIt;
      ++cp->ctxIrIt;
      compress_n_child(cp, cont, ++n);
    }
  };

  for (std::vector<char> buffer(FILE_READ_BUF, 0); tar_file.peek() != EOF;) {
    tar_file.read(buffer.data(), FILE_READ_BUF);
    for (auto it = std::begin(buffer);
         it != std::begin(buffer) + tar_file.gcount(); ++it) {
      const auto c = *it;
      if (c == '\n') continue;

      bool sample_taken = (sample_step_index % par->sampleStep == 0);
      cp->c = c;
      cp->nSym = base_code(c);
      cp->ppIt = std::begin(cp->pp);
      cp->ctxIt = std::begin(cp->ctx);
      cp->ctxIrIt = std::begin(cp->ctxIr);
      cp->probs.clear();
      cp->probs.reserve(nMdl);
      auto tbl64_it = std::begin(tbl64);
      auto tbl32_it = std::begin(tbl32);
      auto lgtbl8_it = std::begin(lgtbl8);
      auto cmls4_it = std::begin(cmls4);

      uint8_t n = 0;  // Counter for the models
      for (const auto& mm : rMs) {
        cp->mm = mm;
        switch (mm.cont) {
          case Container::sketch_8:
            compress_n_impl(cp, cmls4_it++, n);
            break;
          case Container::log_table_8:
            compress_n_impl(cp, lgtbl8_it++, n);
            break;
          case Container::table_32:
            compress_n_impl(cp, tbl32_it++, n);
            break;
          case Container::table_64:
            compress_n_impl(cp, tbl64_it++, n);
            break;
        }
        ++n;
        ++cp->ppIt;
        ++cp->ctxIt;
        ++cp->ctxIrIt;
      }

      const auto entr = entropy(std::begin(cp->w), std::begin(cp->probs),
                                std::end(cp->probs));
      // prf_file << precision(PREC_PRF, entr) << '\n';
      normalize(std::begin(cp->w), std::begin(cp->wNext), std::end(cp->wNext));
      ////        update_weights(begin(cp->w), begin(cp->probs),
      /// end(cp->probs));
      ++symsNo;
      sumEnt += entr;
      if (sample_taken) entropies.push_back(entr);
      ++sample_step_index;
      if (par->verbose) show_progress(symsNo, totalSize, par->message);
    }

    if (entropies.size() >= FILE_WRITE_BUF) {
      write_entropies();
      entropies.clear();
      entropies.reserve(FILE_WRITE_BUF);
    }
  }
  write_entropies();

  tar_file.close();
  prf_file.close();
  aveEnt = sumEnt / symsNo;
}

template <typename ContIter>
inline void FCM::compress_n_parent(std::unique_ptr<CompressPar>& cp,
                                   ContIter cont, uint8_t n) const {
  prc_t prb;

  if (cp->mm.ir == 0) {
    if (cp->c != 'N') {
      cp->ppIt->config_ir0(cp->c, *cp->ctxIt);
      auto f = freqs_ir0<decltype((*cont)->query(0))>(cont, cp->ppIt->l);
      prb = prob(std::begin(f), cp->ppIt);
    } else {
      // cp->c = TAR_ALT_N;//todo
      cp->ppIt->config_ir0(cp->c, *cp->ctxIt);
      prb = 1.0 / std::pow(2.0, entropyN);
    }
    cp->probs.push_back(prb);
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.gamma, prb);
    update_ctx_ir0(*cp->ctxIt, cp->ppIt);
  } else if (cp->mm.ir == 1) {
    if (cp->c != 'N') {
      cp->ppIt->config_ir1(cp->c, *cp->ctxIrIt);
      auto f = freqs_ir1<decltype(2 * (*cont)->query(0))>(cont, cp->ppIt->shl,
                                                          cp->ppIt->r);
      prb = prob(std::begin(f), cp->ppIt);
    } else {
      // cp->c = TAR_ALT_N;//todo
      cp->ppIt->config_ir1(cp->c, *cp->ctxIrIt);
      prb = 1.0 / std::pow(2.0, entropyN);
    }
    cp->probs.push_back(prb);
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.gamma, prb);
    update_ctx_ir1(*cp->ctxIrIt, cp->ppIt);
  } else if (cp->mm.ir == 2) {
    if (cp->c != 'N') {
      cp->ppIt->config_ir2(cp->c, *cp->ctxIt, *cp->ctxIrIt);
      auto f = freqs_ir2<decltype(2 * (*cont)->query(0))>(cont, cp->ppIt);
      prb = prob(std::begin(f), cp->ppIt);
    } else {
      // cp->c = TAR_ALT_N;//todo
      cp->ppIt->config_ir2(cp->c, *cp->ctxIt, *cp->ctxIrIt);
      prb = 1.0 / std::pow(2.0, entropyN);
    }
    cp->probs.push_back(prb);
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.gamma, prb);
    update_ctx_ir2(*cp->ctxIt, *cp->ctxIrIt, cp->ppIt);
  }
}

template <typename ContIter>
inline void FCM::compress_n_child(std::unique_ptr<CompressPar>& cp,
                                  ContIter cont, uint8_t n) const {
  prc_t prb;

  if (cp->mm.child->ir == 0) {
    if (cp->c != 'N') {
      cp->ppIt->config_ir0(cp->c, *cp->ctxIt);  // l
      auto f = freqs_ir0<decltype((*cont)->query(0))>(cont, cp->ppIt->l);
      prb = prob(std::begin(f), cp->ppIt);
      cp->probs.push_back(prb);
      correct_stmm(cp, std::begin(f));
    } else {
      // cp->c = TAR_ALT_N;//todo
      cp->ppIt->config_ir0(cp->c, *cp->ctxIt);  // l
      auto f = freqs_ir0<decltype((*cont)->query(0))>(cont, cp->ppIt->l);
      prb = 1.0 / std::pow(2.0, entropyN);
      cp->probs.push_back(prb);
      correct_stmm(cp, std::begin(f));
    }
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.child->gamma, prb);
    update_ctx_ir0(*cp->ctxIt, cp->ppIt);
  } else if (cp->mm.child->ir == 1) {
    if (cp->c != 'N') {
      cp->ppIt->config_ir1(cp->c, *cp->ctxIrIt);  // r
      auto f = freqs_ir1<decltype(2 * (*cont)->query(0))>(cont, cp->ppIt->shl,
                                                          cp->ppIt->r);
      prb = prob(std::begin(f), cp->ppIt);
      cp->probs.push_back(prb);
      correct_stmm(cp, std::begin(f));
    } else {
      // cp->c = TAR_ALT_N;//todo
      cp->ppIt->config_ir1(cp->c, *cp->ctxIrIt);  // r
      auto f = freqs_ir1<decltype(2 * (*cont)->query(0))>(cont, cp->ppIt->shl,
                                                          cp->ppIt->r);
      prb = 1.0 / std::pow(2.0, entropyN);
      cp->probs.push_back(prb);
      correct_stmm(cp, std::begin(f));
    }
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.child->gamma, prb);
    update_ctx_ir1(*cp->ctxIrIt, cp->ppIt);
  } else if (cp->mm.child->ir == 2) {
    if (cp->c != 'N') {
      cp->ppIt->config_ir2(cp->c, *cp->ctxIt, *cp->ctxIrIt);  // l and r
      auto f = freqs_ir2<decltype(2 * (*cont)->query(0))>(cont, cp->ppIt);
      prb = prob(std::begin(f), cp->ppIt);
      cp->probs.push_back(prb);
      correct_stmm(cp, std::begin(f));
    } else {
      // cp->c = TAR_ALT_N;//todo
      cp->ppIt->config_ir2(cp->c, *cp->ctxIt, *cp->ctxIrIt);  // l and r
      auto f = freqs_ir2<decltype(2 * (*cont)->query(0))>(cont, cp->ppIt);
      prb = 1.0 / std::pow(2.0, entropyN);
      cp->probs.push_back(prb);
      correct_stmm(cp, std::begin(f));
    }
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.child->gamma, prb);
    update_ctx_ir2(*cp->ctxIt, *cp->ctxIrIt, cp->ppIt);
  }
}

void FCM::self_compress(std::unique_ptr<Param>& par, uint64_t ID,
                        uint8_t round) {
  std::string message;
  if (par->verbose) {
    if (round == 3)
      message =
          "        [-] Compressing segment " + std::to_string(ID + 1) + " ";
    else
      message = "    [-] Compressing segment " + std::to_string(ID + 1) + " ";
  }

  self_compress_alloc();

  if (tMs.size() == 1 && tTMsSize == 0)  // 1 MM
    switch (tMs[0].cont) {
      case Container::sketch_8:
        self_compress_1(par, std::begin(cmls4), ID);
        break;
      case Container::log_table_8:
        self_compress_1(par, std::begin(lgtbl8), ID);
        break;
      case Container::table_32:
        self_compress_1(par, std::begin(tbl32), ID);
        break;
      case Container::table_64:
        self_compress_1(par, std::begin(tbl64), ID);
        break;
    }
  else
    self_compress_n(par, ID);

  if (par->verbose)
    std::cerr << "\r" << message << "done. Ave. entropy = "
              << fixed_precision(PREC_PRF, selfEnt[ID]) << " bps." << '\n';
}

inline void FCM::self_compress_alloc() {
  for (auto& e : cmls4) e.reset();
  for (auto& e : lgtbl8) e.reset();
  for (auto& e : tbl32) e.reset();
  for (auto& e : tbl64) e.reset();
  cmls4.clear();
  lgtbl8.clear();
  tbl32.clear();
  tbl64.clear();

  for (const auto& m : tMs) {
    switch (m.cont) {
      case Container::sketch_8:
        cmls4.push_back(std::make_unique<CMLS4>(m.w, m.d));
        break;
      case Container::log_table_8:
        lgtbl8.push_back(std::make_unique<LogTable8>(m.k));
        break;
      case Container::table_32:
        tbl32.push_back(std::make_unique<Table32>(m.k));
        break;
      case Container::table_64:
        tbl64.push_back(std::make_unique<Table64>(m.k));
        break;
    }
  }
}

template <typename ContIter>
inline void FCM::self_compress_1(std::unique_ptr<Param>& par, ContIter cont,
                                 uint64_t ID) {
  uint64_t ctx{0};
  uint64_t ctxIr{(1ull << (2 * tMs[0].k)) - 1};
  uint64_t symsNo{0};
  prc_t sumEnt{0};
  std::ifstream seqF(par->seq);
  ProbPar pp{tMs[0].alpha, ctxIr /* mask: 1<<2k-1=4^k-1 */,
             static_cast<uint8_t>(tMs[0].k << 1u)};
  const auto totalSize = file_size(par->seq);
  prc_t entr;

  for (std::vector<char> buffer(FILE_READ_BUF, 0); seqF.peek() != EOF;) {
    seqF.read(buffer.data(), FILE_READ_BUF);
    for (auto it = std::begin(buffer); it != std::begin(buffer) + seqF.gcount();
         ++it) {
      const auto c = *it;
      if (c != '\n') {
        ++symsNo;
        if (tMs[0].ir == 0) {
          pp.config_ir0(c, ctx);
          if (c != 'N') {
            auto f = freqs_ir0<decltype((*cont)->query(0))>(cont, pp.l);
            entr = entropy(prob(std::begin(f), &pp));
          } else {
            entr = entropyN;
          }
          // cout << precision(PREC_PRF, entr) << '\n';
          sumEnt += entr;
          (*cont)->update(pp.l | pp.numSym);
          update_ctx_ir0(ctx, &pp);
        } else if (tMs[0].ir == 1) {
          pp.config_ir1(c, ctxIr);
          if (c != 'N') {
            auto f =
                freqs_ir1<decltype(2 * (*cont)->query(0))>(cont, pp.shl, pp.r);
            entr = entropy(prob(std::begin(f), &pp));
          } else {
            entr = entropyN;
          }
          // cout << precision(PREC_PRF, entr) << '\n';
          sumEnt += entr;
          (*cont)->update((pp.revNumSym << pp.shl) | pp.r);
          update_ctx_ir1(ctxIr, &pp);
        } else if (tMs[0].ir == 2) {
          pp.config_ir2(c, ctx, ctxIr);
          if (c != 'N') {
            auto f = freqs_ir2<decltype(2 * (*cont)->query(0))>(cont, &pp);
            entr = entropy(prob(std::begin(f), &pp));
          } else {
            entr = entropyN;
          }
          // cout << precision(PREC_PRF, entr) << '\n';
          sumEnt += entr;
          (*cont)->update(pp.l | pp.numSym);
          update_ctx_ir2(ctx, ctxIr, &pp);
        }
        if (par->verbose) show_progress(symsNo, totalSize, par->message);
      }
    }
  }
  /*mut.lock();*/ selfEnt[ID] = sumEnt / symsNo; /*mut.unlock();*/
  seqF.close();
}

inline void FCM::self_compress_n(std::unique_ptr<Param>& par, uint64_t ID) {
  uint64_t symsNo{0};
  prc_t sumEnt{0};
  std::ifstream seqF(par->seq);
  auto cp = std::make_unique<CompressPar>();
  const auto nMdl = static_cast<uint8_t>(tMs.size() + tTMsSize);
  cp->nMdl = nMdl;
  cp->ctx.resize(nMdl);  // Fill with zeros (resize)
  cp->ctxIr.reserve(nMdl);
  for (const auto& mm : tMs) {  // Mask: 1<<2k - 1 = 4^k - 1
    cp->ctxIr.push_back((1ull << (mm.k << 1)) - 1);
    if (mm.child) cp->ctxIr.push_back((1ull << (mm.k << 1)) - 1);
  }
  cp->w.resize(nMdl, static_cast<prc_t>(1) / nMdl);
  cp->wNext.resize(nMdl, static_cast<prc_t>(0));
  cp->pp.reserve(nMdl);
  auto maskIter = begin(cp->ctxIr);
  for (const auto& mm : tMs) {
    cp->pp.emplace_back(mm.alpha, *maskIter++,
                        static_cast<uint8_t>(mm.k << 1u));
    if (mm.child)
      cp->pp.emplace_back(mm.child->alpha, *maskIter++,
                          static_cast<uint8_t>(mm.k << 1u));
  }
  const auto totalSize = file_size(par->seq);
  const auto self_compress_n_impl = [&](auto& cp, auto cont, uint8_t& n) {
    uint64_t valUpd = 0;
    self_compress_n_parent(cp, cont, n, valUpd);
    if (cp->mm.child) {
      ++cp->ppIt;
      ++cp->ctxIt;
      ++cp->ctxIrIt;
      compress_n_child(cp, cont, ++n);
    }
    (*cont)->update(valUpd);
  };

  for (std::vector<char> buffer(FILE_READ_BUF, 0); seqF.peek() != EOF;) {
    seqF.read(buffer.data(), FILE_READ_BUF);
    for (auto it = std::begin(buffer); it != std::begin(buffer) + seqF.gcount();
         ++it) {
      const auto c = *it;
      if (c != '\n') {
        ++symsNo;
        cp->c = c;
        cp->nSym = base_code(c);
        cp->ppIt = std::begin(cp->pp);
        cp->ctxIt = std::begin(cp->ctx);
        cp->ctxIrIt = std::begin(cp->ctxIr);
        cp->probs.clear();
        cp->probs.reserve(nMdl);
        auto tbl64_it = std::begin(tbl64);
        auto tbl32_it = std::begin(tbl32);
        auto lgtbl8_it = std::begin(lgtbl8);
        auto cmls4_it = std::begin(cmls4);

        uint8_t n = 0;  // Counter for the models
        for (const auto& mm : tMs) {
          cp->mm = mm;
          switch (mm.cont) {
            case Container::sketch_8:
              self_compress_n_impl(cp, cmls4_it++, n);
              break;
            case Container::log_table_8:
              self_compress_n_impl(cp, lgtbl8_it++, n);
              break;
            case Container::table_32:
              self_compress_n_impl(cp, tbl32_it++, n);
              break;
            case Container::table_64:
              self_compress_n_impl(cp, tbl64_it++, n);
              break;
          }
          ++n;
          ++cp->ppIt;
          ++cp->ctxIt;
          ++cp->ctxIrIt;
        }

        const auto ent = entropy(std::begin(cp->w), std::begin(cp->probs),
                                 std::end(cp->probs));
        // cout << precision(PREC_PRF, ent) << '\n';
        normalize(std::begin(cp->w), std::begin(cp->wNext),
                  std::end(cp->wNext));
        ////        update_weights(begin(cp->w), begin(cp->probs),
        /// end(cp->probs));
        sumEnt += ent;
        if (par->verbose) show_progress(symsNo, totalSize, par->message);
      }
    }
  }
  /*mut.lock();*/ selfEnt[ID] = sumEnt / symsNo; /*mut.unlock();*/
  seqF.close();
}

template <typename ContIter>
inline void FCM::self_compress_n_parent(std::unique_ptr<CompressPar>& cp,
                                        ContIter cont, uint8_t n,
                                        uint64_t& valUpd) const {
  prc_t prb;

  if (cp->mm.ir == 0) {
    cp->ppIt->config_ir0(cp->c, *cp->ctxIt);
    if (cp->c != 'N') {
      auto f = freqs_ir0<decltype((*cont)->query(0))>(cont, cp->ppIt->l);
      prb = prob(begin(f), cp->ppIt);
    } else {
      prb = 1.0 / std::pow(2.0, entropyN);
    }
    cp->probs.push_back(prb);
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.gamma, prb);
    valUpd = cp->ppIt->l | cp->ppIt->numSym;
    update_ctx_ir0(*cp->ctxIt, cp->ppIt);
  } else if (cp->mm.ir == 1) {
    cp->ppIt->config_ir1(cp->c, *cp->ctxIrIt);
    if (cp->c != 'N') {
      auto f = freqs_ir1<decltype(2 * (*cont)->query(0))>(cont, cp->ppIt->shl,
                                                          cp->ppIt->r);
      prb = prob(std::begin(f), cp->ppIt);
    } else {
      prb = 1.0 / std::pow(2.0, entropyN);
    }
    cp->probs.push_back(prb);
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.gamma, prb);
    valUpd = (cp->ppIt->revNumSym << cp->ppIt->shl) | cp->ppIt->r;
    update_ctx_ir1(*cp->ctxIrIt, cp->ppIt);
  } else if (cp->mm.ir == 2) {
    cp->ppIt->config_ir2(cp->c, *cp->ctxIt, *cp->ctxIrIt);
    if (cp->c != 'N') {
      auto f = freqs_ir2<decltype(2 * (*cont)->query(0))>(cont, cp->ppIt);
      prb = prob(std::begin(f), cp->ppIt);
    } else {
      prb = 1.0 / std::pow(2.0, entropyN);
    }
    cp->probs.push_back(prb);
    cp->wNext[n] = weight_next(cp->w[n], cp->mm.gamma, prb);
    valUpd = cp->ppIt->l | cp->ppIt->numSym;
    update_ctx_ir2(*cp->ctxIt, *cp->ctxIrIt, cp->ppIt);
  }
}

void FCM::aggregate_slf_ent(std::vector<PosRow>& pos_out, uint8_t round,
                            uint8_t run_num, std::string ref,
                            bool no_redun) const {
  auto selfEnt_beg = std::begin(selfEnt);

  for (auto& row : pos_out)
    if (row.round == round && row.run_num == run_num && row.ref == ref)
      row.self_ent = (!no_redun ? *selfEnt_beg++ : DBLANK);
}

template <typename OutT, typename ContIter>
auto FCM::freqs_ir0(ContIter cont, uint64_t l) const
    -> std::array<OutT, CARDIN> {
  // return {(*cont)->query(l), (*cont)->query(l | 1ull), (*cont)->query(l |
  // 2ull),
  //         (*cont)->query(l | 3ull)};
  return (*cont)->query_counters(l);
}

template <typename OutT, typename ContIter>
auto FCM::freqs_ir1(ContIter cont, uint64_t shl, uint64_t r) const
    -> std::array<OutT, CARDIN> {
  return {static_cast<OutT>((*cont)->query((3ull << shl) | r)),
          static_cast<OutT>((*cont)->query((2ull << shl) | r)),
          static_cast<OutT>((*cont)->query((1ull << shl) | r)),
          static_cast<OutT>((*cont)->query(r))};
}

template <typename OutT, typename ContIter, typename ProbParIter>
auto FCM::freqs_ir2(ContIter cont, ProbParIter pp) const
    -> std::array<OutT, CARDIN> {
  return {
      static_cast<OutT>((*cont)->query(pp->l) +
                        (*cont)->query((3ull << pp->shl) | pp->r)),
      static_cast<OutT>((*cont)->query(pp->l | 1ull) +
                        (*cont)->query((2ull << pp->shl) | pp->r)),
      static_cast<OutT>((*cont)->query(pp->l | 2ull) +
                        (*cont)->query((1ull << pp->shl) | pp->r)),
      static_cast<OutT>((*cont)->query(pp->l | 3ull) + (*cont)->query(pp->r))};
}

inline prc_t FCM::weight_next(prc_t w, prc_t g, prc_t p) const {
  //    return pow(w, g) * p;
  return Power(w, g) * p;
}

template <typename FreqIter>
inline void FCM::correct_stmm(std::unique_ptr<CompressPar>& cp,
                              FreqIter fFirst) const {
  const auto best_id = [=](FreqIter fFirst) {
    //  if (are_all(fFirst, 0) || are_all(fFirst, 1)) {
    //  if (are_all(fFirst, 0)) { // The same as GeCo
    if (are_all(fFirst, 1)) {  // Seems to be the best
      return static_cast<uint8_t>(255);
    }
    const auto maxPos = std::max_element(fFirst, fFirst + CARDIN);
    if (has_multi_max(fFirst, maxPos)) {
      return static_cast<uint8_t>(254);
    }
    return static_cast<uint8_t>(maxPos - fFirst);
  };
  auto stmm = cp->mm.child;
  const auto best = best_id(fFirst);

  if (stmm->enabled) {
    if (best == static_cast<uint8_t>(255))
      miss_stmm(stmm);
    else if (best == static_cast<uint8_t>(254) || best == cp->nSym)
      hit_stmm(stmm);
    else {
      miss_stmm(stmm);
      switch (stmm->ir) {
        case 0:
          cp->ppIt->config_ir0(best);
          break;
        case 1:
          cp->ppIt->config_ir1(best);
          break;
        case 2:
          cp->ppIt->config_ir2(best);
          break;
        default:
          break;
      }
    }
  } else if (!stmm->enabled && best != static_cast<uint8_t>(255) &&
             best != static_cast<uint8_t>(254)) {
    stmm->enabled = true;
#ifdef ARRAY_HISTORY
    std::fill(std::begin(stmm->history), std::end(stmm->history), false);
#else
    stmm->history = 0u;
#endif
  }
}
////template <typename FreqIter>
////inline bool FCM::correct_stmm
////(unique_ptr<CompressPar>& cp, const FreqIter& fFirst) {
////  auto stmm = cp->mm.child;
////  const auto best = best_id(fFirst);
////  if (stmm->enabled) {
////    if (best==static_cast<uint8_t>(255))
////      miss_stmm(stmm);
////    else if (best==static_cast<uint8_t>(254) || best==cp->nSym)
////      hit_stmm(stmm);
////    else {
////      miss_stmm(stmm);
////      stmm->ir==0 ? cp->ppIt->config(best) : cp->ppIt->config_ir(best);
////    }
////  }
////  else if (!stmm->enabled &&
////           best!=static_cast<uint8_t>(255) &&
/// best!=static_cast<uint8_t>(254)) { /    stmm->enabled = true; /    #ifdef
/// ARRAY_HISTORY /    std::fill(stmm->begin(history), end(stmm->history),
/// false); /    #else /    stmm->history = 0u; /    #endif /    // The
/// following makes the output entropy worst /    return true; /  } /  return
/// false;
////}

#ifdef ARRAY_HISTORY
template <typename History, typename Value>
inline void FCM::update_hist_stmm(History& hist, Value val) const {
  std::rotate(std::begin(hist), std::begin(hist) + 1, std::end(hist));
  hist.back() = val;
}

template <typename TmPar>
inline void FCM::hit_stmm(const TmPar& stmm) const {
  stmm_update_hist(stmm->history, false);
}

template <typename TmPar>
inline void FCM::miss_stmm(TmPar stmm) const {
  if (pop_count(std::begin(stmm->history), stmm->k) > stmm->thresh)
    stmm->enabled = false;
  else
    stmm_update_hist(stmm->history, true);
}
#else
template <typename History, typename Value>
inline void FCM::update_hist_stmm(History& hist, Value val,
                                  uint32_t mask) const {
  hist = ((hist << 1u) | val) & mask;  // ull for 64 bits
}

template <typename TmPar /*Tolerant model parameter*/>
inline void FCM::hit_stmm(const TmPar& stmm) const {
  update_hist_stmm(stmm->history, 0u, stmm->mask);
}

template <typename TmPar>
inline void FCM::miss_stmm(TmPar stmm) const {
  if (pop_count(stmm->history) > stmm->thresh)
    stmm->enabled = false;
  else
    update_hist_stmm(stmm->history, 1u, stmm->mask);
}
#endif

template <typename FreqIter, typename ProbParIter>
inline prc_t FCM::prob(FreqIter fFirst, ProbParIter pp) const {
  return (*(fFirst + pp->numSym) + pp->alpha) /
         std::accumulate(fFirst, fFirst + CARDIN, pp->sAlpha);

  // const uint64_t aDen = 1 / pp->alpha;
  // // const uint64_t aDen = 1000;

  // uint64_t n0 = *(fFirst);
  // uint64_t n1 = *(fFirst + 1);
  // uint64_t n2 = *(fFirst + 2);
  // uint64_t n3 = *(fFirst + 3);
  // uint64_t n = *(fFirst + pp->numSym);

  // uint64_t freq0 = 1 + aDen * n0;
  // uint64_t freq1 = 1 + aDen * n1;
  // uint64_t freq2 = 1 + aDen * n2;
  // uint64_t freq3 = 1 + aDen * n3;
  // uint64_t freq_n = 1 + aDen * n;

  // uint64_t cumul = freq0 + freq1 + freq2 + freq3;

  // return (prc_t)freq_n / cumul;
}

template <typename FreqIter, typename ProbParIter>
auto FCM::entropy(FreqIter fFirst, ProbParIter pp) const -> prc_t {
  auto prob_rev = std::accumulate(fFirst, fFirst + CARDIN, pp->sAlpha) /
                  (*(fFirst + pp->numSym) + pp->alpha);
  return std::log2(prob_rev);
}

inline prc_t FCM::entropy(prc_t P) const {
  // First version
  return std::log2(1 / P);
  //  return -std::log2(P);

  // Second version
  //  static auto prevP  = static_cast<prc_t>(1); // Can't be zero
  //  static auto result = static_cast<prc_t>(1); // Can't be zero
  //  if (P == prevP)
  //    return result;
  //  prevP  = P;
  //  result = -log2(P);
  //  return result;

  // Third version
  //  static constexpr int n_buckets = 4;  // Should be a power of 2
  //  static struct { prc_t p; prc_t result; } cache[n_buckets];
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
inline prc_t FCM::entropy(WIter wFirst, PIter PFirst, PIter PLast) const {
  return std::log2(1 /
                   inner_product(PFirst, PLast, wFirst, static_cast<prc_t>(0)));
  //  return -std::log2(
  //    inner_product(PFirst, PLast, wFirst, static_cast<prc_t>(0)));

  // todo
  // const auto p = inner_product(PFirst, PLast, wFirst, static_cast<prc_t>(0));
  // // if (*(PFirst + 1) > 1)
  // // if (*PFirst!=2.0 && p > 1)
  // if (p > 1)
  //   std::cerr << "\n"
  //             << "p1=" << *PFirst << "  w1=" << *wFirst
  //             << "  p2=" << *(PFirst + 1) << "  w2=" << *(wFirst + 1)
  //             << "   p=" << p;

  // return std::log2(1 /p);
}

// template <typename OutIter, typename InIter>
// inline void FCM::update_weights
//(OutIter wFirst, InIter PFirst, InIter PLast) const {
//  const auto wFirstKeep = wFirst;
//  for (auto mIter=begin(Ms); PFirst!=PLast; ++mIter, ++wFirst, ++PFirst) {
//    *wFirst = pow(*wFirst, mIter->gamma) * *PFirst;
//    if (mIter->child) {
//      ++wFirst;  ++PFirst;
//////      if (mIter->child->enabled)  // Lowers the performance
//        *wFirst = pow(*wFirst, mIter->child->gamma) * *PFirst;
//////      else                        // Lowers the performance
//////        *wFirst = static_cast<prc_t>(0);
//    }
//  }
//  normalize(wFirstKeep, wFirst);
//}

template <typename ProbParIter>
inline void FCM::update_ctx_ir0(uint64_t& ctx, ProbParIter pp) const {
  ctx = (pp->l & pp->mask) | pp->numSym;
}

template <typename ProbParIter>
inline void FCM::update_ctx_ir1(uint64_t& ctxIr, ProbParIter pp) const {
  ctxIr = (pp->revNumSym << pp->shl) | pp->r;
}

template <typename ProbParIter>
inline void FCM::update_ctx_ir2(uint64_t& ctx, uint64_t& ctxIr,
                                ProbParIter pp) const {
  ctx = (pp->l & pp->mask) | pp->numSym;
  ctxIr = (pp->revNumSym << pp->shl) | pp->r;
}