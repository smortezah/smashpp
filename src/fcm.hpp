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
  prec_t aveEnt;
  
  explicit FCM  (const Param&);
  void store    (const Param&);         // Build FCM (finite-context models)
  void compress (const Param&);
////  void report     (const Param&) const;

 private:
  vector<MMPar>                 Ms;     // Markov models
  vector<STMMPar>               TMs;    // Tolerant Markov models
  vector<shared_ptr<Table64>>   tbl64;
  vector<shared_ptr<Table32>>   tbl32;
  vector<shared_ptr<LogTable8>> lgtbl8;
  vector<shared_ptr<CMLS4>>     cmls4;
  
  void config      (const Param&);      // Set models parameters
  void set_cont    ();
  void alloc_model ();                  // Allocate memory to models

  void store_1     (const Param&);      // Build models one thread
  void store_n     (const Param&);      // Build models multiple threads
  template <typename Mask, typename ContIter>
  void store_impl  (const string&, Mask, ContIter);  // Fill data structure

  template <typename ContIter>
  void compress_1 (const string&, const string&, ContIter); //Compress with 1 MM
  void compress_n     (const string&, const string&); //Compress with n Markov M
  void compress_n_ave (const string&, const string&, shared_ptr<CompressPar>);
  template <typename ContIter>
  void compress_n_impl           (shared_ptr<CompressPar>, ContIter);
  template <typename ContIter>
  void compress_n_parent         (shared_ptr<CompressPar>, ContIter);
  template <typename ContIter>
  void compress_n_child_enabled  (shared_ptr<CompressPar>, ContIter);
  template <typename ContIter>
  void compress_n_child_disabled (shared_ptr<CompressPar>, ContIter);

  template <typename OutT, typename ContIter, typename ProbParIter>
  void freqs    (array<OutT,4>&, ContIter, ProbParIter)   const;
  template <typename OutT, typename ContIter, typename ProbParIter>
  void freqs_ir (array<OutT,4>&, ContIter, ProbParIter)   const;
  template<typename Par, typename Value>
  void stmm_update_hist  (Par, Value);
  template <typename Par, typename FreqIter, typename ProbParIter>
  prec_t stmm_hit_prob   (Par, FreqIter, ProbParIter);
  template <typename Par, typename FreqIter, typename ProbParIter>
  prec_t stmm_miss_prob  (Par, FreqIter, ProbParIter);
  template <typename FreqIter, typename ProbParIter>
  prec_t prob            (FreqIter, ProbParIter)   const;  // Probability
  prec_t entropy         (prec_t)                  const;
  template <typename OutIter, typename InIter>
  prec_t entropy         (OutIter, InIter, InIter) const;
  template <typename OutIter, typename InIter>
  void update_weights    (OutIter, InIter, InIter) const;
  template <typename ProbParIter>
  void update_ctx        (u64&, ProbParIter)       const;
  template <typename ProbParIter>
  void update_ctx_ir     (u64&, u64&, ProbParIter) const;
};

#endif //SMASHPP_FCM_HPP