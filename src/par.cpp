// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.

#include <fstream>
#include <vector>
#include <algorithm>
#include <memory>
#include "par.hpp"
#include "assert.hpp"
#include "def.hpp"
#include "exception.hpp"
#include "container.hpp"
#include "file.hpp"
#include "mdlpar.hpp"
// #include "vizdef.hpp"
using namespace smashpp;

void Param::parse(int argc, char**& argv) {
  if (argc < 2) {
    help();
    throw EXIT_SUCCESS;
  }

  std::vector<std::string> vArgs;
  vArgs.reserve(static_cast<uint64_t>(argc));
  for (int i = 0; i != argc; ++i)
    vArgs.emplace_back(static_cast<std::string>(argv[i]));

  bool man_rm{false};
  bool man_tm{false};
  std::string rModelsPars;
  std::string tModelsPars;

  for (auto i = std::begin(vArgs); i != std::end(vArgs); ++i) {
    if (*i == "-r" || *i == "--ref") {
      if (i + 1 != std::end(vArgs)) {
        ref = *++i;
        check_file(ref);
        refName = file_name(ref);
      } else {
        error("reference file not specified. Use \"-r <fileName>\".");
      }
    } else if (*i == "-t" || *i == "--tar") {
      if (i + 1 != std::end(vArgs)) {
        tar = *++i;
        check_file(tar);
        tarName = file_name(tar);
      } else {
        error("target file not specified. Use \"-t <fileName>\".");
      }
    } else if ((*i == "-l" || *i == "--level") && i + 1 != std::end(vArgs)) {
      level = static_cast<uint8_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_LVL, MAX_LVL, LVL, "Level", "[]", "default", Problem::warning);
      range->assert(level);
    } else if ((*i == "-m" || *i == "--min") && i + 1 != end(vArgs)) {
      manSegSize = true;
      segSize = std::stoul(*++i);
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_SSIZE, MAX_SSIZE, SSIZE, "Minimum segment size", "[]", "default",
          Problem::warning);
      range->assert(segSize);
    } else if ((*i == "-rm" || *i == "--ref-model") &&
               i + 1 != std::end(vArgs)) {
      man_rm = true;
      rModelsPars = *++i;
      if (rModelsPars.front() == '-' || rModelsPars.empty())
        error("incorrect reference model parameters.");
      else
        parseModelsPars(std::begin(rModelsPars), std::end(rModelsPars), refMs);
    } else if ((*i == "-tm" || *i == "--tar-model") &&
               i + 1 != std::end(vArgs)) {
      man_tm = true;
      tModelsPars = *++i;
      if (tModelsPars.front() == '-' || tModelsPars.empty())
        error("incorrect target model parameters.");
      else
        parseModelsPars(std::begin(tModelsPars), std::end(tModelsPars), tarMs);
    } else if ((*i == "-w" || *i == "--wsize") && i + 1 != std::end(vArgs)) {
      manWSize = true;
      wsize = static_cast<uint32_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_WS, MAX_WS, WS, "Window size", "[]", "default", Problem::warning);
      range->assert(wsize);
    } else if ((*i == "-th" || *i == "--thresh") && i + 1 != std::end(vArgs)) {
      manThresh = true;
      thresh = std::stof(*++i);
      auto range = std::make_unique<ValRange<float>>(
          MIN_THRSH, MAX_THRSH, THRSH, "Threshold", "(]", "default",
          Problem::warning);
      range->assert(thresh);
    } else if ((*i == "-wt" || *i == "--wtype") && i + 1 != std::end(vArgs)) {
      const auto is_win_type = [](std::string t) {
        return (t == "0" || t == "rectangular" || t == "1" || t == "hamming" ||
                t == "2" || t == "hann" || t == "3" || t == "blackman" ||
                t == "4" || t == "triangular" || t == "5" || t == "welch" ||
                t == "6" || t == "sine" || t == "7" || t == "nuttall");
      };
      const std::string cmd{*++i};
      auto set = std::make_unique<ValSet<WType>>(
          SET_WTYPE, WT, "Window type", "default", Problem::warning,
          win_type(cmd), is_win_type(cmd));
      set->assert(wtype);
    } else if ((*i == "-e" || *i == "--ent-n") && i + 1 != std::end(vArgs)) {
      entropyN = static_cast<prc_t>(std::stod(*++i));
      auto range = std::make_unique<ValRange<prc_t>>(
          MIN_ENTR_N, MAX_ENTR_N, ENTR_N, "Entropy of N bases", "[]", "default",
          Problem::warning);
      range->assert(entropyN);
    } else if ((*i == "-n" || *i == "--nthr") && i + 1 != std::end(vArgs)) {
      nthr = static_cast<uint8_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_THRD, MAX_THRD, THRD, "Number of threads", "[]", "default",
          Problem::warning);
      range->assert(nthr);
    } else if ((*i == "-d" || *i == "--step") && i + 1 != std::end(vArgs)) {
      sampleStep = std::stoull(*++i);
      if (sampleStep == 0) sampleStep = 1ull;
    } else if ((*i == "-fs" || *i == "--filter-scale") &&
               i + 1 != std::end(vArgs)) {
      manFilterScale = true;
      const auto is_filter_scale = [](std::string s) {
        return (s == "S" || s == "small" || s == "M" || s == "medium" ||
                s == "L" || s == "large");
      };
      const std::string cmd{*++i};
      auto set = std::make_unique<ValSet<FilterScale>>(
          SET_FSCALE, FS, "Filter scale", "default", Problem::warning,
          filter_scale(cmd), is_filter_scale(cmd));
      set->assert(filterScale);
    } else if (*i == "-nr" || *i == "--no-redun")
      noRedun = true;
    else if (*i == "-sb" || *i == "--save-seq")
      saveSeq = true;
    else if (*i == "-sp" || *i == "--save-profile")
      saveProfile = true;
    else if (*i == "-sf" || *i == "--save-fitler")
      saveFilter = true;
    else if (*i == "-ss" || *i == "--save-segment")
      saveSegment = true;
    else if (*i == "-sa" || *i == "--save-all")
      saveAll = true;
    else if (*i == "-compress")
      compress = true;
    else if (*i == "-filter")
      filter = true;
    else if (*i == "-segment")
      segment = true;
    else if (*i == "-R" || *i == "--report")
      report = (i + 1 != std::end(vArgs)) ? *++i : "report.txt";
    else if (*i == "-h" || *i == "--help") {
      help();
      throw EXIT_SUCCESS;
    } else if (*i == "-v" || *i == "--verbose")
      verbose = true;
  }

  // Mandatory args
  const bool has_t{has(std::begin(vArgs), std::end(vArgs), "-t")};
  const bool has_tar{has(std::begin(vArgs), std::end(vArgs), "--tar")};
  const bool has_r{has(std::begin(vArgs), std::end(vArgs), "-r")};
  const bool has_ref{has(std::begin(vArgs), std::end(vArgs), "--ref")};
  if (!has_t && !has_tar)
    error("target file not specified. Use \"-t <fileName>\".");
  else if (!has_r && !has_ref)
    error("reference file not specified. Use \"-r <fileName>\".");

  if (!man_rm && !man_tm) {
    parseModelsPars(std::begin(LEVEL[level]), std::end(LEVEL[level]), refMs);
    parseModelsPars(std::begin(REFFREE_LEVEL[level]),
                    std::end(REFFREE_LEVEL[level]), tarMs);
  } else if (!man_rm && man_tm) {
    refMs = tarMs;
  } else if (man_rm && !man_tm) {
    tarMs = refMs;
  }

  manFilterScale = !manThresh;
  manFilterScale = !manWSize;

  keep_in_range(1ull, wsize,
                std::min(file_size(ref), file_size(tar)) / sampleStep);

  // Fasta/Fastq to Seq
  auto convert_to_seq = [&](std::string f, const FileType& type) {
    rename(f.c_str(), (f + LBL_BAK).c_str());
    to_seq(f + LBL_BAK, f, type);
  };
  refType = file_type(ref);
  if (refType == FileType::fasta)
    convert_to_seq(ref, FileType::fasta);
  else if (refType == FileType::fastq)
    convert_to_seq(ref, FileType::fastq);
  else if (refType != FileType::seq)
    error("\"" + refName + "\" has unknown format.");

  tarType = file_type(tar);
  if (tarType == FileType::fasta)
    convert_to_seq(tar, FileType::fasta);
  else if (tarType == FileType::fastq)
    convert_to_seq(tar, FileType::fastq);
  else if (tarType != FileType::seq)
    error("\"" + tarName + "\" has unknown format.");
}

template <typename Iter>
inline void Param::parseModelsPars(Iter begin, Iter end,
                                   std::vector<MMPar>& Ms) {
  std::vector<std::string> mdls;
  split(begin, end, ':', mdls);
  for (const auto& e : mdls) {
    // Markov and tolerant models
    std::vector<std::string> m_tm;
    split(std::begin(e), std::end(e), '/', m_tm);
    std::vector<std::string> m;
    split(std::begin(m_tm[0]), std::end(m_tm[0]), ',', m);

    if (m.size() == 4) {
      if (std::stoi(m[0]) > K_MAX_LGTBL8)
        Ms.emplace_back(MMPar(static_cast<uint8_t>(std::stoi(m[0])), W, D,
                              static_cast<uint8_t>(std::stoi(m[1])),
                              std::stof(m[2]), std::stof(m[3])));
      else
        Ms.emplace_back(MMPar(static_cast<uint8_t>(std::stoi(m[0])),
                              static_cast<uint8_t>(std::stoi(m[1])),
                              std::stof(m[2]), std::stof(m[3])));
    } else if (m.size() == 6) {
      Ms.emplace_back(MMPar(static_cast<uint8_t>(std::stoi(m[0])),
                            pow2(std::stoull(m[1])),
                            static_cast<uint8_t>(std::stoi(m[2])),
                            static_cast<uint8_t>(std::stoi(m[3])),
                            std::stof(m[4]), std::stof(m[5])));
    }

    // Tolerant models
    if (m_tm.size() == 2) {
      std::vector<std::string> tm;
      split(std::begin(m_tm[1]), std::end(m_tm[1]), ',', tm);
      Ms.back().child = std::make_shared<STMMPar>(
          STMMPar(static_cast<uint8_t>(std::stoi(m[0])),
                  static_cast<uint8_t>(std::stoi(tm[0])),
                  static_cast<uint8_t>(std::stoi(tm[1])), std::stof(tm[2]),
                  std::stof(tm[3])));
    }
  }
}

void Param::print_title(std::string&& str) const {
  std::cerr << bold(std::move(str)) << '\n';
}

inline void Param::help() const {
  const auto t = [](std::string&& str) {  // Print title
    std::cerr << bold(std::move(str)) << '\n';
  };
  const auto l = [](const std::string& str) {  // Line
    std::cerr << "  " << str << '\n';
  };
  // Print column 1: left-aligned + column 2: left-aligned
  const auto ll = [](const std::string& strL, uint8_t n,
                      const std::string& strR) {
    std::cerr << "  " << std::left << std::setw(27 + n * 8) << strL;
    std::cerr.clear();
    std::cerr << strR << '\n';
  };
  // Print column 1: right-aligned + column 2: left-aligned
  const auto rl = [](const std::string& strL, uint8_t n,
                      const std::string& strR) {
    std::cerr << "  " << std::right << std::setw(27 + n * 8) << strL;
    std::cerr.clear();
    std::cerr << strR << '\n';
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

  ll(bold("-r") + ",  " + bold("--ref") + "  " + underline("FILE"), 3,
     "reference file (Seq/Fasta/Fastq)");
  
  ll(bold("-t") + ",  " + bold("--tar") + "  " + underline("FILE"), 3,
     "target file    (Seq/Fasta/Fastq)");
  l("");
  
  l(italic("Options"));
  
  ll(bold("-v") + ",  " + bold("--verbose"), 2, "more information");
  
  ll(bold("-l") + ",  " + bold("--level") + "  " + underline("INT"), 3,
     "level of compression: [" + std::to_string(MIN_LVL) + ", " +
         std::to_string(MAX_LVL) + "]");
  
  ll(bold("-m") + ",  " + bold("--min") + "    " + underline("INT"), 3,
     "min segment size: [" + std::to_string(MIN_SSIZE) + ", " +
         std::to_string(MAX_SSIZE) + "]");
  
  ll(bold("-nr") + ", " + bold("--no-redun"), 2,
     "do NOT compute self complexity");
  
  ll(bold("-e") + ",  " + bold("--ent-n") + "  " + underline("FLOAT"), 3,
     "Entropy of 'N's: [" + string_format("%.1f", MIN_ENTR_N) + ", " +
         string_format("%.1f", MAX_ENTR_N) + "]");
  
  ll(bold("-n") + ",  " + bold("--nthr") + "   " + underline("INT"), 3,
     "number of threads: [" + std::to_string(MIN_THRD) + ", " +
         std::to_string(MAX_THRD) + "]");

  ll(bold("-fs") + ", " + bold("--filter-scale") + " " + underline("S") + "|" +
         underline("M") + "|" + underline("L"),
     5, "scale of the filter:");
  ll("", 0, "{S|small, M|medium, L|large}");
  
  ll(bold("-w") + ",  " + bold("--wsize") + "  " + underline("INT"), 3,
     "window size: [" + std::to_string(MIN_WS) + ", " + std::to_string(MAX_WS) +
         "]");
  
  ll(bold("-wt") + ", " + bold("--wtype") + "  " + underline("INT") + "/" +
         underline("STRING"),
     4, "type of windowing function:");
  ll("", 0, "{0|rectangular, 1|hamming, 2|hann,");
  ll("", 0, "3|blackman, 4|triangular, 5|welch,");
  ll("", 0, "6|sine, 7|nuttall}");
  
  ll(bold("-d") + ",  " + bold("--step") + "    " + underline("INT"), 3,
     "sampling steps");
  
  ll(bold("-th") + ", " + bold("--thresh") + "  " + underline("FLOAT"), 3,
     "threshold: [" + string_format("%.1f", MIN_THRSH) + ", " +
         string_format("%.1f", MAX_THRSH) + "]");
  
  ll(bold("-rb") + ", " + bold("--ref-beg-grd") + "  " + underline("INT"), 3,
     "reference beginning guard");
  
  ll(bold("-re") + ", " + bold("--ref-end-grd") + "  " + underline("INT"), 3,
     "reference ending guard");
  
  ll(bold("-tb") + ", " + bold("--tar-beg-grd") + "  " + underline("INT"), 3,
     "target beginning guard");
  
  ll(bold("-te") + ", " + bold("--tar-end-grd") + "  " + underline("INT"), 3,
     "target ending guard");
  
  ll(bold("-sb") + ", " + bold("--save-seq"), 2,
     "save sequence (input: FASTA/FASTQ)");
  
  ll(bold("-sp") + ", " + bold("--save-profile"), 2, "save profile (*.prf)");
  
  ll(bold("-sf") + ", " + bold("--save-filter"), 2,
     "save filtered file (*.fil)");
  
  ll(bold("-sb") + ", " + bold("--save-seq"), 2,
     "save sequence (input: FASTA/FASTQ)");
  
  ll(bold("-ss") + ", " + bold("--save-segment"), 2,
     "save segmented files (*.s" + italic("i") + ")");
  
  ll(bold("-sa") + ", " + bold("--save-all"), 2, "save profile, filetered and");
  ll("", 0, "segmented files");
  
  ll(bold("-h") + ",  " + bold("--help"), 2, "usage guide");
  
  l(bold("-rm") + ", " + bold("--ref-model") + "  " + italic("k") + ",[" +
    italic("w") + "," + italic("d") + ",]ir," + italic("a") + "," +
    italic("g") + "/" + italic("t") + ",ir," + italic("a") + "," + italic("g") +
    ":...");
  
  l(bold("-tm") + ", " + bold("--tar-model") + "  " + italic("k") + ",[" +
    italic("w") + "," + italic("d") + ",]ir," + italic("a") + "," +
    italic("g") + "/" + italic("t") + ",ir," + italic("a") + "," + italic("g") +
    ":...");
  
  ll("", 0, "parameters of models");
  
  rl("(" + underline("INT") + ") ", 1, italic("k") + ":  context size");
  
  rl("(" + underline("INT") + ") ", 1,
     italic("w") + ":  width of sketch in log2 form,");
  ll("", 0, "    e.g., set 10 for w=2^10=1024");
  
  rl("(" + underline("INT") + ") ", 1, italic("d") + ":  depth of sketch");
  
  rl("(" + underline("INT") + ") ", 1, "ir: inverted repeat: {0, 1, 2}");
  ll("", 0, "    0: regular (not inverted)");
  ll("", 0, "    1: inverted, solely");
  ll("", 0, "    2: both regular and inverted");

  rl("(" + underline("FLOAT") + ") ", 1, italic("a") + ":  estimator");
  
  rl("(" + underline("FLOAT") + ") ", 1,
     italic("g") + ":  forgetting factor: [0.0, 1.0)");
  
  rl("(" + underline("INT") + ") ", 1,
     italic("t") + ":  threshold (no. substitutions)");
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

inline WType Param::win_type(std::string t) const {
  if (t == "0" || t == "rectangular")
    return WType::rectangular;
  else if (t == "1" || t == "hamming")
    return WType::hamming;
  else if (t == "2" || t == "hann")
    return WType::hann;
  else if (t == "3" || t == "blackman")
    return WType::blackman;
  else if (t == "4" || t == "triangular")
    return WType::triangular;
  else if (t == "5" || t == "welch")
    return WType::welch;
  else if (t == "6" || t == "sine")
    return WType::sine;
  else if (t == "7" || t == "nuttall")
    return WType::nuttall;
  else
    return WType::hann;
}

inline std::string Param::print_win_type() const {
  switch (wtype) {
    case WType::rectangular:
      return "Rectangular";
      break;
    case WType::hamming:
      return "Hamming";
      break;
    case WType::hann:
      return "Hann";
      break;
    case WType::blackman:
      return "Blackman";
      break;
    case WType::triangular:
      return "Triangular";
      break;
    case WType::welch:
      return "Welch";
      break;
    case WType::sine:
      return "Sine";
      break;
    case WType::nuttall:
      return "Nuttall";
      break;
    default:
      return "Rectangular";
  }
}

inline FilterScale Param::filter_scale(std::string s) const {
  if (s == "S" || s == "small")
    return FilterScale::s;
  else if (s == "M" || s == "medium")
    return FilterScale::m;
  else if (s == "L" || s == "large")
    return FilterScale::l;
  else
    return FilterScale::l;
}

inline std::string Param::print_filter_scale() const {
  switch (filterScale) {
    case FilterScale::s:
      return "Small";
      break;
    case FilterScale::m:
      return "Medium";
      break;
    case FilterScale::l:
      return "Large";
      break;
    default:
      return "Large";
  }
}

inline void VizParam::parse(int argc, char**& argv) {
  if (argc < 3) {
    help();
    throw EXIT_SUCCESS;
  }

  std::vector<std::string> vArgs;
  vArgs.reserve(static_cast<uint64_t>(argc));
  for (int i = 0; i != argc; ++i)
    vArgs.emplace_back(static_cast<std::string>(argv[i]));

  for (auto i = std::begin(vArgs); i != std::end(vArgs); ++i) {
    if ((*i == "-o" || *i == "--out") && i + 1 != std::end(vArgs))
      image = *++i;
    else if ((*i == "-rn" || *i == "--ref-name") && i + 1 != std::end(vArgs)) {
      refName = *++i;
    } else if ((*i == "-tn" || *i == "--tar-name") &&
               i + 1 != std::end(vArgs)) {
      tarName = *++i;
    } else if ((*i == "-p" || *i == "--opacity") && i + 1 != std::end(vArgs)) {
      opacity = std::stof(*++i);
      auto range =
          std::make_unique<ValRange<float>>(MIN_OPAC, MAX_OPAC, OPAC, "Opacity",
                                            "[]", "default", Problem::warning);
      range->assert(opacity);
    } else if ((*i == "-l" || *i == "--link") && i + 1 != std::end(vArgs)) {
      link = static_cast<uint8_t>(std::stoul(*++i));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_LINK, MAX_LINK, LINK, "Link", "[]", "default", Problem::warning);
      range->assert(link);
    } else if ((*i == "-m" || *i == "--min") && i + 1 != std::end(vArgs)) {
      min = static_cast<uint32_t>(std::stoul(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_MINP, MAX_MINP, MINP, "Min", "[]", "default", Problem::warning);
      range->assert(min);
    } else if ((*i == "-f" || *i == "--mult") && i + 1 != std::end(vArgs)) {
      manMult = true;
      mult = static_cast<uint32_t>(std::stoul(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_MULT, MAX_MULT, MULT, "Mult", "[]", "default", Problem::warning);
      range->assert(mult);
    } else if ((*i == "-b" || *i == "--begin") && i + 1 != std::end(vArgs)) {
      start = static_cast<uint32_t>(std::stoul(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_BEGN, MAX_BEGN, BEGN, "Begin", "[]", "default", Problem::warning);
      range->assert(start);
    } else if ((*i == "-rt" || *i == "--ref-tick") &&
               i + 1 != std::end(vArgs)) {
      refTick = std::stoull(*++i);
      auto range = std::make_unique<ValRange<uint64_t>>(
          MIN_TICK, MAX_TICK, TICK, "Tick hop for reference", "[]", "default",
          Problem::warning);
      range->assert(refTick);
    } else if ((*i == "-tt" || *i == "--tar-tick") &&
               i + 1 != std::end(vArgs)) {
      tarTick = std::stoull(*++i);
      auto range = std::make_unique<ValRange<uint64_t>>(
          MIN_TICK, MAX_TICK, TICK, "Tick hop for target", "[]", "default",
          Problem::warning);
      range->assert(tarTick);
    } else if ((*i == "-th" || *i == "--tick-human") &&
               i + 1 != std::end(vArgs)) {
      tickHumanRead = (std::stoi(*++i) != 0);
    } else if ((*i == "-c" || *i == "--color") && i + 1 != std::end(vArgs)) {
      colorMode = static_cast<uint8_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_COLOR, MAX_COLOR, COLOR, "Color", "[]", "default",
          Problem::warning);
      range->assert(colorMode);
    } else if ((*i == "-w" || *i == "--width") && i + 1 != std::end(vArgs)) {
      width = static_cast<uint32_t>(std::stoul(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_WDTH, MAX_WDTH, WDTH, "Width", "[]", "default", Problem::warning);
      range->assert(width);
    } else if ((*i == "-s" || *i == "--space") && i + 1 != std::end(vArgs)) {
      space = static_cast<uint32_t>(std::stoul(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_SPC, MAX_SPC, SPC, "Space", "[]", "default", Problem::warning);
      range->assert(space);
    } else if (*i == "-nn" || *i == "--no-nrc")
      showNRC = false;
    else if (*i == "-vv" || *i == "--vertical")
      vertical = true;
    else if (*i == "-nr" || *i == "--no-redun")
      showRedun = false;
    else if (*i == "-ni" || *i == "--no-inv")
      inverse = false;
    else if (*i == "-ng" || *i == "--no-reg")
      regular = false;
    else if (*i == "-v" || *i == "--verbose")
      verbose = true;
    else if (*i == "-h" || *i == "--help") {
      help();
      throw EXIT_SUCCESS;
    }
  }
  posFile = vArgs.back();
}

inline void VizParam::help() const {
  // Print title
  const auto t = [](std::string&& str) {
    std::cerr << bold(std::move(str)) << '\n';
  };
  const auto l = [](const std::string& str) {
    std::cerr << "  " << str << '\n';
  };  // Line
  // Print column 1: left-aligned + column 2: left-aligned
  const auto ll = [](const std::string& strL, uint8_t n,
                      const std::string& strR) {
    std::cerr << "  " << std::left << std::setw(25 + n * 8) << strL;
    std::cerr.clear();
    std::cerr << strR << '\n';
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
  
  ll(bold("-rn") + ", " + bold("--ref-name") + " " + underline("STRING"), 3,
     "reference name shown on output. If name");
  ll("", 0, "has space, use \"s, e.g. \"Seq label\".");
  ll("", 0, "Default: name in header of position file.");
  
  ll(bold("-tn") + ", " + bold("--tar-name") + " " + underline("STRING"), 3,
     "target name shown on output");
  
  ll(bold("-vv") + ", " + bold("--vertical"), 2, "vertical view");
  
  ll(bold("-nn") + ", " + bold("--no-nrc"), 2, "do NOT show normalized");
  ll("", 0, "relative compression (NRC)");
  
  ll(bold("-nr") + ", " + bold("--no-redun"), 2, "do NOT show self complexity");
  
  ll(bold("-ni") + ", " + bold("--no-inv"), 2, "do NOT show inverse maps");
  
  ll(bold("-ng") + ", " + bold("--no-reg"), 2, "do NOT show regular maps");
  
  ll(bold("-l") + ",  " + bold("--link") + "     " + underline("INT"), 3,
     "type of the link between maps: [" + std::to_string(MIN_LINK) + ", " +
         std::to_string(MAX_LINK) + "]");
  
  ll(bold("-c") + ",  " + bold("--color") + "    " + underline("INT"), 3,
     "color mode: [" + std::to_string(MIN_COLOR) + ", " +
         std::to_string(MAX_COLOR) + "]");
  
  ll(bold("-p") + ",  " + bold("--opacity") + "  " + underline("FLOAT"), 3,
     "opacity: [" + string_format("%.1f", MIN_OPAC) + ", " +
         string_format("%.1f", MAX_OPAC) + "]");
  
  ll(bold("-w") + ",  " + bold("--width") + "    " + underline("INT"), 3,
     "width of the sequence: [" + std::to_string(MIN_WDTH) + ", " +
         std::to_string(MAX_WDTH) + "]");
  
  ll(bold("-s") + ",  " + bold("--space") + "    " + underline("INT"), 3,
     "space between sequences: [" + std::to_string(MIN_SPC) + ", " +
         std::to_string(MAX_SPC) + "]");
  
  ll(bold("-f") + ",  " + bold("--mult") + "     " + underline("INT"), 3,
     "multiplication factor for");
  ll("", 0,
     "color ID: [" + std::to_string(MIN_MULT) + ", " +
         std::to_string(MAX_MULT) + "]");
  
  ll(bold("-b") + ",  " + bold("--begin") + "    " + underline("INT"), 3,
     "beginning of color ID: [" + std::to_string(MIN_BEGN) + ", " +
         std::to_string(MAX_BEGN) + "]");
  
  ll(bold("-rt") + ", " + bold("--ref-tick") + " " + underline("INT"), 3,
     "reference tick: [" + std::to_string(MIN_TICK) + ", " +
         std::to_string(MAX_TICK) + "]");
  
  ll(bold("-tt") + ", " + bold("--tar-tick") + " " + underline("INT"), 3,
     "target tick: [" + std::to_string(MIN_TICK) + ", " +
         std::to_string(MAX_TICK) + "]");
  
  ll(bold("-th") + ", " + bold("--tick-human") + " " + underline("0|1"), 3,
     "tick human readable: 0=false, 1=true");
  
  ll(bold("-m") + ",  " + bold("--min") + "      " + underline("INT"), 3,
     "minimum block size: [" + std::to_string(MIN_MINP) + ", " +
         std::to_string(MAX_MINP) + "]");
  
  ll(bold("-h") + ",  " + bold("--help"), 2, "usage guide");
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