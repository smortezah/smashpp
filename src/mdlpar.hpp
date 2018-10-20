//
// Created by morteza on 30-06-2018.
//

#ifndef PROJECT_MDLPAR_HPP
#define PROJECT_MDLPAR_HPP

struct STMMPar;

struct MMPar {
  u8        k  /*:BIT_K*/;  // Context size
  u64       w;              // Width of count-min-log sketch
  u8        d  /*:BIT_D*/;  // Depth of count-min-log sketch
  u8        ir /*:BIT_IR*/; // Inverted repeat
  prec_t    alpha;
  prec_t    gamma;
  Container cont;          // Tbl 64, Tbl 32, LogTbl 8, Sketch 4
  shared_ptr<STMMPar> child;

  MMPar () = default;  // Essential
  MMPar (u8 k_, u64 w_, u8 d_, u8 ir_, prec_t a_, prec_t g_)
    : k(k_), w(w_), d(d_), ir(ir_), alpha(a_), gamma(g_),
      cont(Container::TABLE_64), child(nullptr) {}
  MMPar (u8 k_, u8 ir_, prec_t a_, prec_t g_) : MMPar(k_, 0, 0, ir_, a_, g_) {}
};

struct STMMPar {
  u8     k      /*:BIT_K*/;
  u8     thresh /*:BIT_THRESH*/;
  u8     ir     /*:BIT_IR*/;
  prec_t alpha;
  prec_t gamma;
  bool   enabled;
#ifdef ARRAY_HISTORY
  vector<bool> history;
#else
  u32    history;    // k > 32 => change to u64
#endif
  u32    mask;       // For updating the history

  STMMPar (u8 k_, u8 t_, u8 ir_, prec_t a_, prec_t g_)
    : k(k_), thresh(t_), ir(ir_), alpha(a_), gamma(g_), enabled(true),
      mask((1u<<k)-1u) {
    #ifdef ARRAY_HISTORY
      history.resize(k);
    #else
      history = 0;
    #endif
  }
};

struct ProbPar {
  prec_t alpha;
  u64    mask;
  u8     shl;
  prec_t sAlpha;
  u64    l;
  u8     numSym    /*:BIT_NSYM*/;
  u64    r;
  u8     revNumSym /*:BIT_NSYM*/;

  ProbPar () = default;
  ProbPar (prec_t a, u64 m, u8 sh)
    : alpha(a), mask(m), shl(sh), sAlpha(CARDIN*alpha) {}
  void config    (u64);
  void config    (u8);
  void config    (char, u64);
  void config_ir (u8);
  void config_ir (char, u64, u64);
};

inline void ProbPar::config (u64 ctx) {
  l = ctx<<2u;
}

inline void ProbPar::config (u8 nsym) {
  numSym = nsym;
}

inline void ProbPar::config (char c, u64 ctx) {
  numSym = NUM[static_cast<u8>(c)];
  l      = ctx<<2u;
}

inline void ProbPar::config_ir (u8 nsym) {
  numSym    = nsym;
  revNumSym = static_cast<u8>(3 - nsym);
}

inline void ProbPar::config_ir (char c, u64 ctx, u64 ctxIr) {
  numSym    = NUM[static_cast<u8>(c)];
  l         = ctx<<2u;
  revNumSym = static_cast<u8>(3 - numSym);
  r         = ctxIr>>2u;
}

struct CompressPar {
  vector<u64>               ctx;
  vector<u64>               ctxIr;
  vector<prec_t>            w;    // float <-> double
  vector<prec_t>            wNext;
  vector<prec_t>            probs;
  vector<ProbPar>           pp;
  vector<ProbPar>::iterator ppIt;
  vector<u64>::iterator     ctxIt;
  vector<u64>::iterator     ctxIrIt;
  u8                        nMdl /*:BIT_NMDL*/;
  u8                        nSym /*:BIT_NSYM*/;
  char                      c;
  MMPar                     mm;

  CompressPar () = default;
};

#endif //PROJECT_MDLPAR_HPP