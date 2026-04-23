// Smash++
// Morteza Hosseini    mhosayny@gmail.com

#include "par.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <memory>
#include <vector>

#include "check.hpp"
#include "container.hpp"
#include "def.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "mdlpar.hpp"
#include "print.hpp"
using namespace smashpp;

void Param::parse(int argc, char**& argv) {
  if (argc < 2) {
    help();
    throw EXIT_SUCCESS;
  }

  std::vector<std::string> vArgs;
  vArgs.reserve(static_cast<uint64_t>(argc));
  for (int i = 0; i != argc; ++i)
    vArgs.push_back(static_cast<std::string>(argv[i]));

  // Save the list of parameters for writing into position file
  for (auto iter = std::begin(vArgs) + 1; iter != std::end(vArgs); ++iter)
    param_list += *iter + ' ';
  param_list.pop_back();

  auto looks_like_option = [](const std::string& value) {
    return value.size() > 1 && value.front() == '-' &&
           !((value[1] >= '0' && value[1] <= '9') || value[1] == '.');
  };
  auto require_value = [&](auto& iter, const std::string& label,
                           const std::string& usage) -> std::string {
    if (iter + 1 == std::end(vArgs) || looks_like_option(*(iter + 1)))
      error(label + " not specified. Use \"" + usage + "\".");
    return *++iter;
  };

  bool man_rm{false};
  bool man_tm{false};
  std::string rModelsPars;
  std::string tModelsPars;

  for (auto i = std::begin(vArgs) + 1; i != std::end(vArgs); ++i) {
    if (*i == "-h" || *i == "--help") {
      help();
      throw EXIT_SUCCESS;
    } else if (*i == "-V" || *i == "--version") {
      std::cerr << "Smash++ " << VERSION << "\n";
      throw EXIT_SUCCESS;
    } else if (*i == "-v" || *i == "--verbose") {
      verbose = true;
    } else if (*i == "-ll" || *i == "--list-levels") {
      std::cerr << "Level  Model parameters" << '\n';
      for (size_t i = 0; i != LEVEL.size(); ++i)
        std::cerr << "[ " << i << " ]  " << LEVEL[i] << '\n';
      throw EXIT_SUCCESS;
    } else if (*i == "-r" || *i == "--reference") {
      ref = require_value(i, "reference file", "-r <fileName>");
      check_file(ref);
      refName = file_name(ref);
      refType = file_type(ref);
    } else if (*i == "-t" || *i == "--target") {
      tar = require_value(i, "target file", "-t <fileName>");
      check_file(tar);
      tarName = file_name(tar);
      tarType = file_type(tar);
    } else if (*i == "-l" || *i == "--level") {
      man_level = true;
      level = static_cast<uint8_t>(
          std::stoi(require_value(i, "level", "-l <INT>")));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_LVL, MAX_LVL, LVL, "Level", Interval::closed, "default",
          Problem::warning);
      range->check(level);
    } else if (*i == "-m" || *i == "--min-segment-size") {
      segSize = std::stoul(
          require_value(i, "minimum segment size", "-m <INT>"));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_SSIZE, MAX_SSIZE, SSIZE, "Minimum segment size", Interval::closed,
          "default", Problem::warning);
      range->check(segSize);
    } else if (*i == "-rm" || *i == "--reference-model") {
      man_rm = true;
      rModelsPars =
          require_value(i, "reference model parameters", "-rm <STRING>");
      if (rModelsPars.empty() || rModelsPars.front() == '-')
        error("incorrect reference model parameters.");
      else
        parseModelsPars(std::begin(rModelsPars), std::end(rModelsPars), refMs);
    } else if (*i == "-fmt" || *i == "--format") {
      auto format_ = require_value(i, "output format", "-fmt <STRING>");
      std::transform(std::begin(format_), std::end(format_),
                     std::begin(format_), ::tolower);
      if (format_ == "pos") {  // default
      } else if (format_ == "json") {
        format = Format::json;
      } else {
        warning("incorrect format inserted. Used \"pos\" instead.\n");
      }
    } else if (*i == "-tm" || *i == "--target-model") {
      man_tm = true;
      tModelsPars =
          require_value(i, "target model parameters", "-tm <STRING>");
      if (tModelsPars.empty() || tModelsPars.front() == '-')
        error("incorrect target model parameters.");
      else
        parseModelsPars(std::begin(tModelsPars), std::end(tModelsPars), tarMs);
    } else if (*i == "-f" || *i == "--filter-size") {
      manWSize = true;
      filt_size = static_cast<uint32_t>(
          std::stoi(require_value(i, "filter size", "-f <INT>")));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_WS, MAX_WS, WS, "Filter size", Interval::closed, "default",
          Problem::warning);
      range->check(filt_size);
    } else if (*i == "-th" || *i == "--threshold") {
      manThresh = true;
      thresh = std::stof(require_value(i, "threshold", "-th <FLOAT>"));
      auto range = std::make_unique<ValRange<float>>(
          MIN_THRSH, MAX_THRSH, THRSH, "Threshold", Interval::open_closed,
          "default", Problem::warning);
      range->check(thresh);
    } else if (*i == "-ft" || *i == "--filter-type") {
      const auto is_win_type = [](std::string t) {
        return (t == "0" || t == "rectangular" || t == "1" || t == "hamming" ||
                t == "2" || t == "hann" || t == "3" || t == "blackman" ||
                t == "4" || t == "triangular" || t == "5" || t == "welch" ||
                t == "6" || t == "sine" || t == "7" || t == "nuttall");
      };
      const std::string cmd{
          require_value(i, "filter type", "-ft <INT/STRING>")};
      auto set = std::make_unique<ValSet<FilterType>>(
          SET_WTYPE, FT, "Window type", "default", Problem::warning,
          win_type(cmd), is_win_type(cmd));
      set->check(filt_type);
    } else if (*i == "-e" || *i == "--entropy-N") {
      entropyN = static_cast<prc_t>(
          std::stod(require_value(i, "entropy of N bases", "-e <FLOAT>")));
      auto range = std::make_unique<ValRange<prc_t>>(
          MIN_ENTR_N, MAX_ENTR_N, ENTR_N, "Entropy of N bases",
          Interval::closed, "default", Problem::warning);
      range->check(entropyN);
    } else if (*i == "-n" || *i == "--num-threads") {
      nthr = static_cast<uint8_t>(
          std::stoi(require_value(i, "number of threads", "-n <INT>")));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_THRD, MAX_THRD, THRD, "Number of threads", Interval::closed,
          "default", Problem::warning);
      range->check(nthr);
    } else if (*i == "-d" || *i == "--sampling-step") {
      manSampleStep = true;
      sampleStep = std::stoull(
          require_value(i, "sampling step", "-d <INT>"));
      if (sampleStep == 0) sampleStep = 1ull;
    } else if (*i == "-fs" || *i == "--filter-scale") {
      manFilterScale = true;
      const auto is_filter_scale = [](std::string s) {
        return (s == "S" || s == "small" || s == "M" || s == "medium" ||
                s == "L" || s == "large");
      };
      const std::string cmd{
          require_value(i, "filter scale", "-fs <STRING>")};
      auto set = std::make_unique<ValSet<FilterScale>>(
          SET_FSCALE, filterScale, "Filter scale", "default", Problem::warning,
          filter_scale(cmd), is_filter_scale(cmd));
      set->check(filterScale);
    } else if (*i == "-rb" || *i == "--reference-begin-guard") {
      ref_guard->beg = static_cast<int16_t>(
          std::stoi(require_value(i, "reference beginning guard", "-rb <INT>")));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Reference beginning guard",
          Interval::closed, "default", Problem::warning);
      range->check(ref_guard->beg);
    } else if (*i == "-re" || *i == "--reference-end-guard") {
      ref_guard->end = static_cast<int16_t>(
          std::stoi(require_value(i, "reference ending guard", "-re <INT>")));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Reference ending guard",
          Interval::closed, "default", Problem::warning);
      range->check(ref_guard->end);
    } else if (*i == "-tb" || *i == "--target-begin-guard") {
      tar_guard->beg = static_cast<int16_t>(
          std::stoi(require_value(i, "target beginning guard", "-tb <INT>")));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Target beginning guard",
          Interval::closed, "default", Problem::warning);
      range->check(tar_guard->beg);
    } else if (*i == "-te" || *i == "--target-end-guard") {
      tar_guard->end = static_cast<int16_t>(
          std::stoi(require_value(i, "target ending guard", "-te <INT>")));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Target ending guard",
          Interval::closed, "default", Problem::warning);
      range->check(tar_guard->end);
    } else if (*i == "-ar" || *i == "--asymmetric-regions") {
      asym_region = true;
    } else if (*i == "-dp") {  // hidden option :)
      deep = false;
    } else if (*i == "-nr" || *i == "--no-self-complexity") {
      noRedun = true;
    } else if (*i == "-sb" || *i == "--save-sequence") {
      saveSeq = true;
    } else if (*i == "-sp" || *i == "--save-profile") {
      saveProfile = true;
    } else if (*i == "-sf" || *i == "--save-filtered") {
      saveFilter = true;
    } else if (*i == "-ss" || *i == "--save-segmented") {
      saveSegment = true;
    } else if (*i == "-sa" || *i == "--save-profile-filtered-segmented") {
      saveAll = true;
    }
  }

  // Mandatory args
  const bool has_t{has(std::begin(vArgs), std::end(vArgs), "-t")};
  const bool has_target{has(std::begin(vArgs), std::end(vArgs), "--target")};
  const bool has_r{has(std::begin(vArgs), std::end(vArgs), "-r")};
  const bool has_reference{
      has(std::begin(vArgs), std::end(vArgs), "--reference")};
  if (!has_t && !has_target)
    error("target file not specified. Use \"-t <fileName>\".");
  else if (!has_r && !has_reference)
    error("reference file not specified. Use \"-r <fileName>\".");

  if (!man_rm && !man_tm) {
    if (!man_level) {
      set_auto_model_par();
    } else {
      parseModelsPars(std::begin(LEVEL[level]), std::end(LEVEL[level]), refMs);
      parseModelsPars(std::begin(REFFREE_LEVEL[level]),
                      std::end(REFFREE_LEVEL[level]), tarMs);
    }
  } else if (!man_rm && man_tm) {
    refMs = tarMs;
  } else if (man_rm && !man_tm) {
    tarMs = refMs;
  }

  if (!manSampleStep) {
    const auto min_ref_tar = std::min(file_size(ref), file_size(tar));
    sampleStep = static_cast<uint64_t>(std::ceil(min_ref_tar / 5000.0));
  }

  keep_in_range(1ull, filt_size,
                std::min(file_size(ref), file_size(tar)) / sampleStep);
}

void Param::set_auto_model_par() {
  const auto ref_size{file_size(ref)};
  const auto tar_size{file_size(tar)};
  const uint32_t small{300 * 1024};        // 300 K
  const uint32_t medium{1024 * 1024};      // 1 M
  const uint32_t large{10 * 1024 * 1024};  // 10 M
  std::array<std::string, 4> par{"11,0,0.01,0.95", "13,0,0.008,0.95",
                                 "18,0,0.002,0.95", "20,0,0.002,0.95"};
  std::string ref_par, tar_par;

  if (ref_size < small)
    ref_par = par[0];
  else if (ref_size < medium)
    ref_par = par[1];
  else if (ref_size < large)
    ref_par = par[2];
  else
    ref_par = par[3];

  if (tar_size < small)
    tar_par = par[0];
  else if (tar_size < medium)
    tar_par = par[1];
  else if (tar_size < large)
    tar_par = par[2];
  else
    tar_par = par[3];

  parseModelsPars(std::begin(ref_par), std::end(ref_par), refMs);
  parseModelsPars(std::begin(tar_par), std::end(tar_par), tarMs);
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
  const std::string tab1 = "      ";
  const std::string tab2 = tab1 + "      ";
  const std::string tab3 = tab2 + "    ";

  std::cerr
      << bold("SYNOPSIS") << '\n'
      << tab1 << "./smashpp [OPTIONS]  -r <REF_FILE>  -t <TAR_FILE>\n"
      << '\n'
      << bold("SAMPLE") << '\n'
      << tab1 << "./smashpp -r ref -t tar -l 0 -m 1000\n"
      << tab1 << "./smashpp \\ \n"
      << tab1 << "    --reference ref \\ \n"
      << tab1 << "    --target tar \\ \n"
      << tab1 << "    --format json \\ \n"
      << tab1 << "    --verbose \n"
      << '\n'
      << bold("OPTIONS") << '\n'
      << "    " << italic("Required") << ":\n"
      << tab1 << bold("-r") << ", " << bold("--reference") << " <FILE>       "
      << "reference file (Seq/FASTA/FASTQ)\n"
      << tab1 << bold("-t") << ", " << bold("--target") << " <FILE>          "
      << "target file    (Seq/FASTA/FASTQ)\n"
      << '\n'
      << "    " << italic("Optional") << ":\n"
      << tab1 << bold("-l") << ", " << bold("--level") << " <INT>\n"
      << tab2 << "level of compression: " << std::to_string(MIN_LVL) << " to "
      << std::to_string(MAX_LVL) << ". Default: " << std::to_string(LVL) << '\n'
      << '\n'
      << tab1 << bold("-m") << ", " << bold("--min-segment-size") << " <INT>\n"
      << tab2 << "minimum segment size: " << std::to_string(MIN_SSIZE) << " to "
      << std::to_string(MAX_SSIZE) << ". Default: " << std::to_string(SSIZE)
      << '\n'
      << '\n'
      << tab1 << bold("-fmt") << ", " << bold("--format") << " <STRING>\n"
      << tab2
      << "format of the output (position) file: {pos, json}.\n"
      << tab2 << "Default: pos\n"
      << '\n'
      << tab1 << bold("-e") << ", " << bold("--entropy-N") << " <FLOAT>\n"
      << tab2 << "entropy of 'N's: " << string_format("%.1f", MIN_ENTR_N)
      << " to " << string_format("%.1f", MAX_ENTR_N)
      << ". Default: " << string_format("%.1f", ENTR_N) << '\n'
      << '\n'
      << tab1 << bold("-n") << ", " << bold("--num-threads") << " <INT>\n"
      << tab2 << "number of threads: " << std::to_string(MIN_THRD) << " to "
      << std::to_string(MAX_THRD) << ". Default: " << std::to_string(THRD)
      << '\n'
      << '\n'
      << tab1 << bold("-f") << ", " << bold("--filter-size") << " <INT>\n"
      << tab2 << "filter size: " << std::to_string(MIN_WS) << " to "
      << std::to_string(MAX_WS) << ". Default: " << std::to_string(WS) << '\n'
      << '\n'
      << tab1 << bold("-ft") << ", " << bold("--filter-type")
      << " <INT/STRING>\n"
      << tab2 << "filter type (windowing function): {0/rectangular,\n"
      << tab2 << "1/hamming, 2/hann, 3/blackman, 4/triangular, 5/welch,\n"
      << tab2 << "6/sine, 7/nuttall}. Default: hann\n"
      << '\n'
      << tab1 << bold("-fs") << ", " << bold("--filter-scale") << " <STRING>\n"
      << tab2 << "filter scale: {S/small, M/medium, L/large}\n"
      << '\n'
      << tab1 << bold("-d") << ", " << bold("--sampling-step") << " <INT>\n"
      << tab2 << "sampling step. Default: " << std::to_string(SAMPLE_STEP)
      << '\n'
      << '\n'
      << tab1 << bold("-th") << ", " << bold("--threshold") << " <FLOAT>\n"
      << tab2 << "threshold: " << string_format("%.1f", MIN_THRSH) << " to "
      << string_format("%.1f", MAX_THRSH)
      << ". Default: " << string_format("%.1f", THRSH) << '\n'
      << '\n'
      << tab1 << bold("-rb") << ", " << bold("--reference-begin-guard")
      << " <INT>\n"
      << tab2 << "reference begin guard: "
      << std::to_string(std::numeric_limits<decltype(ref_guard->beg)>::min())
      << " to " + std::to_string(
                      std::numeric_limits<decltype(ref_guard->beg)>::max())
      << ". Default: " << std::to_string(ref_guard->beg) << '\n'
      << '\n'
      << tab1 << bold("-re") << ", " << bold("--reference-end-guard")
      << " <INT>\n"
      << tab2 << "reference ending guard: "
      << std::to_string(std::numeric_limits<decltype(ref_guard->end)>::min())
      << " to "
      << std::to_string(std::numeric_limits<decltype(ref_guard->end)>::max())
      << ". Default: " << std::to_string(ref_guard->end) << '\n'
      << '\n'
      << tab1 << bold("-tb") << ", " << bold("--target-begin-guard")
      << " <INT>\n"
      << tab2 << "target begin guard: "
      << std::to_string(std::numeric_limits<decltype(tar_guard->beg)>::min())
      << " to " + std::to_string(
                      std::numeric_limits<decltype(tar_guard->beg)>::max())
      << ". Default: " << std::to_string(tar_guard->beg) << '\n'
      << '\n'
      << tab1 << bold("-te") << ", " << bold("--target-end-guard") << " <INT>\n"
      << tab2 << "target ending guard: "
      << std::to_string(std::numeric_limits<decltype(tar_guard->end)>::min())
      << " to "
      << std::to_string(std::numeric_limits<decltype(tar_guard->end)>::max())
      << ". Default: " << std::to_string(tar_guard->end) << '\n'
      << '\n'
      << tab1 << bold("-ar") << ", " << bold("--asymmetric-regions") << '\n'
      << tab2 << "consider asymmetric regions. Default: not used\n"
      << '\n'
      // << "  " << bold("-dp") '\n'
      // << "        deep compression. Default: no\n"
      // << '\n'
      << tab1 << bold("-nr") << ", " << bold("--no-self-complexity") << '\n'
      << tab2 << "do not compute self complexity. Default: not used\n"
      << '\n'
      << tab1 << bold("-sb") << ", " << bold("--save-sequence") << '\n'
      << tab2 << "save sequence (input: FASTA/FASTQ). Default: not used\n"
      << '\n'
      << tab1 << bold("-sp") << ", " << bold("--save-profile") << '\n'
      << tab2 << "save profile (*.prf). Default: not used\n"
      << '\n'
      << tab1 << bold("-sf") << ", " << bold("--save-filtered") << '\n'
      << tab2 << "save filtered file (*.fil). Default: not used\n"
      << '\n'
      << tab1 << bold("-ss") << ", " << bold("--save-segmented") << '\n'
      << tab2 << "save segmented files (*.s[i]). Default: not used\n"
      << '\n'
      << tab1 << bold("-sa") << ", "
      << bold("--save-profile-filtered-segmented") << '\n'
      << tab2 << "save profile, filetered and segmented files.\n"
      << tab2 << "Default: not used\n"
      << '\n'
      << tab1 << bold("-rm") << ", " << bold("--reference-model") << "  "
      << italic("k") << ",[" << italic("w") << "," << italic("d") << ",]ir,"
      << italic("a") << "," << italic("g") << "/" << italic("t") << ",ir,"
      << italic("a") << "," << italic("g") << ":...\n"
      << tab1 << bold("-tm") << ", " << bold("--target-model") << "     "
      << italic("k") << ",[" << italic("w") << "," << italic("d") << ",]ir,"
      << italic("a") << "," << italic("g") << "/" << italic("t") << ",ir,"
      << italic("a") << "," << italic("g") << ":...\n"
      << tab2 << "parameters of models\n"
      << tab3 << "<INT>  " << italic("k") << ":  context size\n"
      << tab3 << "<INT>  " << italic("w")
      << ":  width of sketch in log2 form,\n"
      << tab3 << "           e.g., set 10 for w=2^10=1024\n"
      << tab3 << "<INT>  " << italic("d") << ":  depth of sketch\n"
      << tab3 << "<INT>  " << italic("ir") << ": inverted repeat: {0, 1, 2}\n"
      << tab3 << "           0: regular (not inverted)\n"
      << tab3 << "           1: inverted, solely\n"
      << tab3 << "           2: both regular and inverted\n"
      << tab2 << "  <FLOAT>  " << italic("a") << ":  estimator\n"
      << tab2 << "  <FLOAT>  " << italic("g")
      << ":  forgetting factor: 0.0 to 1.0\n"
      << tab3 << "<INT>  " << italic("t")
      << ":  threshold (number of substitutions)\n"
      << '\n'
      << tab1 << bold("-ll") << ", " << bold("--list-levels") << '\n'
      << tab2 << "list of compression levels\n"
      << '\n'
      << tab1 << bold("-h") << ", " << bold("--help") << '\n'
      << tab2 << "usage guide\n"
      << '\n'
      << tab1 << bold("-v") << ", " << bold("--verbose") << '\n'
      << tab2 << "more information\n"
      << '\n'
      << tab1 << bold("-V") << ", " << bold("--version") << '\n'
      << tab2 << "show version\n"
      << '\n'
      << bold("AUTHOR") << '\n'
      << tab1 << "Morteza Hosseini      mhosayny@gmail.com\n";
}

FilterType Param::win_type(std::string t) const {
  if (t == "0" || t == "rectangular")
    return FilterType::rectangular;
  else if (t == "1" || t == "hamming")
    return FilterType::hamming;
  else if (t == "2" || t == "hann")
    return FilterType::hann;
  else if (t == "3" || t == "blackman")
    return FilterType::blackman;
  else if (t == "4" || t == "triangular")
    return FilterType::triangular;
  else if (t == "5" || t == "welch")
    return FilterType::welch;
  else if (t == "6" || t == "sine")
    return FilterType::sine;
  else if (t == "7" || t == "nuttall")
    return FilterType::nuttall;
  else
    return FilterType::hann;
}

std::string Param::print_win_type() const {
  switch (filt_type) {
    case FilterType::rectangular:
      return "Rectangular";
      break;
    case FilterType::hamming:
      return "Hamming";
      break;
    case FilterType::hann:
      return "Hann";
      break;
    case FilterType::blackman:
      return "Blackman";
      break;
    case FilterType::triangular:
      return "Triangular";
      break;
    case FilterType::welch:
      return "Welch";
      break;
    case FilterType::sine:
      return "Sine";
      break;
    case FilterType::nuttall:
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
    return FilterScale::m;
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
      return "Medium";
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

  auto looks_like_option = [](const std::string& value) {
    return value.size() > 1 && value.front() == '-' &&
           !((value[1] >= '0' && value[1] <= '9') || value[1] == '.');
  };
  auto require_value = [&](auto& iter, const std::string& label,
                           const std::string& usage) -> std::string {
    if (iter + 1 == std::end(vArgs) || looks_like_option(*(iter + 1)))
      error(label + " not specified. Use \"" + usage + "\".");
    return *++iter;
  };

  for (auto i = std::begin(vArgs) + 1; i != std::end(vArgs); ++i) {
    if (*i == "viz" || *i == "-viz") {
      continue;
    }
    if (*i == "-h" || *i == "--help") {
      help();
      throw EXIT_SUCCESS;
    } else if (*i == "-v" || *i == "--verbose") {
      verbose = true;
    } else if (*i == "-V" || *i == "--version") {
      std::cerr << "Smash++ " << VERSION << "\n";
      throw EXIT_SUCCESS;
    } else if (*i == "-o" || *i == "--output") {
      image = require_value(i, "output image name", "-o <SVG_FILE>");
    } else if (*i == "-rn" || *i == "--reference-name") {
      refName = require_value(i, "reference name", "-rn <STRING>");
    } else if (*i == "-tn" || *i == "--target-name") {
      tarName = require_value(i, "target name", "-tn <STRING>");
    } else if (*i == "-p" || *i == "--opacity") {
      opacity = std::stof(require_value(i, "opacity", "-p <FLOAT>"));
      auto range = std::make_unique<ValRange<float>>(
          MIN_OPAC, MAX_OPAC, OPAC, "Opacity", Interval::closed, "default",
          Problem::warning);
      range->check(opacity);
    } else if (*i == "-l" || *i == "--link") {
      link =
          static_cast<uint8_t>(std::stoul(require_value(i, "link", "-l <INT>")));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_LINK, MAX_LINK, LINK, "Link", Interval::closed, "default",
          Problem::warning);
      range->check(link);
    } else if (*i == "-m" || *i == "--min-block-size") {
      min = static_cast<uint32_t>(
          std::stoul(require_value(i, "minimum block size", "-m <INT>")));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_MINP, MAX_MINP, MINP, "Min", Interval::closed, "default",
          Problem::warning);
      range->check(min);
    } else if (*i == "-tc" || *i == "--total-colors") {
      man_tot_color = true;
      auto val =
          std::stoi(require_value(i, "total number of colors", "-tc <INT>"));
      keep_in_range(1, val, 255);
      tot_color = static_cast<uint8_t>(val);
    } else if (*i == "-rt" || *i == "--reference-tick") {
      refTick = std::stoull(
          require_value(i, "reference tick", "-rt <INT>"));
      auto range = std::make_unique<ValRange<uint64_t>>(
          MIN_TICK, MAX_TICK, TICK, "Tick hop for reference", Interval::closed,
          "default", Problem::warning);
      range->check(refTick);
    } else if (*i == "-tt" || *i == "--target-tick") {
      tarTick =
          std::stoull(require_value(i, "target tick", "-tt <INT>"));
      auto range = std::make_unique<ValRange<uint64_t>>(
          MIN_TICK, MAX_TICK, TICK, "Tick hop for target", Interval::closed,
          "default", Problem::warning);
      range->check(tarTick);
    } else if (*i == "-th" || *i == "--tick-human-readable") {
      tickHumanRead =
          (std::stoi(require_value(i, "tick human readable", "-th <INT>")) != 0);
    } else if (*i == "-c" || *i == "--color") {
      colorMode =
          static_cast<uint8_t>(std::stoi(require_value(i, "color mode", "-c <INT>")));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_COLOR, MAX_COLOR, COLOR, "Color", Interval::closed, "default",
          Problem::warning);
      range->check(colorMode);
    } else if (*i == "-w" || *i == "--width") {
      width = static_cast<uint32_t>(
          std::stoul(require_value(i, "width", "-w <INT>")));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_WDTH, MAX_WDTH, WDTH, "Width", Interval::closed, "default",
          Problem::warning);
      range->check(width);
    } else if (*i == "-s" || *i == "--space") {
      space = static_cast<uint32_t>(
          std::stoul(require_value(i, "space", "-s <INT>")));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_SPC, MAX_SPC, SPC, "Space", Interval::closed, "default",
          Problem::warning);
      range->check(space);
    } else if (*i == "-nrr" || *i == "--no-relative-redundancy") {
      showRelRedun = false;
    } else if (*i == "-nr" || *i == "--no-redundancy") {
      showRedun = false;
    } else if (*i == "-ni" || *i == "--no-inverted") {
      inverse = false;
    } else if (*i == "-ng" || *i == "--no-regular") {
      regular = false;
    } else if (*i == "-n" || *i == "--show-N") {
      showN = true;
    } else if (*i == "-vv" || *i == "--vertical-view") {
      vertical = true;
    } else if (*i == "-stat" || *i == "--statistics") {
      stat = true;
    } else if (!i->empty() && i->front() != '-') {
      if (!posFile.empty())
        error("multiple position files specified for visualization.");
      posFile = *i;
    } else {
      error("unknown option \"" + *i + "\".");
    }
  }
  if (posFile.empty())
    error("position file not specified. Use \"smashpp viz <POS_FILE>\".");
}

void VizParam::help() const {
  const std::string tab1 = "      ";
  const std::string tab2 = tab1 + "      ";
  const std::string tab3 = tab2 + "    ";

  std::cerr
      << bold("SYNOPSIS") << '\n'
      << tab1 << "./smashpp viz|-viz [OPTIONS]  -o <SVG_FILE>  <POS_FILE>\n"
      << '\n'
      << bold("SAMPLE") << '\n'
      << tab1 << "./smashpp -viz -o simil.svg ref.tar.pos\n"
      << tab1 << "./smashpp viz \\ \n"
      << tab1 << "    --output similarity.svg \\ \n"
      << tab1 << "    --vertical-view \\ \n"
      << tab1 << "    ref.tar.json\n"
      << '\n'
      << bold("OPTIONS") << '\n'
      << "    " << italic("Required") << ":\n"
      << tab1
      << "<POS_FILE>    position file generated by Smash++ (*.pos/*.json)\n"
      << '\n'
      << "    " << italic("Optional") << ":\n"
      << tab1 << bold("-o") << ", " << bold("--output") << " <SVG_FILE>\n"
      << tab2 << "output image name (*.svg). Default: map.svg\n"
      << '\n'
      << tab1 << bold("-rn") << ", " << bold("--reference-name")
      << " <STRING>\n"
      << tab2 << "reference name shown on output. If it has spaces, use\n"
      << tab2 << "double quotes, e.g. \"Seq label\". Default: the name in\n"
      << tab2 << "the header of position file\n"
      << '\n'
      << tab1 << bold("-tn") << ", " << bold("--target-name") << " <STRING>\n"
      << tab2 << "target name shown on output\n"
      << '\n'
      << tab1 << bold("-l") << ", " << bold("--link") << " <INT>\n"
      << tab2 << "type of the link between maps: " << std::to_string(MIN_LINK)
      << " to " << std::to_string(MAX_LINK)
      << ". Default: " << std::to_string(link) << '\n'
      << '\n'
      << tab1 << bold("-c") << ", " << bold("--color") << " <INT>\n"
      << tab2 << "color mode: {" << std::to_string(MIN_COLOR) << ", "
      << std::to_string(MAX_COLOR)
      << "}. Default: " << std::to_string(colorMode) << '\n'
      << '\n'
      << tab1 << bold("-p") << ", " << bold("--opacity") << " <FLOAT>\n"
      << tab2 << "opacity: " << string_format("%.1f", MIN_OPAC) << " to "
      << string_format("%.1f", MAX_OPAC)
      << ". Default: " << string_format("%.1f", opacity) << '\n'
      << '\n'
      << tab1 << bold("-w") << ", " << bold("--width") << " <INT>\n"
      << tab2 << "width of the sequence: " << std::to_string(MIN_WDTH) << " to "
      << std::to_string(MAX_WDTH) << ". Default: " << std::to_string(width)
      << '\n'
      << '\n'
      << tab1 << bold("-s") << ", " << bold("--space") << " <INT>\n"
      << tab2 << "space between sequences: " << std::to_string(MIN_SPC)
      << " to " << std::to_string(MAX_SPC)
      << ". Default: " << std::to_string(space) << '\n'
      << '\n'
      << tab1 << bold("-tc") << ", " << bold("--total-colors") << " <INT>\n"
      << tab2 << "total number of colors: 1 to 255\n"
      << '\n'
      << tab1 << bold("-rt") << ", " << bold("--reference-tick") << " <INT>\n"
      << tab2 << "reference tick: " << std::to_string(MIN_TICK) << " to "
      << std::to_string(MAX_TICK) << '\n'
      << '\n'
      << tab1 << bold("-tt") << ", " << bold("--target-tick") << " <INT>\n"
      << tab2 << "target tick: " << std::to_string(MIN_TICK) << " to "
      << std::to_string(MAX_TICK) << '\n'
      << '\n'
      << tab1 << bold("-th") << ", " << bold("--tick-human-readable")
      << " <INT>\n"
      << tab2 << "tick human readable: {0: false, 1: true}. Default: "
      << std::to_string(tickHumanRead) << '\n'
      << '\n'
      << tab1 << bold("-m") << ", " << bold("--min-block-size") << " <INT>\n"
      << tab2 << "minimum block size: " << std::to_string(MIN_MINP) << " to "
      << std::to_string(MAX_MINP) << ". Default: " << std::to_string(min)
      << '\n'
      << '\n'
      << tab1 << bold("-vv") << ", " << bold("--vertical-view") << '\n'
      << tab2 << "vertical view. Default: not used\n"
      << '\n'
      << tab1 << bold("-nrr") << ", " << bold("--no-relative-redundancy")
      << '\n'
      << tab2 << "do not show relative redundancy (relative complexity).\n"
      << tab2 << "Default: not used\n"
      << '\n'
      << tab1 << bold("-nr") << ", " << bold("--no-redundancy") << '\n'
      << tab2 << "do not show redundancy. Default: not used\n"
      << '\n'
      << tab1 << bold("-ni") << ", " << bold("--no-inverted") << '\n'
      << tab2 << "do not show inverse maps. Default: not used\n"
      << '\n'
      << tab1 << bold("-ng") << ", " << bold("--no-regular") << '\n'
      << tab2 << "do not show regular maps. Default: not used\n"
      << '\n'
      << tab1 << bold("-n") << ", " << bold("--show-N") << '\n'
      << tab2 << "show 'N' bases. Default: not used\n"
      << '\n'
      << tab1 << bold("-stat") << ", " << bold("--statistics") << '\n'
      << tab2 << "save statistics (*.csv). Default: stat.csv\n"
      << '\n'
      << tab1 << bold("-h") << ", " << bold("--help") << '\n'
      << tab2 << "usage guide\n"
      << '\n'
      << tab1 << bold("-v") << ", " << bold("--verbose") << '\n'
      << tab2 << "more information\n"
      << '\n'
      << tab1 << bold("-V") << ", " << bold("--version") << '\n'
      << tab2 << "show version\n"
      << '\n'
      << bold("AUTHOR") << '\n'
      << tab1 << "Morteza Hosseini      mhosayny@gmail.com\n";
}
