//
// Created by morteza on 2/4/18.
//

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include "par.hpp"
#include "tbl64.hpp"
#include "tbl32.hpp"
#include "logtbl8.hpp"
#include "cmls4.hpp"

struct ModelPar {
  u8    ir;      // Inverted repeat
  u8    k;       // Context size
  float alpha;
  u8    mode;    // Tbl 64, Tbl 32, LogTbl 8, Sketch 4
  u64   w;       // Width of count-min-log sketch
  u8    d;       // Depth of count-min-log sketch
};

template <typename ctx_t>
struct Prob_s {
  Prob_s () = default;
  Prob_s (float a, ctx_t m, u8 s) : alpha(a), mask(m), shl(s) {}
  void config (char c, ctx_t ctx) { numSym=NUM[static_cast<u8>(c)];  l=ctx<<2; }
  void config (char c, ctx_t ctx, ctx_t ctxIr) {
    numSym=NUM[static_cast<u8>(c)];          l=ctx<<2;
    revNumSym=REVNUM[static_cast<u8>(c)];    r=ctxIr>>2;
  }
  float alpha;
  ctx_t mask;
  u8    shl;
  ctx_t l;
  u8    numSym;
  ctx_t r;
  u8    revNumSym;
};

#include <future>
class FCM    // Finite-context model
{
 public:
  explicit FCM    (const Param&);
  ~FCM            ();
  void buildModel (const Param&);          // Build FCM (finite-context model)
  void compress   (const Param&) const;

 private:
  vector<ModelPar> model;
  Table64*         tbl64;
  Table32*         tbl32;
  LogTable8*       logtbl8;
  CMLS4*           sketch4;
  u8               MODE_COMB;
  u8               IR_COMB;
  
  void setModels    (const Param&); // Set models parameters
  void allocModels  ();             // Allocate memory to models
  void setModesComb ();             // Set combination of modes of models
  void setIRsComb   ();             // Set combination of inv. repeats of models
  void bldMdlOneThr (const Param&); // Build models one thread
  void bldMdlMulThr (const Param&); // Build models multiple threads
  // Create data structure
  template <typename mask_t, typename ds_t>
  void createDS (const string&, mask_t, ds_t&);
  // Compress data structure
  template <typename mask_t, typename ds_t>
  void compDS1 (const string&, mask_t, const ds_t&) const;
  template <typename mask0_t, typename mask1_t, typename ds0_t, typename ds1_t>
  void compDS2 (const string&, mask0_t,mask1_t,const ds0_t&,const ds1_t&) const;
  template <typename mask0_t, typename mask1_t, typename mask2_t,
    typename ds0_t, typename ds1_t, typename ds2_t>
  void compDS3 (const string&, mask0_t, mask1_t, mask2_t,
                const ds0_t&, const ds1_t&, const ds2_t&) const;
  // Probability
  template <typename ds_t, typename ctx_t>
  double prob  (const ds_t&, const Prob_s<ctx_t>&) const; // Probability
  template <typename ds_t, typename ctx_t>
  double probR (const ds_t&, const Prob_s<ctx_t>&) const; // Prob. reciprocal
  template <typename ds_t, typename ctx_t>
  double probIr  (const ds_t&, const Prob_s<ctx_t>&) const; // Prob. IR
  template <typename ds_t, typename ctx_t>
  double probIrR (const ds_t&, const Prob_s<ctx_t>&) const; // Prob. IR recip
  
//  void setWeight (double*&, const double*&) const;
  void setWeight (double&, double&, double, double) const;
  void setWeight (double&, double&, double&, double, double, double) const;
  
  template <typename ctx_t>
  void updateCtx (ctx_t&, const Prob_s<ctx_t>&) const;
  template <typename ctx_t>
  void updateCtx (ctx_t&, ctx_t&, const Prob_s<ctx_t>&) const;
  
  // Print variadic inputs
  template <typename T>                   void print (T)          const;
  template <typename T, typename... Args> void print (T, Args...) const;
};

#endif //SMASHPP_FCM_HPP