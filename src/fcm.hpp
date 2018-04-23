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
using std::unique_ptr;

struct ModelPar {
  u8    k;         // Context size
  u64   w;         // Width of count-min-log sketch
  u8    d;         // Depth of count-min-log sketch
  u8    Mir;       // Markov model Inverted repeat
  float Malpha;
  float Mgamma;
  u8    TMthresh;  // Substitutional tolerant Markov model threshold
  u8    TMir;
  float TMalpha;
  float TMgamma;
  u8    mode;      // Tbl 64, Tbl 32, LogTbl 8, Sketch 4
  ModelPar (u8);
  ModelPar (u8, u8);
  ModelPar (u8, u8, float);
  ModelPar (u8, u8, float, float);
};

//template <typename ctx_t>
//struct Prob_s {
//  float  alpha;
//  double sAlpha;
//  ctx_t  mask;
//  u8     shl;
//  ctx_t  l;
//  u8     numSym;
//  ctx_t  r;
//  u8     revNumSym;
//  Prob_s () = default;
//  Prob_s (float, ctx_t, u8);
//  void config (char, ctx_t);
//  void config (char, ctx_t, ctx_t);
//};

//#include <memory>//todo check if needed
class FCM    // Finite-context model
{
 public:
  double          aveEnt;
  
  explicit FCM    (const Param&);
//  void buildModel (const Param&);   // Build FCM (finite-context model)
//  void compress   (const Param&);
//  void report     (const Param&) const;

 private:
  vector<ModelPar>      model;
  unique_ptr<Table64>   tbl64;
  unique_ptr<Table32>   tbl32;
  unique_ptr<LogTable8> logtbl8;
  unique_ptr<CMLS4>     sketch4;
  u8                    MODE_COMB;
  u8                    IR_COMB;
  
  void config (const Param&); // Set models parameters
  template <typename inIter_t, typename vec_t/*=vector<string>*/>//Split by dlim
  void split        (inIter_t, inIter_t, char, vec_t&) const;
//  void allocModels  ();             // Allocate memory to models
//  void setModesComb ();             // Set combination of modes of models
//  void setIRsComb   ();             // Set combination of inv. repeats of models
//  void bldMdl1Thr   (const Param&); // Build models one thread
//  void bldMdlNThr   (const Param&); // Build models multiple threads
//  template <typename msk_t, typename ds_t>
//  void fillDS    (const string&, msk_t, ds_t&);    // Fill data structure
//  // Compress data structure
//  template <typename msk_t, typename ds_t>
//  void comp1mdl  (const string&, msk_t, const ds_t &);
//  template <typename msk0_t, typename msk1_t, typename ds0_t, typename ds1_t>
//  void comp2mdl  (const string&, msk0_t, msk1_t, const ds0_t&, const ds1_t&);
//  template <typename msk0_t, typename msk1_t, typename msk2_t,
//    typename ds0_t, typename ds1_t, typename ds2_t>
//  void comp3mdl  (const string&, msk0_t, msk1_t, msk2_t,
//                  const ds0_t&, const ds1_t&, const ds2_t&);
//  void comp4mdl  (const string&);   // It has all possible models
//  template <typename ds_t, typename ctx_t>
//  double prob    (const ds_t&, const Prob_s<ctx_t>&) const;  // Probability
////  double prob    (const ds_t&, const Prob_s<ctx_t>&) const;  // Probability
//  template <typename ds_t, typename ctx_t>
//  double probR   (const ds_t&, const Prob_s<ctx_t>&) const;  // Prob. reciprocal
//  template <typename ds_t, typename ctx_t>
//  double probIr  (const ds_t&, const Prob_s<ctx_t>&) const;  // Prob. IR
//  template <typename ds_t, typename ctx_t>
//  double probIrR (const ds_t&, const Prob_s<ctx_t>&) const;  // Prob. IR recip
//  template <u8 N>
//  double entropy (std::array<double,N>& w,
//                  const std::initializer_list<double>& Pm) const;
//  template <typename ctx_t>
//  void updateCtx (ctx_t&, const Prob_s<ctx_t>&) const;
//  template <typename ctx_t>
//  void updateCtx (ctx_t&, ctx_t&, const Prob_s<ctx_t>&) const;
//  // Print variadic inputs
//  template <typename T>
//  void print (T) const;
//  template <typename T, typename... Args>
//  void print (T, Args...) const;
};

#endif //SMASHPP_FCM_HPP