// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#include "filter.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <memory>
#include <numeric>

#include "file.hpp"
#include "naming.hpp"
#include "number.hpp"
#include "segment.hpp"
#include "string.hpp"
using namespace smashpp;

namespace {
class ProfileReader {
 public:
  explicit ProfileReader(std::span<const prc_t> profile) : profile_(profile) {}

  auto read(float& value) -> bool {
    if (pos_ >= profile_.size()) {
      return false;
    }

    value = static_cast<float>(profile_[pos_++]);
    return true;
  }

 private:
  std::span<const prc_t> profile_;
  size_t pos_{0};
};

auto make_segment(const Param& par, uint8_t round, uint64_t total_size)
    -> std::shared_ptr<Segment> {
  auto seg = std::make_shared<Segment>();
  seg->thresh = par.thresh;
  seg->minSize = par.segSize;
  seg->round = round;
  seg->sample_step = par.sampleStep;
  seg->totalSize = total_size;

  if (round == 2) {
    seg->set_guards(par.ref_guard.beg, par.ref_guard.end);
  } else if (round == 1 || round == 3) {
    seg->set_guards(par.tar_guard.beg, par.tar_guard.end);
  }

  return seg;
}
}  // namespace

Filter::Filter() : nSegs(0) {}

Filter::Filter(const Param& par) : nSegs(0), filt_type(par.filt_type) {
  set_filt_size(par);
  if (!par.quiet && (par.filter || par.segment) && par.verbose) {
    show_info(par);
  }
}

inline void Filter::set_filt_size(const Param& par) {
  if (par.manFilterScale) {
    switch (par.filterScale) {
      case FilterScale::s:
        filt_size = 45;
        break;
      case FilterScale::m:
        filt_size = 250;  // 499
        break;
      case FilterScale::l:
        filt_size = 1000;  // 1499
        break;
    }
  } else {
    filt_size = is_odd(par.filt_size) ? par.filt_size : par.filt_size + 1;
  }

  window.resize(filt_size);
}

inline void Filter::show_info(const Param& par) const {
  constexpr uint8_t lblWidth = 19;
  constexpr uint8_t colWidth = 8;
  constexpr uint8_t tblWidth = 60;  // static_cast<uint8_t>(lblWidth+Ms.size()*colWidth);

  const auto rule = [](uint8_t n, const std::string& s) {
    for (auto i = n / s.size(); i--;) {
      std::cerr << s;
    }
    std::cerr << '\n';
  };
  const auto toprule = [&]() { rule(tblWidth, "~"); };
  const auto midrule = [&]() { rule(tblWidth, "~"); };
  const auto botrule = [&]() { rule(tblWidth, " "); };
  const auto label = [=](std::string s) { std::cerr << std::setw(lblWidth) << std::left << s; };
  const auto header = [=](std::string s) {
    std::cerr << std::setw(2 * colWidth) << std::left << s;
  };
  const auto filter_vals = [&](char c) {
    std::cerr << std::setw(colWidth) << std::left;
    switch (c) {
      case 'f':
        std::cerr << par.print_win_type();
        break;
      case 's':
        std::cerr << par.print_filter_scale();
        break;
      case 'w':
        std::cerr << filt_size;
        break;
      case 't':
        std::cerr << par.thresh;
        break;
      default:
        break;
    }
    std::cerr << '\n';
  };
  const auto file_vals = [&](char c) {
    std::cerr << std::setw(2 * colWidth) << std::left;
    switch (c) {
      case '1':
        std::cerr << file_size(par.ref);
        break;
      case 'r':
        std::cerr << par.refName;
        break;
      case '2':
        std::cerr << file_size(par.tar);
        break;
      case 't':
        std::cerr << par.tarName;
        break;
      default:
        break;
    }
  };

  toprule();
  label("Filter & Segment");
  std::cerr << '\n';
  midrule();
  label("Window function");
  filter_vals('f');
  if (par.manFilterScale || !par.manWSize) {
    label("Filter scale");
    filter_vals('s');
  }
  label("Window size");
  filter_vals('w');
  if (par.manThresh) {
    label("Threshold");
    filter_vals('t');
  }
  botrule();  // cerr << '\n';

  toprule();
  label("Files");
  header("Size (B)");
  header("Name");
  std::cerr << '\n';
  midrule();
  label("Reference");
  file_vals('1');
  file_vals('r');
  std::cerr << '\n';
  label("Target");
  file_vals('2');
  file_vals('t');
  std::cerr << '\n';
  botrule();
}

void Filter::smooth_seg(std::vector<PosRow>& pos_out, std::span<const prc_t> profile, Param& par,
                        uint8_t round, uint64_t& current_pos_row) {
  if (!par.quiet && (par.verbose || round == 1)) {
    par.message = (round == 3) ? "    " : "";
    par.message += std::format("[+] Filtering {} ", italic(par.tarName));
  }

  if (profile.empty()) {
    error("profile is empty.");
  }

  if (window.size() != 1) {
    if (filt_type == FilterType::rectangular) {
      (par.saveFilter || par.saveAll) ? smooth_seg_rect<true>(pos_out, profile, par, round)
                                      : smooth_seg_rect<false>(pos_out, profile, par, round);
    } else {
      (par.saveFilter || par.saveAll) ? smooth_seg_non_rect<true>(pos_out, profile, par, round)
                                      : smooth_seg_non_rect<false>(pos_out, profile, par, round);
    }
  } else {
    smooth_seg_win1(pos_out, profile, par, round);
  }

  for (uint64_t i = current_pos_row, j = 0; i != current_pos_row + nSegs; ++i, ++j) {
    pos_out.at(i).round = round;
    pos_out.at(i).run_num = static_cast<uint8_t>(par.ID);
    pos_out.at(i).ref = par.ref;
    pos_out.at(i).tar = par.tar;
    pos_out.at(i).seg_num = j;
  }

  if (!par.saveAll && !par.saveProfile) {
    remove((gen_name(par.ID, par.refName, par.tarName, Format::profile)).c_str());
  }
  if (!par.saveAll && !par.saveFilter) {
    remove((gen_name(par.ID, par.refName, par.tarName, Format::filter)).c_str());
  }

  if (!par.quiet && (par.verbose || round == 1)) {
    std::cerr << "\r" << par.message << "done => " << nSegs << " segment" << (nSegs == 1 ? "" : "s")
              << '\n';
  }
}

void Filter::smooth_seg_win1(std::vector<PosRow>& pos_out, std::span<const prc_t> profile,
                             const Param& par, uint8_t round) {
  const auto filter_name{gen_name(par.ID, par.ref, par.tar, Format::filter)};
  std::ofstream filter_file;
  if (par.saveFilter || par.saveAll) {
    filter_file.open(filter_name);
  }

  ProfileReader profile_reader(profile);

  auto seg = make_segment(par, round, file_size(par.tar));
  auto filtered{0.f};
  uint64_t symsNo{0};

  for (; profile_reader.read(filtered);) {
    if (par.saveFilter || par.saveAll) {
      filter_file << precision(PREC_FIL, filtered) << '\n';
    }
    seg->partition(pos_out, filtered);
    ++seg->pos;
    if (!par.quiet && par.verbose) {
      show_progress(++symsNo, seg->totalSize, par.message);
    }
  }

  if (par.saveFilter || par.saveAll) {
    filter_file.close();
  }
  seg->finalize_partition(pos_out);
  nSegs = seg->nSegs;
}

inline void Filter::make_window(uint32_t filter_size) {
  window.resize(filter_size);

  switch (filt_type) {
    case FilterType::hamming:
      make_hamming(filter_size);
      break;
    case FilterType::hann:
      make_hann(filter_size);
      break;
    case FilterType::blackman:
      make_blackman(filter_size);
      break;
    case FilterType::triangular:
      make_triangular(filter_size);
      break;
    case FilterType::welch:
      make_welch(filter_size);
      break;
    case FilterType::sine:
      make_sine(filter_size);
      break;
    case FilterType::nuttall:
      make_nuttall(filter_size);
      break;
    default:
      break;
  }
}

inline void Filter::make_hamming(uint32_t filter_size) {
  if (filter_size == 1) {
    error("The size of Hamming window must be greater than 1.");
  }
  float num{0.f};
  uint32_t den{0};
  if (is_odd(filter_size)) {
    num = PI;
    den = (filter_size - 1) >> 1u;
  } else {
    num = 2 * PI;
    den = filter_size - 1;
  }

  for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;) {
    window[n] = window[last - n] = static_cast<float>(0.54 - 0.46 * std::cos(n * num / den));
  }
}

inline void Filter::make_hann(uint32_t filter_size) {
  if (filter_size == 1) {
    error("The size of Hann window must be greater than 1.");
  }
  float num{0.f};
  uint32_t den{0};
  if (is_odd(filter_size)) {
    num = PI;
    den = (filter_size - 1) >> 1u;
  } else {
    num = 2 * PI;
    den = filter_size - 1;
  }

  for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;) {
    window[n] = window[last - n] = static_cast<float>(0.5 * (1 - std::cos(n * num / den)));
  }
}

inline void Filter::make_blackman(uint32_t filter_size) {
  if (filter_size == 1) {
    error("The size of Blackman window must be greater than 1.");
  }
  float num1{0.f};
  float num2{0.f};
  uint32_t den{0};
  if (is_odd(filter_size)) {
    num1 = PI;
    num2 = 2 * PI;
    den = (filter_size - 1) >> 1u;
  } else {
    num1 = 2 * PI;
    num2 = 4 * PI;
    den = filter_size - 1;
  }

  for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;) {
    window[n] = window[last - n] =
        static_cast<float>(0.42 - 0.5 * std::cos(n * num1 / den) + 0.08 * std::cos(n * num2 / den));
  }
  if (window.front() < 0) {  // Because of low precision
    window.front() = 0.0;
  }
  if (window.back() < 0) {  // Because of low precision
    window.back() = 0.0;
  }
}

// Bartlett window:  w(n) = 1 - |(n - (N-1)/2) / (N-1)/2|
inline void Filter::make_triangular(uint32_t filter_size) {
  if (filter_size == 1) {
    error("The size of triangular window must be greater than 1.");
  }

  if (is_odd(filter_size)) {
    const uint32_t den{(filter_size - 1) >> 1u};
    for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;) {
      window[n] = window[last - n] = 1 - std::fabs(static_cast<float>(n) / den - 1);
    }
  } else {
    constexpr auto num{2.0f};
    const uint32_t den{filter_size - 1};
    for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;) {
      window[n] = window[last - n] = 1 - std::fabs(n * num / den - 1);
    }
  }
}

inline void Filter::make_welch(uint32_t filter_size) {  // w(n) = 1 - ((n - (N-1)/2) / (N-1)/2)^2
  if (filter_size == 1) {
    error("The size of Welch window must be greater than 1.");
  }

  if (is_odd(filter_size)) {
    const uint32_t den{(filter_size - 1) >> 1u};
    for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;) {
      window[n] = window[last - n] = float(1 - Power(float(n) / den - 1, 2));
    }
  } else {
    constexpr auto num{2.0f};
    const uint32_t den{filter_size - 1};
    for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;) {
      window[n] = window[last - n] = float(1 - Power(n * num / den - 1, 2));
    }
  }
}

inline void Filter::make_sine(uint32_t filter_size) {
  if (filter_size == 1) {
    error("The size of sine window must be greater than 1.");
  }
  constexpr float num{PI};
  const uint32_t den{filter_size - 1};

  for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;) {
    window[n] = window[last - n] = std::sin(n * num / den);
  }
}

inline void Filter::make_nuttall(uint32_t filter_size) {
  if (filter_size == 1) {
    error("The size of Nuttall window must be greater than 1.");
  }
  float num1{0.f};
  float num2{0.f};
  float num3{0.f};
  uint32_t den{0};
  if (is_odd(filter_size)) {
    num1 = PI;
    num2 = 2 * PI;
    num3 = 3 * PI;
    den = (filter_size - 1) >> 1u;
  } else {
    num1 = 2 * PI;
    num2 = 4 * PI;
    num3 = 6 * PI;
    den = filter_size - 1;
  }

  for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;) {
    window[n] = window[last - n] =
        static_cast<float>(0.36 - 0.49 * std::cos(n * num1 / den) + 0.14 * cos(n * num2 / den) -
                           0.01 * std::cos(n * num3 / den));
  }
  window.front() = window.back() = 0.0;
}

template <bool SaveFilter>
inline void Filter::smooth_seg_rect(std::vector<PosRow>& pos_out, std::span<const prc_t> profile,
                                    const Param& par, uint8_t round) {
  const auto filterName{gen_name(par.ID, par.ref, par.tar, Format::filter)};
  std::ofstream filF;
  if constexpr (SaveFilter) {
    filF.open(filterName);
  }

  ProfileReader profile_reader(profile);
  auto seg = make_segment(par, round, file_size(par.tar));

  std::vector<float> seq;
  seq.reserve(filt_size);
  auto entropy{0.f};
  uint64_t symsNo{0};  // No. syms based on profile
  auto sum{0.f};

  // First value
  {
    auto i = (filt_size >> 1u) + 1;
    for (; i-- && profile_reader.read(entropy);) {
      seq.push_back(entropy);
      sum += entropy;
    }
    auto num_ent_exist = (filt_size >> 1u) + 1 - i;
    seq.insert(std::begin(seq), num_ent_exist - 1, 2.0f);
    sum += static_cast<float>(num_ent_exist - 1) * 2.0f;
  }

  filt_size = static_cast<uint32_t>(seq.size());
  const auto half_wsize{filt_size >> 1u};

  auto filtered = sum / filt_size;
  if (SaveFilter) {
    filF << precision(PREC_FIL, filtered) << '\n';
  }
  seg->partition(pos_out, filtered);
  if (!par.quiet && par.verbose) {
    show_progress(++symsNo, seg->totalSize, par.message);
  }

  // The rest
  uint32_t idx{0};
  for (; profile_reader.read(entropy);) {
    sum += entropy - seq[idx];
    filtered = sum / filt_size;
    if (SaveFilter) {
      filF << precision(PREC_FIL, filtered) << '\n';
    }
    ++seg->pos;
    seg->partition(pos_out, filtered);
    seq[idx] = entropy;
    idx = (idx + 1) % filt_size;
    if (!par.quiet && par.verbose) {
      show_progress(++symsNo, seg->totalSize, par.message);
    }
  }
  // Until half of the window goes outside the array
  for (auto i = 1u; i != half_wsize + 1; ++i) {
    sum -= seq[idx];
    filtered = sum / filt_size;
    if (SaveFilter) {
      filF << precision(PREC_FIL, filtered) << '\n';
    }
    ++seg->pos;
    seg->partition(pos_out, filtered);
    idx = (idx + 1) % filt_size;
    if (!par.quiet && par.verbose) {
      show_progress(++symsNo, seg->totalSize, par.message);
    }
  }
  seg->finalize_partition(pos_out);
  if (!par.quiet && par.verbose) {
    show_progress(++symsNo, seg->totalSize, par.message);
  }

  if constexpr (SaveFilter) {
    filF.close();
  }
  nSegs = seg->nSegs;
}

template <bool SaveFilter>
inline void Filter::smooth_seg_non_rect(std::vector<PosRow>& pos_out,
                                        std::span<const prc_t> profile, const Param& par,
                                        uint8_t round) {
  const auto filterName{gen_name(par.ID, par.ref, par.tar, Format::filter)};
  std::ofstream filF;
  if constexpr (SaveFilter) {
    filF.open(filterName);
  }

  ProfileReader profile_reader(profile);
  auto seg = make_segment(par, round, file_size(par.tar));
  std::vector<float> filtered_values;
  filtered_values.reserve(FILE_WRITE_BUF);
  auto write_filtered_values = [&]() {
    for (auto e : filtered_values) {
      filF << precision(PREC_FIL, e) << "\n";
    }
  };

  std::vector<float> seq;
  seq.reserve(filt_size);
  auto entropy{0.f};
  uint64_t symsNo{0};  // No. syms based on profile

  // First value
  {
    auto i = (filt_size >> 1u) + 1;
    for (; i-- && profile_reader.read(entropy);) {
      seq.push_back(entropy);
    }
    auto num_ent_exist = (filt_size >> 1u) + 1 - i;
    seq.insert(std::begin(seq), num_ent_exist - 1, 2.0f);
  }

  make_window(static_cast<uint32_t>(seq.size()));

  const auto winBeg{std::begin(window)};
  const auto winEnd{std::end(window)};
  const auto half_wsize = (window.size() >> 1u);
  auto sum_weights{std::accumulate(winBeg, winEnd, 0.f)};
  const auto seq_size = seq.size();
  std::vector<float> linear_seq;
  linear_seq.reserve(seq_size + filt_size);
  while (linear_seq.size() < seq_size + filt_size) {
    linear_seq.insert(std::end(linear_seq), std::begin(seq), std::end(seq));
  }
  const auto update_seq = [&](uint32_t pos, float value) {
    seq[pos] = value;
    for (auto linear_pos = static_cast<size_t>(pos); linear_pos < linear_seq.size();
         linear_pos += seq_size) {
      linear_seq[linear_pos] = value;
    }
  };
  const auto filtered_at = [&](uint32_t pos) {
    const auto sum = std::inner_product(winBeg, winEnd, std::begin(linear_seq) + pos, 0.f);
    return sum / sum_weights;
  };

  auto filtered = filtered_at(0);
  // if (SaveFilter) filF << precision(PREC_FIL, filtered) << '\n';
  if (SaveFilter) {
    filtered_values.push_back(filtered);
  }
  seg->partition(pos_out, filtered);
  if (!par.quiet && par.verbose) {
    show_progress(++symsNo, seg->totalSize, par.message);
  }

  // The rest
  uint32_t idx{0};
  for (; profile_reader.read(entropy);) {
    update_seq(idx, entropy);
    idx = (idx + 1) % filt_size;
    filtered = filtered_at(idx);
    if (SaveFilter) {
      filtered_values.push_back(filtered);
      if (filtered_values.size() >= FILE_WRITE_BUF) {
        write_filtered_values();
        filtered_values.clear();
        filtered_values.reserve(FILE_WRITE_BUF);
      }
    }
    ++seg->pos;
    seg->partition(pos_out, filtered);
    if (!par.quiet && par.verbose) {
      show_progress(++symsNo, seg->totalSize, par.message);
    }
  }
  // Until half of the window goes outside the array
  for (auto i = half_wsize; i--;) {
    update_seq(idx, 2.0f);
    idx = (idx + 1) % filt_size;
    filtered = filtered_at(idx);
    if (SaveFilter) {
      filtered_values.push_back(filtered);
      if (filtered_values.size() >= FILE_WRITE_BUF) {
        write_filtered_values();
        filtered_values.clear();
        filtered_values.reserve(FILE_WRITE_BUF);
      }
    }
    ++seg->pos;
    seg->partition(pos_out, filtered);
    if (!par.quiet && par.verbose) {
      show_progress(++symsNo, seg->totalSize, par.message);
    }
  }
  seg->finalize_partition(pos_out);
  if constexpr (SaveFilter) {
    write_filtered_values();
  }
  if (!par.quiet && par.verbose) {
    show_progress(++symsNo, seg->totalSize, par.message);
  }

  if constexpr (SaveFilter) {
    filF.close();
  }
  nSegs = seg->nSegs;
}

auto Filter::extract_seg(std::vector<PosRow>& pos_out, uint8_t round, uint8_t run_num,
                         const std::string& ref, bool write_files) const
    -> std::vector<SegmentView> {
  std::vector<SegmentView> segments;
  uint64_t seg_idx{0};

  for (const auto& row : pos_out) {
    if (row.round == round && row.run_num == run_num && row.ref == ref) {
      const uint64_t max_tar_pos{file_size(row.tar) - 1};
      const auto end_pos = std::min(row.end_pos, max_tar_pos);
      if (row.beg_pos > end_pos) {
        continue;
      }

      const auto size = end_pos - row.beg_pos + 1;
      segments.push_back({row.tar, row.beg_pos, size});

      if (write_files) {
        SubSeq subseq;
        subseq.inName = row.tar;
        const auto seg{gen_name(row.run_num, row.ref, row.tar, Format::segment)};
        subseq.outName = std::format("{}{}", seg, seg_idx);
        subseq.begPos = row.beg_pos;
        subseq.size = static_cast<std::streamsize>(size);
        extract_subseq(subseq);
      }
      ++seg_idx;
    }
  }

  return segments;
}

#ifdef BENCH
template <typename Iter, typename Value>
inline void Filter::shift_left_insert(Iter first, Value v) {
  std::copy(first + 1, first + filt_size, first);
  *(first + filt_size - 1) = v;
}
#endif
