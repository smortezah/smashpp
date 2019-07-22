// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include "def.hpp"
#include "mdlpar.hpp"

namespace smashpp {
static constexpr uint8_t MIN_THRD{1};
static constexpr uint8_t MAX_THRD{8};
static constexpr uint8_t MIN_LVL{0};
static constexpr uint8_t MAX_LVL{5};
static constexpr uint32_t MIN_SSIZE{1};
static constexpr uint32_t MAX_SSIZE{0xffffffff};  // 2^32 - 1
static constexpr prc_t MIN_ENTR_N{0.0};
static constexpr prc_t MAX_ENTR_N{100.0};
static constexpr uint32_t MIN_WS{1};
static constexpr uint32_t MAX_WS{0xffffffff};  // 2^32 - 1
static constexpr auto FT{FilterType::hann};         // Window type -- filter
static constexpr float MIN_THRSH{0};
static constexpr float MAX_THRSH{20};
static constexpr uint8_t K_MAX_TBL64{11};   // Max ctx table 64     (128 MB mem)
static constexpr uint8_t K_MAX_TBL32{13};   // Max ctx table 32     (1   GB mem)
static constexpr uint8_t K_MAX_LGTBL8{14};  // Max ctx log table 8  (1   GB mem)
static constexpr uint64_t W{2 << 29ull};    // Width of CML sketch
static constexpr uint8_t D{5};              // Depth of CML sketch
static const std::string LBL_BAK{"_bk"};    // Label  - backup files
static const std::string POS_HDR{"#" + SMASHPP};  // Hdr of pos file
static constexpr int FILE_BUF{8 * 1024};  // 8K

// Visualization
static constexpr uint8_t MIN_LINK{1};
static constexpr uint8_t MAX_LINK{6};
static constexpr uint8_t MIN_COLOR{0};
static constexpr uint8_t MAX_COLOR{1};
static constexpr auto OPAC{0.9f};
static constexpr auto MIN_OPAC{0.0f};
static constexpr auto MAX_OPAC{1.0f};
static constexpr uint32_t WDTH{10};
static constexpr uint32_t MIN_WDTH{8};
static constexpr uint32_t MAX_WDTH{100};
static constexpr uint32_t SPC{40};
static constexpr uint32_t MIN_SPC{5};
static constexpr uint32_t MAX_SPC{200};
static constexpr uint32_t MULT{43};  // 5 colors without overlapping
static constexpr uint32_t MIN_MULT{1};
static constexpr uint32_t MAX_MULT{255};
static constexpr uint32_t BEGN{0};
static constexpr uint32_t MIN_BEGN{0};
static constexpr uint32_t MAX_BEGN{255};
static constexpr uint32_t MINP{1};
static constexpr uint32_t MIN_MINP{1};
static constexpr uint32_t MAX_MINP{0xffffffff};  // 2^32 - 1
static constexpr uint64_t TICK{100};             // Major tick
static constexpr uint64_t MIN_TICK{1};
static constexpr uint64_t MAX_TICK{0xffffffff};

class Param {
 public:
  std::string ref, tar;
  std::string refName, tarName;
  std::string seq;
  bool verbose;
  uint8_t level;
  uint32_t segSize;
  prc_t entropyN;
  uint8_t nthr;
  uint32_t filt_size;
  FilterType filt_type;
  uint64_t sampleStep;
  float thresh;
  bool man_level, manWSize, manThresh, manFilterScale;
  FilterScale filterScale;
  bool saveSeq, saveProfile, saveFilter, saveSegment, saveAll;
  FileType refType, tarType;
  bool showInfo;
  bool compress;
  bool filter;
  bool segment;
  uint32_t ID;
  bool noRedun;
  bool deep;
  bool asym_region;
  std::vector<MMPar> refMs, tarMs;
  std::string message;

  struct TarGuard {
    int16_t beg;
    int16_t end;
    TarGuard() : beg(0), end(0) {}
  };
  struct RefGuard {
    int16_t beg;
    int16_t end;
    RefGuard() : beg(0), end(0) {}
  };
  std::unique_ptr<TarGuard> tar_guard;
  std::unique_ptr<RefGuard> ref_guard;

  Param()  // Define Param::Param(){} in *.hpp => compile error
      : verbose(false),
        segSize(50),
        entropyN(2.0),
        nthr(4),
        filt_size(256),
        filt_type(FT),
        sampleStep(1ull),
        thresh(1.5),
        manWSize(false),
        manThresh(false),
        manFilterScale(false),
        filterScale(FilterScale::m),
        saveSeq(false),
        saveProfile(false),
        saveFilter(false),
        saveSegment(false),
        saveAll(false),
        refType(FileType::seq),
        tarType(FileType::seq),
        showInfo(true),
        ID(0),
        noRedun(false),
        deep(false),
        asym_region(false),
        tar_guard(std::make_unique<TarGuard>()),
        ref_guard(std::make_unique<RefGuard>()) {}

  void parse(int, char**&);
  auto win_type(std::string) const -> FilterType;
  auto print_win_type() const -> std::string;
  auto filter_scale(std::string) const -> FilterScale;
  auto print_filter_scale() const -> std::string;

 private:
  void set_auto_model_par();
  template <typename Iter>
  void parseModelsPars(Iter, Iter, std::vector<MMPar>&);
  void help() const;
};

class VizParam {
 public:
  bool verbose;
  bool inverse;
  bool regular;
  bool showNRC, showRedun;
  bool showN;
  std::string image;
  uint8_t link;
  uint8_t colorMode;
  float opacity;
  uint32_t width;
  uint32_t space;
  uint32_t mult;
  uint32_t start;
  uint32_t min;
  bool manMult;
  std::string posFile;
  uint64_t refTick, tarTick;
  bool tickHumanRead;
  bool vertical;
  std::string refName, tarName;

  VizParam()
      : verbose(false),
        inverse(true),
        regular(true),
        showNRC(true),
        showRedun(true),
        showN(false),
        image("map.svg"),
        link(1),
        colorMode(0),
        opacity(OPAC),
        width(WDTH),
        space(SPC),
        mult(MULT),
        start(BEGN),
        min(MINP),
        manMult(false),
        refTick(0),
        tarTick(0),
        tickHumanRead(true),
        vertical(false) {}

  void parse(int, char**&);

 private:
  void help() const;
};
}  // namespace smashpp

#endif  // SMASHPP_PAR_HPP