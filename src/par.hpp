// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include "def.hpp"
#include "mdlpar.hpp"

namespace smashpp {
static constexpr uint8_t THRD{4};  // No. threads
static constexpr uint8_t MIN_THRD{1};
static constexpr uint8_t MAX_THRD{8};
static constexpr uint8_t LVL{0};  // Level
static constexpr uint8_t MIN_LVL{0};
static constexpr uint8_t MAX_LVL{5};
static constexpr uint32_t SSIZE{1};  // Min segment size
static constexpr uint32_t MIN_SSIZE{1};
static constexpr uint32_t MAX_SSIZE{0xffffffff};  // 2^32 - 1
static constexpr prc_t ENTR_N{2.0};               // Entropy of Ns
static constexpr prc_t MIN_ENTR_N{0.0};
static constexpr prc_t MAX_ENTR_N{100.0};
static constexpr char TAR_ALT_N{'T'};  // Alter. to Ns in target file
static constexpr uint32_t WS{256};     // Window size -- filter
static constexpr uint32_t MIN_WS{1};
static constexpr uint32_t MAX_WS{0xffffffff};  // 2^32 - 1
static constexpr auto WT{WType::hann};         // Window type -- filter
static constexpr auto FS{FilterScale::l};      // Filt. scale -- filter
static constexpr float THRSH{1.5};             // Threshold   -- filter
static constexpr float MIN_THRSH{0};
static constexpr float MAX_THRSH{20};
static constexpr uint8_t K_MAX_TBL64{11};   // Max ctx table 64     (128 MB mem)
static constexpr uint8_t K_MAX_TBL32{13};   // Max ctx table 32     (1   GB mem)
static constexpr uint8_t K_MAX_LGTBL8{14};  // Max ctx log table 8  (1   GB mem)
static constexpr uint64_t W{2 << 29ull};    // Width of CML sketch
static constexpr uint8_t D{5};              // Depth of CML sketch
static constexpr uint8_t PREC_FIL{3};       // Precisions - floats in filt. file
static const std::string LBL_BAK{"_bk"};    // Label  - backup files
static const std::string POS_HDR{"#" + SMASHPP};  // Hdr of pos file
// static const std::string POS_NRC{"N"};      // Hdr of pos file
// static const std::string POS_REDUN{"R"};    // Hdr of pos file
static constexpr int FILE_BUF{8 * 1024};  // 8K
static constexpr double DBLANK{-2.0};

class Param {
 public:
  std::string ref;
  std::string tar;
  std::string seq;
  std::string refName;
  std::string tarName;
  bool verbose;
  uint8_t level;
  uint32_t segSize;
  prc_t entropyN;
  uint8_t nthr;
  uint32_t wsize;
  WType wtype;
  uint64_t sampleStep;
  float thresh;
  bool manSegSize;
  bool manWSize;
  bool manThresh;
  bool manFilterScale;
  FilterScale filterScale;
  bool saveSeq;
  bool saveProfile;
  bool saveFilter;
  bool saveSegment;
  bool saveAll;
  FileType refType;
  FileType tarType;
  bool showInfo;
  bool compress;
  bool filter;
  bool segment;
  uint32_t ID;
  bool noRedun;
  std::vector<MMPar> refMs;
  std::vector<MMPar> tarMs;
  int16_t ref_beg_guard;
  int16_t ref_end_guard;
  int16_t tar_beg_guard;
  int16_t tar_end_guard;

  // Define Param::Param(){} in *.hpp => compile error
  Param()
      : verbose(false),
        level(LVL),
        segSize(SSIZE),
        entropyN(ENTR_N),
        nthr(THRD),
        wsize(WS),
        wtype(WT),
        sampleStep(1ull),
        thresh(THRSH),
        manSegSize(false),
        manWSize(false),
        manThresh(false),
        manFilterScale(false),
        filterScale(FS),
        saveSeq(false),
        saveProfile(false),
        saveFilter(false),
        saveSegment(false),
        saveAll(false),
        refType(FileType::seq),
        tarType(FileType::seq),
        showInfo(true),
        compress(false),
        filter(false),
        segment(false),
        ID(0),
        noRedun(false),
        ref_beg_guard(0),
        ref_end_guard(0),
        tar_beg_guard(0),
        tar_end_guard(0) {}

  void parse(int, char**&, std::string="audible");
  auto win_type(std::string) const -> WType;
  auto print_win_type() const -> std::string;
  auto filter_scale(std::string) const -> FilterScale;
  auto print_filter_scale() const -> std::string;

 private:
  template <typename Iter>
  void parseModelsPars(Iter, Iter, std::vector<MMPar>&);
  void help() const;
};

class VizParam {
 public:
  bool verbose;
  bool inverse;
  bool regular;
  bool showNRC;
  bool showRedun;
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
  uint64_t refTick;
  uint64_t tarTick;
  bool tickHumanRead;
  bool vertical;
  std::string refName;
  std::string tarName;

  VizParam()
      : verbose(false),
        inverse(true),
        regular(true),
        showNRC(true),
        showRedun(true),
        image(IMAGE),
        link(LINK),
        colorMode(COLOR),
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