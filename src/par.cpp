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
#include "print.hpp"
using namespace smashpp;

Param::Param(std::shared_ptr<Param> par) {
  ref = par->ref;
  tar = par->tar;
  seq = par->seq;
  refName = par->refName;
  tarName = par->tarName;
  verbose = par->verbose;
  level = par->level;
  segSize = par->segSize;
  entropyN = par->entropyN;
  nthr = par->nthr;
  wsize = par->wsize;
  wtype = par->wtype;
  sampleStep = par->sampleStep;
  thresh = par->thresh;
  manSegSize = par->manSegSize;
  manWSize = par->manWSize;
  manThresh = par->manThresh;
  manFilterScale = par->manFilterScale;
  filterScale = par->filterScale;
  saveSeq = par->saveSeq;
  saveProfile = par->saveProfile;
  saveFilter = par->saveFilter;
  saveSegment = par->saveSegment;
  saveAll = par->saveAll;
  refType = par->refType;
  tarType = par->tarType;
  showInfo = par->showInfo;
  compress = par->compress;
  filter = par->filter;
  segment = par->segment;
  ID = par->ID;
  noRedun = par->noRedun;
  refMs = par->refMs;
  tarMs = par->tarMs;
  ref_beg_guard = par->ref_beg_guard;
  ref_end_guard = par->ref_end_guard;
  tar_beg_guard = par->tar_beg_guard;
  tar_end_guard = par->tar_end_guard;
}

void Param::parse(int argc, char**& argv) {
  if (argc < 2) {
    help();
    throw EXIT_SUCCESS;
  }

  std::vector<std::string> vArgs;
  vArgs.reserve(static_cast<uint64_t>(argc));
  for (int i = 0; i != argc; ++i)
    vArgs.push_back(static_cast<std::string>(argv[i]));

  auto trig_inserted = [](auto iter, std::string short_name,
                          std::string long_name) -> bool {
    return (*iter == "-" + short_name || *iter == "--" + long_name);
  };

  auto option_inserted = [&](auto iter, std::string short_name,
                             std::string long_name) -> bool {
    return (iter + 1 != std::end(vArgs)) &&
           trig_inserted(iter, short_name, long_name);
  };

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
    } else if (option_inserted(i, "l", "level")) {
      level = static_cast<uint8_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_LVL, MAX_LVL, level, "Level", "[]", "default",
          Problem::warning);
      range->assert(level);
    } else if (option_inserted(i, "m", "min")) {
      manSegSize = true;
      segSize = std::stoul(*++i);
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_SSIZE, MAX_SSIZE, segSize, "Minimum segment size", "[]", "default",
          Problem::warning);
      range->assert(segSize);
    } else if (option_inserted(i, "rm", "ref-model")) {
      man_rm = true;
      rModelsPars = *++i;
      if (rModelsPars.front() == '-' || rModelsPars.empty())
        error("incorrect reference model parameters.");
      else
        parseModelsPars(std::begin(rModelsPars), std::end(rModelsPars), refMs);
    } else if (option_inserted(i, "tm", "tar-model")) {
      man_tm = true;
      tModelsPars = *++i;
      if (tModelsPars.front() == '-' || tModelsPars.empty())
        error("incorrect target model parameters.");
      else
        parseModelsPars(std::begin(tModelsPars), std::end(tModelsPars), tarMs);
    } else if (option_inserted(i, "w", "wsize")) {
      manWSize = true;
      wsize = static_cast<uint32_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_WS, MAX_WS, wsize, "Window size", "[]", "default", Problem::warning);
      range->assert(wsize);
    } else if (option_inserted(i, "th", "thresh")) {
      manThresh = true;
      thresh = std::stof(*++i);
      auto range = std::make_unique<ValRange<float>>(
          MIN_THRSH, MAX_THRSH, thresh, "Threshold", "(]", "default",
          Problem::warning);
      range->assert(thresh);
    } else if (option_inserted(i, "wt", "wtype")) {
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
    } else if (option_inserted(i, "e", "ent-n")) {
      entropyN = static_cast<prc_t>(std::stod(*++i));
      auto range = std::make_unique<ValRange<prc_t>>(
          MIN_ENTR_N, MAX_ENTR_N, entropyN, "Entropy of N bases", "[]", "default",
          Problem::warning);
      range->assert(entropyN);
    } else if (option_inserted(i, "n", "nthr")) {
      nthr = static_cast<uint8_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_THRD, MAX_THRD, nthr, "Number of threads", "[]", "default",
          Problem::warning);
      range->assert(nthr);
    } else if (option_inserted(i, "d", "step")) {
      sampleStep = std::stoull(*++i);
      if (sampleStep == 0) sampleStep = 1ull;
    } else if (option_inserted(i, "fs", "filter-scale")) {
      manFilterScale = true;
      const auto is_filter_scale = [](std::string s) {
        return (s == "S" || s == "small" || s == "M" || s == "medium" ||
                s == "L" || s == "large");
      };
      const std::string cmd{*++i};
      auto set = std::make_unique<ValSet<FilterScale>>(
          SET_FSCALE, filterScale, "Filter scale", "default", Problem::warning,
          filter_scale(cmd), is_filter_scale(cmd));
      set->assert(filterScale);
    } else if (option_inserted(i, "rb", "ref-beg-grd")) {
      ref_beg_guard = static_cast<int16_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Reference beginning guard",
          "[]", "default", Problem::warning);
      range->assert(ref_beg_guard);
    } else if (option_inserted(i, "re", "ref-end-grd")) {
      ref_end_guard = static_cast<int16_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Reference ending guard",
          "[]", "default", Problem::warning);
      range->assert(ref_end_guard);
    } else if (option_inserted(i, "tb", "tar-beg-grd")) {
      tar_beg_guard = static_cast<int16_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Target beginning guard",
          "[]", "default", Problem::warning);
      range->assert(tar_beg_guard);
    } else if (option_inserted(i, "te", "tar-end-grd")) {
      tar_end_guard = static_cast<int16_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Target ending guard", "[]",
          "default", Problem::warning);
      range->assert(tar_end_guard);
    } else if (trig_inserted(i, "nr", "no-redun")) {
      noRedun = true;
    } else if (trig_inserted(i, "sb", "save-seq")) {
      saveSeq = true;
    } else if (trig_inserted(i, "sp", "save-profile")) {
      saveProfile = true;
    } else if (trig_inserted(i, "sf", "save-fitler")) {
      saveFilter = true;
    } else if (trig_inserted(i, "ss", "save-segment")) {
      saveSegment = true;
    } else if (trig_inserted(i, "sa", "save-all")) {
      saveAll = true;
    } else if (*i == "-compress") {
      compress = true;
    } else if (*i == "-filter") {
      filter = true;
    } else if (*i == "-segment") {
      segment = true;
    } else if (trig_inserted(i, "h", "help")) {
      help();
      throw EXIT_SUCCESS;
    } else if (trig_inserted(i, "v", "verbose")) {
      verbose = true;
    }
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
void Param::parseModelsPars(Iter begin, Iter end, std::vector<MMPar>& Ms) {
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
        Ms.push_back(MMPar(static_cast<uint8_t>(std::stoi(m[0])), W, D,
                           static_cast<uint8_t>(std::stoi(m[1])),
                           std::stof(m[2]), std::stof(m[3])));
      else
        Ms.push_back(MMPar(static_cast<uint8_t>(std::stoi(m[0])),
                           static_cast<uint8_t>(std::stoi(m[1])),
                           std::stof(m[2]), std::stof(m[3])));
    } else if (m.size() == 6) {
      Ms.push_back(MMPar(static_cast<uint8_t>(std::stoi(m[0])),
                         pow2(std::stoull(m[1])),
                         static_cast<uint8_t>(std::stoi(m[2])),
                         static_cast<uint8_t>(std::stoi(m[3])), std::stof(m[4]),
                         std::stof(m[5])));
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

void Param::help() const {
  print_title("Usage");
  print_line("./smashpp [OPTIONS]  -r <REF-FILE>  -t <TAR-FILE>");
  std::cerr << '\n';

  print_title("Mandatory arguments");

  print_aligned("-r", "<FILE>", "=", "reference file (Seq/FASTA/FASTQ)");

  print_aligned("-t", "<FILE>", "=", "target file    (Seq/FASTA/FASTQ)");
  std::cerr << '\n';

  print_title("Options");

  print_aligned("-l", "<INT>", "=",
                "level of compression: [" + std::to_string(MIN_LVL) + ", " +
                    std::to_string(MAX_LVL) + "]. Default",
                "->", std::to_string(level));

  print_aligned("-m", "<INT>", "=",
                "min segment size: [" + std::to_string(MIN_SSIZE) + ", " +
                    std::to_string(MAX_SSIZE) + "]",
                "->", std::to_string(segSize));

  print_aligned("-e", "<FLOAT>", "=",
                "entropy of 'N's: [" + string_format("%.1f", MIN_ENTR_N) +
                    ", " + string_format("%.1f", MAX_ENTR_N) + "]",
                "->", string_format("%.1f", entropyN));

  print_aligned("-n", "<INT>", "=",
                "number of threads: [" + std::to_string(MIN_THRD) + ", " +
                    std::to_string(MAX_THRD) + "]",
                "->", std::to_string(nthr));

  print_aligned("-fs", "[S][M][L]", "=", "scale of the filter:", "->", "L");
  print_aligned("", "", "", "{S|small, M|medium, L|large}");

  print_aligned("-w", "<INT>", "=",
                "window size: [" + std::to_string(MIN_WS) + ", " +
                    std::to_string(MAX_WS) + "]",
                "->", std::to_string(wsize));

  print_aligned("-wt", "<INT/STRING>", "=", "type of windowing function:", "->",
                "hann");
  print_aligned("", "", "", "{0|rectangular, 1|hamming, 2|hann,");
  print_aligned("", "", "", "3|blackman, 4|triangular, 5|welch,");
  print_aligned("", "", "", "6|sine, 7|nuttall}");

  print_aligned("-d", "<INT>", "=", "sampling steps", "->",
                std::to_string(sampleStep));

  print_aligned("-th", "<FLOAT>", "=",
                "threshold: [" + string_format("%.1f", MIN_THRSH) + ", " +
                    string_format("%.1f", MAX_THRSH) + "]",
                "->", string_format("%.1f", thresh));

  print_aligned("-rb", "<INT>", "=", "reference beginning guard", "->",
                std::to_string(ref_beg_guard));

  print_aligned("-re", "<INT>", "=", "reference ending guard", "->",
                std::to_string(ref_end_guard));

  print_aligned("-tb", "<INT>", "=", "target beginning guard", "->",
                std::to_string(tar_beg_guard));

  print_aligned("-te", "<INT>", "=", "target ending guard", "->",
                std::to_string(tar_end_guard));

  print_line(bold("-rm") + " <" + italic("k") + ",[" + italic("w") + "," +
             italic("d") + ",]ir," + italic("a") + "," + italic("g") + "/" +
             italic("t") + ",ir," + italic("a") + "," + italic("g") + ":...>");

  print_line(bold("-tm") + " <" + italic("k") + ",[" + italic("w") + "," +
             italic("d") + ",]ir," + italic("a") + "," + italic("g") + "/" +
             italic("t") + ",ir," + italic("a") + "," + italic("g") + ":...>");

  print_aligned("", "", "=", "parameters of models");

  print_aligned_right_left("", "<INT>", "", italic("k") + ":  context size");

  print_aligned_right_left("", "<INT>", "",
                           italic("w") + ":  width of sketch in log2 form,");
  print_aligned_right_left("", "", "", "    e.g., set 10 for w=2^10=1024");

  print_aligned_right_left("", "<INT>", "", italic("d") + ":  depth of sketch");

  print_aligned_right_left("", "<INT>", "", "ir: inverted repeat: {0, 1, 2}");
  print_aligned_right_left("", "", "", "    0: regular (not inverted)");
  print_aligned_right_left("", "", "", "    1: inverted, solely");
  print_aligned_right_left("", "", "", "    2: both regular and inverted");

  print_aligned_right_left("", "<FLOAT>", "", italic("a") + ":  estimator");

  print_aligned_right_left("", "<FLOAT>", "",
                           italic("g") + ":  forgetting factor: [0.0, 1.0)");

  print_aligned_right_left("", "<INT>", "",
                           italic("t") + ":  threshold (no. substitutions)");
  std::cerr << '\n';

  print_title("Flags");

  print_aligned("-h", "", "=", "usage guide", "->", "no");

  print_aligned("-v", "", "=", "more information", "->", "no");

  print_aligned("-nr", "", "=", "do NOT compute self complexity", "->", "no");

  print_aligned("-sb", "", "=", "save sequence (input: FASTA/FASTQ)", "->", "no");

  print_aligned("-sp", "", "=", "save profile (*.prf)", "->", "no");

  print_aligned("-sf", "", "=", "save filtered file (*.fil)", "->", "no");

  print_aligned("-ss", "", "=", "save segmented files (*.s[i])", "->", "no");

  print_aligned("-sa", "", "=", "save profile, filetered and", "->", "no");
  print_aligned("", "", "", "segmented files");

  print_title("AUTHOR");
  print_line("Morteza Hosseini   seyedmorteza@ua.pt");
std::cerr<<'\n';

  print_title("SAMPLE");
  // line("./smashpp -t TAR -r REF");
std::cerr<<'\n';

  // // title("COPYRIGHT");
  // // line("Copyright (C) " + DEV_YEARS +
  // //      ", IEETA, University of Aveiro. You may  ");
  // // line("redistribute copies of this Free software under the terms of the");
  // // line("GPL v3 (General Public License) <www.gnu.org/licenses/gpl.html>.");
  // // line("There is NO WARRANTY, to the extent permitted by law.");
}

WType Param::win_type(std::string t) const {
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

std::string Param::print_win_type() const {
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

FilterScale Param::filter_scale(std::string s) const {
  if (s == "S" || s == "small")
    return FilterScale::s;
  else if (s == "M" || s == "medium")
    return FilterScale::m;
  else if (s == "L" || s == "large")
    return FilterScale::l;
  else
    return FilterScale::l;
}

std::string Param::print_filter_scale() const {
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

void VizParam::parse(int argc, char**& argv) {
  if (argc < 3) {
    help();
    throw EXIT_SUCCESS;
  }

  std::vector<std::string> vArgs;
  vArgs.reserve(static_cast<uint64_t>(argc));
  for (int i = 0; i != argc; ++i)
    vArgs.push_back(static_cast<std::string>(argv[i]));

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

void VizParam::help() const {
  // title("NAME");
  // line("Smash++ Visualizer v" + VERSION + " - Visualization of Samsh++ output");
  // line("");

  // title("SYNOPSIS");
  // line("./smashpp -viz  " + underline("OPTIONS") + "...  -o " +
  //      underline("SVG-FILE") + "  " + underline("POS-FILE"));
  // line("");

  // title("SAMPLE");
  // // line("./smashpp -viz -o out.svg ab.pos");
  // line("");

  // title("DESCRIPTION");
  // line(italic("Mandatory arguments") + ":");

  // line_left_left(underline("POS-FILE"), 1, "positions file, generated by");
  // line_left_left("", 0, "Smash++ tool (*.pos)");
  // line("");

  // line(italic("Options") + ":");

  // line_left_left(bold("-v") + ",  " + bold("--verbose"), 2, "more information");

  // line_left_left(
  //     bold("-o") + ",  " + bold("--out") + " " + underline("SVG-FILE"), 3,
  //     "output image name (*.svg)");

  // line_left_left(
  //     bold("-rn") + ", " + bold("--ref-name") + " " + underline("STRING"), 3,
  //     "reference name shown on output. If name");
  // line_left_left("", 0, "has space, use \"s, e.g. \"Seq label\".");
  // line_left_left("", 0, "Default: name in header of position file.");

  // line_left_left(
  //     bold("-tn") + ", " + bold("--tar-name") + " " + underline("STRING"), 3,
  //     "target name shown on output");

  // line_left_left(bold("-vv") + ", " + bold("--vertical"), 2, "vertical view");

  // line_left_left(bold("-nn") + ", " + bold("--no-nrc"), 2,
  //                "do NOT show normalized");
  // line_left_left("", 0, "relative compression (NRC)");

  // line_left_left(bold("-nr") + ", " + bold("--no-redun"), 2,
  //                "do NOT show self complexity");

  // line_left_left(bold("-ni") + ", " + bold("--no-inv"), 2,
  //                "do NOT show inverse maps");

  // line_left_left(bold("-ng") + ", " + bold("--no-reg"), 2,
  //                "do NOT show regular maps");

  // line_left_left(
  //     bold("-l") + ",  " + bold("--link") + "     " + underline("INT"), 3,
  //     "type of the link between maps: [" + std::to_string(MIN_LINK) + ", " +
  //         std::to_string(MAX_LINK) + "]");

  // line_left_left(
  //     bold("-c") + ",  " + bold("--color") + "    " + underline("INT"), 3,
  //     "color mode: [" + std::to_string(MIN_COLOR) + ", " +
  //         std::to_string(MAX_COLOR) + "]");

  // line_left_left(
  //     bold("-p") + ",  " + bold("--opacity") + "  " + underline("FLOAT"), 3,
  //     "opacity: [" + string_format("%.1f", MIN_OPAC) + ", " +
  //         string_format("%.1f", MAX_OPAC) + "]");

  // line_left_left(
  //     bold("-w") + ",  " + bold("--width") + "    " + underline("INT"), 3,
  //     "width of the sequence: [" + std::to_string(MIN_WDTH) + ", " +
  //         std::to_string(MAX_WDTH) + "]");

  // line_left_left(
  //     bold("-s") + ",  " + bold("--space") + "    " + underline("INT"), 3,
  //     "space between sequences: [" + std::to_string(MIN_SPC) + ", " +
  //         std::to_string(MAX_SPC) + "]");

  // line_left_left(
  //     bold("-f") + ",  " + bold("--mult") + "     " + underline("INT"), 3,
  //     "multiplication factor for");
  // line_left_left("", 0,
  //                "color ID: [" + std::to_string(MIN_MULT) + ", " +
  //                    std::to_string(MAX_MULT) + "]");

  // line_left_left(
  //     bold("-b") + ",  " + bold("--begin") + "    " + underline("INT"), 3,
  //     "beginning of color ID: [" + std::to_string(MIN_BEGN) + ", " +
  //         std::to_string(MAX_BEGN) + "]");

  // line_left_left(
  //     bold("-rt") + ", " + bold("--ref-tick") + " " + underline("INT"), 3,
  //     "reference tick: [" + std::to_string(MIN_TICK) + ", " +
  //         std::to_string(MAX_TICK) + "]");

  // line_left_left(
  //     bold("-tt") + ", " + bold("--tar-tick") + " " + underline("INT"), 3,
  //     "target tick: [" + std::to_string(MIN_TICK) + ", " +
  //         std::to_string(MAX_TICK) + "]");

  // line_left_left(
  //     bold("-th") + ", " + bold("--tick-human") + " " + underline("0|1"), 3,
  //     "tick human readable: 0=false, 1=true");

  // line_left_left(
  //     bold("-m") + ",  " + bold("--min") + "      " + underline("INT"), 3,
  //     "minimum block size: [" + std::to_string(MIN_MINP) + ", " +
  //         std::to_string(MAX_MINP) + "]");

  // line_left_left(bold("-h") + ",  " + bold("--help"), 2, "usage guide");
  // line("");

  // title("AUTHOR");
  // line_left_left("Morteza Hosseini", 0, "seyedmorteza@ua.pt");
  // line("");

  // title("COPYRIGHT");
  // line("Copyright (C) " + DEV_YEARS +
  //      ", IEETA, University of Aveiro. You may  ");
  // line("redistribute copies of this Free software under the terms of the");
  // line("GPL v3 (General Public License) <www.gnu.org/licenses/gpl.html>.");
  // line("There is NO WARRANTY, to the extent permitted by law.");
}