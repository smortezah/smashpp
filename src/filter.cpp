// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.
#include <cmath>
#include <numeric>
#include "number.hpp"
#include "filter.hpp"
#include "segment.hpp"
#include "string.hpp"
#include "file.hpp"
#include "naming.hpp"
using namespace smashpp;

Filter::Filter() : nSegs(0) {}

Filter::Filter(std::unique_ptr<Param>& par) : nSegs(0), filt_type(par->filt_type) {
  set_filt_size(par);
  if ((par->filter || par->segment) && par->verbose) show_info(par);
}

inline void Filter::set_filt_size(std::unique_ptr<Param>& par) {
  if (par->manFilterScale) {
    // const auto biggest = std::min(file_size(par->tar), file_size(par->ref));
    // const auto lg = std::log10(biggest / par->sampleStep);
    switch (par->filterScale) {
      case FilterScale::s:
        filt_size = 45;
        // filt_size = std::pow(2, 2 * lg) + 1;
        break;
      case FilterScale::m:
        filt_size = 499;
        // filt_size = std::pow(2, 2 * lg + 1) + 1;
        break;
      case FilterScale::l:
        filt_size = 1499;
        // filt_size = std::pow(2, 2 * lg + 2) + 1;
        break;
    }
  } else {
    filt_size = is_odd(par->filt_size) ? par->filt_size : par->filt_size + 1;
  }

  window.resize(filt_size);
}

inline void Filter::show_info(std::unique_ptr<Param>& par) const {
  constexpr uint8_t lblWidth = 19;
  constexpr uint8_t colWidth = 8;
  constexpr uint8_t tblWidth =
      60;  // static_cast<uint8_t>(lblWidth+Ms.size()*colWidth);

  const auto rule = [](uint8_t n, const std::string& s) {
    for (auto i = n / s.size(); i--;) std::cerr << s;
    std::cerr << '\n';
  };
  const auto toprule = [&]() { rule(tblWidth, "~"); };
  const auto midrule = [&]() { rule(tblWidth, "~"); };
  const auto botrule = [&]() { rule(tblWidth, " "); };
  const auto label = [=](std::string s) {
    std::cerr << std::setw(lblWidth) << std::left << s;
  };
  const auto header = [=](std::string s) {
    std::cerr << std::setw(2 * colWidth) << std::left << s;
  };
  const auto filter_vals = [&](char c) {
    std::cerr << std::setw(colWidth) << std::left;
    switch (c) {
      case 'f':
        std::cerr << par->print_win_type();
        break;
      case 's':
        std::cerr << par->print_filter_scale();
        break;
      case 'w':
        std::cerr << filt_size;
        break;
      case 't':
        std::cerr << par->thresh;
        break;
      default:
        break;
    }
    std::cerr << '\n';
  };
  const auto file_vals = [&](char c) {
    std::cerr << std::setw(2 * colWidth) << std::left;
    // const auto lacale = "en_US.UTF8";
    // std::setlocale(LC_ALL, "en_US.UTF-8");
    switch (c) {
      case '1':
        // std::cerr.imbue(std::locale(lacale));
        std::cerr << file_size(par->ref);
        break;
      // case 'r':  cerr<<par->ref; break;
      case 'r':
        std::cerr << par->refName;
        break;
      case '2':
        // std::cerr.imbue(std::locale(lacale));
        std::cerr << file_size(par->tar);
        break;
      // case 't':  cerr<<par->tar; break;
      case 't':
        std::cerr << par->tarName;
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
  if (par->manFilterScale || !par->manWSize) {
    label("Filter scale");
    filter_vals('s');
  }
  label("Window size");
  filter_vals('w');
  if (par->manThresh) {
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

void Filter::smooth_seg(std::vector<PosRow>& pos_out,
                        std::unique_ptr<Param>& par, uint8_t round,
                        uint64_t& current_pos_row) {
  if (par->verbose || round == 1) {
    par->message = (round == 3) ? "    " : "";
    par->message += "[+] Filtering " + italic(par->tarName) + " ";
  }

  if (window.size() != 1) {
    if (filt_type == FilterType::rectangular) {
      (par->saveFilter || par->saveAll)
          ? smooth_seg_rect<true>(pos_out, par, round)
          : smooth_seg_rect<false>(pos_out, par, round);
    } else {
      // make_window(filt_size);
      (par->saveFilter || par->saveAll)
          ? smooth_seg_non_rect<true>(pos_out, par, round)
          : smooth_seg_non_rect<false>(pos_out, par, round);  // todo erroneous
    }
  } else {
    smooth_seg_win1(pos_out, par, round);
  }

  for (uint64_t i = current_pos_row, j = 0; i != current_pos_row + nSegs;
       ++i, ++j) {
    pos_out.at(i).round = round;
    pos_out.at(i).run_num = par->ID;
    pos_out.at(i).ref = par->ref;
    pos_out.at(i).tar = par->tar;
    pos_out.at(i).seg_num = j;
  }

  if (!par->saveAll && !par->saveProfile)
    remove((gen_name(par->ID, par->refName, par->tarName, Format::profile))
               .c_str());
  if (!par->saveAll && !par->saveFilter)
    remove((gen_name(par->ID, par->refName, par->tarName, Format::filter))
               .c_str());

  if (par->verbose || round == 1) {
    std::cerr << "\r" << par->message << "done => " << nSegs << " segment"
              << (nSegs == 1 ? "" : "s") << '\n';
  }
}

void Filter::smooth_seg_win1(std::vector<PosRow>& pos_out,
                             std::unique_ptr<Param>& par, uint8_t round) {
  const auto profile_name{
      gen_name(par->ID, par->ref, par->tar, Format::profile)};
  check_file(profile_name);
  std::ifstream profile(profile_name);
  const auto filter_name{gen_name(par->ID, par->ref, par->tar, Format::filter)};
  std::ofstream filter_file(filter_name);

  const auto jump_lines = [&]() {
    for (uint64_t i = par->sampleStep; i--;) ignore_this_line(profile);
  };

  auto seg = std::make_shared<Segment>();
  seg->thresh = par->thresh;
  seg->minSize = par->segSize;
  seg->round = round;
  seg->sample_step = par->sampleStep;

  uint8_t maxCtx = 0;
  for (const auto& e : par->refMs)
    if (e.k > maxCtx) maxCtx = e.k;

  if (round == 2)
    seg->set_guards(maxCtx, par->ref_guard->beg, par->ref_guard->end);
  else if (round == 1 || round == 3)
    seg->set_guards(maxCtx, par->tar_guard->beg, par->tar_guard->end);

  // seg->totalSize = file_lines(profile_name) / par->sampleStep;
  seg->totalSize = file_lines(profile_name);//todo
  auto filtered{0.f};
  uint64_t symsNo{0};

  for (; profile >> filtered; jump_lines()) {
    if (par->saveFilter || par->saveAll) filter_file << precision(PREC_FIL) << filtered << '\n';
    seg->partition(pos_out, filtered);
    if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  }

  filter_file.close();
  profile.close();
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
  if (filter_size == 1) error("The size of Hamming window must be greater than 1.");
  float num{0.f};
  uint32_t den{0};
  if (is_odd(filter_size)) {
    num = PI;
    den = (filter_size - 1) >> 1u;
  } else {
    num = 2 * PI;
    den = filter_size - 1;
  }

  for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;)
    window[n] = window[last - n] =
        static_cast<float>(0.54 - 0.46 * std::cos(n * num / den));
}

inline void Filter::make_hann(uint32_t filter_size) {
  if (filter_size == 1) error("The size of Hann window must be greater than 1.");
  float num{0.f};
  uint32_t den{0};
  if (is_odd(filter_size)) {
    num = PI;
    den = (filter_size - 1) >> 1u;
  } else {
    num = 2 * PI;
    den = filter_size - 1;
  }

  for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;)
    window[n] = window[last - n] =
        static_cast<float>(0.5 * (1 - std::cos(n * num / den)));
}

inline void Filter::make_blackman(uint32_t filter_size) {
  if (filter_size == 1) error("The size of Blackman window must be greater than 1.");
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

  for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;)
    window[n] = window[last - n] =
        static_cast<float>(0.42 - 0.5 * std::cos(n * num1 / den) +
                           0.08 * std::cos(n * num2 / den));
  if (window.front() < 0) window.front() = 0.0;  // Because of low precision
  if (window.back() < 0) window.back() = 0.0;    // Because of low precision
}

// Bartlett window:  w(n) = 1 - |(n - (N-1)/2) / (N-1)/2|
inline void Filter::make_triangular(uint32_t filter_size) {
  if (filter_size == 1)
    error("The size of triangular window must be greater than 1.");

  if (is_odd(filter_size)) {
    const uint32_t den{(filter_size - 1) >> 1u};
    for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;)
      window[n] = window[last - n] =
          1 - std::fabs(static_cast<float>(n) / den - 1);
  } else {
    constexpr auto num{2.0f};
    const uint32_t den{filter_size - 1};
    for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;)
      window[n] = window[last - n] = 1 - std::fabs(n * num / den - 1);
  }
}

inline void Filter::make_welch(uint32_t filter_size) {  // w(n) = 1 - ((n - (N-1)/2) / (N-1)/2)^2
  if (filter_size == 1) error("The size of Welch window must be greater than 1.");

  if (is_odd(filter_size)) {
    const uint32_t den{(filter_size - 1) >> 1u};
    for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;)
      window[n] = window[last - n] = float(1 - Power(float(n) / den - 1, 2));
  } else {
    constexpr auto num{2.0f};
    const uint32_t den{filter_size - 1};
    for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;)
      window[n] = window[last - n] = float(1 - Power(n * num / den - 1, 2));
  }
}

inline void Filter::make_sine(uint32_t filter_size) {
  if (filter_size == 1) error("The size of sine window must be greater than 1.");
  constexpr float num{PI};
  const uint32_t den{filter_size - 1};

  for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;)
    window[n] = window[last - n] = std::sin(n * num / den);
}

inline void Filter::make_nuttall(uint32_t filter_size) {
  if (filter_size == 1) error("The size of Nuttall window must be greater than 1.");
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

  for (auto n = (filter_size + 1) >> 1u, last = filter_size - 1; n--;)
    window[n] = window[last - n] = static_cast<float>(
        0.36 - 0.49 * std::cos(n * num1 / den) + 0.14 * cos(n * num2 / den) -
        0.01 * std::cos(n * num3 / den));
  window.front() = window.back() = 0.0;
}

template <bool SaveFilter>
inline void Filter::smooth_seg_rect(std::vector<PosRow>& pos_out,
                                    std::unique_ptr<Param>& par,
                                    uint8_t round) {
  // const auto profile_name{
  //     gen_name(par->ID, par->ref, par->tar, Format::profile)};
  // check_file(profile_name);
  // std::ifstream prfF(profile_name);
  // const auto filter_name{gen_name(par->ID, par->ref, par->tar, Format::filter)};
  // std::ofstream filF(filter_name);

  // auto seg = std::make_shared<Segment>();
  // seg->thresh = par->thresh;
  // seg->minSize = par->segSize;
  // {
  //   uint8_t maxCtx = 0;
  //   for (const auto& e : par->refMs)
  //     if (e.k > maxCtx) maxCtx = e.k;

  //   if (round == 2)
  //     seg->set_guards(maxCtx, par->ref_guard->beg, par->ref_guard->end);
  //   else if (round == 1 || round == 3)
  //     seg->set_guards(maxCtx, par->tar_guard->beg, par->tar_guard->end);
  // }
  // seg->totalSize = file_lines(profile_name) / par->sampleStep;
  // // const auto totalSize = (file_lines(profile_name) / par->sampleStep) + 1;

  // const auto sum_win_weights{filt_size};
  // uint64_t symsNo{0};

  // const auto buff_size = 256;  // todo
  // const auto half_wsize = (filt_size >> 1u);

  // std::vector<float> seq(half_wsize, 0);
  // seq.reserve(filt_size + buff_size);  // Essential

  // const auto jump_lines = [&]() {
  //   for (auto i = par->sampleStep; i--;) ignore_this_line(prfF);
  // };
  // float entropy;
  // for (auto i = half_wsize + 1; i-- && (prfF >> entropy);) {
  //   seq.push_back(entropy);
  //   jump_lines();
  // }

  // std::vector<float>::iterator data_beg;
  // uint64_t running_times;
  // auto filtered{0.f};

  // if (seq.size() < filt_size || prfF.peek() == EOF) {
  //   data_beg = std::begin(seq);
  //   running_times = seq.size() - half_wsize;
  // } else {
  //   do {
  //     data_beg = std::begin(seq);
  //     filtered =
  //         std::accumulate(data_beg, data_beg + filt_size, 0.f) / sum_win_weights;

  //     for (auto i = buff_size-filt_size; i-- && (prfF >> entropy); ++data_beg) {
  //       if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  //       seg->partition(pos_out, filtered);
  //       if (par->verbose) show_progress(++symsNo, seg->totalSize, message);

  //       seq.push_back(entropy);
  //       jump_lines();

  //       filtered += (entropy - *data_beg) / sum_win_weights;
  //       ++seg->pos;
  //     }

  //     seq.erase(std::begin(seq), data_beg);  // todo ~ slow
  //   } while (prfF);

  //   data_beg = std::begin(seq) + 1;
  //   running_times = half_wsize;
  // }

  // seq.resize(seq.size() + half_wsize);  // Append half_wsize zeros
  // filtered = std::accumulate(data_beg, data_beg + filt_size, 0.f) / sum_win_weights;

  // for (auto i = running_times; i--;) {
  //   if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  //   seg->partition(pos_out, filtered);
  //   if (par->verbose) show_progress(++symsNo, seg->totalSize, message);
  //   filtered -= *data_beg / sum_win_weights;
  //   ++data_beg;
  //   ++seg->pos;
  // }

  // seg->finalize_partition(pos_out);
  // if (par->verbose) show_progress(++symsNo, seg->totalSize, message);

  // prfF.close();
  // filF.close();
  // if (!SaveFilter) remove(filter_name.c_str());
  // nSegs = seg->nSegs;

  const auto profileName{
      gen_name(par->ID, par->ref, par->tar, Format::profile)};
  const auto filterName{gen_name(par->ID, par->ref, par->tar, Format::filter)};
  check_file(profileName);
  std::ifstream prfF(profileName);
  std::ofstream filF(filterName);
  auto seg = std::make_shared<Segment>();
  seg->thresh = par->thresh;
  seg->minSize = par->segSize;
  seg->round = round;
  seg->sample_step = par->sampleStep;
  {
    uint8_t maxCtx = 0;
    for (const auto& e : par->refMs)
      if (e.k > maxCtx) maxCtx = e.k;

    if (round == 2)
      seg->set_guards(maxCtx, par->ref_guard->beg, par->ref_guard->end);
    else if (round == 1 || round == 3)
      seg->set_guards(maxCtx, par->tar_guard->beg, par->tar_guard->end);
  }

  // seg->totalSize = file_lines(profileName) / par->sampleStep;
  seg->totalSize = file_lines(profileName); //todo
  // const auto totalSize = (file_lines(profileName) / par->sampleStep) + 1;
  const auto jump_lines = [&]() {
    for (uint64_t i = par->sampleStep; i--;) ignore_this_line(prfF);
  };

  auto sum{0.f};
  uint64_t symsNo{0};  // No. syms based on profile
  auto entropy{0.f};
  std::vector<float> seq;
  seq.reserve(filt_size);

  // First value
  // for (auto i = half_wsize + 1; i-- && (prfF >> entropy);jump_lines()) {
  for (auto i = (filt_size >> 1u) + 1; i-- && (prfF >> entropy);jump_lines()) {
    const auto val{entropy};
    seq.push_back(val);
    sum += val;
    if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  }

  if (seq.size() <= (filt_size >> 1u)) filt_size = 2 * seq.size() + 1;
  const auto half_wsize{filt_size >> 1u};

  auto filtered = sum / filt_size;
  // auto filtered = sum / seq.size();
  if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  seg->partition(pos_out, filtered);

  // Next half_wsize values
  for (auto i = half_wsize; i-- && (prfF >> entropy);jump_lines()) {
    const auto val{entropy};
    seq.push_back(val);
    sum += val;
    filtered = sum / filt_size;
    // filtered = sum / seq.size();
    if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
    ++seg->pos;
    seg->partition(pos_out, filtered);
    if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  }

  // The rest
  uint32_t idx{0};
  for (; prfF >> entropy;jump_lines()) {
    const auto val = entropy;
    sum += val - seq[idx];
    filtered = sum / filt_size;
    if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
    ++seg->pos;
    seg->partition(pos_out, filtered);
    seq[idx] = val;
    idx = (idx + 1) % filt_size;   
    if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  }
  prfF.close();

  // Until half of the window goes outside the array
  for (auto i = 1u; i != half_wsize + 1; ++i) {
    sum -= seq[idx];
    filtered = sum / filt_size;
    // filtered = sum / (filt_size - i);
    if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
    ++seg->pos;
    seg->partition(pos_out, filtered);
    idx = (idx + 1) % filt_size;
    if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  }

  seg->finalize_partition(pos_out);
  if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);

  filF.close();
  if (!SaveFilter) remove(filterName.c_str());
  nSegs = seg->nSegs;
}

template <bool SaveFilter>
inline void Filter::smooth_seg_non_rect(std::vector<PosRow>& pos_out,
                                        std::unique_ptr<Param>& par,
                                        uint8_t round) {
  // const auto profile_name{
  //     gen_name(par->ID, par->ref, par->tar, Format::profile)};
  // check_file(profile_name);
  // std::ifstream prfF(profile_name);
  // const auto filter_name{gen_name(par->ID, par->ref, par->tar, Format::filter)};
  // std::ofstream filF(filter_name);

  // auto seg = std::make_shared<Segment>();
  // seg->thresh = par->thresh;
  // seg->minSize = par->segSize;
  // {
  //   uint8_t maxCtx = 0;
  //   for (const auto& e : par->refMs)
  //     if (e.k > maxCtx) maxCtx = e.k;

  //   if (round == 2)
  //     seg->set_guards(maxCtx, par->ref_guard->beg, par->ref_guard->end);
  //   else if (round == 1 || round == 3)
  //     seg->set_guards(maxCtx, par->tar_guard->beg, par->tar_guard->end);
  // }
  // seg->totalSize = file_lines(profile_name) / par->sampleStep;
  // // const auto totalSize = (file_lines(profile_name) / par->sampleStep) + 1;

  // const auto win_beg = std::begin(window);
  // const auto win_end = std::end(window);
  // const auto sum_win_weights{std::accumulate(win_beg, win_end, 0.f)};
  // uint64_t symsNo{0};

  // // const auto buff_size = 256;  // todo
  // uint32_t buff_size;
  // if (filt_size < 256)
  //   buff_size = 256;
  // else if (filt_size < 512)
  //   buff_size = 512;
  // else if (filt_size < 1024)
  //   buff_size = 1024;
  // else if (filt_size < 2048)
  //   buff_size = 2048;

  // const auto half_wsize = (filt_size >> 1u);

  // std::vector<float> seq(half_wsize, 0);
  // seq.reserve(filt_size + buff_size);  // Essential

  // const auto jump_lines = [&]() {
  //   for (auto i = par->sampleStep; i--;) ignore_this_line(prfF);
  // };
  // float entropy;
  // for (auto i = half_wsize + 1; i-- && (prfF >> entropy);) {
  //   seq.push_back(entropy);
  //   jump_lines();
  // }

  // std::vector<float>::iterator data_beg;
  // uint64_t running_times;
  // auto filtered{0.f};

  // if (seq.size() < filt_size || prfF.peek() == EOF) {
  //   data_beg = std::begin(seq);
  //   running_times = seq.size() - half_wsize;
  // } else {

  //     //todo
  //     // for (std::vector<float> buffer(buff_size, 0); prfF.peek() != EOF;) {
  //     //   prfF.read(seq.data(), buff_size - half_wsize - 1);
  //     //   for (auto it = std::begin(buffer);
  //     //        it != std::begin(buffer) + prfF.gcount(); ++it) {
  //     //   }
  //     // }

  //   do {
  //     data_beg = std::begin(seq);

  //     for (auto i = buff_size - filt_size; i-- && (prfF >> entropy); ++data_beg) {
  //       filtered =
  //           std::inner_product(data_beg, data_beg + filt_size, win_beg, 0.f) /
  //           sum_win_weights;
  //       if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  //       seg->partition(pos_out, filtered);
  //       if (par->verbose) show_progress(++symsNo, seg->totalSize, message);

  //       seq.push_back(entropy);
  //       jump_lines();

  //       ++seg->pos;
  //     }

  //     seq.erase(std::begin(seq), data_beg);  // todo ~ slow
  //   } while (prfF.peek() != EOF);

  //   data_beg = std::begin(seq) + 1;
  //   running_times = half_wsize;
  // }

  // seq.resize(seq.size() + half_wsize);  // Append half_wsize zeros

  // for (auto i = running_times; i--;) {
  //   filtered = std::inner_product(data_beg, data_beg + filt_size, win_beg, 0.f) /
  //              sum_win_weights;
  //   if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  //   seg->partition(pos_out, filtered);
  //   if (par->verbose) show_progress(++symsNo, seg->totalSize, message);

  //   ++data_beg;
  //   ++seg->pos;
  // }

  // seg->finalize_partition(pos_out);
  // if (par->verbose) show_progress(++symsNo, seg->totalSize, message);

  // prfF.close();
  // filF.close();
  // if (!SaveFilter) remove(filter_name.c_str());
  // nSegs = seg->nSegs;

  const auto profileName{
      gen_name(par->ID, par->ref, par->tar, Format::profile)};
  const auto filterName{gen_name(par->ID, par->ref, par->tar, Format::filter)};
  check_file(profileName);
  std::ifstream prfF(profileName);
  std::ofstream filF(filterName);
  auto seg = std::make_shared<Segment>();
  seg->thresh = par->thresh;
  seg->minSize = par->segSize;
  seg->round = round;
  seg->sample_step = par->sampleStep;
  {
    uint8_t maxCtx = 0;
    for (const auto& e : par->refMs)
      if (e.k > maxCtx) maxCtx = e.k;

    if (round == 2)
      seg->set_guards(maxCtx, par->ref_guard->beg, par->ref_guard->end);
    else if (round == 1 || round == 3)
      seg->set_guards(maxCtx, par->tar_guard->beg, par->tar_guard->end);
  }
  // const auto winBeg{std::begin(window)};
  // const auto winEnd{std::end(window)};
  // const auto half_wsize = (filt_size >> 1u);
  // // auto sWeight{std::accumulate(winBeg + half_wsize, winEnd, 0.f)};
  // auto sum_weights{std::accumulate(winBeg, winEnd, 0.f)};
  
  // seg->totalSize = file_lines(profileName) / par->sampleStep;
  seg->totalSize = file_lines(profileName);//todo
  const auto jump_lines = [&]() {
    for (uint64_t i = par->sampleStep; i--;) ignore_this_line(prfF);
  };

  std::string num;
  std::vector<float> seq;
  seq.reserve(filt_size);
  auto entropy{0.f};
  uint64_t symsNo{0};  // No. syms based on profile







  // // First value
  // for (auto i = (filt_size >> 1u) + 1; i-- && (prfF >> entropy); jump_lines())
  //   seq.push_back(entropy);

  // if (seq.size() <= (filt_size >> 1u)) 
  //   make_window(2 * seq.size() + 1);
  // else
  //   make_window(filt_size);

  const auto winBeg{std::begin(window)};
  const auto winEnd{std::end(window)};
  const auto half_wsize = (window.size() >> 1u);
  auto sum_weights{std::accumulate(winBeg, winEnd, 0.f)};

  // auto sum = std::inner_product(std::begin(seq), std::end(seq),
  //                               winBeg + half_wsize, 0.f);
  // // std::inner_product(winBeg + half_wsize, winEnd, std::begin(seq), 0.f);
  // auto filtered = sum / sum_weights;
  // // filtered = sum / sWeight;
  // // if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  // seg->partition(pos_out, filtered);
  // if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);

  // Next half_wsize values
  for (auto i = half_wsize; i-- && (prfF >> entropy); jump_lines()) {
    seq.push_back(entropy);
    if (SaveFilter) filF << precision(PREC_FIL) << entropy << '\n';
    ++seg->pos;
    seg->partition(pos_out, entropy);
    if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  }

  // The rest
  uint32_t idx{0};
  for (auto seqBeg = std::begin(seq); prfF >> entropy; jump_lines()) {
    seq[idx] = entropy;
    idx = (idx + 1) % filt_size;
    auto sum = (std::inner_product(winBeg, winEnd - idx, seqBeg + idx, 0.f) +
           std::inner_product(winEnd - idx, winEnd, seqBeg, 0.f));
    auto filtered = sum / sum_weights;
    // filtered = sum / sWeight;
    if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
    ++seg->pos;
    seg->partition(pos_out, filtered);   
    if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  }
  prfF.close();

  // Until half of the window goes outside the array
  const auto offset{idx};
  for (auto i = 1u; i != half_wsize + 1; ++i) {
    // auto seqBeg = std::begin(seq), seqEnd = std::end(seq);
    // if (++idx < filt_size + 1)
    //   sum = (std::inner_product(seqBeg + idx, seqEnd, winBeg, 0.f) +
    //          std::inner_product(seqBeg, seqBeg + offset, winEnd - idx, 0.f));
    // else
    //   sum = std::inner_product(seqBeg + (idx % filt_size), seqBeg + offset,
    //                            winBeg, 0.f);
    // filtered = sum / sum_weights;
    if (SaveFilter) filF << precision(PREC_FIL) << seq[i-1] << '\n';
    ++seg->pos;
    seg->partition(pos_out, seq[i-1]);
    if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  }
  seg->finalize_partition(pos_out);
  if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);







  
  // // First value
  // // for (auto i = half_wsize + 1; i-- && (prfF >> entropy); jump_lines())
  // for (auto i = (filt_size >> 1u) + 1; i-- && (prfF >> entropy); jump_lines())
  //   seq.push_back(entropy);

  // if (seq.size() <= (filt_size >> 1u)) 
  //   make_window(2 * seq.size() + 1);
  // else
  //   make_window(filt_size);

  // const auto winBeg{std::begin(window)};
  // const auto winEnd{std::end(window)};
  // const auto half_wsize = (window.size() >> 1u);
  // auto sum_weights{std::accumulate(winBeg, winEnd, 0.f)};

  // auto sum = std::inner_product(std::begin(seq), std::end(seq),
  //                               winBeg + half_wsize, 0.f);
  // // std::inner_product(winBeg + half_wsize, winEnd, std::begin(seq), 0.f);
  // auto filtered = sum / sum_weights;
  // // filtered = sum / sWeight;
  // if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  // seg->partition(pos_out, filtered);
  // if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);

  // // Next half_wsize values
  // for (auto i = half_wsize; i-- && (prfF >> entropy); jump_lines()) {
  //   seq.push_back(entropy);
  //   sum = std::inner_product(winBeg + i, winEnd, std::begin(seq), 0.f);
  //   filtered = sum / sum_weights;
  //   // sWeight += window[i];
  //   // filtered = sum / sWeight;
  //   if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  //   ++seg->pos;
  //   seg->partition(pos_out, filtered);
  //   if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  // }

  // // The rest
  // uint32_t idx{0};
  // for (auto seqBeg = std::begin(seq); prfF >> entropy; jump_lines()) {
  //   seq[idx] = entropy;
  //   idx = (idx + 1) % filt_size;
  //   sum = (std::inner_product(winBeg, winEnd - idx, seqBeg + idx, 0.f) +
  //          std::inner_product(winEnd - idx, winEnd, seqBeg, 0.f));
  //   filtered = sum / sum_weights;
  //   // filtered = sum / sWeight;
  //   if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  //   ++seg->pos;
  //   seg->partition(pos_out, filtered);   
  //   if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  // }
  // prfF.close();

  // // Until half of the window goes outside the array
  // const auto offset{idx};
  // for (auto i = 1u; i != half_wsize + 1; ++i) {
  //   auto seqBeg = std::begin(seq), seqEnd = std::end(seq);
  //   if (++idx < filt_size + 1)
  //     sum = (std::inner_product(seqBeg + idx, seqEnd, winBeg, 0.f) +
  //            std::inner_product(seqBeg, seqBeg + offset, winEnd - idx, 0.f));
  //   else
  //     sum = std::inner_product(seqBeg + (idx % filt_size), seqBeg + offset,
  //                              winBeg, 0.f);
  //   filtered = sum / sum_weights;
  //   // sWeight -= window[filt_size - i];
  //   // filtered = sum / sWeight;
  //   if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  //   ++seg->pos;
  //   seg->partition(pos_out, filtered);
  //   if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);
  // }
  // seg->finalize_partition(pos_out);
  // if (par->verbose) show_progress(++symsNo, seg->totalSize, par->message);




  filF.close();
  if (!SaveFilter) remove(filterName.c_str());
  nSegs = seg->nSegs;
}

// bool Filter::is_mergable (const Position& pos1, const Position& pos2) const {
//   const auto minBeg = std::min(pos1.beg, pos2.beg);
//   const auto maxBeg = std::max(pos1.beg, pos2.beg);
//   const auto minEnd = std::min(pos1.end, pos2.end);
//   const auto maxEnd = std::max(pos1.end, pos2.end);

//   return static_cast<double>(minEnd-maxBeg) > 0.8 * (maxEnd-minBeg);
// }

void Filter::extract_seg(std::vector<PosRow>& pos_out, uint8_t round,
                         uint8_t run_num, std::string ref) const {
  uint64_t seg_idx{0};

  for (const auto& row : pos_out) {
    if (row.round == round && row.run_num == run_num && row.ref == ref) {
      auto subseq = std::make_unique<SubSeq>();
      subseq->inName = row.tar;
      const auto seg{gen_name(row.run_num, row.ref, row.tar, Format::segment)};
      subseq->outName = seg + std::to_string(seg_idx);
      subseq->begPos = row.beg_pos;
      const uint64_t max_tar_pos{file_size(row.tar) - 1};
      subseq->size = static_cast<std::streamsize>(
          row.end_pos > max_tar_pos ? max_tar_pos - subseq->begPos + 1
                                    : row.end_pos - subseq->begPos + 1);
      extract_subseq(subseq);
      ++seg_idx;
    }
  }
}

#ifdef BENCH
template <typename Iter, typename Value>
inline void Filter::shift_left_insert(Iter first, Value v) {
  std::copy(first + 1, first + filt_size, first);
  *(first + filt_size - 1) = v;
}
#endif