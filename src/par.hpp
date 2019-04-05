// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include "def.hpp"
#include "mdlpar.hpp"

namespace smashpp {
// todo
// template <typename Iterator>
// string get_arg (Iterator& iter, const string& shortName, const string&
// longName) {
//   if (*iter=="-"+shortName || *iter=="--"+longName)
//     return *++iter;
// }

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
  std::string report;
  bool compress;
  bool filter;
  bool segment;
  uint32_t ID;
  bool noRedun;
  std::vector<MMPar> refMs;
  std::vector<MMPar> tarMs;

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
        noRedun(false) {}

  void parse(int, char**&);
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