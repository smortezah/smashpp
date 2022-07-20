// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt

#ifndef SMASHPP_MDLPAR_HPP
#define SMASHPP_MDLPAR_HPP

#include <memory>
#include "def.hpp"

namespace smashpp {
struct STMMPar;

struct MMPar {
  uint8_t k;   // Context size
  uint64_t w;  // Width of count-min-log sketch
  uint8_t d;   // Depth ...
  uint8_t ir;  // Inverted repeat
  prc_t alpha;
  prc_t gamma;
  Container cont;  // Tbl 64, Tbl 32, LogTbl 8, Sketch 4
  std::shared_ptr<STMMPar> child;

  MMPar() = default;  // Essential
  MMPar(uint8_t k_, uint64_t w_, uint8_t d_, uint8_t ir_, prc_t a_, prc_t g_)
      : k(k_),
        w(w_),
        d(d_),
        ir(ir_),
        alpha(a_),
        gamma(g_),
        cont(Container::table_64),
        child(nullptr) {}
  MMPar(uint8_t k_, uint8_t ir_, prc_t a_, prc_t g_)
      : MMPar(k_, 0, 0, ir_, a_, g_) {}
};

struct STMMPar {
  uint8_t k;
  uint8_t thresh;
  uint8_t ir;
  prc_t alpha;
  prc_t gamma;
  bool enabled;
#ifdef ARRAY_HISTORY
  std::vector<bool> history;
#else
  uint32_t history;  // k > 32 => change to uint64_t
#endif
  uint32_t mask;  // For updating the history

  STMMPar(uint8_t k_, uint8_t t_, uint8_t ir_, prc_t a_, prc_t g_)
      : k(k_),
        thresh(t_),
        ir(ir_),
        alpha(a_),
        gamma(g_),
        enabled(true),
        mask((1u << k) - 1u) {
#ifdef ARRAY_HISTORY
    history.resize(k);
#else
    history = 0;
#endif
  }
};

struct ProbPar {
  prc_t alpha;
  prc_t sAlpha;
  uint64_t mask;
  uint8_t shl;
  uint64_t l;
  uint8_t numSym;
  uint64_t r;
  uint8_t revNumSym;

  ProbPar() = default;
  ProbPar(prc_t alpha_, uint64_t mask_, uint8_t shiftLeft_)
      : alpha(alpha_), sAlpha(CARDIN * alpha), mask(mask_), shl(shiftLeft_) {}
  void config_ir0(uint64_t);
  void config_ir0(uint8_t);
  void config_ir0(char, uint64_t);
  void config_ir1(uint8_t);
  void config_ir1(char, uint64_t);
  void config_ir2(uint8_t);
  void config_ir2(char, uint64_t, uint64_t);
};

inline void ProbPar::config_ir0(uint64_t ctx) { l = ctx << 2u; }

inline void ProbPar::config_ir0(uint8_t nsym) { numSym = nsym; }

inline void ProbPar::config_ir0(char c, uint64_t ctx) {
  numSym = base_code(c);
  l = ctx << 2u;
}

inline void ProbPar::config_ir1(uint8_t nsym) {
  numSym = nsym;
  revNumSym = static_cast<uint8_t>(3 - nsym);
}

inline void ProbPar::config_ir1(char c, uint64_t ctxIr) {
  numSym = base_code(c);
  revNumSym = static_cast<uint8_t>(3 - numSym);
  r = ctxIr >> 2u;
}

inline void ProbPar::config_ir2(uint8_t nsym) {
  numSym = nsym;
  revNumSym = static_cast<uint8_t>(3 - nsym);
}

inline void ProbPar::config_ir2(char c, uint64_t ctx, uint64_t ctxIr) {
  numSym = base_code(c);
  l = ctx << 2u;
  revNumSym = static_cast<uint8_t>(3 - numSym);
  r = ctxIr >> 2u;
}

struct CompressPar {
  std::vector<uint64_t> ctx;
  std::vector<uint64_t> ctxIr;
  std::vector<prc_t> w;
  std::vector<prc_t> wNext;
  std::vector<prc_t> probs;
  std::vector<ProbPar> pp;
  std::vector<ProbPar>::iterator ppIt;
  std::vector<uint64_t>::iterator ctxIt;
  std::vector<uint64_t>::iterator ctxIrIt;
  uint8_t nMdl;
  uint8_t nSym;
  char c;
  MMPar mm;

  CompressPar() = default;
};
}  // namespace smashpp

#endif  // SMASHPP_MDLPAR_HPP