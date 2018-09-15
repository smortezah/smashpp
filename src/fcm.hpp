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
  template <typename ContIter>
  void compress_1 (const string&, ContIter);  // Compress with 1 Markov model
  void compress_n (const string&);            // Compress with n Markov models
  template <typename ContIter>
  void compress_n_parent (shared_ptr<CompressPar>, ContIter);
  template <typename ContIter>
  void compress_n_child_enabled (shared_ptr<CompressPar>, ContIter);
  template <typename ContIter>
  void compress_n_child_disabled (shared_ptr<CompressPar>, ContIter);
  template <typename OutT, typename ContIter, typename ProbParIter>
  array<OutT,4> freqs (ContIter, ProbParIter) const;
  template <typename OutT, typename ContIter, typename ProbParIter>
  array<OutT,4> freqs_ir (ContIter, ProbParIter) const;
  template <typename Iter>
  u8 best_sym (Iter) const;
  template <typename Iter>
  u8 best_sym_abs (Iter) const;
  template<typename Par, typename Value>
  void stmm_update_hist (Par, Value);
  template <typename Par, typename FreqIter, typename ProbParIter>
  double stmm_hit_prob (Par, FreqIter, ProbParIter);
  template <typename Par, typename FreqIter, typename ProbParIter>
  double stmm_miss_prob (Par, u8, FreqIter, ProbParIter);
  template <typename Par, typename FreqIter, typename ProbParIter>
  double stmm_miss_prob_ir (Par, u8, FreqIter, ProbParIter);
  template <typename FreqIter, typename ProbParIter>
  double prob (FreqIter, ProbParIter) const;  // Probability
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