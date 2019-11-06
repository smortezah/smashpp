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

  auto option_inserted = [&](auto iter, std::string name) -> bool {
    return (iter + 1 <= std::end(vArgs)) && (*iter == name);
  };

  bool man_rm{false};
  bool man_tm{false};
  std::string rModelsPars;
  std::string tModelsPars;

  for (auto i = std::begin(vArgs); i != std::end(vArgs); ++i) {
    if (*i == "-h") {
      help();
      throw EXIT_SUCCESS;
    } else if (*i == "--version") {
      std::cerr << "Smash++ " << VERSION << "\n"
                << "Maintained by Morteza Hosseini (seyedmorteza@ua.pt)" << "\n"
                << "Copyright (C) " << DEV_YEARS
                << " IEETA, University of Aveiro." << "\n";
      throw EXIT_SUCCESS;
    } else if (*i == "-v") {
      verbose = true;
    } else if (*i == "-ll") {
      std::cerr << "Level Model parameters" << '\n';
      for (size_t i = 0; i != LEVEL.size(); ++i)
        std::cerr << "[ " << i << " ]  " << LEVEL[i] << '\n';
      throw EXIT_SUCCESS;
    } else if (*i == "-r") {
      if (i + 1 != std::end(vArgs)) {
        ref = *++i;
        check_file(ref);
        refName = file_name(ref);
        refType = file_type(ref);
      } else {
        error("reference file not specified. Use \"-r <fileName>\".");
      }
    } else if (*i == "-t") {
      if (i + 1 != std::end(vArgs)) {
        tar = *++i;
        check_file(tar);
        tarName = file_name(tar);
        tarType = file_type(tar);
      } else {
        error("target file not specified. Use \"-t <fileName>\".");
      }
    } else if (option_inserted(i, "-l")) {
      man_level = true;
      level = static_cast<uint8_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_LVL, MAX_LVL, LVL, "Level", Interval::closed, "default",
          Problem::warning);
      range->assert(level);
    } else if (option_inserted(i, "-m")) {
      segSize = std::stoul(*++i);
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_SSIZE, MAX_SSIZE, SSIZE, "Minimum segment size", Interval::closed,
          "default", Problem::warning);
      range->assert(segSize);
    } else if (option_inserted(i, "-rm")) {
      man_rm = true;
      rModelsPars = *++i;
      if (rModelsPars.front() == '-' || rModelsPars.empty())
        error("incorrect reference model parameters.");
      else
        parseModelsPars(std::begin(rModelsPars), std::end(rModelsPars), refMs);
    } else if (option_inserted(i, "-tm")) {
      man_tm = true;
      tModelsPars = *++i;
      if (tModelsPars.front() == '-' || tModelsPars.empty())
        error("incorrect target model parameters.");
      else
        parseModelsPars(std::begin(tModelsPars), std::end(tModelsPars), tarMs);
    } else if (option_inserted(i, "-f")) {
      manWSize = true;
      filt_size = static_cast<uint32_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_WS, MAX_WS, WS, "Filter size", Interval::closed, "default",
          Problem::warning);
      range->assert(filt_size);
    } else if (option_inserted(i, "-th")) {
      manThresh = true;
      thresh = std::stof(*++i);
      auto range = std::make_unique<ValRange<float>>(
          MIN_THRSH, MAX_THRSH, THRSH, "Threshold", Interval::open_closed,
          "default", Problem::warning);
      range->assert(thresh);
    } else if (option_inserted(i, "-ft")) {
      const auto is_win_type = [](std::string t) {
        return (t == "0" || t == "rectangular" || t == "1" || t == "hamming" ||
                t == "2" || t == "hann" || t == "3" || t == "blackman" ||
                t == "4" || t == "triangular" || t == "5" || t == "welch" ||
                t == "6" || t == "sine" || t == "7" || t == "nuttall");
      };
      const std::string cmd{*++i};
      auto set = std::make_unique<ValSet<FilterType>>(
          SET_WTYPE, FT, "Window type", "default", Problem::warning,
          win_type(cmd), is_win_type(cmd));
      set->assert(filt_type);
    } else if (option_inserted(i, "-e")) {
      entropyN = static_cast<prc_t>(std::stod(*++i));
      auto range = std::make_unique<ValRange<prc_t>>(
          MIN_ENTR_N, MAX_ENTR_N, ENTR_N, "Entropy of N bases",
          Interval::closed, "default", Problem::warning);
      range->assert(entropyN);
    } else if (option_inserted(i, "-n")) {
      nthr = static_cast<uint8_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_THRD, MAX_THRD, THRD, "Number of threads", Interval::closed,
          "default", Problem::warning);
      range->assert(nthr);
    } else if (option_inserted(i, "-d")) {
      sampleStep = std::stoull(*++i);
      if (sampleStep == 0) sampleStep = 1ull;
    } else if (option_inserted(i, "-fs")) {
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
    } else if (option_inserted(i, "-rb")) {
      ref_guard->beg = static_cast<int16_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Reference beginning guard",
          Interval::closed, "default", Problem::warning);
      range->assert(ref_guard->beg);
    } else if (option_inserted(i, "-re")) {
      ref_guard->end = static_cast<int16_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Reference ending guard",
          Interval::closed, "default", Problem::warning);
      range->assert(ref_guard->end);
    } else if (option_inserted(i, "-tb")) {
      tar_guard->beg = static_cast<int16_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Target beginning guard",
          Interval::closed, "default", Problem::warning);
      range->assert(tar_guard->beg);
    } else if (option_inserted(i, "-te")) {
      tar_guard->end = static_cast<int16_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<int16_t>>(
          std::numeric_limits<int16_t>::min(),
          std::numeric_limits<int16_t>::max(), 0, "Target ending guard", Interval::closed,
          "default", Problem::warning);
      range->assert(tar_guard->end);
    } else if (*i == "-ar") {
      asym_region = true;
    } else if (*i == "-dp") {
      deep = true;
    } else if (*i == "-nr") {
      noRedun = true;
    } else if (*i == "-sb") {
      saveSeq = true;
    } else if (*i == "-sp") {
      saveProfile = true;
    } else if (*i == "-sf") {
      saveFilter = true;
    } else if (*i == "-ss") {
      saveSegment = true;
    } else if (*i == "-sa") {
      saveAll = true;
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

  manFilterScale = !manThresh;
  manFilterScale = !manWSize;

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
  const std::string delim_descr1 = "=";
  const std::string delim_descr2 = " ";
  const std::string delim_def = "->";

  print_title("SYNOPSIS");
  print_line("./smashpp [OPTIONS]  -r <REF-FILE>  -t <TAR-FILE>");
  print_line("");

  print_title("OPTIONS");
  print_line(italic("Required")+":");

  print_align(bold("-r"), "FILE", delim_descr1, "reference file (Seq/FASTA/FASTQ)");

  print_align(bold("-t"), "FILE", delim_descr1, "target file    (Seq/FASTA/FASTQ)");
  print_line("");

  print_line(italic("Optional")+":");
  print_align(bold("-l"), "INT", delim_descr1,
                "level of compression: [" + std::to_string(MIN_LVL) + ", " +
                    std::to_string(MAX_LVL) + "]. Default",
                delim_def, std::to_string(LVL));

  print_align(bold("-m"), "INT", delim_descr1,
                "min segment size: [" + std::to_string(MIN_SSIZE) + ", " +
                    std::to_string(MAX_SSIZE) + "]",
                delim_def, std::to_string(SSIZE));

  print_align(bold("-e"), "FLOAT", delim_descr1,
                "entropy of 'N's: [" + string_format("%.1f", MIN_ENTR_N) +
                    ", " + string_format("%.1f", MAX_ENTR_N) + "]",
                delim_def, string_format("%.1f", ENTR_N));

  print_align(bold("-n"), "INT", delim_descr1,
                "number of threads: [" + std::to_string(MIN_THRD) + ", " +
                    std::to_string(MAX_THRD) + "]",
                delim_def, std::to_string(THRD));

  print_align(bold("-f"), "INT", delim_descr1,
                "filter size: [" + std::to_string(MIN_WS) + ", " +
                    std::to_string(MAX_WS) + "]",
                delim_def, std::to_string(WS));

  print_align(bold("-ft"), "INT/STRING", delim_descr1,
                "filter type (windowing function):", delim_def, "hann");
  print_align("", delim_descr2, "{0/rectangular, 1/hamming, 2/hann,");
  print_align("", "", "", "3/blackman, 4/triangular, 5/welch,");
  print_align("", "", "", "6/sine, 7/nuttall}");

  // print_align("-fs", "[S][M][L]", delim_descr1, "filter scale");
  // // print_align("-fs", "[S][M][L]", delim_descr1, "filter scale:", delim_def, "L");
  // print_align("", "", "", "{S/small, M/medium, L/large}");

  // print_align("-d", "INT", delim_descr1, "sampling steps", delim_def,
  //               std::to_string(SAMPLE_STEP));

  // print_align("-th", "FLOAT", delim_descr1,
  //               "threshold: [" + string_format("%.1f", MIN_THRSH) + ", " +
  //                   string_format("%.1f", MAX_THRSH) + "]",
  //               delim_def, string_format("%.1f", THRSH));

  // print_align(
  //     "-rb", "INT", delim_descr1,
  //     "ref beginning guard: [" +
  //         std::to_string(std::numeric_limits<decltype(ref_guard->beg)>::min()) +
  //         ", " +
  //         std::to_string(std::numeric_limits<decltype(ref_guard->beg)>::max()) +
  //         "]",
  //     delim_def, std::to_string(ref_guard->beg));

  // print_align(
  //     "-re", "INT", delim_descr1,
  //     "ref ending guard: [" +
  //         std::to_string(std::numeric_limits<decltype(ref_guard->end)>::min()) +
  //         ", " +
  //         std::to_string(std::numeric_limits<decltype(ref_guard->end)>::max()) +
  //         "]",
  //     delim_def, std::to_string(ref_guard->end));

  // print_align(
  //     "-tb", "INT", delim_descr1,
  //     "tar beginning guard: [" +
  //         std::to_string(std::numeric_limits<decltype(tar_guard->beg)>::min()) +
  //         ", " +
  //         std::to_string(std::numeric_limits<decltype(tar_guard->beg)>::max()) +
  //         "]",
  //     delim_def, std::to_string(tar_guard->beg));

  // print_align(
  //     "-te", "INT", delim_descr1,
  //     "tar ending guard: [" +
  //         std::to_string(std::numeric_limits<decltype(tar_guard->end)>::min()) +
  //         ", " +
  //         std::to_string(std::numeric_limits<decltype(tar_guard->end)>::max()) +
  //         "]",
  //     delim_def, std::to_string(tar_guard->end));

  // print_align("-ar", "", delim_descr1, "consider asymmetric regions", delim_def, "no");

  // print_align("-dp", "", delim_descr1, "deep compression", delim_def, "no");

  // print_align("-nr", "", delim_descr1, "do NOT compute self complexity", delim_def, "no");

  // print_align("-sb", "", delim_descr1, "save sequence (input: FASTA/FASTQ)", delim_def,
  //               "no");

  // print_align("-sp", "", delim_descr1, "save profile (*.prf)", delim_def, "no");

  // print_align("-sf", "", delim_descr1, "save filtered file (*.fil)", delim_def, "no");

  // print_align("-ss", "", delim_descr1, "save segmented files (*.s[i])", delim_def, "no");

  // print_align("-sa", "", delim_descr1, "save profile, filetered and", delim_def, "no");
  // print_align("", "", "", "segmented files");

  // print_line(bold("-rm") + " " + italic("k") + ",[" + italic("w") + "," +
  //            italic("d") + ",]ir," + italic("a") + "," + italic("g") + "/" +
  //            italic("t") + ",ir," + italic("a") + "," + italic("g") + ":...");

  // print_line(bold("-tm") + " " + italic("k") + ",[" + italic("w") + "," +
  //            italic("d") + ",]ir," + italic("a") + "," + italic("g") + "/" +
  //            italic("t") + ",ir," + italic("a") + "," + italic("g") + ":...");

  // print_align("", "", delim_descr1, "parameters of models");

  // print_align_right_left("", "INT", "", italic("k") + ":  context size");

  // print_align_right_left("", "INT", "",
  //                          italic("w") + ":  width of sketch in log2 form,");
  // print_align_right_left("", "", "", "    e.g., set 10 for w=2^10=1024");

  // print_align_right_left("", "INT", "", italic("d") + ":  depth of sketch");

  // print_align_right_left("", "INT", "", "ir: inverted repeat: {0, 1, 2}");
  // print_align_right_left("", "", "", "    0: regular (not inverted)");
  // print_align_right_left("", "", "", "    1: inverted, solely");
  // print_align_right_left("", "", "", "    2: both regular and inverted");

  // print_align_right_left("", "FLOAT", "", italic("a") + ":  estimator");

  // print_align_right_left("", "FLOAT", "",
  //                          italic("g") + ":  forgetting factor: [0.0, 1.0)");

  // print_align_right_left("", "INT", "",
  //                          italic("t") + ":  threshold (no. substitutions)");

  // print_align("-ll", "", delim_descr1, "list of compression levels");

  // print_align("-h", "", delim_descr1, "usage guide");

  // print_align("-v", "", delim_descr1, "more information");

  // print_line(bold("--version") + "          = show version");

  // std::cerr << '\n';

  // print_title("AUTHOR");
  // print_align("Morteza Hosseini", "seyedmorteza@ua.pt");
  // std::cerr << '\n';

  // print_title("SAMPLE");
  // print_line("./smashpp -r ref -t tar -l 3 -f 50");
  // std::cerr << '\n';

  // // // title("COPYRIGHT");
  // // // line("Copyright (C) " + DEV_YEARS +
  // // //      ", IEETA, University of Aveiro. You may  ");
  // // // line("redistribute copies of this Free software under the terms of the");
  // // // line("GPL v3 (General Public License) <www.gnu.org/licenses/gpl.html>.");
  // // // line("There is NO WARRANTY, to the extent permitted by law.");
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

  auto option_inserted = [&](auto iter, std::string name) -> bool {
    return (iter + 1 <= std::end(vArgs)) && (*iter == name);
  };

  for (auto i = std::begin(vArgs); i != std::end(vArgs); ++i) {
    if (*i == "-h") {
      help();
      throw EXIT_SUCCESS;
    } else if (*i == "-v") {
      verbose = true;
    } else if (*i == "--version") {
      std::cerr << "Smash++ " << VERSION << "\n"
                << "Maintained by Morteza Hosseini (seyedmorteza@ua.pt)"
                << "\n"
                << "Copyright (C) " << DEV_YEARS
                << " IEETA, University of Aveiro."
                << "\n";
      throw EXIT_SUCCESS;
    } else if (option_inserted(i, "-o")) {
      image = *++i;
    } else if (option_inserted(i, "-rn")) {
      refName = *++i;
    } else if (option_inserted(i, "-tn")) {
      tarName = *++i;
    } else if (option_inserted(i, "-p")) {
      opacity = std::stof(*++i);
      auto range = std::make_unique<ValRange<float>>(
          MIN_OPAC, MAX_OPAC, OPAC, "Opacity", Interval::closed, "default",
          Problem::warning);
      range->assert(opacity);
    } else if (option_inserted(i, "-l")) {
      link = static_cast<uint8_t>(std::stoul(*++i));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_LINK, MAX_LINK, LINK, "Link", Interval::closed, "default", Problem::warning);
      range->assert(link);
    } else if (option_inserted(i, "-m")) {
      min = static_cast<uint32_t>(std::stoul(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_MINP, MAX_MINP, MINP, "Min", Interval::closed, "default", Problem::warning);
      range->assert(min);
    } else if (option_inserted(i, "-tc")) {
      man_tot_color = true;
      auto val = std::stoi(*++i);
      keep_in_range(1, val, 255);
      tot_color = static_cast<uint8_t>(val);
    } else if (option_inserted(i, "-rt")) {
      refTick = std::stoull(*++i);
      auto range = std::make_unique<ValRange<uint64_t>>(
          MIN_TICK, MAX_TICK, TICK, "Tick hop for reference", Interval::closed, "default",
          Problem::warning);
      range->assert(refTick);
    } else if (option_inserted(i, "-tt")) {
      tarTick = std::stoull(*++i);
      auto range = std::make_unique<ValRange<uint64_t>>(
          MIN_TICK, MAX_TICK, TICK, "Tick hop for target", Interval::closed, "default",
          Problem::warning);
      range->assert(tarTick);
    } else if (option_inserted(i, "-th")) {
      tickHumanRead = (std::stoi(*++i) != 0);
    } else if (option_inserted(i, "-c")) {
      colorMode = static_cast<uint8_t>(std::stoi(*++i));
      auto range = std::make_unique<ValRange<uint8_t>>(
          MIN_COLOR, MAX_COLOR, COLOR, "Color", Interval::closed, "default",
          Problem::warning);
      range->assert(colorMode);
    } else if (option_inserted(i, "-w")) {
      width = static_cast<uint32_t>(std::stoul(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_WDTH, MAX_WDTH, WDTH, "Width", Interval::closed, "default", Problem::warning);
      range->assert(width);
    } else if (option_inserted(i, "-s")) {
      space = static_cast<uint32_t>(std::stoul(*++i));
      auto range = std::make_unique<ValRange<uint32_t>>(
          MIN_SPC, MAX_SPC, SPC, "Space", Interval::closed, "default", Problem::warning);
      range->assert(space);
    } else if (*i == "-nn") {
      showNRC = false;
    } else if (*i == "-vv") {
      vertical = true;
    } else if (*i == "-nr") {
      showRedun = false;
    } else if (*i == "-ni") {
      inverse = false;
    } else if (*i == "-ng") {
      regular = false;
    } else if (*i == "-stat") {
      stat = true;
    } else if (*i == "-n") {
      showN = true;
    }
  }
  posFile = vArgs.back();
}

void VizParam::help() const {
  const std::string delim_descr1 = "=";
  const std::string delim_descr2 = " ";
  const std::string delim_def = "->";

  print_title("SYNOPSIS");
  print_line("./smashpp -viz [OPTIONS]  -o <SVG-FILE>  <POS-FILE>");
  print_line("");

  print_title("OPTIONS");
  print_line(italic("Required") + ":");
  print_align("<POS-FILE>", delim_descr1, "position file, generated by");
  print_align("", delim_descr2, "Smash++ tool (*.pos)", "", " ");
  print_line("");

  print_line(italic("Optional") + ":");
  print_align(bold("-o"), "SVG-FILE", delim_descr1,
              "output image name (*.svg).    Default", delim_def, "map.svg");

  print_align(bold("-rn"), "STRING", delim_descr1,
              "reference name shown on output. If it");
  print_align("", delim_descr2, "has spaces, use double quotes, e.g.");
  print_align("", delim_descr2, "\"Seq label\". Default: name in header");
  print_align("", delim_descr2, "of position file");

  print_align(bold("-tn"), "STRING", delim_descr1,
              "target name shown on output");

  print_align(bold("-l"), "INT", delim_descr1,
              "type of the link between maps: [" + std::to_string(MIN_LINK) +
                  ", " + std::to_string(MAX_LINK) + "]",
              delim_def, std::to_string(link));

  print_align(bold("-c"), "INT", delim_descr1,
              "color mode: [" + std::to_string(MIN_COLOR) + ", " +
                  std::to_string(MAX_COLOR) + "]",
              delim_def, std::to_string(colorMode));

  print_align(bold("-p"), "FLOAT", delim_descr1,
              "opacity: [" + string_format("%.1f", MIN_OPAC) + ", " +
                  string_format("%.1f", MAX_OPAC) + "]",
              delim_def, string_format("%.1f", opacity));

  print_align(bold("-w"), "INT", delim_descr1,
              "width of the sequence: [" + std::to_string(MIN_WDTH) + ", " +
                  std::to_string(MAX_WDTH) + "]",
              delim_def, std::to_string(width));

  print_align(bold("-s"), "INT", delim_descr1,
              "space between sequences: [" + std::to_string(MIN_SPC) + ", " +
                  std::to_string(MAX_SPC) + "]",
              delim_def, std::to_string(space));

  print_align(bold("-tc"), "INT", delim_descr1,
              "total number of colors: [" + std::to_string(1) + ", " +
                  std::to_string(255) + "]");

  print_align(bold("-rt"), "INT", delim_descr1,
              "reference tick: [" + std::to_string(MIN_TICK) + ", " +
                  std::to_string(MAX_TICK) + "]");

  print_align(bold("-tt"), "INT", delim_descr1,
              "target tick: [" + std::to_string(MIN_TICK) + ", " +
                  std::to_string(MAX_TICK) + "]");

  print_align(bold("-th"), "[0][1]", delim_descr1,
              "tick human readable: 0=false, 1=true", delim_def,
              std::to_string(tickHumanRead));

  print_align(bold("-m"), "INT", delim_descr1,
              "minimum block size: [" + std::to_string(MIN_MINP) + ", " +
                  std::to_string(MAX_MINP) + "]",
              delim_def, std::to_string(min));

  print_align(bold("-vv"), delim_descr1, "vertical view", delim_def, "no");

  print_align(bold("-nn"), delim_descr1, "do NOT show normalized relative",
              delim_def, "no");
  print_align("", delim_descr2, "compression (NRC)");

  print_align(bold("-nr"), delim_descr1, "do NOT show self complexity",
              delim_def, "no");

  print_align(bold("-ni"), delim_descr1, "do NOT show inverse maps", delim_def,
              "no");

  print_align(bold("-ng"), delim_descr1, "do NOT show regular maps", delim_def,
              "no");

  print_align(bold("-n"), delim_descr1, "show 'N' bases", delim_def, "no");

  print_align(bold("-stat"), delim_descr1, "save stats (*.csv)", delim_def,
              "stat.csv");

  print_align(bold("-h"), delim_descr1, "usage guide");

  print_align(bold("-v"), delim_descr1, "more information");

  print_align(bold("--version"), delim_descr1, "show version");
  print_line("");

  print_title("AUTHOR");
  print_align("Morteza Hosseini", delim_descr2, "seyedmorteza@ua.pt");
  print_line("");

  print_title("SAMPLE");
  print_line("./smashpp -viz -vv -o simil.svg ref.tar.pos");
  print_line("");

  // title("COPYRIGHT");
  // line("Copyright (C) " + DEV_YEARS +
  //      ", IEETA, University of Aveiro. You may  ");
  // line("redistribute copies of this Free software under the terms of the");
  // line("GPL v3 (General Public License) <www.gnu.org/licenses/gpl.html>.");
  // line("There is NO WARRANTY, to the extent permitted by law.");
}