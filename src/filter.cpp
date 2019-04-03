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

Filter::Filter(std::unique_ptr<Param>& par) : wtype(par->wtype) {
  set_wsize(par);
  if ((par->filter || par->segment) && par->verbose) show_info(par);
}

inline void Filter::set_wsize(std::unique_ptr<Param>& par) {
  if (par->manFilterScale) {
    const auto biggest = std::min(file_size(par->tar), file_size(par->ref));
    const auto lg = std::log10(biggest / par->sampleStep);
    switch (par->filterScale) {
      case FilterScale::s:
        wsize = std::pow(2, 2 * lg) + 1;
        break;
      case FilterScale::m:
        wsize = std::pow(2, 2 * lg + 1) + 1;
        break;
      case FilterScale::l:
        wsize = std::pow(2, 2 * lg + 2) + 1;
        break;
    }
  } else {
    wsize = is_odd(par->wsize) ? par->wsize : par->wsize + 1;
  }

  window.resize(wsize);
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
        std::cerr << wsize;
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
    switch (c) {
      case '1':
        std::cerr.imbue(std::locale("en_US.UTF8"));
        std::cerr << file_size(par->ref);
        break;
      // case 'r':  cerr<<par->ref; break;
      case 'r':
        std::cerr << par->refName;
        break;
      case '2':
        std::cerr.imbue(std::locale("en_US.UTF8"));
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

void Filter::smooth_seg(std::unique_ptr<Param>& par) {
  message = "Filtering & segmenting " + italic(par->tarName) + " ";

  if (wtype == WType::rectangular) {
    (par->saveFilter || par->saveAll) ? smooth_seg_rect<true>(par)
                                      : smooth_seg_rect<false>(par);
  } else {
    make_window();
    (par->saveFilter || par->saveAll) ? smooth_seg_non_rect<true>(par)
                                      : smooth_seg_non_rect<false>(par);
  }

  if (!par->saveAll && !par->saveProfile)
    remove((gen_name(par->ID, par->refName, par->tarName, Format::profile))
               .c_str());
  if (!par->saveAll && !par->saveFilter)
    remove((gen_name(par->ID, par->refName, par->tarName, Format::filter))
               .c_str());

  std::cerr << message << "finished. Detected " << nSegs << " segment"
            << (nSegs == 1 ? "" : "s") << ".\n";
}

inline void Filter::make_window() {
  switch (wtype) {
    case WType::hamming:
      make_hamming();
      break;
    case WType::hann:
      make_hann();
      break;
    case WType::blackman:
      make_blackman();
      break;
    case WType::triangular:
      make_triangular();
      break;
    case WType::welch:
      make_welch();
      break;
    case WType::sine:
      make_sine();
      break;
    case WType::nuttall:
      make_nuttall();
      break;
    default:
      break;
  }
}

inline void Filter::make_hamming() {
  if (wsize == 1) error("The size of Hamming window must be greater than 1.");
  float num{0.f};
  uint32_t den{0};
  if (is_odd(wsize)) {
    num = PI;
    den = (wsize - 1) >> 1u;
  } else {
    num = 2 * PI;
    den = wsize - 1;
  }

  for (auto n = (wsize + 1) >> 1u, last = wsize - 1; n--;)
    window[n] = window[last - n] =
        static_cast<float>(0.54 - 0.46 * std::cos(n * num / den));
}

inline void Filter::make_hann() {
  if (wsize == 1) error("The size of Hann window must be greater than 1.");
  float num{0.f};
  uint32_t den{0};
  if (is_odd(wsize)) {
    num = PI;
    den = (wsize - 1) >> 1u;
  } else {
    num = 2 * PI;
    den = wsize - 1;
  }

  for (auto n = (wsize + 1) >> 1u, last = wsize - 1; n--;)
    window[n] = window[last - n] =
        static_cast<float>(0.5 * (1 - std::cos(n * num / den)));
}

inline void Filter::make_blackman() {
  if (wsize == 1) error("The size of Blackman window must be greater than 1.");
  float num1{0.f};
  float num2{0.f};
  uint32_t den{0};
  if (is_odd(wsize)) {
    num1 = PI;
    num2 = 2 * PI;
    den = (wsize - 1) >> 1u;
  } else {
    num1 = 2 * PI;
    num2 = 4 * PI;
    den = wsize - 1;
  }

  for (auto n = (wsize + 1) >> 1u, last = wsize - 1; n--;)
    window[n] = window[last - n] =
        static_cast<float>(0.42 - 0.5 * std::cos(n * num1 / den) +
                           0.08 * std::cos(n * num2 / den));
  if (window.front() < 0) window.front() = 0.0;  // Because of low precision
  if (window.back() < 0) window.back() = 0.0;    // Because of low precision
}

// Bartlett window:  w(n) = 1 - |(n - (N-1)/2) / (N-1)/2|
inline void Filter::make_triangular() {
  if (wsize == 1)
    error("The size of triangular window must be greater than 1.");

  if (is_odd(wsize)) {
    const uint32_t den{(wsize - 1) >> 1u};
    for (auto n = (wsize + 1) >> 1u, last = wsize - 1; n--;)
      window[n] = window[last - n] =
          1 - std::fabs(static_cast<float>(n) / den - 1);
  } else {
    constexpr auto num{2.0f};
    const uint32_t den{wsize - 1};
    for (auto n = (wsize + 1) >> 1u, last = wsize - 1; n--;)
      window[n] = window[last - n] = 1 - std::fabs(n * num / den - 1);
  }
}

inline void Filter::make_welch() {  // w(n) = 1 - ((n - (N-1)/2) / (N-1)/2)^2
  if (wsize == 1) error("The size of Welch window must be greater than 1.");

  if (is_odd(wsize)) {
    const uint32_t den{(wsize - 1) >> 1u};
    for (auto n = (wsize + 1) >> 1u, last = wsize - 1; n--;)
      window[n] = window[last - n] = float(1 - Power(float(n) / den - 1, 2));
  } else {
    constexpr auto num{2.0f};
    const uint32_t den{wsize - 1};
    for (auto n = (wsize + 1) >> 1u, last = wsize - 1; n--;)
      window[n] = window[last - n] = float(1 - Power(n * num / den - 1, 2));
  }
}

inline void Filter::make_sine() {
  if (wsize == 1) error("The size of sine window must be greater than 1.");
  constexpr float num{PI};
  const uint32_t den{wsize - 1};

  for (auto n = (wsize + 1) >> 1u, last = wsize - 1; n--;)
    window[n] = window[last - n] = std::sin(n * num / den);
}

inline void Filter::make_nuttall() {
  if (wsize == 1) error("The size of Nuttall window must be greater than 1.");
  float num1{0.f};
  float num2{0.f};
  float num3{0.f};
  uint32_t den{0};
  if (is_odd(wsize)) {
    num1 = PI;
    num2 = 2 * PI;
    num3 = 3 * PI;
    den = (wsize - 1) >> 1u;
  } else {
    num1 = 2 * PI;
    num2 = 4 * PI;
    num3 = 6 * PI;
    den = wsize - 1;
  }

  for (auto n = (wsize + 1) >> 1u, last = wsize - 1; n--;)
    window[n] = window[last - n] = static_cast<float>(
        0.36 - 0.49 * std::cos(n * num1 / den) + 0.14 * cos(n * num2 / den) -
        0.01 * std::cos(n * num3 / den));
  window.front() = window.back() = 0.0;
}

template <bool SaveFilter>
inline void Filter::smooth_seg_rect(std::unique_ptr<Param>& par) {
  const auto profileName{
      gen_name(par->ID, par->ref, par->tar, Format::profile)};
  const auto filterName{gen_name(par->ID, par->ref, par->tar, Format::filter)};
  const auto positionName{
      gen_name(par->ID, par->ref, par->tar, Format::position)};
  check_file(profileName);
  std::ifstream prfF(profileName);
  std::ofstream filF(filterName);
  std::ofstream posF(positionName);
  std::vector<float> seq;
  seq.reserve(wsize);
  auto seg = std::make_shared<Segment>();
  seg->thresh = par->thresh;
  if (par->manSegSize) seg->minSize = par->segSize;
  {
    uint8_t maxCtx = 0;
    for (const auto& e : par->refMs)
      if (e.k > maxCtx) maxCtx = e.k;
    seg->set_guards(maxCtx);
  }
  std::string num;
  auto sum{0.f};
  auto filtered{0.f};
  uint64_t symsNo{0};
  seg->totalSize = file_lines(profileName) / par->sampleStep;
  // const auto totalSize = (file_lines(profileName) / par->sampleStep) + 1;
  const auto jump_lines = [&]() {
    for (uint64_t i = par->sampleStep - 1; i--;) ignore_this_line(prfF);
  };

  // First value
  for (auto i = (wsize >> 1u) + 1; i-- && getline(prfF, num);) {
    const auto val{stof(num)};
    seq.emplace_back(val);
    sum += val;
    show_progress(++symsNo, seg->totalSize, message);
    jump_lines();
  }
  filtered = sum / seq.size();
  if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  seg->partition(posF, filtered);

  // Next wsize>>1 values
  for (auto i = (wsize >> 1u); i-- && getline(prfF, num);) {
    const auto val{stof(num)};
    seq.emplace_back(val);
    sum += val;
    ++seg->pos;
    filtered = sum / seq.size();
    if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, seg->totalSize, message);
    jump_lines();
  }

  // The rest
  uint32_t idx{0};
  while (getline(prfF, num)) {
    const auto val = stof(num);
    sum += val - seq[idx];
    ++seg->pos;
    filtered = sum / wsize;
    if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
    seg->partition(posF, filtered);
    seq[idx] = val;
    idx = (idx + 1) % wsize;
    show_progress(++symsNo, seg->totalSize, message);
    jump_lines();
  }
  prfF.close();

  // Until half of the window goes outside the array
  for (auto i = 1u; i != (wsize >> 1u) + 1; ++i) {
    sum -= seq[idx];
    ++seg->pos;
    filtered = sum / (wsize - i);
    if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
    seg->partition(posF, filtered);
    idx = (idx + 1) % wsize;
    show_progress(++symsNo, seg->totalSize, message);
  }
  seg->partition_last(posF);
  show_progress(++symsNo, seg->totalSize, message);

  posF.close();
  if (file_is_empty(positionName)) remove(positionName.c_str());
  filF.close();
  if (!SaveFilter) remove(filterName.c_str());
  // if (!par->saveAll && !SaveFilter) remove(filterName.c_str());
  nSegs = seg->nSegs;
}

template <bool SaveFilter>
inline void Filter::smooth_seg_non_rect(std::unique_ptr<Param>& par) {
  const auto profileName{
      gen_name(par->ID, par->ref, par->tar, Format::profile)};
  const auto filterName{gen_name(par->ID, par->ref, par->tar, Format::filter)};
  const auto positionName{
      gen_name(par->ID, par->ref, par->tar, Format::position)};
  check_file(profileName);
  std::ifstream prfF(profileName);
  std::ofstream filF(filterName);
  std::ofstream posF(positionName);
  std::vector<float> seq;
  seq.reserve(wsize);
  auto seg = std::make_shared<Segment>();
  seg->thresh = par->thresh;
  if (par->manSegSize) seg->minSize = par->segSize;
  {
    uint8_t maxCtx = 0;
    for (const auto& e : par->refMs)
      if (e.k > maxCtx) maxCtx = e.k;
    seg->set_guards(maxCtx);
  }
  const auto winBeg{std::begin(window)};
  const auto winEnd{std::end(window)};
  auto sWeight{std::accumulate(winBeg + (wsize >> 1u), winEnd, 0.f)};
  std::string num;
  auto sum{0.f};
  auto filtered{0.f};
  uint64_t symsNo{0};
  seg->totalSize = file_lines(profileName) / par->sampleStep;
  // const auto totalSize = (file_lines(profileName) / par->sampleStep) + 1;
  const auto jump_lines = [&]() {
    for (uint64_t i = par->sampleStep - 1; i--;) ignore_this_line(prfF);
  };

  // First value
  for (auto i = (wsize >> 1u) + 1; i-- && std::getline(prfF, num);
       jump_lines()) {
    seq.emplace_back(stof(num));
    jump_lines();
  }
  sum = inner_product(winBeg + (wsize >> 1u), winEnd, std::begin(seq), 0.f);
  filtered = sum / sWeight;
  if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
  seg->partition(posF, filtered);
  show_progress(++symsNo, seg->totalSize, message);

  // Next wsize>>1 values
  for (auto i = (wsize >> 1u); i-- && std::getline(prfF, num);) {
    seq.emplace_back(stof(num));
    sum = std::inner_product(winBeg + i, winEnd, std::begin(seq), 0.f);
    ++seg->pos;
    sWeight += window[i];
    filtered = sum / sWeight;
    if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, seg->totalSize, message);
    jump_lines();
  }

  // The rest
  uint32_t idx{0};
  for (auto seqBeg = std::begin(seq); std::getline(prfF, num);) {
    seq[idx] = std::stof(num);
    idx = (idx + 1) % wsize;
    sum = (std::inner_product(winBeg, winEnd - idx, seqBeg + idx, 0.f) +
           std::inner_product(winEnd - idx, winEnd, seqBeg, 0.f));
    ++seg->pos;
    filtered = sum / sWeight;
    if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, seg->totalSize, message);
    jump_lines();
  }
  prfF.close();

  // Until half of the window goes outside the array
  const auto offset{idx};
  for (auto i = 1u; i != (wsize >> 1u) + 1; ++i) {
    auto seqBeg = std::begin(seq), seqEnd = std::end(seq);
    if (++idx < wsize + 1)
      sum = (std::inner_product(seqBeg + idx, seqEnd, winBeg, 0.f) +
             std::inner_product(seqBeg, seqBeg + offset, winEnd - idx, 0.f));
    else
      sum = std::inner_product(seqBeg + (idx % wsize), seqBeg + offset, winBeg,
                               0.f);
    ++seg->pos;
    sWeight -= window[wsize - i];
    filtered = sum / sWeight;
    if (SaveFilter) filF << precision(PREC_FIL) << filtered << '\n';
    seg->partition(posF, filtered);
    show_progress(++symsNo, seg->totalSize, message);
  }
  seg->partition_last(posF);
  show_progress(++symsNo, seg->totalSize, message);

  posF.close();
  if (file_is_empty(positionName)) remove(positionName.c_str());
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

void Filter::merge_extract_seg(uint32_t ID, std::string ref,
                               std::string tar) const {
  check_file(gen_name(ID, ref, tar, Format::position));
  std::ifstream pos_file(gen_name(ID, ref, tar, Format::position));
  const auto segName{gen_name(ID, ref, tar, Format::segment)};
  auto subseq = std::make_unique<SubSeq>();
  subseq->inName = tar;
  uint64_t i{0};
  const uint64_t maxTarPos{file_size(tar) - 1};

  // vector<Position> pos;
  // for (uint64_t beg,end,ent; pos_file >> beg >> end >> ent;) {
  //   pos.emplace_back(Position(beg, end));
  // }
  // std::sort(std::begin(pos), std::end(pos),
  //   [] (const Position& a, const Position& b) { return a.beg < b.beg; });

  for (std::string beg, end, ent; pos_file >> beg >> end >> ent; ++i) {
    subseq->outName = segName + std::to_string(i);
    subseq->begPos = std::stoull(beg);
    if (std::stoull(end) > maxTarPos)
      subseq->size =
          static_cast<std::streamsize>(maxTarPos - subseq->begPos + 1);
    else
      subseq->size =
          static_cast<std::streamsize>(std::stoull(end) - subseq->begPos + 1);
    extract_subseq(subseq);
  }

  pos_file.close();
}

void Filter::aggregate_mid_pos(uint32_t ID, std::string ref,
                               std::string tar) const {
  const std::string file1_name{gen_name(ID, ref, tar, Format::position)};
  std::ifstream file1(file1_name);
  std::ofstream mid_file(gen_name(ID, ref, tar, Format::midposition));
  int i{0};

  for (std::string beg1, end1, ent1, self_ent1;
       file1 >> beg1 >> end1 >> ent1 >> self_ent1; ++i) {
    const std::string ref2{gen_name(ID, ref, tar, Format::segment) +
                           std::to_string(i)};
    const std::string tar2{ref};
    const std::string file2_name{gen_name(ID, ref2, tar2, Format::position)};

    if (!file_is_empty(file2_name)) {
      std::ifstream file2(file2_name);

      int j{0};
      for (std::string beg2, end2, ent2, self_ent2;
           file2 >> beg2 >> end2 >> ent2 >> self_ent2; ++j) {
        const std::string ref3{gen_name(ID, ref2, ref, Format::segment) +
                               std::to_string(j)};
        const std::string tar3{ref2};
        const std::string file3_name{
            gen_name(ID, ref3, tar3, Format::position)};

        if (!file_is_empty(file3_name)) {
          std::ifstream file3(file3_name);

          for (std::string beg3, end3, ent3, self_ent3; file3 >> beg3 >> end3 >> ent3 >> self_ent3;) {
            mid_file << beg2 << '\t' << end2 << '\t' << ent2 << '\t'
                     << self_ent2 << '\t';

            const auto beg_final =
                std::to_string(std::stoull(beg1) + std::stoull(beg3));
            const auto end_final =
                std::to_string(std::stoull(beg1) + std::stoull(end3));

            if (ID == 0)
              mid_file << beg_final << '\t' << end_final;
            else if (ID == 1)
              mid_file << end_final << '\t' << beg_final;

            // todo ent1 or ent3???
            mid_file << '\t' << ent1 << '\t' << self_ent3 << '\n';
          }

          file3.close();
        }
      }

      file2.close();
    } else {
      mid_file << DBLANK << '\t' << DBLANK << '\t' << DBLANK << '\t' << DBLANK
               << '\t';
      if (ID == 0)
        mid_file << beg1 << '\t' << end1;
      else if (ID == 1)
        mid_file << end1 << '\t' << beg1;
      mid_file << '\t' << ent1 << '\t' << self_ent1 << '\n';
    }

    // remove(file2_name.c_str());//todo
  }

  file1.close();
  // remove(file1_name.c_str());//todo
  mid_file.close();
}

// void Filter::aggregate_mid_pos(uint32_t ID, std::string ref,
//                                std::string tar) const {
//   const std::string file1_name{gen_name(ID, ref, tar, Format::position)};
//   std::ifstream file1(file1_name);
//   std::ofstream mid_file(gen_name(ID, ref, tar, Format::midposition));
//   int i{0};

//   for (std::string beg1, end1, ent1, self_ent1;
//        file1 >> beg1 >> end1 >> ent1 >> self_ent1; ++i) {
//     const std::string file2_name{gen_name(
//         ID, gen_name(ID, ref, tar, Format::segment) + std::to_string(i), ref,
//         Format::position)};

//     if (!file_is_empty(file2_name)) {
//       std::ifstream file2(file2_name);

//       for (std::string beg2, end2, ent2, self_ent2;
//            file2 >> beg2 >> end2 >> ent2 >> self_ent2;) {
//         mid_file << beg2 << '\t' << end2 << '\t' << ent2 << '\t' << self_ent2
//                  << '\t';
//         if (ID == 0)
//           mid_file << beg1 << '\t' << end1;
//         else if (ID == 1)
//           mid_file << end1 << '\t' << beg1;
//         mid_file << '\t' << ent1 << '\t' << self_ent1 << '\n';
//       }

//       file2.close();
//     } else {
//       mid_file << DBLANK << '\t' << DBLANK << '\t' << DBLANK << '\t' << DBLANK
//                << '\t';
//       if (ID == 0)
//         mid_file << beg1 << '\t' << end1;
//       else if (ID == 1)
//         mid_file << end1 << '\t' << beg1;
//       mid_file << '\t' << ent1 << '\t' << self_ent1 << '\n';
//     }

//     remove(file2_name.c_str());//todo
//   }

//   file1.close();
//   remove(file1_name.c_str());//todo
//   mid_file.close();
// }

void Filter::move_mid_to_pos_file(std::string mid_file_name,
                                  std::ofstream& pos_file) const {
  std::ifstream mid_file(mid_file_name);
  const uint64_t chunk_size{file_size(mid_file_name)};
  std::vector<char> buffer(chunk_size, 0);

  mid_file.read(buffer.data(), chunk_size);
  pos_file.write(buffer.data(), chunk_size);

  mid_file.close();
  remove(mid_file_name.c_str());
}

void Filter::aggregate_final_pos(std::string ref, std::string tar) const {
  const auto mid0_file_name{gen_name(0, ref, tar, Format::midposition)};
  const auto mid1_file_name{gen_name(1, ref, tar, Format::midposition)};
  const bool mid0_file_is_empty{file_is_empty(mid0_file_name)};
  const bool mid1_file_is_empty{file_is_empty(mid1_file_name)};

  if (mid0_file_is_empty && mid1_file_is_empty) {
    std::cerr << bold("The reference and the target are not similar.\n");
    return;
  }

  const auto pos_file_name{gen_name(ref, tar, Format::position)};
  std::ofstream pos_file(pos_file_name);
  pos_file << POS_HDR << '\t' << file_name(ref) << '\t'
           << std::to_string(file_size(ref)) << '\t' << file_name(tar) << '\t'
           << std::to_string(file_size(tar)) << '\n';

  if (!mid0_file_is_empty && mid1_file_is_empty) {
    move_mid_to_pos_file(mid0_file_name, pos_file);
  } else if (mid0_file_is_empty && !mid1_file_is_empty) {
    move_mid_to_pos_file(mid1_file_name, pos_file);
  } else {
    move_mid_to_pos_file(mid0_file_name, pos_file);
    move_mid_to_pos_file(mid1_file_name, pos_file);
  }

  pos_file.close();
}

#ifdef BENCH
template <typename Iter, typename Value>
inline void Filter::shift_left_insert(Iter first, Value v) {
  std::copy(first + 1, first + wsize, first);
  *(first + wsize - 1) = v;
}
#endif