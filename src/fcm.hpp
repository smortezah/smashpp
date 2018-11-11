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

namespace smashpp {
class FCM {   // Finite-context models
 public:
  prec_t aveEnt;
  
  explicit FCM  (Param&);
  void store    (const Param&);         // Build FCM (finite-context models)
  void compress (const Param&);
  void self_compress (const Param&);
////  void report     (const Param&) const;

 private:
  vector<MMPar>                 rMs;     // Ref Markov models
  vector<STMMPar>               rTMs;    // Ref Tolerant Markov models
  vector<MMPar>                 tMs;     // Tar Markov models
  vector<STMMPar>               tTMs;    // Tar Tolerant Markov models
  vector<unique_ptr<Table64>>   tbl64;
  vector<unique_ptr<Table32>>   tbl32;
  vector<unique_ptr<LogTable8>> lgtbl8;
  vector<unique_ptr<CMLS4>>     cmls4;

  void config (string&&, string&&);      // Set models parameters
  template <typename Iter>
  void set_Ms_TMs (Iter, Iter, vector<MMPar>&, vector<STMMPar>&);
  void set_cont (vector<MMPar>&);
  void show_info (const Param&) const;  // Show inputs information on the screen
  void alloc_model ();                  // Allocate memory to models

  void store_1 (const Param&);          // Build models one thread
  void store_n (const Param&);          // Build models multiple threads
  template <typename Mask, typename ContIter>
  void store_impl (const string&, Mask, ContIter);  // Fill data struct

  template <typename ContIter>
  void compress_1 (const Param&, ContIter); // Compress with 1 Markov model
  void compress_n (const Param&);           // Compress with n Markov Models
  template <typename ContIter>
  void compress_n_impl (unique_ptr<CompressPar>&, ContIter, u8&) const;
  template <typename ContIter>
  void compress_n_parent (unique_ptr<CompressPar>&, ContIter, u8) const;
  template <typename ContIter>
  void compress_n_child (unique_ptr<CompressPar>&, ContIter, u8) const;

  void self_compress_alloc ();
  template <typename ContIter>
  void self_compress_1 (const Param&, ContIter);
  void self_compress_n (const Param&);
  template <typename ContIter>
  void self_compress_n_impl (unique_ptr<CompressPar>&, ContIter, u8&) const;
  template <typename ContIter>
  void self_compress_n_parent (unique_ptr<CompressPar>&, ContIter, u8,
                               u64&) const;
  template <typename ContIter>
  void self_compress_n_child (unique_ptr<CompressPar>&, ContIter, u8) const;

    //  template <typename OutT, typename ContIter, typename ProbParIter>
//  void freqs    (array<OutT,4>&, ContIter, ProbParIter)   const;
  template <typename OutT, typename ContIter>
  void freqs (array<OutT,4>&, ContIter, u64) const;
  template <typename OutT, typename ContIter, typename ProbParIter>
  void freqs_ir (array<OutT,4>&, ContIter, ProbParIter) const;
  template <typename FreqIter>
  void correct_stmm (unique_ptr<CompressPar>&, FreqIter) const;
  template <typename FreqIter>
  u8 best_id (FreqIter) const;
#ifdef ARRAY_HISTORY
  template <typename History, typename Value>
  void update_hist_stmm (History&, Value) const;
  template <typename TmPar>
  void hit_stmm (const TmPar&) const;
  template <typename TmPar>
  void miss_stmm (TmPar) const;
#else
  template <typename History, typename Value>
  void update_hist_stmm (History&, Value, u32) const;
  template <typename TmPar>
  void hit_stmm (const TmPar&) const;
  template <typename Par>
  void miss_stmm (Par) const;
#endif
  template <typename FreqIter, typename ProbParIter>
  prec_t prob (FreqIter, ProbParIter) const; //Probability
  prec_t entropy (prec_t) const;
  template <typename WIter, typename PIter>
  prec_t entropy (WIter, PIter, PIter) const;
//  template <typename OutIter, typename InIter>
//  prec_t entropy (OutIter, InIter, InIter) const;
//  template <typename OutIter, typename InIter>
//  void update_weights (OutIter, InIter, InIter) const;
  template <typename ProbParIter>
  void update_ctx (u64&, ProbParIter) const;
  template <typename ProbParIter>
  void update_ctx_ir (u64&, u64&, ProbParIter) const;
};
}

#endif //SMASHPP_FCM_HPP