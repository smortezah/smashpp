#ifndef SMASHPP_PAR_HPP
#define SMASHPP_PAR_HPP

#include <fstream>
#include <vector>
#include <algorithm>
#include <memory>
#include "assert.hpp"
#include "def.hpp"
#include "exception.hpp"
#include "container.hpp"
#include "file.hpp"
#include "mdlpar.hpp"
#include "vizdef.hpp"

namespace smashpp {
//todo
// template <typename Iterator>
// string get_arg (Iterator& iter, const string& shortName, const string& longName) {
//   if (*iter=="-"+shortName || *iter=="--"+longName)
//     return *++iter;
// }

class Param {  // Parameters
 public:
  string      ref, tar, seq;
  string      refName, tarName;
  bool        verbose;
  u8          level;
  u32         segSize;
  prc_t       entropyN;
  u8          nthr;
  u32         wsize;
  WType       wtype;
  u64         sampleStep;
  float       thresh;
  bool        manSegSize, manWSize, manThresh, manFilterScale;
  FilterScale filterScale;
  bool        saveSeq, saveProfile, saveFilter, saveSegment, saveAll;
  FileType    refType, tarType;
  bool        showInfo;
  string      report;
  bool        compress, filter, segment;
  u32         ID;
  bool        noRedun;
  vector<MMPar> refMs, tarMs;

  // Define Param::Param(){} in *.hpp => compile error
  Param () : verbose(false), level(LVL), segSize(SSIZE), entropyN(ENTR_N), 
    nthr(THRD), wsize(WS), wtype(WT), sampleStep(1ull), thresh(THRSH), 
    manSegSize(false), manWSize(false), manThresh(false), manFilterScale(false),
    filterScale(FS), saveSeq(false), saveProfile(false), saveFilter(false),
    saveSegment(false), saveAll(false), refType(FileType::SEQ), 
    tarType(FileType::SEQ), showInfo(true), compress(false), filter(false),
    segment(false), ID(0), noRedun(false) {}

  void parse (int, char**&);
  auto win_type (const string&) const -> WType;
  auto print_win_type () const -> string;
  auto filter_scale (const string&) const -> FilterScale;
  auto print_filter_scale () const -> string;

 private:
  template <typename Iter>
  void parseModelsPars (Iter, Iter, vector<MMPar>&);
  void help () const;
};

class VizParam {
 public:
  bool   verbose, inverse, regular, showNRC, showRedun;
  string image;
  u8     link, colorMode;
  float  opacity;
  u32    width, space, mult, start, min;
  bool   manMult;
  string posFile;
  u64    refTick, tarTick;
  bool   vertical;

  VizParam () : verbose(false), inverse(true), regular(true), showNRC(true),
    showRedun(true), image(IMAGE), link(LINK), colorMode(COLOR), opacity(OPAC),
    width(WDTH), space(SPC), mult(MULT), start(BEGN), min(MINP), manMult(false),
    refTick(0), tarTick(0), vertical(false) {}

  void parse (int, char**&);

 private:
  void help () const;
};


inline void Param::parse (int argc, char**& argv) {
  if (argc < 2) { help();  throw EXIT_SUCCESS; }

  vector<string> vArgs;    vArgs.reserve(static_cast<u64>(argc));
  for (int i=0; i!=argc; ++i)
    vArgs.emplace_back(static_cast<string>(argv[i]));

  bool man_rm=false, man_tm=false;
  string rModelsPars, tModelsPars;

  for (auto i=begin(vArgs); i!=end(vArgs); ++i) {
    if (*i=="-r"  || *i=="--ref") {
      if (i+1 != end(vArgs)) {
        ref = *++i;
        check_file(ref);
        refName = file_name(ref);
      } else error("reference file not specified. Use \"-r <fileName>\".");
    }
    else if (*i=="-t"  || *i=="--tar") {
      if (i+1 != end(vArgs)) {
        tar = *++i;
        check_file(tar);
        tarName = file_name(tar);
      } else error("target file not specified. Use \"-t <fileName>\".");
    }
    else if ((*i=="-l" || *i=="--level") && i+1!=end(vArgs)) {
      level = static_cast<u8>(stoi(*++i));
      auto range = make_unique<ValRange<u8>>(MIN_LVL, MAX_LVL, LVL, 
        "Level", "[]", "default", Problem::WARNING);
      range->assert(level);
    }
    else if ((*i=="-m" || *i=="--min") && i+1!=end(vArgs)) {
      manSegSize = true;
      segSize = stoul(*++i);
      auto range = make_unique<ValRange<u32>>(MIN_SSIZE, MAX_SSIZE, SSIZE, 
        "Minimum segment size", "[]", "default", Problem::WARNING);
      range->assert(segSize);
    }
    else if ((*i=="-rm" || *i=="--ref-model") && i+1!=end(vArgs)) {
      man_rm = true;
      rModelsPars = *++i;
      if (rModelsPars.front()=='-' || rModelsPars.empty())
        error("incorrect reference model parameters.");
      else
        parseModelsPars(begin(rModelsPars), end(rModelsPars), refMs);
    }
    else if ((*i=="-tm" || *i=="--tar-model") && i+1!=end(vArgs)) {
      man_tm = true;
      tModelsPars = *++i;
      if (tModelsPars.front()=='-' || tModelsPars.empty())
        error("incorrect target model parameters.");
      else
        parseModelsPars(begin(tModelsPars), end(tModelsPars), tarMs);
    }
    else if ((*i=="-w" || *i=="--wsize") && i+1!=end(vArgs)) {
      manWSize = true;
      wsize = static_cast<u32>(stoi(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_WS, MAX_WS, WS, 
        "Window size", "[]", "default", Problem::WARNING);
      range->assert(wsize);
    }
    else if ((*i=="-th"|| *i=="--thresh") && i+1!=end(vArgs)) {
      manThresh = true;
      thresh = stof(*++i);
      auto range = make_unique<ValRange<float>>(MIN_THRSH, MAX_THRSH, THRSH,
        "Threshold", "(]", "default", Problem::WARNING);
      range->assert(thresh);
    }
    else if ((*i=="-wt"|| *i=="--wtype") && i+1!=end(vArgs)) {
      const auto is_win_type = [] (const string& t) {
        if (t=="0" || t=="rectangular" || t=="1" || t=="hamming"  ||
            t=="2" || t=="hann"        || t=="3" || t=="blackman" ||
            t=="4" || t=="triangular"  || t=="5" || t=="welch"    ||
            t=="6" || t=="sine"        || t=="7" || t=="nuttall")
          return true;
        return false;
      };
      const string cmd {*++i};
      auto set = make_unique<ValSet<WType>>(SET_WTYPE, WT, "Window type",
        "default", Problem::WARNING, win_type(cmd), is_win_type(cmd));
      set->assert(wtype);
    }
    else if ((*i=="-e" || *i=="--ent-n") && i+1!=end(vArgs)) {
      entropyN = static_cast<prc_t>(stod(*++i));
      auto range = make_unique<ValRange<prc_t>>(MIN_ENTR_N, MAX_ENTR_N, ENTR_N, 
        "Entropy of N bases", "[]", "default", Problem::WARNING);
      range->assert(entropyN);
    }
    else if ((*i=="-n" || *i=="--nthr") && i+1!=end(vArgs)) {
      nthr = static_cast<u8>(stoi(*++i));
      auto range = make_unique<ValRange<u8>>(MIN_THRD, MAX_THRD, THRD, 
        "Number of threads", "[]", "default", Problem::WARNING);
      range->assert(nthr);
    }
    else if ((*i=="-d" || *i=="--step") && i+1!=end(vArgs)) {
      sampleStep = stoull(*++i);
      if (sampleStep==0)  sampleStep=1ull;
    }
    else if ((*i=="-fs"|| *i=="--filter-scale") && i+1!=end(vArgs)) {
      manFilterScale = true;
      const auto is_filter_scale = [] (const string& s) {
        if (s=="S" || s=="small" || s=="M" || s=="medium" || 
            s=="L" || s=="large")
          return true;
        return false;
      };
      const string cmd {*++i};
      auto set = make_unique<ValSet<FilterScale>>(SET_FSCALE, FS,
        "Filter scale", "default", Problem::WARNING, filter_scale(cmd),
        is_filter_scale(cmd));
      set->assert(filterScale);
    }
    else if (*i=="-nr" || *i=="--no-redun")        noRedun    =true;
    else if (*i=="-sb" || *i=="--save-seq")        saveSeq    =true;
    else if (*i=="-sp" || *i=="--save-profile")    saveProfile=true;
    else if (*i=="-sf" || *i=="--save-fitler")     saveFilter =true;
    else if (*i=="-ss" || *i=="--save-segment")    saveSegment=true;
    else if (*i=="-sa" || *i=="--save-all")        saveAll    =true;
    else if (*i=="-compress")                      compress   =true;
    else if (*i=="-filter")                        filter     =true;
    else if (*i=="-segment")                       segment    =true;
    else if (*i=="-R"  || *i=="--report")
      report = (i+1!=end(vArgs)) ? *++i : "report.txt";
    else if (*i=="-h"  || *i=="--help") { help();  throw EXIT_SUCCESS; }
    else if (*i=="-v"  || *i=="--verbose")         verbose    =true;
  }

  // Mandatory args
  const bool has_t   {has(begin(vArgs), end(vArgs), "-t")   };
  const bool has_tar {has(begin(vArgs), end(vArgs), "--tar")};
  const bool has_r   {has(begin(vArgs), end(vArgs), "-r")   };
  const bool has_ref {has(begin(vArgs), end(vArgs), "--ref")};
  if (!has_t && !has_tar)
    error("target file not specified. Use \"-t <fileName>\".");
  else if (!has_r && !has_ref)
    error("reference file not specified. Use \"-r <fileName>\".");
  
  if (!man_rm && !man_tm) {
    parseModelsPars(begin(LEVEL[level]), end(LEVEL[level]), refMs);
    parseModelsPars(begin(REFFREE_LEVEL[level]), end(REFFREE_LEVEL[level]),
      tarMs);
  }
  else if (!man_rm && man_tm)  { refMs = tarMs; }
  else if (man_rm  && !man_tm) { tarMs = refMs; }

  manFilterScale = !manThresh;
  manFilterScale = !manWSize;
  
  keep_in_range(1ull, wsize, min(file_size(ref),file_size(tar))/sampleStep);

  // Fasta/Fastq to Seq
  auto convert_to_seq = [&](const string& f, const FileType& type) {
    rename(f.c_str(), (f+LBL_BAK).c_str());
    to_seq(f+LBL_BAK, f, type);
  };
  refType = file_type(ref);
  if      (refType==FileType::FASTA)  convert_to_seq(ref, FileType::FASTA);
  else if (refType==FileType::FASTQ)  convert_to_seq(ref, FileType::FASTQ);
  else if (refType!=FileType::SEQ) error("\""+refName+"\" has unknown format.");

  tarType = file_type(tar);
  if      (tarType==FileType::FASTA)  convert_to_seq(tar, FileType::FASTA);
  else if (tarType==FileType::FASTQ)  convert_to_seq(tar, FileType::FASTQ);
  else if (tarType!=FileType::SEQ) error("\""+tarName+"\" has unknown format.");
}

template <typename Iter>
inline void Param::parseModelsPars (Iter begin, Iter end, vector<MMPar>& Ms) {
  vector<string> mdls;      split(begin, end, ':', mdls);
  for (const auto& e : mdls) {
    // Markov and tolerant models
    vector<string> m_tm;    
    split(std::begin(e), std::end(e), '/', m_tm);
    vector<string> m;       
    split(std::begin(m_tm[0]), std::end(m_tm[0]), ',', m);

    if (m.size() == 4) {
      if (stoi(m[0]) > K_MAX_LGTBL8)
        Ms.emplace_back(
          MMPar(u8(stoi(m[0])), W, D, u8(stoi(m[1])), stof(m[2]), stof(m[3])));
      else
        Ms.emplace_back(
          MMPar(u8(stoi(m[0])), u8(stoi(m[1])), stof(m[2]), stof(m[3])));
    }
    else if (m.size() == 6) {
      Ms.emplace_back(
        MMPar(u8(stoi(m[0])), pow2(stoull(m[1])), u8(stoi(m[2])), 
          u8(stoi(m[3])), stof(m[4]), stof(m[5])));
    }
    
    // Tolerant models
    if (m_tm.size() == 2) {
      vector<string> tm;    
      split(std::begin(m_tm[1]), std::end(m_tm[1]), ',', tm);
      Ms.back().child = make_shared<STMMPar>(
        STMMPar(u8(stoi(m[0])), u8(stoi(tm[0])), u8(stoi(tm[1])), stof(tm[2]),
          stof(tm[3])));
    }
  }
}

inline void Param::help () const {
  // Print title
  const auto t = [&](string&& str) { cerr << bold(std::move(str)) << '\n'; };
  const auto l = [&](const string& str) { cerr << "  " << str << '\n'; }; //Line
  // Print column 1: left-aligned + column 2: left-aligned
  const auto ll = [&](const string& strL, u8 n, const string& strR) {
    cerr << "  " << std::left << std::setw(27+n*8) << strL;
    cerr.clear();
    cerr << strR << '\n';
  };
  // Print column 1: right-aligned + column 2: left-aligned
  const auto rl = [&](const string& strL, u8 n, const string& strR) {
    cerr << "  " << std::right << std::setw(27+n*8) << strL;
    cerr.clear();
    cerr << strR << '\n';
  };

  t("NAME");
  l("Smash++ v" + VERSION + " - rearrangements finder");
  l("");
  t("SYNOPSIS");
  l("./smashpp  " + underline("OPTIONS") + "...  -r " + underline("REF-FILE") +
    "  -t " + underline("TAR-FILE"));
  l("");
  t("SAMPLE");
  // l("./smashpp -t TAR -r REF");
  l("");
  t("DESCRIPTION");
  l(italic("Mandatory arguments"));
  ll(bold("-r") + ",  " + bold("--ref") + " " + underline("FILE"), 3,
    "reference file (Seq/Fasta/Fastq)");
  ll(bold("-t") + ",  " + bold("--tar") + " " + underline("FILE"), 3,
    "target file    (Seq/Fasta/Fastq)");
  l("");
  l(italic("Options"));
  ll(bold("-v") + ",  " + bold("--verbose"), 2, "more information");
  ll(bold("-l") + ",  " + bold("--level") + " " + underline("INT"), 3,
    "level of compression: [" + to_string(MIN_LVL) + ", " + 
    to_string(MAX_LVL) + "]");
  ll(bold("-m") + ",  " + bold("--min") + "   " + underline("INT"), 3,
    "min segment size: [" + to_string(MIN_SSIZE) + ", " + 
    to_string(MAX_SSIZE) + "]");
  ll(bold("-nr") + ", " + bold("--no-redun"), 2,
    "do NOT compute self complexity");
  ll(bold("-e") + ",  " + bold("--ent-n") + " " + underline("FLOAT"), 3,
    "Entropy of 'N's: [" + string_format("%.1f",MIN_ENTR_N) + ", " +
    string_format("%.1f",MAX_ENTR_N) + "]");
  ll(bold("-n") + ",  " + bold("--nthr") + "  " + underline("INT"), 3,
    "number of threads: [" + to_string(MIN_THRD) + ", " + to_string(MAX_THRD) +
    "]");
  ll(bold("-fs") + ", " + bold("--filter-scale") + " S|M|L", 2,
    "scale of the filter:");
  ll("", 0, "{S|small, M|medium, L|large}");
  ll(bold("-w") + ",  " + bold("--wsize") + " " + underline("INT"), 3,
    "window size: [" + to_string(MIN_WS) + ", " + to_string(MAX_WS) + "]");
  ll(bold("-wt") + ", " + bold("--wtype") + " " + underline("INT") + "/" +
    underline("STRING"), 4, "type of windowing function:");
  ll("", 0, "{0|rectangular, 1|hamming, 2|hann,");
  ll("", 0, "3|blackman, 4|triangular, 5|welch,");
  ll("", 0, "6|sine, 7|nuttall}");
  ll(bold("-d") + ",  " + bold("--step") + "   " + underline("INT"), 3,
    "sampling steps");
  ll(bold("-th") + ", " + bold("--thresh") + " " + underline("FLOAT"), 3,
    "threshold: [" + string_format("%.1f",MIN_THRSH) + ", " + 
    string_format("%.1f",MAX_THRSH) + "]");
  ll(bold("-sp") + ", " + bold("--save-profile"), 2, "save profile (*.prf)");
  ll(bold("-sf") + ", " + bold("--save-filter"), 2,
    "save filtered file (*.fil)");
  ll(bold("-sb") + ", " + bold("--save-seq"), 2,
    "save sequence (input: Fasta/Fastq)");
  ll(bold("-ss") + ", " + bold("--save-segment"), 2,
    "save segmented files (*-s_" + italic("i") + ")");
  ll(bold("-sa") + ", " + bold("--save-all"), 2, "save profile, filetered and");
  ll("", 0, "segmented files");
  // ll(bold("-R") + ",  " + bold("--report"), 2,
  //   "save results in file \"report\");
  ll(bold("-h") + ",  " + bold("--help"), 2, "usage guide");
  l(bold("-rm") + ", " + bold("--ref-model") + "  " + italic("k") + ",[" +
    italic("w") + "," + italic("d") + ",]ir," + italic("a") + "," + 
    italic("g") + "/" + italic("t") + ",ir," + italic("a") + "," + 
    italic("g") + ":...");
  l(bold("-tm") + ", " + bold("--tar-model") + "  " + italic("k") + ",[" +
    italic("w") + "," + italic("d") + ",]ir," + italic("a") + "," + 
    italic("g") + "/" + italic("t") + ",ir," + italic("a") + "," + 
    italic("g") + ":...");
  ll("", 0, "parameters of models");
  rl("(" + underline("INT") + ") ", 1, italic("k") + ":  context size");
  rl("(" + underline("INT") + ") ", 1, italic("w") + 
    ":  width of sketch in log2 form,");
  ll("", 0, "    e.g., set 10 for w=2^10=1024");
  rl("(" + underline("INT") + ") ", 1, italic("d") + ":  depth of sketch");
  rl("(" + underline("INT") + ") ", 1, "ir: inverted repeat: {0, 1, 2}");
  ll("", 0, "    0: regular (not inverted)");
  ll("", 0, "    1: inverted, solely");
  ll("", 0, "    2: both regular and inverted");
  rl("(" + underline("FLOAT") + ") ", 1, italic("a") + ":  estimator");
  rl("(" + underline("FLOAT") + ") ", 1, italic("g") + 
    ":  forgetting factor: [0.0, 1.0)");
  rl("(" + underline("INT") + ") ", 1, italic("t") + 
    ":  threshold (no. substitutions)");
  l("");
  t("AUTHOR");
  ll("Morteza Hosseini", 0, "seyedmorteza@ua.pt");
  l("");
  t("COPYRIGHT");
  l("Copyright (C) " + DEV_YEARS + ", IEETA, University of Aveiro. You may  ");
  l("redistribute copies of this Free software under the terms of the");
  l("GPL v3 (General Public License) <www.gnu.org/licenses/gpl.html>.");
  l("There is NO WARRANTY, to the extent permitted by law.");
}

inline WType Param::win_type (const string& t) const {
  if      (t=="0" || t=="rectangular")   return WType::RECTANGULAR;
  else if (t=="1" || t=="hamming")       return WType::HAMMING;
  else if (t=="2" || t=="hann")          return WType::HANN;
  else if (t=="3" || t=="blackman")      return WType::BLACKMAN;
  else if (t=="4" || t=="triangular")    return WType::TRIANGULAR;
  else if (t=="5" || t=="welch")         return WType::WELCH;
  else if (t=="6" || t=="sine")          return WType::SINE;
  else if (t=="7" || t=="nuttall")       return WType::NUTTALL;
  else                                   return WType::HANN;
}

inline string Param::print_win_type () const {
  switch (wtype) {
  case WType::RECTANGULAR:  return "Rectangular";  break;
  case WType::HAMMING:      return "Hamming";      break;
  case WType::HANN:         return "Hann";         break;
  case WType::BLACKMAN:     return "Blackman";     break;
  case WType::TRIANGULAR:   return "Triangular";   break;
  case WType::WELCH:        return "Welch";        break;
  case WType::SINE:         return "Sine";         break;
  case WType::NUTTALL:      return "Nuttall";      break;
  default:                  return "Rectangular";
  }
}

inline FilterScale Param::filter_scale (const string& s) const {
  if      (s=="S" || s=="small")   return FilterScale::S;
  else if (s=="M" || s=="medium")  return FilterScale::M;
  else if (s=="L" || s=="large")   return FilterScale::L;
  else                             return FilterScale::L;
}

inline string Param::print_filter_scale () const {
  switch (filterScale) {
  case FilterScale::S:  return "Small";   break;
  case FilterScale::M:  return "Medium";  break;
  case FilterScale::L:  return "Large";   break;
  default:              return "Large";
  }
}

inline void VizParam::parse (int argc, char**& argv) {
  if (argc < 3) { help();  throw EXIT_SUCCESS; }

  vector<string> vArgs;    vArgs.reserve(static_cast<u64>(argc));
  for (int i=0; i!=argc; ++i)
    vArgs.emplace_back(static_cast<string>(argv[i]));

  for (auto i=begin(vArgs); i!=end(vArgs); ++i) {
    if ((*i=="-o" || *i=="--out") && i+1!=end(vArgs))
      image = *++i;
    else if ((*i=="-p" || *i=="--opacity") && i+1!=end(vArgs)) {
      opacity = stof(*++i);
      auto range = make_unique<ValRange<float>>(MIN_OPAC, MAX_OPAC, OPAC, 
        "Opacity", "[]", "default", Problem::WARNING);
      range->assert(opacity);
    }
    else if ((*i=="-l" || *i=="--link") && i+1!=end(vArgs)) {
      link = static_cast<u8>(stoul(*++i));
      auto range = make_unique<ValRange<u8>>(MIN_LINK, MAX_LINK, LINK, 
        "Link", "[]", "default", Problem::WARNING);
      range->assert(link);
    }
    else if ((*i=="-m" || *i=="--min") && i+1!=end(vArgs)) {
      min = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_MINP, MAX_MINP, MINP,
        "Min", "[]", "default", Problem::WARNING);
      range->assert(min);
    }
    else if ((*i=="-f" || *i=="--mult") && i+1!=end(vArgs)) {
      manMult = true;
      mult = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_MULT, MAX_MULT, MULT,
        "Mult", "[]", "default", Problem::WARNING);
      range->assert(mult);
    }
    else if ((*i=="-b" || *i=="--begin") && i+1!=end(vArgs)) {
      start = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_BEGN, MAX_BEGN, BEGN,
        "Begin", "[]", "default", Problem::WARNING);
      range->assert(start);
    }
    else if ((*i=="-rt" || *i=="--ref-tick") && i+1!=end(vArgs)) {
      refTick = stoull(*++i);
      auto range = make_unique<ValRange<u64>>(MIN_TICK, MAX_TICK, TICK,
        "Tick hop for reference", "[]", "default", Problem::WARNING);
      range->assert(refTick);
    }
    else if ((*i=="-tt" || *i=="--tar-tick") && i+1!=end(vArgs)) {
      tarTick = stoull(*++i);
      auto range = make_unique<ValRange<u64>>(MIN_TICK, MAX_TICK, TICK,
        "Tick hop for target", "[]", "default", Problem::WARNING);
      range->assert(tarTick);
    }
    else if ((*i=="-c" || *i=="--color") && i+1!=end(vArgs)) {
      colorMode = static_cast<u8>(stoi(*++i));
      auto range = make_unique<ValRange<u8>>(MIN_COLOR, MAX_COLOR, COLOR, 
        "Color", "[]", "default", Problem::WARNING);
      range->assert(colorMode);
    }
    else if ((*i=="-w" || *i=="--width") && i+1!=end(vArgs)) {
      width = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_WDTH, MAX_WDTH, WDTH,
        "Width", "[]", "default", Problem::WARNING);
      range->assert(width);
    }
    else if ((*i=="-s" || *i=="--space") && i+1!=end(vArgs)) {
      space = static_cast<u32>(stoul(*++i));
      auto range = make_unique<ValRange<u32>>(MIN_SPC, MAX_SPC, SPC,
        "Space", "[]", "default", Problem::WARNING);
      range->assert(space);
    }
    else if (*i=="-nn" || *i=="--no-nrc")      showNRC  =false;
    else if (*i=="-vv" || *i=="--vertical")    vertical =true;
    else if (*i=="-nr" || *i=="--no-redun")    showRedun=false;
    else if (*i=="-ni" || *i=="--no-inv")      inverse  =false;
    else if (*i=="-ng" || *i=="--no-reg")      regular  =false;
    else if (*i=="-v"  || *i=="--verbose")     verbose  =true;
    else if (*i=="-h"  || *i=="--help") { help();   throw EXIT_SUCCESS; }
  }
  posFile = vArgs.back();
}

inline void VizParam::help () const {
  // Print title
  const auto t = [&](string&& str) { cerr << bold(std::move(str)) << '\n'; };
  const auto l = [&](const string& str) { cerr << "  " << str << '\n'; }; //Line
  // Print column 1: left-aligned + column 2: left-aligned
  const auto ll = [&](const string& strL, u8 n, const string& strR) {
    cerr << "  " << std::left << std::setw(27+n*8) << strL;
    cerr.clear();
    cerr << strR << '\n';
  };

  t("NAME");
  l("Smash++ Visualizer v" + VERSION + " - Visualization of Samsh++ output");
  l("");
  t("SYNOPSIS");
  l("./smashpp -viz  " + underline("OPTIONS") + "...  -o " + 
    underline("SVG-FILE") + "  " + underline("POS-FILE"));
  l("");
  t("SAMPLE");
  // l("./smashpp -viz -o out.svg ab.pos");
  l("");
  t("DESCRIPTION");
  l(italic("Mandatory arguments") + ":");
  ll(underline("POS-FILE"), 1, "positions file, generated by");
  ll("", 0, "Smash++ tool (*.pos)");
  l("");
  l(italic("Options") + ":");
  ll(bold("-v") + ",  " + bold("--verbose"), 2, "more information");
  ll(bold("-o") + ",  " + bold("--out") + " " + underline("SVG-FILE"), 3, 
    "output image name (*.svg)");
  ll(bold("-vv") + ", " + bold("--vertical"), 2, "vertical view");
  ll(bold("-nn") + ", " + bold("--no-nrc"), 2, "do NOT show normalized");
  ll("", 0, "relative compression (NRC)");
  ll(bold("-nr") + ", " + bold("--no-redun"), 2, "do NOT show self complexity");
  ll(bold("-ni") + ", " + bold("--no-inv"), 2, "do NOT show inverse maps");
  ll(bold("-ng") + ", " + bold("--no-reg"), 2, "do NOT show regular maps");
  ll(bold("-l") + ",  " + bold("--link") + "     " + underline("INT"), 3,
    "type of the link between maps: [" + to_string(MIN_LINK) + ", " +
    to_string(MAX_LINK) + "]");
  ll(bold("-c") + ",  " + bold("--color") + "    " + underline("INT"), 3,
    "color mode: [" + to_string(MIN_COLOR) + ", " + to_string(MAX_COLOR) + "]");
  ll(bold("-p") + ",  " + bold("--opacity") + "  " + underline("FLOAT"), 3,
    "opacity: [" + string_format("%.1f",MIN_OPAC) + ", " + 
    string_format("%.1f",MAX_OPAC) + "]");
  ll(bold("-w") + ",  " + bold("--width") + "    " + underline("INT"), 3, 
    "width of the sequence: [" + to_string(MIN_WDTH) + ", " + 
    to_string(MAX_WDTH) + "]");
  ll(bold("-s") + ",  " + bold("--space") + "    " + underline("INT"), 3, 
    "space between sequences: [" + to_string(MIN_SPC) + ", " + 
    to_string(MAX_SPC) + "]");
  ll(bold("-f") + ",  " + bold("--mult") + "     " + underline("INT"), 3,
    "multiplication factor for");
  ll("", 0, "color ID: [" + to_string(MIN_MULT) + ", " + to_string(MAX_MULT) +
    "]");
  ll(bold("-b") + ",  " + bold("--begin") + "    " + underline("INT"), 3,
    "beginning of color ID: [" + to_string(MIN_BEGN) + ", " + 
    to_string(MAX_BEGN) + "]");
  ll(bold("-rt") + ", " + bold("--ref-tick") + " " + underline("INT"), 3,
    "reference tick: [" + to_string(MIN_TICK) + ", " + to_string(MAX_TICK) +
    "]");
  ll(bold("-tt") + ", " + bold("--tar-tick") + " " + underline("INT"), 3,
    "target tick: [" + to_string(MIN_TICK) + ", " + to_string(MAX_TICK) + "]");
  ll(bold("-m") + ",  " + bold("--min") + "      " + underline("INT"), 3,
    "minimum block size: [" + to_string(MIN_MINP) + ", " + to_string(MAX_MINP) +
    "]");
  ll(bold("-h") + ",  " + bold("--help"), 2, "usage guide");
  l("");
  t("AUTHORS");
  ll("Morteza Hosseini", 0, "seyedmorteza@ua.pt");
  ll("Diogo   Pratas", 0, "pratas@ua.pt");
  l("");
  t("COPYRIGHT");
  l("Copyright (C) " + DEV_YEARS + ", IEETA, University of Aveiro. You may  ");
  l("redistribute copies of this Free software under the terms of the");
  l("GPL v3 (General Public License) <www.gnu.org/licenses/gpl.html>.");
  l("There is NO WARRANTY, to the extent permitted by law.");
}
}

#endif //SMASHPP_PAR_HPP