// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2020, IEETA, University of Aveiro, Portugal.

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
static constexpr uint8_t PREC_PRF{3};  // Precisions - floats in Inf. prof
static constexpr char TAR_ALT_N{'T'};  // Alter. to Ns in target file

class FCM {  // Finite-context models
 public:
  prc_t aveEnt;
  std::vector<prc_t> selfEnt;
  std::vector<MMPar> rMs;  // Ref Markov models
  std::vector<MMPar> tMs;  // Tar Markov models
  uint64_t tarSegID;
  std::string tarSegMsg;

  explicit FCM(std::unique_ptr<Param>&);
  void store(std::unique_ptr<Param>&, uint8_t);  // Build FCM
  void compress(std::unique_ptr<Param>&, uint8_t);
  void self_compress(std::unique_ptr<Param>&, uint64_t, uint8_t);
  void aggregate_slf_ent(std::vector<PosRow>&, uint8_t, uint8_t, std::string,
                         bool) const;

 private:
  std::vector<std::unique_ptr<Table64>> tbl64;
  std::vector<std::unique_ptr<Table32>> tbl32;
  std::vector<std::unique_ptr<LogTable8>> lgtbl8;
  std::vector<std::unique_ptr<CMLS4>> cmls4;
  std::string message;
  prc_t entropyN;
  uint8_t rTMsSize;
  uint8_t tTMsSize;

  void set_cont(std::vector<MMPar>&);
  void show_info(
      std::unique_ptr<Param>&) const;  // Show inputs info on the screen
  void alloc_model();                  // Allocate memory to models

  void store_1(std::unique_ptr<Param>&);  // Build models one thread
  void store_n(std::unique_ptr<Param>&);  // Build models multiple threads
  template <typename Mask, typename ContIter>
  void store_impl(std::string, Mask, ContIter);  // Fill data struct

  template <typename ContIter>
  void compress_1(std::unique_ptr<Param>&, ContIter);  // Compress with 1 model
  void compress_n(std::unique_ptr<Param>&);            // Compress with n Models
  template <typename ContIter>
  void compress_n_parent(std::unique_ptr<CompressPar>&, ContIter,
                         uint8_t) const;
  template <typename ContIter>
  void compress_n_child(std::unique_ptr<CompressPar>&, ContIter, uint8_t) const;

  void self_compress_alloc();
  template <typename ContIter>
  void self_compress_1(std::unique_ptr<Param>&, ContIter, uint64_t);
  void self_compress_n(std::unique_ptr<Param>&, uint64_t);
  template <typename ContIter>
  void self_compress_n_parent(std::unique_ptr<CompressPar>&, ContIter, uint8_t,
                              uint64_t&) const;

  template <typename OutT, typename ContIter>
  void freqs_ir0(std::array<OutT, 4>&, ContIter, uint64_t) const;
  template <typename OutT, typename ContIter>
  void freqs_ir1(std::array<OutT, 4>&, ContIter, uint64_t, uint64_t) const;
  template <typename OutT, typename ContIter, typename ProbParIter>
  void freqs_ir2(std::array<OutT, 4>&, ContIter, ProbParIter) const;
  auto weight_next(prc_t, prc_t, prc_t) const -> prc_t;
  template <typename FreqIter>
  void correct_stmm(std::unique_ptr<CompressPar>&, FreqIter) const;
#ifdef ARRAY_HISTORY
  template <typename History, typename Value>
  void update_hist_stmm(History&, Value) const;
  template <typename TmPar>
  void hit_stmm(const TmPar&) const;
  template <typename TmPar>
  void miss_stmm(TmPar) const;
#else
  template <typename History, typename Value>
  void update_hist_stmm(History&, Value, uint32_t) const;
  template <typename TmPar>
  void hit_stmm(const TmPar&) const;
  template <typename Par>
  void miss_stmm(Par) const;
#endif
  template <typename FreqIter, typename ProbParIter>
  auto prob(FreqIter, ProbParIter) const -> prc_t;
  auto entropy(prc_t) const -> prc_t;
  template <typename WIter, typename PIter>
  auto entropy(WIter, PIter, PIter) const -> prc_t;
  template <typename ProbParIter>
  void update_ctx_ir0(uint64_t&, ProbParIter) const;
  template <typename ProbParIter>
  void update_ctx_ir1(uint64_t&, ProbParIter) const;
  template <typename ProbParIter>
  void update_ctx_ir2(uint64_t&, uint64_t&, ProbParIter) const;
};
}  // namespace smashpp

#endif  // SMASHPP_FCM_HPP