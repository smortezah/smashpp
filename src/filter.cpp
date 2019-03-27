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

Filter::Filter(const Param& p) : wtype(p.wtype) {
  set_wsize(p);
  if ((p.filter || p.segment) && p.verbose) show_info(p);
}

inline void Filter::set_wsize(const Param& p) {
  if (p.manFilterScale) {
    const auto biggest = std::min(file_size(p.tar), file_size(p.ref));
    const auto lg = std::log10(biggest / p.sampleStep);
    switch (p.filterScale) {
      case FilterScale::S:
        wsize = std::pow(2, 2 * lg) + 1;
        break;
      case FilterScale::M:
        wsize = std::pow(2, 2 * lg + 1) + 1;
        break;
      case FilterScale::L:
        wsize = std::pow(2, 2 * lg + 2) + 1;
        break;
    }
  } else {
    wsize = is_odd(p.wsize) ? p.wsize : p.wsize + 1;
  }

  window.resize(wsize);
}

inline void Filter::show_info(const Param& p) const {
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
        std::cerr << p.print_win_type();
        break;
      case 's':
        std::cerr << p.print_filter_scale();
        break;
      case 'w':
        std::cerr << wsize;
        break;
      case 't':
        std::cerr << p.thresh;
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
        std::cerr << file_size(p.ref);
        break;
      // case 'r':  cerr<<p.ref; break;
      case 'r':
        std::cerr << p.refName;
        break;
      case '2':
        std::cerr.imbue(std::locale("en_US.UTF8"));
        std::cerr << file_size(p.tar);
        break;
      // case 't':  cerr<<p.tar; break;
      case 't':
        std::cerr << p.tarName;
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
  if (p.manFilterScale || !p.manWSize) {
    label("Filter scale");
    filter_vals('s');
  }
  label("Window size");
  filter_vals('w');
  if (p.manThresh) {
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

void Filter::smooth_seg(const Param& p) {
  message = "Filtering & segmenting " + italic(p.tarName) + " ";

  if (wtype == WType::RECTANGULAR) {
    p.saveFilter ? smooth_seg_rect<true>(p) : smooth_seg_rect<false>(p);
  } else {
    make_window();
    p.saveFilter ? smooth_seg_non_rect<true>(p) : smooth_seg_non_rect<false>(p);
  }

  if (!p.saveAll && !p.saveProfile)
    remove((gen_name(p.ID, p.refName, p.tarName, Format::PROFILE)).c_str());
  if (!p.saveAll && !p.saveFilter)
    remove((gen_name(p.ID, p.refName, p.tarName, Format::FILTER)).c_str());

  std::cerr << message << "finished. "
            << "Detected " << nSegs << " segment" << (nSegs == 1 ? "" : "s")
            << ".\n";
}

inline void Filter::make_window() {
  switch (wtype) {
    case WType::HAMMING:
      hamming();
      break;
    case WType::HANN:
      hann();
      break;
    case WType::BLACKMAN:
      blackman();
      break;
    case WType::TRIANGULAR:
      triangular();
      break;
    case WType::WELCH:
      welch();
      break;
    case WType::SINE:
      sine();
      break;
    case WType::NUTTALL:
      nuttall();
      break;
    default:
      break;
  }
}

inline void Filter::hamming() {
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

inline void Filter::hann() {
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

inline void Filter::blackman() {
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
inline void Filter::triangular() {
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

inline void Filter::welch() {  // w(n) = 1 - ((n - (N-1)/2) / (N-1)/2)^2
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

inline void Filter::sine() {
  if (wsize == 1) error("The size of sine window must be greater than 1.");
  constexpr float num{PI};
  const uint32_t den{wsize - 1};

  for (auto n = (wsize + 1) >> 1u, last = wsize - 1; n--;)
    window[n] = window[last - n] = std::sin(n * num / den);
}

inline void Filter::nuttall() {
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
inline void Filter::smooth_seg_rect(const Param& p) {
  const auto profileName{gen_name(p.ID, p.ref, p.tar, Format::PROFILE)};
  const auto filterName{gen_name(p.ID, p.ref, p.tar, Format::FILTER)};
  const auto positionName{gen_name(p.ID, p.ref, p.tar, Format::POSITION)};
  check_file(profileName);
  std::ifstream prfF(profileName);
  std::ofstream filF(filterName);
  std::ofstream posF(positionName);
  std::vector<float> seq;
  seq.reserve(wsize);
  auto seg = std::make_shared<Segment>();
  seg->thresh = p.thresh;
  if (p.manSegSize) seg->minSize = p.segSize;
  {
    uint8_t maxCtx = 0;
    for (const auto& e : p.refMs)
      if (e.k > maxCtx) maxCtx = e.k;
    seg->set_guards(maxCtx);
  }
  std::string num;
  auto sum{0.f};
  auto filtered{0.f};
  uint64_t symsNo{0};
  seg->totalSize = file_lines(profileName) / p.sampleStep;
  // const auto totalSize = (file_lines(profileName) / p.sampleStep) + 1;
  const auto jump_lines = [&]() {
    for (uint64_t i = p.sampleStep - 1; i--;) ignore_this_line(prfF);
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
  nSegs = seg->nSegs;
}

template <bool SaveFilter>
inline void Filter::smooth_seg_non_rect(const Param& p) {
  const auto profileName{gen_name(p.ID, p.ref, p.tar, Format::PROFILE)};
  const auto filterName{gen_name(p.ID, p.ref, p.tar, Format::FILTER)};
  const auto positionName{gen_name(p.ID, p.ref, p.tar, Format::POSITION)};
  check_file(profileName);
  std::ifstream prfF(profileName);
  std::ofstream filF(filterName);
  std::ofstream posF(positionName);
  std::vector<float> seq;
  seq.reserve(wsize);
  auto seg = std::make_shared<Segment>();
  seg->thresh = p.thresh;
  if (p.manSegSize) seg->minSize = p.segSize;
  {
    uint8_t maxCtx = 0;
    for (const auto& e : p.refMs)
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
  seg->totalSize = file_lines(profileName) / p.sampleStep;
  // const auto totalSize = (file_lines(profileName) / p.sampleStep) + 1;
  const auto jump_lines = [&]() {
    for (uint64_t i = p.sampleStep - 1; i--;) ignore_this_line(prfF);
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
  check_file(gen_name(ID, ref, tar, Format::POSITION));
  std::ifstream posF(gen_name(ID, ref, tar, Format::POSITION));
  const auto segName{gen_name(ID, ref, tar, Format::SEGMENT)};
  auto subseq = std::make_unique<SubSeq>();
  subseq->inName = tar;
  uint64_t i{0};
  const uint64_t maxTarPos{file_size(tar) - 1};

  // vector<Position> pos;
  // for (uint64_t beg,end,ent; posF >> beg >> end >> ent;) {
  //   pos.emplace_back(Position(beg, end));
  // }
  // std::sort(std::begin(pos), std::end(pos),
  //   [] (const Position& a, const Position& b) { return a.beg < b.beg; });

  for (std::string beg, end, ent; posF >> beg >> end >> ent; ++i) {
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

  posF.close();
}

void Filter::aggregate_mid_pos(uint32_t ID, std::string origin,
                               std::string dest) const {
  const std::string dirFileName{gen_name(ID, origin, dest, Format::POSITION)};
  std::ifstream fDirect(dirFileName);
  std::ofstream fmid(LBL_MID + "-" +
                     gen_name(ID, origin, dest, Format::POSITION));
  int i{0};

  for (std::string begDir, endDir, entDir, selfEntDir;
       fDirect >> begDir >> endDir >> entDir >> selfEntDir; ++i) {
    const std::string refRev{gen_name(ID, origin, dest, Format::SEGMENT) +
                             std::to_string(i)};
    const std::string revFileName{
        gen_name(ID, refRev, origin, Format::POSITION)};

    if (!file_is_empty(revFileName)) {
      std::ifstream fReverse(revFileName);
      for (std::string begRev, endRev, entRev, selfEntRev;
           fReverse >> begRev >> endRev >> entRev >> selfEntRev;) {
        fmid << begRev << '\t' << endRev << '\t' << entRev << '\t' << selfEntRev
             << '\t';
        if (ID == 0)
          fmid << begDir << '\t' << endDir;
        else if (ID == 1)
          fmid << endDir << '\t' << begDir;
        fmid << '\t' << entDir << '\t' << selfEntDir << '\n';
      }
      fReverse.close();
    } else {
      fmid << DBLANK << '\t' << DBLANK << '\t' << DBLANK << '\t' << DBLANK
           << '\t';
      if (ID == 0)
        fmid << begDir << '\t' << endDir;
      else if (ID == 1)
        fmid << endDir << '\t' << begDir;
      fmid << '\t' << entDir << '\t' << selfEntDir << '\n';
    }

    remove(revFileName.c_str());
  }

  fDirect.close();
  remove(dirFileName.c_str());
  fmid.close();
}

void Filter::aggregate_final_pos(std::string ref, std::string tar) const {
  const auto midf0Name{LBL_MID + "-" + gen_name(0, ref, tar, Format::POSITION)};
  const auto midf1Name{LBL_MID + "-" + gen_name(1, ref, tar, Format::POSITION)};
  const bool midf0IsEmpty{file_is_empty(midf0Name)};
  const bool midf1IsEmpty{file_is_empty(midf1Name)};

  if (midf0IsEmpty && midf1IsEmpty) {
    std::cerr << bold("The reference and the target are not similar.\n");
  } else if (!midf0IsEmpty && midf1IsEmpty) {
    std::ifstream midf0(midf0Name);
    std::ofstream finf(gen_name(ref, tar, Format::POSITION));

    finf << POS_HDR << '\t' << file_name(ref) << '\t'
         << std::to_string(file_size(ref)) << '\t' << file_name(tar) << '\t'
         << std::to_string(file_size(tar)) << '\n';

    const uint64_t size{file_size(midf0Name)};
    std::vector<char> buffer(size, 0);
    midf0.read(buffer.data(), size);
    finf.write(buffer.data(), size);

    midf0.close();
    remove(midf0Name.c_str());
    finf.close();
  } else if (midf0IsEmpty && !midf1IsEmpty) {
    std::ifstream midf1(midf1Name);
    std::ofstream finf(gen_name(ref, tar, Format::POSITION));

    finf << POS_HDR << '\t' << file_name(ref) << '\t'
         << std::to_string(file_size(ref)) << '\t' << file_name(tar) << '\t'
         << std::to_string(file_size(tar)) << '\n';

    const uint64_t size{file_size(midf1Name)};
    std::vector<char> buffer(size, 0);
    midf1.read(buffer.data(), size);
    finf.write(buffer.data(), size);

    midf1.close();
    remove(midf1Name.c_str());
    finf.close();
  } else {
    std::ifstream midf0(midf0Name);
    std::ifstream midf1(midf1Name);
    std::ofstream finf(gen_name(ref, tar, Format::POSITION));

    finf << POS_HDR << '\t' << file_name(ref) << '\t'
         << std::to_string(file_size(ref)) << '\t' << file_name(tar) << '\t'
         << std::to_string(file_size(tar)) << '\n';
    {
      const uint64_t size{file_size(midf0Name)};
      std::vector<char> buffer(size, 0);
      midf0.read(buffer.data(), size);
      finf.write(buffer.data(), size);
    }
    {
      const uint64_t size{file_size(midf1Name)};
      std::vector<char> buffer(size, 0);
      midf1.read(buffer.data(), size);
      finf.write(buffer.data(), size);
    }

    midf0.close();
    remove(midf0Name.c_str());
    midf1.close();
    remove(midf1Name.c_str());
    finf.close();
  }
}

#ifdef BENCH
template <typename Iter, typename Value>
inline void Filter::shift_left_insert(Iter first, Value v) {
  std::copy(first + 1, first + wsize, first);
  *(first + wsize - 1) = v;
}
#endif