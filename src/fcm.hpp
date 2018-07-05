//
// Created by morteza on 2/4/18.
//

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include <memory>
#include "par.hpp"
#include "tbl64.hpp"
#include "tbl32.hpp"
#include "logtbl8.hpp"
#include "cmls4.hpp"
#include "mdlpar.hpp"

class FCM    // Finite-context models
{
 public:
  double aveEnt;
  
  explicit FCM (const Param&);
  void store (const Param&);   // Build FCM (finite-context models)
  void compress (const Param&);
////  void report     (const Param&) const;

 private:
  vector<MMPar>                 Ms;     // Markov models
  vector<STMMPar>               TMs;    // Tolerant Markov models
  vector<shared_ptr<Table64>>   tbl64;
  vector<shared_ptr<Table32>>   tbl32;
  vector<shared_ptr<LogTable8>> lgtbl8;
  vector<shared_ptr<CMLS4>>     cmls4;
  
  void config (const Param&); // Set models parameters
  void set_cont ();
  void alloc_model ();            // Allocate memory to models
  void store_1 (const Param&);    // Build models one thread
  void store_n (const Param&);    // Build models multiple threads
  template <typename Mask, typename ContIter>
  void store_impl (const string&, Mask, ContIter);  // Fill data structure
  // Compress data structure
  template <typename ContIter>
  void compress_1 (const string&, ContIter);        // 1 Markov model
  void compress_n (const string&);
  
  template <typename ContIter, typename ProbParIter, typename CtxIter,
    typename CtxIrIter>
  vector<double> probs_models (char, ContIter&, ProbParIter&, CtxIter&,
                               CtxIrIter&) const;
  
  template <typename OutT, typename ContIter, typename ProbParIter>
  array<OutT,4> freqs (ContIter, ProbParIter) const;
  template <typename OutT, typename ContIter, typename ProbParIter>
  array<OutT,4> freqs_ir (ContIter, ProbParIter) const;
  template <typename Iter>
  u8 best_sym (Iter, Iter) const;
//  template <typename ContIter, typename ProbParIter>
//  u8 best_sym (ContIter, ProbParIter) const;
  template <typename ContIter, typename ProbParIter>
  u8 best_sym_ir (ContIter, ProbParIter) const;
  template <typename ContIter, typename ProbParIter>
  u8 best_sym_abs (ContIter, ProbParIter) const;

  template <typename Par>
  void tm_hit (Par);
  template <typename Par>
  void tm_miss (Par);


  template <typename Iter>
  double prob_frml (Iter, u8, float, float) const;
  template <typename FreqIter, typename ProbParIter>
  double prob (FreqIter, ProbParIter) const;  // Probability
//  template <typename ContIter, typename ProbParIter>
//  double prob (ContIter, ProbParIter) const;  // Probability
//  template <typename ContIter, typename ProbParIter>
//  double prob_ir (ContIter, ProbParIter) const;  // Prob. IR
  double entropy (double) const;
  template <typename OutIter, typename InIter>
  double entropy (OutIter, InIter, InIter) const;
  template <typename OutIter, typename InIter>
  void update_weights (OutIter, InIter, InIter) const;
  template <typename Iter>
  void normalize (Iter, Iter) const;
  template <typename ProbParIter>
  void update_ctx (u64&, ProbParIter) const;
  template <typename ProbParIter>
  void update_ctx_ir (u64&, u64&, ProbParIter) const;
};

#endif //SMASHPP_FCM_HPP