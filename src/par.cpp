// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#include "par.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <format>
#include <fstream>
#include <limits>
#include <memory>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <vector>

#include "check.hpp"
#include "container.hpp"
#include "def.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "mdlpar.hpp"
#include "memory.hpp"
#include "print.hpp"
using namespace smashpp;

namespace {
auto strip_leading_plus(std::string_view value) -> std::string_view {
  if (value.size() > 1 && value.front() == '+') {
    value.remove_prefix(1);
  }
  return value;
}

template <typename Int>
auto parse_integral(std::string_view value, std::string_view label) -> Int {
  static_assert(std::is_integral_v<Int>);

  Int parsed{0};
  const auto input = strip_leading_plus(value);
  const auto* first = input.data();
  const auto* last = first + input.size();
  const auto [ptr, ec] = std::from_chars(first, last, parsed);

  if (input.empty() || ec == std::errc::invalid_argument || ptr != last) {
    error(std::format("{} must be an integer.", label));
  }
  if (ec == std::errc::result_out_of_range) {
    error(std::format("{} is out of range.", label));
  }

  return parsed;
}

template <typename Int>
auto parse_integral(std::string_view value, std::string_view label, Int min, Int max) -> Int {
  const auto parsed = parse_integral<Int>(value, label);
  if (parsed < min || parsed > max) {
    error(std::format("{} must be in range [{},{}].", label, min, max));
  }
  return parsed;
}

template <typename Float>
auto parse_floating(std::string_view value, std::string_view label) -> Float {
  static_assert(std::is_floating_point_v<Float>);

  Float parsed{0};
  const auto input = strip_leading_plus(value);
  const auto* first = input.data();
  const auto* last = first + input.size();
  const auto [ptr, ec] = std::from_chars(first, last, parsed);

  if (input.empty() || ec == std::errc::invalid_argument || ptr != last ||
      ec == std::errc::result_out_of_range || !std::isfinite(parsed)) {
    error(std::format("{} must be a finite number.", label));
  }

  return parsed;
}
}  // namespace

void Param::parse(int argc, char**& argv) {
  if (argc < 2) {
    help();
    throw EXIT_SUCCESS;
  }

  std::vector<std::string> vArgs;
  vArgs.reserve(static_cast<uint64_t>(argc));
  for (int i = 0; i != argc; ++i) {
    vArgs.push_back(static_cast<std::string>(argv[i]));
  }

  // Save the list of parameters for writing into position file
  for (auto iter = std::begin(vArgs) + 1; iter != std::end(vArgs); ++iter) {
    param_list += *iter + ' ';
  }
  param_list.pop_back();

  auto looks_like_option = [](const std::string& value) {
    return value.size() > 1 && value.front() == '-' &&
           !((value[1] >= '0' && value[1] <= '9') || value[1] == '.');
  };
  auto require_value = [&](auto& iter, const std::string& label,
                           const std::string& usage) -> std::string {
    if (iter + 1 == std::end(vArgs) || looks_like_option(*(iter + 1))) {
      error(std::format("{} not specified. Use \"{}\".", label, usage));
    }
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
      std::cerr << std::format("Smash++ {}\n", VERSION);
      throw EXIT_SUCCESS;
    } else if (*i == "-v" || *i == "--verbose") {
      verbose = true;
    } else if (*i == "-ll" || *i == "--list-levels") {
      std::cerr << "Level  Model parameters" << '\n';
      for (size_t i = 0; i != LEVEL.size(); ++i) {
        std::cerr << std::format("[ {} ]  {}\n", i, LEVEL[i]);
      }
      throw EXIT_SUCCESS;
    } else if (*i == "-r" || *i == "--reference") {
      ref = require_value(i, "reference file", "-r <fileName>");
      original_ref = ref;
      check_file(ref);
      refName = file_name(ref);
      refType = file_type(ref);
    } else if (*i == "-t" || *i == "--target") {
      tar = require_value(i, "target file", "-t <fileName>");
      original_tar = tar;
      check_file(tar);
      tarName = file_name(tar);
      tarType = file_type(tar);
    } else if (*i == "-l" || *i == "--level") {
      man_level = true;
      auto parsed_level = parse_integral<int>(require_value(i, "level", "-l <INT>"), "level");
      ValRange<int> range(MIN_LVL, MAX_LVL, LVL, "Level", Interval::closed, "default",
                          Problem::warning);
      range.check(parsed_level);
      level = static_cast<uint8_t>(parsed_level);
    } else if (*i == "-m" || *i == "--min-segment-size") {
      auto parsed_seg_size = parse_integral<uint64_t>(
          require_value(i, "minimum segment size", "-m <INT>"), "minimum segment size");
      ValRange<uint64_t> range(MIN_SSIZE, MAX_SSIZE, SSIZE, "Minimum segment size",
                               Interval::closed, "default", Problem::warning);
      range.check(parsed_seg_size);
      segSize = static_cast<uint32_t>(parsed_seg_size);
    } else if (*i == "-rm" || *i == "--reference-model") {
      man_rm = true;
      rModelsPars = require_value(i, "reference model parameters", "-rm <STRING>");
      if (rModelsPars.empty() || rModelsPars.front() == '-') {
        error("incorrect reference model parameters.");
      } else {
        parseModelsPars(std::begin(rModelsPars), std::end(rModelsPars), refMs);
      }
    } else if (*i == "-fmt" || *i == "--format") {
      auto format_ = require_value(i, "output format", "-fmt <STRING>");
      std::transform(std::begin(format_), std::end(format_), std::begin(format_), ::tolower);
      if (format_ == "pos") {  // default
      } else if (format_ == "json") {
        format = Format::json;
      } else {
        warning("incorrect format inserted. Used \"pos\" instead.\n");
      }
    } else if (*i == "-tm" || *i == "--target-model") {
      man_tm = true;
      tModelsPars = require_value(i, "target model parameters", "-tm <STRING>");
      if (tModelsPars.empty() || tModelsPars.front() == '-') {
        error("incorrect target model parameters.");
      } else {
        parseModelsPars(std::begin(tModelsPars), std::end(tModelsPars), tarMs);
      }
    } else if (*i == "-f" || *i == "--filter-size") {
      manWSize = true;
      auto parsed_filter_size =
          parse_integral<uint64_t>(require_value(i, "filter size", "-f <INT>"), "filter size");
      ValRange<uint64_t> range(MIN_WS, MAX_WS, WS, "Filter size", Interval::closed, "default",
                               Problem::warning);
      range.check(parsed_filter_size);
      filt_size = static_cast<uint32_t>(parsed_filter_size);
    } else if (*i == "-th" || *i == "--threshold") {
      manThresh = true;
      thresh = parse_floating<float>(require_value(i, "threshold", "-th <FLOAT>"), "threshold");
      ValRange<float> range(MIN_THRSH, MAX_THRSH, THRSH, "Threshold", Interval::open_closed,
                            "default", Problem::warning);
      range.check(thresh);
    } else if (*i == "-ft" || *i == "--filter-type") {
      const auto is_win_type = [](std::string t) {
        return (t == "0" || t == "rectangular" || t == "1" || t == "hamming" || t == "2" ||
                t == "hann" || t == "3" || t == "blackman" || t == "4" || t == "triangular" ||
                t == "5" || t == "welch" || t == "6" || t == "sine" || t == "7" || t == "nuttall");
      };
      const std::string cmd{require_value(i, "filter type", "-ft <INT/STRING>")};
      ValSet<FilterType> set(SET_WTYPE, FT, "Window type", "default", Problem::warning,
                             win_type(cmd), is_win_type(cmd));
      set.check(filt_type);
    } else if (*i == "-e" || *i == "--entropy-N") {
      entropyN = parse_floating<prc_t>(require_value(i, "entropy of N bases", "-e <FLOAT>"),
                                       "entropy of N bases");
      ValRange<prc_t> range(MIN_ENTR_N, MAX_ENTR_N, ENTR_N, "Entropy of N bases", Interval::closed,
                            "default", Problem::warning);
      range.check(entropyN);
    } else if (*i == "-n" || *i == "--num-threads") {
      auto parsed_nthr = parse_integral<int>(require_value(i, "number of threads", "-n <INT>"),
                                             "number of threads");
      ValRange<int> range(MIN_THRD, MAX_THRD, THRD, "Number of threads", Interval::closed,
                          "default", Problem::warning);
      range.check(parsed_nthr);
      nthr = static_cast<uint8_t>(parsed_nthr);
    } else if (*i == "-mem" || *i == "--max-memory") {
      manMaxMemory = true;
      maxMemory = parse_memory_size(require_value(i, "memory budget", "-mem <SIZE>"));
    } else if (*i == "-d" || *i == "--sampling-step") {
      manSampleStep = true;
      sampleStep =
          parse_integral<uint64_t>(require_value(i, "sampling step", "-d <INT>"), "sampling step");
      if (sampleStep == 0) {
        sampleStep = 1ull;
      }
    } else if (*i == "--approx-sampled-models") {
      approxSampledModels = true;
    } else if (*i == "-fs" || *i == "--filter-scale") {
      manFilterScale = true;
      const auto is_filter_scale = [](std::string s) {
        return (s == "S" || s == "small" || s == "M" || s == "medium" || s == "L" || s == "large");
      };
      const std::string cmd{require_value(i, "filter scale", "-fs <STRING>")};
      ValSet<FilterScale> set(SET_FSCALE, filterScale, "Filter scale", "default", Problem::warning,
                              filter_scale(cmd), is_filter_scale(cmd));
      set.check(filterScale);
    } else if (*i == "-rb" || *i == "--reference-begin-guard") {
      auto parsed_guard = parse_integral<int>(
          require_value(i, "reference beginning guard", "-rb <INT>"), "reference beginning guard");
      ValRange<int> range(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max(),
                          0, "Reference beginning guard", Interval::closed, "default",
                          Problem::warning);
      range.check(parsed_guard);
      ref_guard->beg = static_cast<int16_t>(parsed_guard);
    } else if (*i == "-re" || *i == "--reference-end-guard") {
      auto parsed_guard = parse_integral<int>(
          require_value(i, "reference ending guard", "-re <INT>"), "reference ending guard");
      ValRange<int> range(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max(),
                          0, "Reference ending guard", Interval::closed, "default",
                          Problem::warning);
      range.check(parsed_guard);
      ref_guard->end = static_cast<int16_t>(parsed_guard);
    } else if (*i == "-tb" || *i == "--target-begin-guard") {
      auto parsed_guard = parse_integral<int>(
          require_value(i, "target beginning guard", "-tb <INT>"), "target beginning guard");
      ValRange<int> range(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max(),
                          0, "Target beginning guard", Interval::closed, "default",
                          Problem::warning);
      range.check(parsed_guard);
      tar_guard->beg = static_cast<int16_t>(parsed_guard);
    } else if (*i == "-te" || *i == "--target-end-guard") {
      auto parsed_guard = parse_integral<int>(require_value(i, "target ending guard", "-te <INT>"),
                                              "target ending guard");
      ValRange<int> range(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max(),
                          0, "Target ending guard", Interval::closed, "default", Problem::warning);
      range.check(parsed_guard);
      tar_guard->end = static_cast<int16_t>(parsed_guard);
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
  const bool has_reference{has(std::begin(vArgs), std::end(vArgs), "--reference")};
  if (!has_t && !has_target) {
    error("target file not specified. Use \"-t <fileName>\".");
  } else if (!has_r && !has_reference) {
    error("reference file not specified. Use \"-r <fileName>\".");
  }

  if (!man_rm && !man_tm) {
    if (!man_level) {
      set_auto_model_par();
    } else {
      parseModelsPars(std::begin(LEVEL[level]), std::end(LEVEL[level]), refMs);
      parseModelsPars(std::begin(REFFREE_LEVEL[level]), std::end(REFFREE_LEVEL[level]), tarMs);
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

  const auto max_filter_size = sampled_count(std::min(file_size(ref), file_size(tar)), sampleStep);
  keep_in_range(1ull, filt_size, max_filter_size);
}

void Param::set_auto_model_par() {
  const auto ref_size{file_size(ref)};
  const auto tar_size{file_size(tar)};
  const uint32_t small{300 * 1024};        // 300 K
  const uint32_t medium{1024 * 1024};      // 1 M
  const uint32_t large{10 * 1024 * 1024};  // 10 M
  std::array<std::string, 4> par{"11,0,0.01,0.95", "13,0,0.008,0.95", "18,0,0.002,0.95",
                                 "20,0,0.002,0.95"};
  std::string ref_par, tar_par;

  if (ref_size < small) {
    ref_par = par[0];
  } else if (ref_size < medium) {
    ref_par = par[1];
  } else if (ref_size < large) {
    ref_par = par[2];
  } else {
    ref_par = par[3];
  }

  if (tar_size < small) {
    tar_par = par[0];
  } else if (tar_size < medium) {
    tar_par = par[1];
  } else if (tar_size < large) {
    tar_par = par[2];
  } else {
    tar_par = par[3];
  }

  parseModelsPars(std::begin(ref_par), std::end(ref_par), refMs);
  parseModelsPars(std::begin(tar_par), std::end(tar_par), tarMs);
}

template <typename Iter>
void Param::parseModelsPars(Iter begin, Iter end, std::vector<MMPar>& Ms) {
  const auto parse_integer = [](const std::string& value, const std::string& label, int64_t min,
                                int64_t max) {
    return parse_integral<int64_t>(value, label, min, max);
  };
  const auto parse_float = [](const std::string& value, const std::string& label) {
    return parse_floating<prc_t>(value, label);
  };
  const auto parse_context_size = [&](const std::string& value) {
    const auto parsed = parse_integer(value, "model context size", 0, 31);
    return static_cast<uint8_t>(parsed);
  };
  const auto parse_width = [&](const std::string& value) {
    const auto parsed = parse_integer(value, "sketch width exponent", 1, 63);
    return pow2(static_cast<uint64_t>(parsed));
  };
  const auto parse_depth = [&](const std::string& value) {
    const auto parsed = parse_integer(value, "sketch depth", 1, 255);
    return static_cast<uint8_t>(parsed);
  };
  const auto parse_ir = [&](const std::string& value) {
    const auto parsed = parse_integer(value, "inverted repeat mode", 0, 2);
    return static_cast<uint8_t>(parsed);
  };
  const auto parse_threshold = [&](const std::string& value) {
    const auto parsed = parse_integer(value, "tolerant model threshold", 0, 255);
    return static_cast<uint8_t>(parsed);
  };
  const auto parse_alpha = [&](const std::string& value, const std::string& label) {
    const auto parsed = parse_float(value, label);
    if (parsed <= 0.0) {
      error(std::format("{} must be greater than 0.", label));
    }
    return parsed;
  };
  const auto parse_gamma = [&](const std::string& value, const std::string& label) {
    const auto parsed = parse_float(value, label);
    if (parsed < 0.0 || parsed > 1.0) {
      error(std::format("{} must be in range [0,1].", label));
    }
    return parsed;
  };

  std::vector<std::string> mdls;
  split(begin, end, ':', mdls);
  for (const auto& e : mdls) {
    if (e.empty()) {
      error("incorrect model parameters.");
    }

    // Markov and tolerant models
    std::vector<std::string> m_tm;
    split(std::begin(e), std::end(e), '/', m_tm);
    if (m_tm.size() > 2 || m_tm[0].empty()) {
      error("incorrect model parameters.");
    }

    std::vector<std::string> m;
    split(std::begin(m_tm[0]), std::end(m_tm[0]), ',', m);
    if (m.size() != 4 && m.size() != 6) {
      error("incorrect model parameters.");
    }
    const auto context = parse_context_size(m[0]);

    if (m.size() == 4) {
      const auto ir = parse_ir(m[1]);
      const auto alpha = parse_alpha(m[2], "model alpha");
      const auto gamma = parse_gamma(m[3], "model gamma");
      if (context > K_MAX_LGTBL8) {
        Ms.emplace_back(context, W, D, ir, alpha, gamma);
      } else {
        Ms.emplace_back(context, ir, alpha, gamma);
      }
    } else {
      Ms.emplace_back(context, parse_width(m[1]), parse_depth(m[2]), parse_ir(m[3]),
                      parse_alpha(m[4], "model alpha"), parse_gamma(m[5], "model gamma"));
    }

    // Tolerant models
    if (m_tm.size() == 2) {
      std::vector<std::string> tm;
      split(std::begin(m_tm[1]), std::end(m_tm[1]), ',', tm);
      if (tm.size() != 4) {
        error("incorrect tolerant model parameters.");
      }

      Ms.back().child = std::make_shared<STMMPar>(context, parse_threshold(tm[0]), parse_ir(tm[1]),
                                                  parse_alpha(tm[2], "tolerant model alpha"),
                                                  parse_gamma(tm[3], "tolerant model gamma"));
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
      << tab2 << std::format("level of compression: {} to {}. Default: {}\n", MIN_LVL, MAX_LVL, LVL)
      << '\n'
      << tab1 << bold("-m") << ", " << bold("--min-segment-size") << " <INT>\n"
      << tab2
      << std::format("minimum segment size: {} to {}. Default: {}\n", MIN_SSIZE, MAX_SSIZE, SSIZE)
      << '\n'
      << tab1 << bold("-fmt") << ", " << bold("--format") << " <STRING>\n"
      << tab2 << "format of the output (position) file: {pos, json}.\n"
      << tab2 << "Default: pos\n"
      << '\n'
      << tab1 << bold("-e") << ", " << bold("--entropy-N") << " <FLOAT>\n"
      << tab2
      << std::format("entropy of 'N's: {:.1f} to {:.1f}. Default: {:.1f}\n", MIN_ENTR_N, MAX_ENTR_N,
                     ENTR_N)
      << '\n'
      << tab1 << bold("-n") << ", " << bold("--num-threads") << " <INT>\n"
      << tab2 << std::format("number of threads: {} to {}. Default: {}\n", MIN_THRD, MAX_THRD, THRD)
      << '\n'
      << tab1 << bold("-mem") << ", " << bold("--max-memory") << " <SIZE>\n"
      << tab2 << "maximum estimated memory use. Supports B/K/M/G/T suffixes.\n"
      << tab2 << "Default: 80% of physical memory when detectable; 0 disables the check\n"
      << '\n'
      << tab1 << bold("-f") << ", " << bold("--filter-size") << " <INT>\n"
      << tab2 << std::format("filter size: {} to {}. Default: {}\n", MIN_WS, MAX_WS, WS) << '\n'
      << tab1 << bold("-ft") << ", " << bold("--filter-type") << " <INT/STRING>\n"
      << tab2 << "filter type (windowing function): {0/rectangular,\n"
      << tab2 << "1/hamming, 2/hann, 3/blackman, 4/triangular, 5/welch,\n"
      << tab2 << "6/sine, 7/nuttall}. Default: hann\n"
      << '\n'
      << tab1 << bold("-fs") << ", " << bold("--filter-scale") << " <STRING>\n"
      << tab2 << "filter scale: {S/small, M/medium, L/large}\n"
      << '\n'
      << tab1 << bold("-d") << ", " << bold("--sampling-step") << " <INT>\n"
      << tab2 << std::format("sampling step. Default: {}\n", SAMPLE_STEP) << '\n'
      << tab1 << bold("--approx-sampled-models") << '\n'
      << tab2 << "use faster approximate sampled multi-model updates. Default: not used\n"
      << '\n'
      << tab1 << bold("-th") << ", " << bold("--threshold") << " <FLOAT>\n"
      << tab2
      << std::format("threshold: {:.1f} to {:.1f}. Default: {:.1f}\n", MIN_THRSH, MAX_THRSH, THRSH)
      << '\n'
      << tab1 << bold("-rb") << ", " << bold("--reference-begin-guard") << " <INT>\n"
      << tab2
      << std::format("reference begin guard: {} to {}. Default: {}\n",
                     std::numeric_limits<decltype(ref_guard->beg)>::min(),
                     std::numeric_limits<decltype(ref_guard->beg)>::max(), ref_guard->beg)
      << '\n'
      << tab1 << bold("-re") << ", " << bold("--reference-end-guard") << " <INT>\n"
      << tab2
      << std::format("reference ending guard: {} to {}. Default: {}\n",
                     std::numeric_limits<decltype(ref_guard->end)>::min(),
                     std::numeric_limits<decltype(ref_guard->end)>::max(), ref_guard->end)
      << '\n'
      << tab1 << bold("-tb") << ", " << bold("--target-begin-guard") << " <INT>\n"
      << tab2
      << std::format("target begin guard: {} to {}. Default: {}\n",
                     std::numeric_limits<decltype(tar_guard->beg)>::min(),
                     std::numeric_limits<decltype(tar_guard->beg)>::max(), tar_guard->beg)
      << '\n'
      << tab1 << bold("-te") << ", " << bold("--target-end-guard") << " <INT>\n"
      << tab2
      << std::format("target ending guard: {} to {}. Default: {}\n",
                     std::numeric_limits<decltype(tar_guard->end)>::min(),
                     std::numeric_limits<decltype(tar_guard->end)>::max(), tar_guard->end)
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
      << tab1 << bold("-sa") << ", " << bold("--save-profile-filtered-segmented") << '\n'
      << tab2 << "save profile, filetered and segmented files.\n"
      << tab2 << "Default: not used\n"
      << '\n'
      << tab1 << bold("-rm") << ", " << bold("--reference-model") << "  " << italic("k") << ",["
      << italic("w") << "," << italic("d") << ",]ir," << italic("a") << "," << italic("g") << "/"
      << italic("t") << ",ir," << italic("a") << "," << italic("g") << ":...\n"
      << tab1 << bold("-tm") << ", " << bold("--target-model") << "     " << italic("k") << ",["
      << italic("w") << "," << italic("d") << ",]ir," << italic("a") << "," << italic("g") << "/"
      << italic("t") << ",ir," << italic("a") << "," << italic("g") << ":...\n"
      << tab2 << "parameters of models\n"
      << tab3 << "<INT>  " << italic("k") << ":  context size\n"
      << tab3 << "<INT>  " << italic("w") << ":  width of sketch in log2 form,\n"
      << tab3 << "           e.g., set 10 for w=2^10=1024\n"
      << tab3 << "<INT>  " << italic("d") << ":  depth of sketch\n"
      << tab3 << "<INT>  " << italic("ir") << ": inverted repeat: {0, 1, 2}\n"
      << tab3 << "           0: regular (not inverted)\n"
      << tab3 << "           1: inverted, solely\n"
      << tab3 << "           2: both regular and inverted\n"
      << tab2 << "  <FLOAT>  " << italic("a") << ":  estimator\n"
      << tab2 << "  <FLOAT>  " << italic("g") << ":  forgetting factor: 0.0 to 1.0\n"
      << tab3 << "<INT>  " << italic("t") << ":  threshold (number of substitutions)\n"
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
      << tab1 << "Morteza Hosseini      seyedmorteza.hosseini@manchester.ac.uk\n";
}

FilterType Param::win_type(std::string t) const {
  if (t == "0" || t == "rectangular") {
    return FilterType::rectangular;
  } else if (t == "1" || t == "hamming") {
    return FilterType::hamming;
  } else if (t == "2" || t == "hann") {
    return FilterType::hann;
  } else if (t == "3" || t == "blackman") {
    return FilterType::blackman;
  } else if (t == "4" || t == "triangular") {
    return FilterType::triangular;
  } else if (t == "5" || t == "welch") {
    return FilterType::welch;
  } else if (t == "6" || t == "sine") {
    return FilterType::sine;
  } else if (t == "7" || t == "nuttall") {
    return FilterType::nuttall;
  } else {
    return FilterType::hann;
  }
}

std::string Param::print_win_type() const {
  switch (filt_type) {
    case FilterType::rectangular:
      return "Rectangular";
    case FilterType::hamming:
      return "Hamming";
    case FilterType::hann:
      return "Hann";
    case FilterType::blackman:
      return "Blackman";
    case FilterType::triangular:
      return "Triangular";
    case FilterType::welch:
      return "Welch";
    case FilterType::sine:
      return "Sine";
    case FilterType::nuttall:
      return "Nuttall";
    default:
      return "Rectangular";
  }
}

FilterScale Param::filter_scale(std::string s) const {
  if (s == "S" || s == "small") {
    return FilterScale::s;
  } else if (s == "M" || s == "medium") {
    return FilterScale::m;
  } else if (s == "L" || s == "large") {
    return FilterScale::l;
  } else {
    return FilterScale::m;
  }
}

std::string Param::print_filter_scale() const {
  switch (filterScale) {
    case FilterScale::s:
      return "Small";
    case FilterScale::m:
      return "Medium";
    case FilterScale::l:
      return "Large";
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
  for (int i = 0; i != argc; ++i) {
    vArgs.push_back(static_cast<std::string>(argv[i]));
  }
  auto looks_like_option = [](const std::string& value) {
    return value.size() > 1 && value.front() == '-' &&
           !((value[1] >= '0' && value[1] <= '9') || value[1] == '.');
  };
  auto require_value = [&](auto& iter, const std::string& label,
                           const std::string& usage) -> std::string {
    if (iter + 1 == std::end(vArgs) || looks_like_option(*(iter + 1))) {
      error(std::format("{} not specified. Use \"{}\".", label, usage));
    }
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
      std::cerr << std::format("Smash++ {}\n", VERSION);
      throw EXIT_SUCCESS;
    } else if (*i == "-o" || *i == "--output") {
      image = require_value(i, "output image name", "-o <SVG_FILE>");
    } else if (*i == "-rn" || *i == "--reference-name") {
      refName = require_value(i, "reference name", "-rn <STRING>");
    } else if (*i == "-tn" || *i == "--target-name") {
      tarName = require_value(i, "target name", "-tn <STRING>");
    } else if (*i == "-p" || *i == "--opacity") {
      opacity = parse_floating<float>(require_value(i, "opacity", "-p <FLOAT>"), "opacity");
      ValRange<float> range(MIN_OPAC, MAX_OPAC, OPAC, "Opacity", Interval::closed, "default",
                            Problem::warning);
      range.check(opacity);
    } else if (*i == "-l" || *i == "--link") {
      auto parsed_link = parse_integral<int>(require_value(i, "link", "-l <INT>"), "link");
      ValRange<int> range(MIN_LINK, MAX_LINK, LINK, "Link", Interval::closed, "default",
                          Problem::warning);
      range.check(parsed_link);
      link = static_cast<uint8_t>(parsed_link);
    } else if (*i == "-m" || *i == "--min-block-size") {
      auto parsed_min = parse_integral<uint64_t>(require_value(i, "minimum block size", "-m <INT>"),
                                                 "minimum block size");
      ValRange<uint64_t> range(MIN_MINP, MAX_MINP, MINP, "Min", Interval::closed, "default",
                               Problem::warning);
      range.check(parsed_min);
      min = static_cast<uint32_t>(parsed_min);
    } else if (*i == "-tc" || *i == "--total-colors") {
      man_tot_color = true;
      auto val = parse_integral<int>(require_value(i, "total number of colors", "-tc <INT>"),
                                     "total number of colors");
      keep_in_range(1, val, 255);
      tot_color = static_cast<uint8_t>(val);
    } else if (*i == "-rt" || *i == "--reference-tick") {
      refTick = parse_integral<uint64_t>(require_value(i, "reference tick", "-rt <INT>"),
                                         "reference tick");
      ValRange<uint64_t> range(MIN_TICK, MAX_TICK, TICK, "Tick hop for reference", Interval::closed,
                               "default", Problem::warning);
      range.check(refTick);
    } else if (*i == "-tt" || *i == "--target-tick") {
      tarTick =
          parse_integral<uint64_t>(require_value(i, "target tick", "-tt <INT>"), "target tick");
      ValRange<uint64_t> range(MIN_TICK, MAX_TICK, TICK, "Tick hop for target", Interval::closed,
                               "default", Problem::warning);
      range.check(tarTick);
    } else if (*i == "-th" || *i == "--tick-human-readable") {
      tickHumanRead = (parse_integral<int>(require_value(i, "tick human readable", "-th <INT>"),
                                           "tick human readable") != 0);
    } else if (*i == "-c" || *i == "--color") {
      auto parsed_color =
          parse_integral<int>(require_value(i, "color mode", "-c <INT>"), "color mode");
      ValRange<int> range(MIN_COLOR, MAX_COLOR, COLOR, "Color", Interval::closed, "default",
                          Problem::warning);
      range.check(parsed_color);
      colorMode = static_cast<uint8_t>(parsed_color);
    } else if (*i == "-w" || *i == "--width") {
      auto parsed_width = parse_integral<uint64_t>(require_value(i, "width", "-w <INT>"), "width");
      ValRange<uint64_t> range(MIN_WDTH, MAX_WDTH, WDTH, "Width", Interval::closed, "default",
                               Problem::warning);
      range.check(parsed_width);
      width = static_cast<uint32_t>(parsed_width);
    } else if (*i == "-s" || *i == "--space") {
      auto parsed_space = parse_integral<uint64_t>(require_value(i, "space", "-s <INT>"), "space");
      ValRange<uint64_t> range(MIN_SPC, MAX_SPC, SPC, "Space", Interval::closed, "default",
                               Problem::warning);
      range.check(parsed_space);
      space = static_cast<uint32_t>(parsed_space);
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
      if (!posFile.empty()) {
        error("multiple position files specified for visualization.");
      }
      posFile = *i;
    } else {
      error(std::format("unknown option \"{}\".", *i));
    }
  }
  if (posFile.empty()) {
    error("position file not specified. Use \"smashpp viz <POS_FILE>\".");
  }
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
      << tab1 << "<POS_FILE>    position file generated by Smash++ (*.pos/*.json)\n"
      << '\n'
      << "    " << italic("Optional") << ":\n"
      << tab1 << bold("-o") << ", " << bold("--output") << " <SVG_FILE>\n"
      << tab2 << "output image name (*.svg). Default: map.svg\n"
      << '\n'
      << tab1 << bold("-rn") << ", " << bold("--reference-name") << " <STRING>\n"
      << tab2 << "reference name shown on output. If it has spaces, use\n"
      << tab2 << "double quotes, e.g. \"Seq label\". Default: the name in\n"
      << tab2 << "the header of position file\n"
      << '\n'
      << tab1 << bold("-tn") << ", " << bold("--target-name") << " <STRING>\n"
      << tab2 << "target name shown on output\n"
      << '\n'
      << tab1 << bold("-l") << ", " << bold("--link") << " <INT>\n"
      << tab2
      << std::format("type of the link between maps: {} to {}. Default: {}\n", MIN_LINK, MAX_LINK,
                     link)
      << '\n'
      << tab1 << bold("-c") << ", " << bold("--color") << " <INT>\n"
      << tab2
      << std::format("color mode: {{{}, {}}}. Default: {}\n", MIN_COLOR, MAX_COLOR, colorMode)
      << '\n'
      << tab1 << bold("-p") << ", " << bold("--opacity") << " <FLOAT>\n"
      << tab2
      << std::format("opacity: {:.1f} to {:.1f}. Default: {:.1f}\n", MIN_OPAC, MAX_OPAC, opacity)
      << '\n'
      << tab1 << bold("-w") << ", " << bold("--width") << " <INT>\n"
      << tab2
      << std::format("width of the sequence: {} to {}. Default: {}\n", MIN_WDTH, MAX_WDTH, width)
      << '\n'
      << tab1 << bold("-s") << ", " << bold("--space") << " <INT>\n"
      << tab2
      << std::format("space between sequences: {} to {}. Default: {}\n", MIN_SPC, MAX_SPC, space)
      << '\n'
      << tab1 << bold("-tc") << ", " << bold("--total-colors") << " <INT>\n"
      << tab2 << "total number of colors: 1 to 255\n"
      << '\n'
      << tab1 << bold("-rt") << ", " << bold("--reference-tick") << " <INT>\n"
      << tab2 << std::format("reference tick: {} to {}\n", MIN_TICK, MAX_TICK) << '\n'
      << tab1 << bold("-tt") << ", " << bold("--target-tick") << " <INT>\n"
      << tab2 << std::format("target tick: {} to {}\n", MIN_TICK, MAX_TICK) << '\n'
      << tab1 << bold("-th") << ", " << bold("--tick-human-readable") << " <INT>\n"
      << tab2 << "tick human readable: {0: false, 1: true}. Default: "
      << std::format("{}\n", static_cast<int>(tickHumanRead)) << '\n'
      << tab1 << bold("-m") << ", " << bold("--min-block-size") << " <INT>\n"
      << tab2 << std::format("minimum block size: {} to {}. Default: {}\n", MIN_MINP, MAX_MINP, min)
      << '\n'
      << tab1 << bold("-vv") << ", " << bold("--vertical-view") << '\n'
      << tab2 << "vertical view. Default: not used\n"
      << '\n'
      << tab1 << bold("-nrr") << ", " << bold("--no-relative-redundancy") << '\n'
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
