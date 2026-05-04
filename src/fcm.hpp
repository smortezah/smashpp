// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_FCM_HPP
#define SMASHPP_FCM_HPP

#include <memory>

#include "cmls4.hpp"
#include "logtbl8.hpp"
#include "mdlpar.hpp"
#include "par.hpp"
#include "tbl32.hpp"
#include "tbl64.hpp"

namespace smashpp {
/// Significant digits used when serializing entropy profile values.
///
/// Compression and filtering keep full `prc_t` precision; rounding happens only when profile values
/// are written to `.prf` files.
static constexpr uint8_t PREC_PRF{3};
static constexpr char TAR_ALT_N{'T'};  // Alter. to Ns in target file

/// Coordinates finite-context model storage, compression, and self-compression.
///
/// The reference models are built by `store()`, then `compress()` fills `profileEnt` for filtering
/// and segmentation. `profileEnt` intentionally stores full-precision entropy values even when
/// saved `.prf` output is rounded.
class FCM {  // Finite-context models
 public:
  prc_t aveEnt;
  std::vector<prc_t> profileEnt;
  std::vector<prc_t> selfEnt;
  std::vector<MMPar> rMs;  // Ref Markov models
  std::vector<MMPar> tMs;  // Tar Markov models
  uint64_t tarSegID;
  std::string tarSegMsg;

  explicit FCM(const Param&);
  void store(Param&, uint8_t);  // Build FCM
  /// Compresses the target and fills `profileEnt` according to `Param::sampleStep`.
  void compress(Param&, uint8_t);
  void self_compress(const Param&, uint64_t, uint8_t);
  void self_compress(const Param&, const SegmentView&, uint64_t, uint8_t);
  void aggregate_slf_ent(std::vector<PosRow>&, uint8_t, uint8_t, std::string, bool) const;

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
  void show_info(std::unique_ptr<Param>&) const;  // Show inputs info on the screen
  void alloc_model();                             // Allocate memory to models

  void store_1(const Param&);            // Build models one thread
  void store_n(const Param&);            // Build models multiple threads
  void store_all_1(const std::string&);  // Build multiple models in one pass
  template <typename Mask, typename ContIter>
  void store_impl(const std::string&, Mask, ContIter);  // Fill data struct

  template <typename ContIter>
  void compress_1(const Param&, ContIter);  // Compress with 1 model
  /// Compresses with multiple models and adaptive mixture weights.
  ///
  /// Default sampled runs still evolve every model on every symbol for backward compatibility.
  /// `--approx-sampled-models` switches unsampled symbols to a cheaper context-only update.
  void compress_n(const Param&);
  template <typename ContIter>
  void compress_n_parent(CompressPar&, ContIter, uint8_t) const;
  template <typename ContIter>
  void compress_n_child(CompressPar&, ContIter, uint8_t) const;
  void compress_n_parent_context(CompressPar&) const;
  void compress_n_child_context(CompressPar&) const;

  void self_compress(const Param&, const SegmentView*, uint64_t, uint8_t);
  void self_compress_alloc();
  template <typename ContIter>
  void self_compress_1(const Param&, ContIter, const SegmentView*, uint64_t);
  void self_compress_n(const Param&, const SegmentView*, uint64_t);
  template <typename ContIter>
  void self_compress_n_parent(CompressPar&, ContIter, uint8_t, uint64_t&) const;

  template <typename OutT, typename ContIter>
  auto freqs_ir0(ContIter, uint64_t) const -> std::array<OutT, CARDIN>;
  template <typename OutT, typename ContIter>
  auto freqs_ir1(ContIter, uint64_t, uint64_t) const -> std::array<OutT, CARDIN>;
  template <typename OutT, typename ContIter, typename ProbParIter>
  auto freqs_ir2(ContIter, ProbParIter) const -> std::array<OutT, CARDIN>;
  auto weight_next(prc_t, prc_t, prc_t) const -> prc_t;
  template <typename FreqIter>
  void correct_stmm(CompressPar&, FreqIter) const;
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
  template <typename FreqIter, typename ProbParIter>
  auto entropy(FreqIter, ProbParIter) const -> prc_t;
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
